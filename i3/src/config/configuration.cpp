/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config.c: Configuration file (calling the parser (src/config_parser.c) with
 *           the correct path, switching key bindings mode).
 *
 */
module;
#include <config.h>

#include <vector>
#include <fstream>
#include <cassert>
#include <err.h>
#include <cerrno>
#include <climits>

#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>
#include <filesystem>
#include <fmt/printf.h>
#include <iostream>
module i3;

import utils;
import i3_config_base;
import i3_config_new;
import i3_config_old;
import log;

using namespace std::literals;
using namespace std::literals;

/*
 * Get the path of the first configuration file found. If override_configpath is
 * specified, that path is returned and saved for further calls. Otherwise,
 * checks the home directory first, then the system directory, always taking
 * into account the XDG Base Directory Specification ($XDG_CONFIG_HOME,
 * $XDG_CONFIG_DIRS).
 *
 */
static std::string get_config_path(const std::string *override_configpath, bool use_system_paths) {
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
        auto tilde = utils::resolve_tilde("~/.config");
        xdg_config_home = tilde;
    } else {
        auto tilde = utils::resolve_tilde(getenv("XDG_CONFIG_HOME"));
        xdg_config_home = tilde;
    }

    auto config_path = xdg_config_home + "/i3/config";

    if (std::filesystem::exists(config_path)) {
        return config_path;
    }

    /* 2: check the traditional path under the home directory */
    auto tilde = utils::resolve_tilde("~/.i3/config");
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

        std::string tok = utils::resolve_tilde(std::string(first, found));
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

static void free_configuration() {
    assert(*global.x != nullptr);

    /* If we are currently in a binding mode, we first revert to the default
     * since we have no guarantee that the current mode will even still exist
     * after parsing the config again. See #2228. */
    switch_mode("default");

    /* First ungrab the keys */
    ungrab_all_keys(*global.x);

    modes.clear();
    global.assignments.clear();
    global.ws_assignments.clear();
    barconfigs.clear();

    for (const auto &con : global.all_cons) {
        /* Assignments changed, previously ran assignments are invalid. */
        if (con->window) {
            con->window->ran_assignments.clear();
        }
        /* Invalidate pixmap caches in case font or colors changed. */
        delete con->deco_render_params;
        con->deco_render_params = nullptr;
    }

    /* Get rid of the current font */
    if (config.font != nullptr) {
        delete config.font;
        config.font = nullptr;
    }

    free(config.ipc_socket_path);
    free(config.restart_state_path);
    included_files.clear();
}

Barconfig::~Barconfig() {
    Barconfig *barconfig = this;

    barconfig->outputs.clear();
    barconfig->bar_bindings.clear();
    barconfig->tray_outputs.clear();

    free(barconfig->id);
    free(barconfig->socket_path);
    free(barconfig->status_command);
    free(barconfig->i3bar_command);
    free(barconfig->font);
    free(barconfig->colors.background);
    free(barconfig->colors.statusline);
    free(barconfig->colors.separator);
    free(barconfig->colors.focused_background);
    free(barconfig->colors.focused_statusline);
    free(barconfig->colors.focused_separator);
    free(barconfig->colors.focused_workspace_border);
    free(barconfig->colors.focused_workspace_bg);
    free(barconfig->colors.focused_workspace_text);
    free(barconfig->colors.active_workspace_border);
    free(barconfig->colors.active_workspace_bg);
    free(barconfig->colors.active_workspace_text);
    free(barconfig->colors.inactive_workspace_border);
    free(barconfig->colors.inactive_workspace_bg);
    free(barconfig->colors.inactive_workspace_text);
    free(barconfig->colors.urgent_workspace_border);
    free(barconfig->colors.urgent_workspace_bg);
    free(barconfig->colors.urgent_workspace_text);
    free(barconfig->colors.binding_mode_border);
    free(barconfig->colors.binding_mode_bg);
    free(barconfig->colors.binding_mode_text);
}

void INIT_COLOR(Colortriple &x, const char *cborder, const char *cbackground, const char *ctext, const char *cindicator) {
    x.border = draw_util_hex_to_color(**global.x, global.x->root_screen, cborder);
    x.background = draw_util_hex_to_color(**global.x, global.x->root_screen, cbackground);
    x.text = draw_util_hex_to_color(**global.x, global.x->root_screen, ctext);
    x.indicator = draw_util_hex_to_color(**global.x, global.x->root_screen, cindicator);
    x.child_border = draw_util_hex_to_color(**global.x, global.x->root_screen, cbackground);
}

/**
 * Launch nagbar to indicate errors in the configuration file.
 */
static void start_config_error_nagbar(bool has_errors) {
    std::string font_pattern = config.font->pattern;
    auto type = has_errors ? bar_type_t::TYPE_ERROR : bar_type_t::TYPE_WARNING;
    std::string text = has_errors ? "You have an error in your i3 config file!" : "Your config is outdated. Please fix the warnings to make sure everything works.";

    std::vector<button_t> buttons{};
    start_nagbar(&global.config_error_nagbar_pid, buttons, text, font_pattern, type, false);
}

/*
 * (Re-)loads the configuration file (sets useful defaults before).
 *
 * If you specify override_configpath, only this path is used to look for a
 * configuration file.
 *
 * load_type specifies the type of loading: C_VALIDATE is used to only verify
 * the correctness of the config file (used with the flag -C). C_LOAD will load
 * the config for normal use and display errors in the nagbar. C_RELOAD will
 * also clear the previous config.
 *
 */
