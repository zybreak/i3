module;
#include <xcb/xcb.h>
#include <climits>
export module utils;

import std;

export namespace utils {

    /*
     * Parses the workspace name as a number. Returns -1 if the workspace should be
     * interpreted as a "named workspace".
     *
     */
    __attribute__((pure)) int ws_name_to_number(std::string const &name) {
        /* positive integers and zero are interpreted as numbers */
        try {
            return std::stoi(name, nullptr, 10);
        } catch (std::logic_error const &e) {
            return -1;
        }
    }

    /*
     * Converts a string into a long using strtol().
     * This is a convenience wrapper checking the parsing result. It returns true
     * if the number could be parsed.
     */
    bool parse_long(char const *str, long *out, int base) {
        char *end = nullptr;
        long result = std::strtol(str, &end, base);
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
    std::ifstream slurp(std::filesystem::path const &path) {
        std::ifstream fileStream(path);
        if (!fileStream) {
            throw std::runtime_error(std::format("Cannot open file \"{}\"", path.native()));
        }

        return fileStream;
    }

    /**
     * This function resolves ~ in pathnames.
     * It may resolve wildcards in the first part of the path, but if no match
     * or multiple matches are found, it just returns a copy of path as given.
     *
     */
    std::string resolve_tilde(std::string_view const path);

    /**
     * Parses a string (or word, if as_word is true). Extracted out of
     * parse_command so that it can be used in src/workspace.c for interpreting
     * workspace commands.
     *
     */
    std::optional<std::string> parse_string(std::string::const_iterator &walk, bool as_word);
}  // namespace utils

/**
 * Reports whether str represents the enabled state (1, yes, true, …).
 *
 */
export bool boolstr(char const *str);

/**
 * Safe-wrapper around realloc which exits if realloc returns NULL (meaning
 * that there is no more memory available).
 *
 */
export void *srealloc(void *ptr, size_t size);

/**
 * Safe-wrapper around strdup which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
export char *sstrdup(char const *str);

/**
 * Safe-wrapper around strndup which exits if strndup returns NULL (meaning that
 * there is no more memory available)
 *
 */
export char *sstrndup(char const *str, size_t size);

/**
 * Safe-wrapper around asprintf which exits if it returns -1 (meaning that
 * there is no more memory available)
 *
 */
export int sasprintf(char **strp, char const *fmt, ...);

/**
 * Returns the name of a temporary file with the specified prefix.
 *
 */
export std::optional<std::string> get_process_filename(char const *prefix);

/**
 * Emulates mkdir -p (creates any missing folders)
 *
 */
export bool mkdirp(std::filesystem::path const &path, std::filesystem::perms mode);

/**
 * Puts the given socket file descriptor into non-blocking mode or dies if
 * setting O_NONBLOCK failed. Non-blocking sockets are a good idea for our
 * IPC model because we should by no means block the window manager.
 *
 */
export void set_nonblock(int sockfd);

/*
 * Returns the colorpixel to use for the given hex color (think of HTML).
 *
 * The hex_color has to start with #, for example #FF00FF.
 *
 * NOTE that get_colorpixel() does _NOT_ check the given color code for validity.
 * This has to be done by the caller.
 *
 */
export uint32_t get_colorpixel(xcb_connection_t *conn, xcb_screen_t *root_screen, char const *hex);

/* Represents a color split by color channel. */
export struct color_t {
    double red;
    double green;
    double blue;
    double alpha;

    /* The colorpixel we use for direct XCB calls. */
    uint32_t colorpixel;

    auto operator<=>(color_t const &r) const = default;
};

/**
 * Part of the struct Config. It makes sense to group colors for background,
 * border and text as every element in i3 has them (window decorations, bar).
 *
 */
export struct Colortriple {
    color_t border;
    color_t background;
    color_t text;
    color_t indicator;
    color_t child_border;

    auto operator<=>(Colortriple const &r) const = default;
};
