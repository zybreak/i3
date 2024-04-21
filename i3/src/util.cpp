/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * util.c: Utility functions, which can be useful everywhere within i3 (see
 *         also libi3).
 *
 */
module;
#include <err.h>
#include <cerrno>
#include <climits>
#include <fstream>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>
#include "i3.h"

#include <cctype>
#include <fcntl.h>
#include <cinttypes>
#include <libgen.h>
#include <clocale>
#include <sys/wait.h>
#include <unistd.h>
#include <fmt/core.h>
#include <fmt/printf.h>

#include <glib.h>

#if defined(__OpenBSD__)
#include <sys/cdefs.h>
#endif
module i3;

import utils;
import log;

/*
 * Returns true if the name consists of only digits.
 *
 */
__attribute__((pure))
bool name_is_digits(const char *name) {
    /* positive integers and zero are interpreted as numbers */
    for (size_t i = 0; i < strlen(name); i++) {
        if (!isdigit(name[i])) {
            return false;
        }
    }

    return true;
}

/*
 * Set 'out' to the layout_t value for the given layout. The function
 * returns true on success or false if the passed string is not a valid
 * layout name.
 *
 */
bool layout_from_name(const char *layout_str, layout_t *out) {
    if (strcmp(layout_str, "default") == 0) {
        *out = L_DEFAULT;
        return true;
    } else if (strcasecmp(layout_str, "stacked") == 0 ||
               strcasecmp(layout_str, "stacking") == 0) {
        *out = L_STACKED;
        return true;
    } else if (strcasecmp(layout_str, "tabbed") == 0) {
        *out = L_TABBED;
        return true;
    } else if (strcasecmp(layout_str, "splitv") == 0) {
        *out = L_SPLITV;
        return true;
    } else if (strcasecmp(layout_str, "splith") == 0) {
        *out = L_SPLITH;
        return true;
    }

    return false;
}

/*
 * Parses the workspace name as a number. Returns -1 if the workspace should be
 * interpreted as a "named workspace".
 *
 */
int ws_name_to_number(const std::string &name) {
    /* positive integers and zero are interpreted as numbers */
    try {
        return std::stoi(name, nullptr, 10);
    } catch (const std::logic_error &e) {
        return -1;
    }
}

/*
 * Updates *destination with new_value and returns true if it was changed or false
 * if it was the same
 *
 */
bool update_if_necessary(uint32_t *destination, const uint32_t new_value) {
    uint32_t old_value = *destination;

    return ((*destination = new_value) != old_value);
}

/*
 * Goes through the list of arguments (for exec()) and add/replace the given option,
 * including the option name, its argument, and the option character.
 */
static char **add_argument(char **original, const char *opt_char, const char *opt_arg, const char *opt_name) {
    int num_args;
    for (num_args = 0; original[num_args] != nullptr; num_args++) {
    }
    char **result = (char**)scalloc(num_args + 3, sizeof(char *));

    /* copy the arguments, but skip the ones we'll replace */
    int write_index = 0;
    bool skip_next = false;
    for (int i = 0; i < num_args; ++i) {
        if (skip_next) {
            skip_next = false;
            continue;
        }
        if (!strcmp(original[i], opt_char) ||
            (opt_name && !strcmp(original[i], opt_name))) {
            if (opt_arg) {
                skip_next = true;
            }
            continue;
        }
        result[write_index++] = sstrdup(original[i]);
    }

    /* add the arguments we'll replace */
    result[write_index++] = (opt_char != nullptr) ? sstrdup(opt_char) : nullptr;
    result[write_index] = (opt_arg != nullptr) ? sstrdup(opt_arg) : nullptr;

    return result;
}

/**
 * Wrapper around correct write which returns -1 (meaning that
 * write failed) or count (meaning that all bytes were written)
 *
 */
static ssize_t writeall(int fd, const void *buf, size_t count) {
    size_t written = 0;

    while (written < count) {
        const ssize_t n = write(fd, ((char *)buf) + written, count - written);
        if (n == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            return n;
        }
        written += static_cast<size_t>(n);
    }

    return written;
}

