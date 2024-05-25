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

/*
 * Returns the name of a temporary file with the specified prefix.
 *
 */
std::optional<std::string> get_process_filename(const char *prefix) {
    /* dir stores the directory path for this and all subsequent calls so that
     * we only create a temporary directory once per i3 instance. */
    static std::optional<std::string> dir = std::nullopt;
    if (!dir) {
        /* Check if XDG_RUNTIME_DIR is set. If so, we use XDG_RUNTIME_DIR/i3 */
        if (char *runtime_dir_env = getenv("XDG_RUNTIME_DIR")) {
            dir = std::format("{}/i3", runtime_dir_env);
            /* mkdirp() should prevent race between multiple i3 instances started
             * in parallel from causing problem */
            using std::filesystem::perms;
            if (mkdirp(dir->c_str(), perms::owner_all) == -1) {
                warn("Could not mkdirp(%s)", dir->c_str());
                errx(EXIT_FAILURE, "Check permissions of $XDG_RUNTIME_DIR = '%s'",
                     getenv("XDG_RUNTIME_DIR"));
                perror("mkdirp()");
                return std::nullopt;
            }
        } else {
            /* If not, we create a (secure) temp directory using the template
             * /tmp/i3-<user>.XXXXXX */
            struct passwd *pw = getpwuid(getuid());
            const char *username = pw ? pw->pw_name : "unknown";
            dir = std::format("/tmp/i3-{}.XXXXXX", username);
            /* mkdtemp modifies dir */
            if (mkdtemp(dir->data()) == nullptr) {
                perror("mkdtemp()");
                return std::nullopt;
            }
        }
    }
    return std::format("{}/{}.{}", *dir, prefix, getpid());
}
