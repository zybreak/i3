/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <err.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cstdarg>
#include <cstdio>
module utils;

/*
 * The s* functions (safe) are wrappers around malloc, strdup, …, which exits if one of
 * the called functions returns NULL, meaning that there is no more memory available
 *
 */
void *srealloc(void *ptr, size_t size) {
    void *result = realloc(ptr, size);
    if (result == nullptr && size > 0) {
        err(EXIT_FAILURE, "realloc(%zd)", size);
    }
    return result;
}

char *sstrdup(char const *str) {
    char *result = strdup(str);
    if (result == nullptr) {
        err(EXIT_FAILURE, "strdup()");
    }
    return result;
}

char *sstrndup(char const *str, size_t size) {
    char *result = strndup(str, size);
    if (result == nullptr) {
        err(EXIT_FAILURE, "strndup()");
    }
    return result;
}

int sasprintf(char **strp, char const *fmt, ...) {
    va_list args;
    int result;

    va_start(args, fmt);
    if ((result = vasprintf(strp, fmt, args)) == -1) {
        err(EXIT_FAILURE, "asprintf(%s)", fmt);
    }
    va_end(args);
    return result;
}