static std::string store_restart_layout() {
    setlocale(LC_NUMERIC, "C");

    auto j = dump_node(global.croot, true);

    setlocale(LC_NUMERIC, "");

    /* create a temporary file if one hasn't been specified, or just
     * resolve the tildes in the specified path */
    std::string filename;
    if (config.restart_state_path == nullptr) {
        auto f = get_process_filename("restart-state");
        if (!f) {
            return "";
        } else {
            filename = f;
        }
    } else {
        auto f = utils::resolve_tilde(config.restart_state_path);
        filename = f;
    }

    /* create the directory, it could have been cleaned up before restarting or
     * may not exist at all in case it was user-specified. */
    char *filenamecopy = sstrdup(filename.c_str());
    char *base = dirname(filenamecopy);
    DLOG(fmt::sprintf("Creating \"%s\" for storing the restart layout\n", base));
    using std::filesystem::perms;
    const perms DEFAULT_DIR_MODE = perms::owner_all | perms::group_read | perms::group_exec | perms::others_read | perms::others_exec;
    if (mkdirp(base, DEFAULT_DIR_MODE) != 0) {
        ELOG(fmt::sprintf("Could not create \"%s\" for storing the restart layout, layout will be lost.\n", base));
    }
    free(filenamecopy);

    int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open()");
        return "";
    }

    auto payload = j.dump();

    if (writeall(fd, payload.c_str(), payload.length()) == -1) {
         ELOG(fmt::sprintf("Could not write restart layout to \"%s\", layout will be lost: %s\n", filename, strerror(errno)));
        close(fd);
        return "";
    }

    close(fd);

    if (payload.length() > 0) {
        DLOG(fmt::sprintf("layout: %.*s\n",  static_cast<int>(payload.length()), payload));
    }

    return filename;
}

/*
 * Restart i3 in-place
 * appends -a to argument list to disable autostart
 *
 */
void i3_restart(bool forget_layout) {
    kill_nagbar(global.config_error_nagbar_pid, true);
    kill_nagbar(global.command_error_nagbar_pid, true);

    restore_geometry();

    ipc_shutdown(SHUTDOWN_REASON_RESTART, -1);

     LOG(fmt::sprintf("restarting \"%s\"...\n", global.start_argv[0]));
    /* make sure -a is in the argument list or add it */
    global.start_argv = add_argument(global.start_argv, "-a", nullptr, nullptr);

    /* make debuglog-on persist */
    if (get_debug_logging()) {
        global.start_argv = add_argument(global.start_argv, "-d", "all", nullptr);
    }

    /* replace -r <file> so that the layout is restored */
    if (!forget_layout) {
        std::string restart_filename = store_restart_layout();

        if (!restart_filename.empty()) {
            global.start_argv = add_argument(global.start_argv, "--restart", restart_filename.c_str(), "-r");
        }
    }

    execvp(global.start_argv[0], global.start_argv);

    /* not reached */
}

/*
 * Escapes the given string if a pango font is currently used.
 * If the string has to be escaped, the input string will be free'd.
 *
 */
std::string pango_escape_markup(std::string input) {
    char *escaped = g_markup_escape_text(input.c_str(), -1);

    return escaped;
}

/*
 * Converts a string into a long using strtol().
 * This is a convenience wrapper checking the parsing result. It returns true
 * if the number could be parsed.
 */
bool parse_long(const char *str, long *out, int base) {
    char *end = nullptr;
    long result = strtol(str, &end, base);
    if (result == LONG_MIN || result == LONG_MAX || result < 0 || (end != nullptr && *end != '\0')) {
        *out = result;
        return false;
    }

    *out = result;
    return true;
}

/*
 * Slurp reads path in its entirety into buf, returning the length of the file
 * or -1 if the file could not be read. buf is set to a buffer of appropriate
 * size, or NULL if -1 is returned.
 *
 */
std::string slurp(const std::string &path) {
    std::filebuf fb;
    if (!fb.open(path, std::ios::in)) {
         ELOG(fmt::sprintf("Cannot open file \"%s\"", path));
        throw std::runtime_error(fmt::format("Cannot open file \"{}\"", path));
    }

    try {
        std::istream is(&fb);
        std::string s(std::istreambuf_iterator<char>(is), {});
        fb.close();
        return s;
    }  catch (std::exception &e) {
        fb.close();
        throw e;
    }
}

/*
 * Convert a direction to its corresponding orientation.
 *
 */
orientation_t orientation_from_direction(direction_t direction) {
    return (direction == D_LEFT || direction == D_RIGHT) ? HORIZ : VERT;
}

/*
 * Convert a direction to its corresponding position.
 *
 */
position_t position_from_direction(direction_t direction) {
    return (direction == D_LEFT || direction == D_UP) ? BEFORE : AFTER;
}

