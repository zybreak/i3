/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module utils;

import std;
import log;

/*
 * Emulates mkdir -p (creates any missing folders)
 *
 */
bool mkdirp(std::filesystem::path const &path, std::filesystem::perms mode) {
    std::error_code ec;
    if (!std::filesystem::create_directories(path, ec)) {
        ELOG(std::format("mkdir({}) failed: {}\n", path.native(), ec.message()));
        return false;
    }
    std::filesystem::permissions(path, mode);
    return true;
}
