/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <err.h>
#include <pwd.h>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
module utils;

import std;
import log;

/*
 * Returns the name of a temporary file with the specified prefix.
 *
 */
std::optional<std::string> get_process_filename(const char *prefix) {
    /* dir stores the directory path for this and all subsequent calls so that
     * we only create a temporary directory once per i3 instance. */
    static std::optional<std::string> dir{};
    if (!dir) {
        /* Check if XDG_RUNTIME_DIR is set. If so, we use XDG_RUNTIME_DIR/i3 */
        if (char *runtime_dir_env = getenv("XDG_RUNTIME_DIR"); runtime_dir_env != nullptr) {
            std::string new_dir = std::format("{}/i3", runtime_dir_env);
            /* mkdirp() should prevent race between multiple i3 instances started
             * in parallel from causing problem */
            using std::filesystem::perms;
            if (mkdirp(new_dir, perms::owner_all)) {
                dir = new_dir;
            } else {
                ELOG(std::format("Check permissions of $XDG_RUNTIME_DIR = '{}'",
                     getenv("XDG_RUNTIME_DIR")));
            }
        }
        
        /* If not, we create a (secure) temp directory using the template
         * /tmp/i3-<user>.XXXXXX */
        struct passwd *pw = getpwuid(getuid());
        const char *username = pw ? pw->pw_name : "unknown";
        std::string new_dir = std::format("/tmp/i3-{}.XXXXXX", username);
        /* mkdtemp modifies dir */
        if (mkdtemp(new_dir.data()) != nullptr) {
            dir = new_dir;
        } else {
            perror("mkdtemp()");
            return std::nullopt;
        }
    }
    return std::format("{}/{}.{}", *dir, prefix, getpid());
}