/*
 * Convert orientation and position to the corresponding direction.
 *
 */
direction_t direction_from_orientation_position(orientation_t orientation, position_t position) {
    if (orientation == HORIZ) {
        return position == BEFORE ? D_LEFT : D_RIGHT;
    } else {
        return position == BEFORE ? D_UP : D_DOWN;
    }
}

ssize_t writeall_nonblock(int fd, const void *buf, size_t count) {
    size_t written = 0;

    while (written < count) {
        const ssize_t n = write(fd, ((char *)buf) + written, count - written);
        if (n == -1) {
            if (errno == EAGAIN) {
                return written;
            } else if (errno == EINTR) {
                continue;
            } else {
                return n;
            }
        }
        written += static_cast<size_t>(n);
    }
    return written;
}

/*
 * Returns the modifier mask (XCB_MOD_MASK_*) for the given keysymbol, for
 * example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function does not initiate any round-trips.
 *
 */
static uint32_t get_mod_mask_for(uint32_t keysym,
                          xcb_key_symbols_t *symbols,
                          xcb_get_modifier_mapping_reply_t *modmap_reply) {
    xcb_keycode_t *codes, *modmap;
    xcb_keycode_t mod_code;

    modmap = xcb_get_modifier_mapping_keycodes(modmap_reply);

    /* Get the list of keycodes for the given symbol */
    if (!(codes = xcb_key_symbols_get_keycode(symbols, keysym))) {
        return 0;
    }

    /* Loop through all modifiers (Mod1-Mod5, Shift, Control, Lock) */
    for (int mod = 0; mod < 8; mod++) {
        for (int j = 0; j < modmap_reply->keycodes_per_modifier; j++) {
            /* Store the current keycode (for modifier 'mod') */
            mod_code = modmap[(mod * modmap_reply->keycodes_per_modifier) + j];
            /* Check if that keycode is in the list of previously resolved
             * keycodes for our symbol. If so, return the modifier mask. */
            for (xcb_keycode_t *code = codes; *code; code++) {
                if (*code != mod_code)
                    continue;

                free(codes);
                /* This corresponds to the XCB_MOD_MASK_* constants */
                return (1 << mod);
            }
        }
    }

    return 0;
}

/*
 * All-in-one function which returns the modifier mask (XCB_MOD_MASK_*) for the
 * given keysymbol, for example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function initiates one round-trip. Use get_mod_mask_for() directly if
 * you already have the modifier mapping and key symbols.
 *
 */
uint32_t aio_get_mod_mask_for(uint32_t keysym, xcb_key_symbols_t *symbols) {
    xcb_get_modifier_mapping_cookie_t cookie;
    xcb_get_modifier_mapping_reply_t *modmap_r;

    xcb_flush(**global.x);

    /* Get the current modifier mapping (this is blocking!) */
    cookie = xcb_get_modifier_mapping(**global.x);
    if (!(modmap_r = xcb_get_modifier_mapping_reply(**global.x, cookie, nullptr))) {
        return 0;
    }

    uint32_t result = get_mod_mask_for(keysym, symbols, modmap_r);
    free(modmap_r);
    return result;
}

/*
 * Returns true if this version of i3 is a debug build (anything which is not a
 * release version), based on the git version number.
 *
 */
bool is_debug_build() {
    /* i3_version contains either something like this:
     *     "4.0.2 (2011-11-11)" (release version)
     * or: "4.0.2-123-gC0FFEE"  (debug version)
     *
     * So we check for the offset of the first opening round bracket to
     * determine whether this is a git version or a release version. */
    if (strchr(I3_VERSION, '(') == nullptr) {
        return true;  // e.g. 4.0.2-123-gC0FFEE
    }
    /* In practice, debug versions do not contain parentheses at all,
     * but leave the logic as it was before so that we can re-add
     * parentheses if we chose to. */
    return ((strchr(I3_VERSION, '(') - I3_VERSION) > 10);
}

/*
 * Converts direction to a string representation.
 *
 */
const char *direction_to_string(direction_t direction) {
    switch (direction) {
        case D_LEFT:
            return "left";
        case D_RIGHT:
            return "right";
        case D_UP:
            return "up";
        case D_DOWN:
            return "down";
    }
    return "invalid";
}

/*
 * Converts position to a string representation.
 *
 */
const char *position_to_string(position_t position) {
    switch (position) {
        case BEFORE:
            return "before";
        case AFTER:
            return "after";
    }
    return "invalid";
}
