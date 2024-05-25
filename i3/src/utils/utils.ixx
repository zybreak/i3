module;
#include <xcb/xcb.h>
export module utils;

import std;

export namespace utils {
    /**
     * This function resolves ~ in pathnames.
     * It may resolve wildcards in the first part of the path, but if no match
     * or multiple matches are found, it just returns a copy of path as given.
     *
     */
    std::string resolve_tilde(const std::string_view path);

    /**
     * Parses a string (or word, if as_word is true). Extracted out of
     * parse_command so that it can be used in src/workspace.c for interpreting
     * workspace commands.
     *
     */
    std::optional<std::string> parse_string(std::string::const_iterator &walk, bool as_word);
}

/**
 * Reports whether str represents the enabled state (1, yes, true, …).
 *
 */
export bool boolstr(const char *str);

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
export char *sstrdup(const char *str);

/**
 * Safe-wrapper around strndup which exits if strndup returns NULL (meaning that
 * there is no more memory available)
 *
 */
export char *sstrndup(const char *str, size_t size);

/**
 * Safe-wrapper around asprintf which exits if it returns -1 (meaning that
 * there is no more memory available)
 *
 */
export int sasprintf(char **strp, const char *fmt, ...);

/**
 * Returns the name of a temporary file with the specified prefix.
 *
 */
export std::optional<std::string> get_process_filename(const char *prefix);

/**
 * Emulates mkdir -p (creates any missing folders)
 *
 */
export int mkdirp(const char *path, std::filesystem::perms mode);

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
export uint32_t get_colorpixel(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *hex);
