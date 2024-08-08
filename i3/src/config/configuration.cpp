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

#include <err.h>

#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import std;
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
static std::optional<std::filesystem::path> get_config_path(const std::optional<std::filesystem::path> override_configpath, bool use_system_paths) {
    std::string xdg_config_home;

    static std::optional<std::filesystem::path> saved_configpath{};

    if (override_configpath) {
        saved_configpath = *override_configpath;
        return saved_configpath;
    }

    if (saved_configpath) {
        return saved_configpath;
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
        return std::nullopt;
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

    return std::nullopt;
}

void INIT_COLOR(Colortriple &x, const char *cborder, const char *cbackground, const char *ctext, const char *cindicator) {
    x.border = draw_util_hex_to_color(**global.x, global.x->root_screen, cborder);
    x.background = draw_util_hex_to_color(**global.x, global.x->root_screen, cbackground);
    x.text = draw_util_hex_to_color(**global.x, global.x->root_screen, ctext);
    x.indicator = draw_util_hex_to_color(**global.x, global.x->root_screen, cindicator);
    x.child_border = draw_util_hex_to_color(**global.x, global.x->root_screen, cbackground);
}

/*
 * Extracts workspace names from keybindings (e.g. “web” from “bindsym $mod+1
 * workspace web”), so that when an output needs a workspace, i3 can start with
 * the first configured one. Needs to be called before reorder_bindings() so
 * that the config-file order is used, not the i3-internal order.
 *
 */
static void extract_workspace_names_from_bindings(Config *config) {
    config->binding_workspace_names.clear();
    for (auto &bind : config->current_mode()->bindings) {
        DLOG(fmt::sprintf("binding with command %s\n",  bind.command));
        if (bind.command.length() < strlen("workspace ") ||
            strncasecmp(bind.command.c_str(), "workspace", strlen("workspace")) != 0) {
            continue;
        }
        DLOG(fmt::sprintf("relevant command = %s\n",  bind.command));
        auto target = bind.command.cbegin();
        std::advance(target, strlen("workspace "));
        while (*target == ' ' || *target == '\t') {
            target++;
        }
        /* We check if this is the workspace
         * next/prev/next_on_output/prev_on_output/back_and_forth command.
         * Beware: The workspace names "next", "prev", "next_on_output",
         * "prev_on_output", "back_and_forth" and "current" are OK,
         * so we check before stripping the double quotes */
        if (strncasecmp(std::to_address(target), "next", strlen("next")) == 0 ||
            strncasecmp(std::to_address(target), "prev", strlen("prev")) == 0 ||
            strncasecmp(std::to_address(target), "next_on_output", strlen("next_on_output")) == 0 ||
            strncasecmp(std::to_address(target), "prev_on_output", strlen("prev_on_output")) == 0 ||
            strncasecmp(std::to_address(target), "back_and_forth", strlen("back_and_forth")) == 0 ||
            strncasecmp(std::to_address(target), "current", strlen("current")) == 0) {
            continue;
        }
        if (strncasecmp(std::to_address(target), "--no-auto-back-and-forth", strlen("--no-auto-back-and-forth")) == 0) {
            std::advance(target, strlen("--no-auto-back-and-forth"));
            while (*target == ' ' || *target == '\t') {
                target++;
            }
        }
        if (strncasecmp(std::to_address(target), "number", strlen("number")) == 0) {
            std::advance(target, strlen("number"));
            while (*target == ' ' || *target == '\t') {
                target++;
            }
        }
        auto target_name = utils::parse_string(target, false);
        if (!target_name) {
            continue;
        }
        if (target_name->starts_with("__")) {
            LOG(fmt::sprintf("Cannot create workspace \"%s\". Names starting with __ are i3-internal.\n", *target_name));
            continue;
        }
        DLOG(fmt::sprintf("Saving workspace name \"%s\"\n", *target_name));

        config->binding_workspace_names.push_back(*target_name);
    }
}


static void free_configuration() {
    /* First ungrab the keys */
    ungrab_all_keys(*global.x);

    global.assignmentManager->clear();
    global.workspaceManager->clear();

    for (const auto &con : global.all_cons) {
        /* Assignments changed, previously ran assignments are invalid. */
        if (con->get_window()) {
            con->get_window()->ran_assignments.clear();
        }
        /* Invalidate pixmap caches in case font or colors changed. */
        con->deco_render_params.reset();
    }
}

static void config_reload() {
    translate_keysyms(&global.keymap.value());
    grab_all_keys(*global.x);
    regrab_all_buttons(*global.x);
    gaps_reapply_workspace_assignments();

    /* Redraw the currently visible decorations on reload, so that the
     * possibly new drawing parameters changed. */
    tree_render();
}

void ConfigurationManager::set_config(std::unique_ptr<Config> _config) {
    bool reload = config ? true : false;
    if (reload) {
        free_configuration();
    }
    
    config = std::move(_config);

    if (!config->font) {
        ELOG("You did not specify required configuration option \"font\"\n");
        using namespace std::literals;
        config->font = load_font(**global.x, global.x->root_screen, "fixed"s);
    }
    
    // TODO: decide if we should call set_mode or no?

    if (reload) {
        config_reload();
    }
}

Config::Config() {
    auto config = this;
    Mode default_mode{"default"};
    config->modes.insert_or_assign("default", default_mode);

    /* Initialize default colors */
    config->client.background = draw_util_hex_to_color(**global.x, global.x->root_screen, "#000000");
    INIT_COLOR(config->client.focused, "#4c7899", "#285577", "#ffffff", "#2e9ef4");
    INIT_COLOR(config->client.focused_inactive, "#333333", "#5f676a", "#ffffff", "#484e50");
    INIT_COLOR(config->client.unfocused, "#333333", "#222222", "#888888", "#292d2e");
    INIT_COLOR(config->client.urgent, "#2f343a", "#900000", "#ffffff", "#900000");
    config->client.got_focused_tab_title = false;

    /* border and indicator color are ignored for placeholder contents */
    INIT_COLOR(config->client.placeholder, "#000000", "#0c0c0c", "#ffffff", "#000000");

    /* the last argument (indicator color) is ignored for bar colors */
    INIT_COLOR(config->bar.focused, "#4c7899", "#285577", "#ffffff", "#000000");
    INIT_COLOR(config->bar.unfocused, "#333333", "#222222", "#888888", "#000000");
    INIT_COLOR(config->bar.urgent, "#2f343a", "#900000", "#ffffff", "#000000");

    config->default_border_width = logical_px(global.x->root_screen, 2);
    config->default_floating_border_width = logical_px(global.x->root_screen, 2);
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
std::unique_ptr<Config> load_configuration(const std::optional<std::filesystem::path> override_configpath) {
    
    auto config = std::make_unique<Config>();

    auto config_path = get_config_path(override_configpath, true);
    if (config_path) {
        config->current_configpath = *config_path;
    } else {
        throw std::runtime_error("Unable to find the configuration file (looked at $XDG_CONFIG_HOME/i3/config, ~/.i3/config, $XDG_CONFIG_DIRS/i3/config and /usr/local/etc/i3/config)");
    }

    std::filesystem::path resolved_path;
    try {
        resolved_path = std::filesystem::canonical(config->current_configpath);
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error(std::format("realpath({}): {}", config->current_configpath.native(), e.what()));
    }

    LOG(fmt::sprintf("Parsing configfile %s\n",  resolved_path.native()));

    ResourceDatabase resourceDatabase{*global.x->conn};
    try {
        ConfigApplier configApplier{config.get()};
        std::ifstream stream{resolved_path};

        if (global.new_parser) {
            NewParser np{ resolved_path, stream, resourceDatabase, configApplier };
            np.parse_file();
        } else {
            OldParser op{ resolved_path, stream, resourceDatabase, configApplier };
            op.parse_file();
            config->included_files = op.included_files;
        }
        
        if (has_duplicate_bindings(config.get())) {
            throw std::domain_error(std::format("Duplicate bindings in configuration file: {}", strerror((*__errno_location()))));
        }
    } catch (const std::domain_error &e) {
        throw e;
    }

    extract_workspace_names_from_bindings(config.get());
    reorder_bindings(config.get());

    return config;
}


