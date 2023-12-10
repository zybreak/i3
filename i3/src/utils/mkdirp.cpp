/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <fmt/printf.h>
#include <cstdlib>
#include <filesystem>

module utils;

/*
 * Emulates mkdir -p (creates any missing folders)
 *
 */
int mkdirp(const char *path, mode_t mode) {

    std::error_code ec;
    if (!std::filesystem::create_directories(path, ec)) {
        //ELOG(fmt::sprintf("mkdir(%s) failed: %s\n",  path, ec.message()));
        return 1;
    }
    return 0;
}
