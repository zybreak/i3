module;
#include <string>
#include <string_view>
export module utils;

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
    char *parse_string(const char **walk, bool as_word);
}

/**
 * Reports whether str represents the enabled state (1, yes, true, …).
 *
 */
export bool boolstr(const char *str);

/**
 * Safe-wrapper around malloc which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
export void *smalloc(size_t size);

/**
 * Safe-wrapper around calloc which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
export void *scalloc(size_t num, size_t size);

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
export char *get_process_filename(const char *prefix);

/**
 * Emulates mkdir -p (creates any missing folders)
 *
 */
export int mkdirp(const char *path, mode_t mode);
