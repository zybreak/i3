/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
#include "libi3.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <algorithm>

/*
 * Get the path of the first configuration file found. If override_configpath is
 * specified, that path is returned and saved for further calls. Otherwise,
 * checks the home directory first, then the system directory, always taking
 * into account the XDG Base Directory Specification ($XDG_CONFIG_HOME,
 * $XDG_CONFIG_DIRS).
 *
 */
std::string get_config_path(const std::string *override_configpath, bool use_system_paths) {
    std::string xdg_config_home;

    static const std::string *saved_configpath = nullptr;

    if (override_configpath != nullptr) {
        saved_configpath = override_configpath;
        return *saved_configpath;
    }

    if (saved_configpath != nullptr) {
        return *saved_configpath;
    }

    /* 1: check for $XDG_CONFIG_HOME/i3/config */
    if (getenv("XDG_CONFIG_HOME") == nullptr) {
        auto tilde = resolve_tilde("~/.config");
        xdg_config_home = tilde;
    } else {
        auto tilde = resolve_tilde(getenv("XDG_CONFIG_HOME"));
        xdg_config_home = tilde;
    }

    auto config_path = xdg_config_home + "/i3/config";

    if (std::filesystem::exists(config_path)) {
        return config_path;
    }

    /* 2: check the traditional path under the home directory */
    auto tilde = resolve_tilde("~/.i3/config");
    config_path = tilde;
    if (std::filesystem::exists(config_path)) {
        return config_path;
    }

    /* The below paths are considered system-level, and can be skipped if the
     * caller only wants user-level configs. */
    if (!use_system_paths) {
        return "";
    }

    std::string xdg_config_dirs;
    /* 3: check for $XDG_CONFIG_DIRS/i3/config */
    if (getenv("XDG_CONFIG_DIRS") == nullptr) {
        xdg_config_dirs = SYSCONFDIR "/xdg";
    } else {
        xdg_config_dirs = getenv("XDG_CONFIG_DIRS");
    }

    auto first = xdg_config_dirs.begin();
    while (true) {
        auto found = std::find(first, xdg_config_dirs.end(), ':');

        if (found == xdg_config_dirs.end()) {
            break;
        }

        std::string tok = resolve_tilde(std::string(first, found).c_str());
        tok += "/i3/config";

        if (std::filesystem::exists(tok)) {
            return tok;
        }

        first = ++found;
    }

    /* 4: check the traditional path under /etc */
    config_path = SYSCONFDIR "/i3/config";
    if (std::filesystem::exists(config_path)) {
        return config_path;
    }

    return "";
}
