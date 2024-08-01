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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
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

import std;
import utils;
import log;


/*
 * Goes through the list of arguments (for exec()) and add/replace the given option,
 * including the option name, its argument, and the option character.
 */
std::vector<std::string> add_argument(std::vector<std::string> &original, const char *opt_char, const char *opt_arg, const char *opt_name) {
    int num_args = original.size();
    std::vector<std::string> result{};
    result.reserve(num_args + 3);

    /* copy the arguments, but skip the ones we'll replace */
    bool skip_next = false;
    for (int i = 0; i < num_args; ++i) {
        if (skip_next) {
            skip_next = false;
            continue;
        }
        if (!strcmp(original[i].c_str(), opt_char) || (opt_name && !strcmp(original[i].c_str(), opt_name))) {
            if (opt_arg) {
                skip_next = true;
            }
            continue;
        }
        result.push_back(original.at(i));
    }

    /* add the arguments we'll replace */
    if (opt_char != nullptr) result.push_back(opt_char);
    if (opt_arg != nullptr) result.push_back(opt_arg);

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
    if (global.config->restart_state_path == nullptr) {
        auto f = get_process_filename("restart-state");
        if (!f) {
            return "";
        } else {
            filename = *f;
        }
    } else {
        auto f = utils::resolve_tilde(global.config->restart_state_path);
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

     LOG(fmt::sprintf("restarting \"%s\"...\n", global.start_argv.front()));
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
    
    char* argv[global.start_argv.size() + 1];
    
    for (int i = 0; i < global.start_argv.size(); i++) {
        argv[i] = (char*)global.start_argv[i].c_str();
    }
    argv[global.start_argv.size()] = nullptr;
    
    execvp(argv[0], argv);

    /* not reached */
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
