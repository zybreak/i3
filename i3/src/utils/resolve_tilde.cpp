/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <err.h>
#include <glob.h>
#include <cstdlib>
#include <string>
#include <string_view>
#include <ranges>

module utils;

namespace utils {
    /*
     * This function resolves ~ in pathnames.
     * It may resolve wildcards in the first part of the path, but if no match
     * or multiple matches are found, it just returns a copy of path as given.
     *
     */
    std::string resolve_tilde(const std::string_view path) {
        static glob_t globbuf{};
        std::string result;

        auto tail = std::ranges::find(path, '/');

        std::string head{path.begin(), tail};

        int res = glob(head.c_str(), GLOB_TILDE, nullptr, &globbuf);

        /* no match, or many wildcard matches are bad */
        if (res == GLOB_NOMATCH || globbuf.gl_pathc != 1)
            result = path;
        else if (res != 0) {
            err(EXIT_FAILURE, "glob() failed");
        } else {
            result = globbuf.gl_pathv[0];
            if (tail != path.end()) {
                result += std::string(tail, path.end());
            }
        }
        globfree(&globbuf);

        return result;
    }
}
