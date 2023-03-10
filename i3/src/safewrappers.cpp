/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
#include "wrapper.h"

#include <err.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

/*
 * The s* functions (safe) are wrappers around malloc, strdup, …, which exits if one of
 * the called functions returns NULL, meaning that there is no more memory available
 *
 */
void *smalloc(size_t size) {
    void *result = malloc(size);
    if (result == nullptr)
        err(EXIT_FAILURE, "malloc(%zd)", size);
    return result;
}

void *scalloc(size_t num, size_t size) {
    void *result = calloc(num, size);
    if (result == nullptr)
        err(EXIT_FAILURE, "calloc(%zd, %zd)", num, size);
    return result;
}

void *srealloc(void *ptr, size_t size) {
    void *result = realloc(ptr, size);
    if (result == nullptr && size > 0)
        err(EXIT_FAILURE, "realloc(%zd)", size);
    return result;
}

char *sstrdup(const char *str) {
    char *result = strdup(str);
    if (result == nullptr)
        err(EXIT_FAILURE, "strdup()");
    return result;
}

char *sstrndup(const char *str, size_t size) {
    char *result = strndup(str, size);
    if (result == nullptr)
        err(EXIT_FAILURE, "strndup()");
    return result;
}

int sasprintf(char **strp, const char *fmt, ...) {
    va_list args;
    int result;

    va_start(args, fmt);
    if ((result = vasprintf(strp, fmt, args)) == -1)
        err(EXIT_FAILURE, "asprintf(%s)", fmt);
    va_end(args);
    return result;
}
