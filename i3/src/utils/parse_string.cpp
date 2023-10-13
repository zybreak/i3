/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <err.h>
#include <cerrno>
#include <cstdlib>
module utils;

static void *scalloc(size_t num, size_t size) {
    void *result = calloc(num, size);
    if (result == nullptr)
        err(EXIT_FAILURE, "calloc(%zd, %zd)", num, size);
    return result;
}

namespace utils {
    /*
     * Parses a string (or word, if as_word is true). Extracted out of
     * parse_command so that it can be used in src/workspace.c for interpreting
     * workspace commands.
     *
     */
    char *parse_string(const char **walk, bool as_word) {
        const char *beginning = *walk;
        /* Handle quoted strings (or words). */
        if (**walk == '"') {
            beginning++;
            (*walk)++;
            for (; **walk != '\0' && **walk != '"'; (*walk)++)
                if (**walk == '\\' && *(*walk + 1) != '\0')
                    (*walk)++;
        } else {
            if (!as_word) {
                /* For a string (starting with 's'), the delimiters are
                 * comma (,) and semicolon (;) which introduce a new
                 * operation or command, respectively. Also, newlines
                 * end a command. */
                while (**walk != ';' && **walk != ',' &&
                       **walk != '\0' && **walk != '\r' &&
                       **walk != '\n')
                    (*walk)++;
            } else {
                /* For a word, the delimiters are white space (' ' or
                 * '\t'), closing square bracket (]), comma (,) and
                 * semicolon (;). */
                while (**walk != ' ' && **walk != '\t' &&
                       **walk != ']' && **walk != ',' &&
                       **walk != ';' && **walk != '\r' &&
                       **walk != '\n' && **walk != '\0')
                    (*walk)++;
            }
        }
        if (*walk == beginning)
            return nullptr;

        char *str = (char*)scalloc(*walk - beginning + 1, 1);
        /* We copy manually to handle escaping of characters. */
        int inpos, outpos;
        for (inpos = 0, outpos = 0;
             inpos < (*walk - beginning);
             inpos++, outpos++) {
            /* We only handle escaped double quotes and backslashes to not break
             * backwards compatibility with people using \w in regular expressions
             * etc. */
            if (beginning[inpos] == '\\' && (beginning[inpos + 1] == '"' || beginning[inpos + 1] == '\\'))
                inpos++;
            str[outpos] = beginning[inpos];
        }

        return str;
    }

}
