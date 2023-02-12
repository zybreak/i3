/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
#include "libi3.h"

#include <cstring>

static bool CS_STARTS_WITH(const char *string, const char *needle) {
    return strncmp(string, needle, strlen(needle)) == 0;
}

/*
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
std::string format_placeholders(const std::string &format, placeholder_t *placeholders, int num) {
    if (format.empty())
        return "";

    /* We have to first iterate over the string to see how much buffer space
     * we need to allocate. */
    unsigned long buffer_len = format.length() + 1;
    for (const char *walk = format.c_str(); *walk != '\0'; walk++) {
        for (int i = 0; i < num; i++) {
            if (!CS_STARTS_WITH(walk, placeholders[i].name))
                continue;

            buffer_len = buffer_len - strlen(placeholders[i].name) + strlen(placeholders[i].value);
            walk += strlen(placeholders[i].name) - 1;
            break;
        }
    }

    /* Now we can parse the format string. */
    char buffer[buffer_len];
    char *outwalk = buffer;
    for (const char *walk = format.c_str(); *walk != '\0'; walk++) {
        if (*walk != '%') {
            *(outwalk++) = *walk;
            continue;
        }

        bool matched = false;
        for (int i = 0; i < num; i++) {
            if (!CS_STARTS_WITH(walk, placeholders[i].name)) {
                continue;
            }

            matched = true;
            outwalk += sprintf(outwalk, "%s", placeholders[i].value);
            walk += strlen(placeholders[i].name) - 1;
            break;
        }

        if (!matched)
            *(outwalk++) = *walk;
    }

    *outwalk = '\0';
    return buffer;
}
