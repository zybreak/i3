/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module i3;

import std;

static bool CS_STARTS_WITH(const char *string, const char *needle) {
    return strncmp(string, needle, strlen(needle)) == 0;
}

static unsigned long get_buffer_len(const std::string &format, std::vector<placeholder_t> &placeholders) {
    unsigned long buffer_len = format.length() + 1;
    for (const char *walk = format.c_str(); *walk != '\0'; walk++) {
        for (auto &placeholder : placeholders) {
            if (!CS_STARTS_WITH(walk, placeholder.name.c_str()))
                continue;

            buffer_len = buffer_len - placeholder.name.size() + placeholder.value.size();
            walk += placeholder.name.size() - 1;
            break;
        }
    }

    return buffer_len;
}

/*
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
std::string format_placeholders(const std::string &format, std::vector<placeholder_t> &placeholders) {
    if (format.empty())
        return "";

    /* We have to first iterate over the string to see how much buffer space
     * we need to allocate. */
    auto buffer_len = get_buffer_len(format, placeholders);

    /* Now we can parse the format string. */
    char buffer[buffer_len];
    char *outwalk = buffer;
    for (const char *walk = format.c_str(); *walk != '\0'; walk++) {
        if (*walk != '%') {
            *(outwalk++) = *walk;
            continue;
        }

        bool matched = false;
        for (auto &placeholder : placeholders) {
            if (!CS_STARTS_WITH(walk, placeholder.name.c_str())) {
                continue;
            }

            matched = true;
            outwalk += sprintf(outwalk, "%s", placeholder.value.c_str());
            walk += placeholder.name.size() - 1;
            break;
        }

        if (!matched)
            *(outwalk++) = *walk;
    }

    *outwalk = '\0';
    return buffer;
}
