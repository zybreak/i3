/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module utils;

import std;

namespace utils {

    /*
     * Parses a string (or word, if as_word is true). Extracted out of
     * parse_command so that it can be used in src/workspace.c for interpreting
     * workspace commands.
     *
     */
    std::optional<std::string> parse_string(std::string::const_iterator &walk, bool as_word) {
        std::string::const_iterator beginning = walk;
        /* Handle quoted strings (or words). */
        if (*walk == '"') {
            beginning++;
            walk++;
            for (; *walk != '\0' && *walk != '"'; walk++) {
                if (*walk == '\\' && *(walk + 1) != '\0') {
                    walk++;
                }
            }
        } else {
            if (!as_word) {
                /* For a string (starting with 's'), the delimiters are
                     * comma (,) and semicolon (;) which introduce a new
                     * operation or command, respectively. Also, newlines
                     * end a command. */
                while (*walk != ';' && *walk != ',' &&
                       *walk != '\0' && *walk != '\r' &&
                       *walk != '\n') {
                    walk++;
                }
            } else {
                /* For a word, the delimiters are white space (' ' or
                     * '\t'), closing square bracket (]), comma (,) and
                     * semicolon (;). */
                while (*walk != ' ' && *walk != '\t' &&
                       *walk != ']' && *walk != ',' &&
                       *walk != ';' && *walk != '\r' &&
                       *walk != '\n' && *walk != '\0') {
                    walk++;
                }
            }
        }
        if (walk == beginning) {
            return std::nullopt;
        }
       
        std::string str{};
        //str.allocate(std::distance(beginning, walk));
        /* We copy manually to handle escaping of characters. */
        for (std::string::const_iterator inpos = beginning;
             inpos < walk;
             inpos++) {
            /* We only handle escaped double quotes and backslashes to not break
                 * backwards compatibility with people using \w in regular expressions
                 * etc. */
            if (*inpos == '\\' && (*(inpos + 1) == '"' || *(inpos + 1) == '\\')) {
                inpos++;
            }
            str.push_back(*inpos);
        }

        return str;
    }

}