bool load_configuration(const std::string *override_configpath, config_load_t load_type) {
    if (load_type == config_load_t::C_RELOAD) {
        free_configuration();
    }

    auto default_mode = std::make_unique<Mode>("default");
    current_mode = default_mode.get();
    modes.push_back(std::move(default_mode));


    /* Clear the old config or initialize the data structure */
    memset(&config, 0, sizeof(config));

    /* Initialize default colors */
    config.client.background = draw_util_hex_to_color(**global.x, global.x->root_screen, "#000000");
    INIT_COLOR(config.client.focused, "#4c7899", "#285577", "#ffffff", "#2e9ef4");
    INIT_COLOR(config.client.focused_inactive, "#333333", "#5f676a", "#ffffff", "#484e50");
    INIT_COLOR(config.client.unfocused, "#333333", "#222222", "#888888", "#292d2e");
    INIT_COLOR(config.client.urgent, "#2f343a", "#900000", "#ffffff", "#900000");
    config.client.got_focused_tab_title = false;

    /* border and indicator color are ignored for placeholder contents */
    INIT_COLOR(config.client.placeholder, "#000000", "#0c0c0c", "#ffffff", "#000000");

    /* the last argument (indicator color) is ignored for bar colors */
    INIT_COLOR(config.bar.focused, "#4c7899", "#285577", "#ffffff", "#000000");
    INIT_COLOR(config.bar.unfocused, "#333333", "#222222", "#888888", "#000000");
    INIT_COLOR(config.bar.urgent, "#2f343a", "#900000", "#ffffff", "#000000");

    config.default_border = BS_NORMAL;
    config.default_floating_border = BS_NORMAL;
    config.default_border_width = logical_px(global.x->root_screen, 2);
    config.default_floating_border_width = logical_px(global.x->root_screen, 2);
    /* Set default_orientation to NO_ORIENTATION for auto orientation. */
    config.default_orientation = NO_ORIENTATION;

    config.gaps.inner = 0;
    config.gaps.top = 0;
    config.gaps.right = 0;
    config.gaps.bottom = 0;
    config.gaps.left = 0;

    /* Set default urgency reset delay to 500ms */
    if (config.workspace_urgency_timer == 0) {
        config.workspace_urgency_timer = 0.5;
    }

    config.focus_wrapping = FOCUS_WRAPPING_ON;

    config.tiling_drag = TILING_DRAG_MODIFIER;

    current_configpath = get_config_path(override_configpath, true);
    if (current_configpath.empty()) {
        errx(EXIT_FAILURE,
             "Unable to find the configuration file (looked at $XDG_CONFIG_HOME/i3/config, ~/.i3/config, $XDG_CONFIG_DIRS/i3/config and /usr/local/etc/i3/config)");
    }

    included_files.clear();

    char resolved_path[PATH_MAX] = {'\0'};
    if (realpath(current_configpath.c_str(), resolved_path) == nullptr) {
        errx(EXIT_FAILURE, "realpath(%s): %s", current_configpath.c_str(), strerror((*__errno_location())));
    }

    LOG(fmt::sprintf("Parsing configfile %s\n",  resolved_path));

    ResourceDatabase resourceDatabase{*global.x->conn};
    try {
        ConfigApplier configListener{};
        std::ifstream stream{resolved_path};

        if (global.new_parser) {
            NewParser np{ stream, resourceDatabase, load_type, configListener };
            np.parse_file();
        } else {
            OldParser op{ resolved_path, stream, resourceDatabase, load_type, configListener };
            op.parse_file();
            for (auto &included_file : op.included_files) {
                included_files.push_back(std::move(included_file));
            }
        }
        if (has_duplicate_bindings()) {
            errx(EXIT_FAILURE, "Duplicate bindings in configuration file: %s\n", strerror((*__errno_location())));
        }
    } catch (const std::domain_error &e) {
        auto use_nagbar = (load_type != config_load_t::C_VALIDATE);

        if (use_nagbar) {
            ELOG(fmt::sprintf("FYI: You are using i3 version %s\n", I3_VERSION));

            start_config_error_nagbar(true);
        }
    } catch (const std::exception &e) {
        errx(EXIT_FAILURE, "Error parsing configuration file: %s\n", e.what());
    }

    extract_workspace_names_from_bindings();
    reorder_bindings();

    if (config.font == nullptr && load_type != config_load_t::C_VALIDATE) {
        ELOG("You did not specify required configuration option \"font\"\n");
        config.font = load_font(**global.x, global.x->root_screen, "fixed", true);
    }

    /* Make bar config blocks without a configured font use the i3-wide font. */
    if (load_type != config_load_t::C_VALIDATE) {
        for (auto &current : barconfigs) {
            if (current->font != nullptr) {
                continue;
            }
            current->font = sstrdup(config.font->pattern.c_str());
        }
    }

    if (load_type == config_load_t::C_RELOAD) {
        translate_keysyms();
        grab_all_keys(*global.x);
        regrab_all_buttons(*global.x);
        gaps_reapply_workspace_assignments();

        /* Redraw the currently visible decorations on reload, so that the
         * possibly new drawing parameters changed. */
        tree_render();
    }

    return true;
}


