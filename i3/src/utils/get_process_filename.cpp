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
#include <filesystem>

module utils;

/*
 * Returns the name of a temporary file with the specified prefix.
 *
 */
char *get_process_filename(const char *prefix) {
    /* dir stores the directory path for this and all subsequent calls so that
     * we only create a temporary directory once per i3 instance. */
    static char *dir = nullptr;
    if (dir == nullptr) {
        /* Check if XDG_RUNTIME_DIR is set. If so, we use XDG_RUNTIME_DIR/i3 */
        if ((dir = getenv("XDG_RUNTIME_DIR"))) {
            char *tmp;
            sasprintf(&tmp, "%s/i3", dir);
            dir = tmp;
            /* mkdirp() should prevent race between multiple i3 instances started
             * in parallel from causing problem */
            using std::filesystem::perms;
            if (mkdirp(dir, perms::owner_all) == -1) {
                warn("Could not mkdirp(%s)", dir);
                errx(EXIT_FAILURE, "Check permissions of $XDG_RUNTIME_DIR = '%s'",
                     getenv("XDG_RUNTIME_DIR"));
                perror("mkdirp()");
                return nullptr;
            }
        } else {
            /* If not, we create a (secure) temp directory using the template
             * /tmp/i3-<user>.XXXXXX */
            struct passwd *pw = getpwuid(getuid());
            const char *username = pw ? pw->pw_name : "unknown";
            sasprintf(&dir, "/tmp/i3-%s.XXXXXX", username);
            /* mkdtemp modifies dir */
            if (mkdtemp(dir) == nullptr) {
                perror("mkdtemp()");
                return nullptr;
            }
        }
    }
    char *filename;
    sasprintf(&filename, "%s/%s.%d", dir, prefix, getpid());
    return filename;
}
