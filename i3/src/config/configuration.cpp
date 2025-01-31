/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
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
import bindings;

using namespace std::literals;

/*
 * Get the path of the first configuration file found. If override_configpath is
 * specified, that path is returned and saved for further calls. Otherwise,
 * checks the home directory first, then the system directory, always taking
 * into account the XDG Base Directory Specification ($XDG_CONFIG_HOME,
 * $XDG_CONFIG_DIRS).
 *
 */
static std::optional<std::filesystem::path> get_config_path(std::optional<std::filesystem::path> const override_configpath, bool use_system_paths) {
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

void INIT_COLOR(X &x, Colortriple &color, char const *cborder, char const *cbackground, char const *ctext, char const *cindicator) {
    color.border = draw_util_hex_to_color(*x, x.root_screen, cborder);
    color.background = draw_util_hex_to_color(*x, x.root_screen, cbackground);
    color.text = draw_util_hex_to_color(*x, x.root_screen, ctext);
    color.indicator = draw_util_hex_to_color(*x, x.root_screen, cindicator);
    color.child_border = draw_util_hex_to_color(*x, x.root_screen, cbackground);
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
        DLOG(fmt::sprintf("binding with command %s\n", bind.command));
        if (bind.command.length() < strlen("workspace ") ||
            strncasecmp(bind.command.c_str(), "workspace", strlen("workspace")) != 0) {
            continue;
        }
        DLOG(fmt::sprintf("relevant command = %s\n", bind.command));
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

    for (auto const &con : global.all_cons) {
        /* Assignments changed, previously ran assignments are invalid. */
        if (con->get_window()) {
            con->get_window()->ran_assignments.clear();
        }
        /* Invalidate pixmap caches in case font or colors changed. */
        con->deco_render_params.reset();
    }
}

static void config_reload() {
    translate_keysyms(global.xkb->keymap());
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
        config->font = load_font(*x, x.root_screen, "fixed"s);
    }

    // TODO: decide if we should call set_mode or no?

    if (reload) {
        config_reload();
    }
}

Config::Config() {
    auto *config = this;
    Mode default_mode{"default"};
    config->modes.insert_or_assign("default", default_mode);
}

void ConfigurationManager::init_color(Config &config) {
    /* Initialize default colors */
    config.client.background = draw_util_hex_to_color(*x, x.root_screen, "#000000");
    INIT_COLOR(x, config.client.focused, "#4c7899", "#285577", "#ffffff", "#2e9ef4");
    INIT_COLOR(x, config.client.focused_inactive, "#333333", "#5f676a", "#ffffff", "#484e50");
    INIT_COLOR(x, config.client.unfocused, "#333333", "#222222", "#888888", "#292d2e");
    INIT_COLOR(x, config.client.urgent, "#2f343a", "#900000", "#ffffff", "#900000");
    config.client.got_focused_tab_title = false;

    /* border and indicator color are ignored for placeholder contents */
    INIT_COLOR(x, config.client.placeholder, "#000000", "#0c0c0c", "#ffffff", "#000000");

    /* the last argument (indicator color) is ignored for bar colors */
    INIT_COLOR(x, config.bar.focused, "#4c7899", "#285577", "#ffffff", "#000000");
    INIT_COLOR(x, config.bar.unfocused, "#333333", "#222222", "#888888", "#000000");
    INIT_COLOR(x, config.bar.urgent, "#2f343a", "#900000", "#ffffff", "#000000");

    config.default_border_width = logical_px(x.root_screen, 2);
    config.default_floating_border_width = logical_px(x.root_screen, 2);
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
std::unique_ptr<Config> ConfigurationManager::load_configuration(std::optional<std::filesystem::path> const override_configpath) {
    auto config = std::make_unique<Config>();
    init_color(*config);

    auto config_path = get_config_path(override_configpath, true);
    if (config_path) {
        config->current_configpath = *config_path;
    } else {
        throw std::runtime_error("Unable to find the configuration file (looked at $XDG_CONFIG_HOME/i3/config, ~/.i3/config, $XDG_CONFIG_DIRS/i3/config and /usr/local/etc/i3/config)");
    }

    std::filesystem::path resolved_path;
    try {
        resolved_path = std::filesystem::canonical(config->current_configpath);
    } catch (std::filesystem::filesystem_error const &e) {
        throw std::runtime_error(std::format("realpath({}): {}", config->current_configpath.native(), e.what()));
    }

    LOG(fmt::sprintf("Parsing configfile %s\n", resolved_path.native()));

    ResourceDatabase resourceDatabase{*x};
    try {
        ConfigApplier configApplier{config.get()};
        std::ifstream stream{resolved_path};

        if (global.new_parser) {
            NewParser np{resolved_path, stream, resourceDatabase, configApplier};
            np.parse_file();
        } else {
            OldParser op{resolved_path, stream, resourceDatabase, configApplier};
            op.parse_file();
            config->included_files = op.included_files;
        }

        if (has_duplicate_bindings(config.get())) {
            throw std::domain_error(std::format("Duplicate bindings in configuration file: {}", strerror((*__errno_location()))));
        }
    } catch (std::domain_error const &e) {
        throw;
    }

    extract_workspace_names_from_bindings(config.get());
    reorder_bindings(config.get());

    return config;
}

/*
 * Reorders bindings by event_state_mask descendingly so that get_binding()
 * correctly matches more specific bindings before more generic bindings. Take
 * the following binding configuration as an example:
 *
 *   bindsym n nop lower-case n pressed
 *   bindsym Shift+n nop upper-case n pressed
 *
 * Without reordering, the first binding’s event_state_mask of 0x0 would match
 * the actual event_stat_mask of 0x1 and hence trigger instead of the second
 * keybinding.
 *
 */
void reorder_bindings(Config *config) {
    std::ranges::for_each(config->modes, [](auto &mode) {
        std::ranges::sort(mode.second.bindings, [](auto const &first, auto const &second) {
            if (first.event_state_mask < second.event_state_mask) {
                return 1;
            } else if (first.event_state_mask == second.event_state_mask) {
                return 0;
            } else {
                return -1;
            }
        });
    });
}

/*
 * Returns true if a is a key binding for the same key as b.
 *
 */
static bool binding_same_key(Binding *a, Binding *b) {
    /* Check if the input types are different */
    if (a->input_type != b->input_type) {
        return false;
    }
    
    /* Check if one is using keysym while the other is using bindsym. */
    if ((a->symbol.empty() && !b->symbol.empty()) ||
        (!a->symbol.empty() && b->symbol.empty())) {
        return false;
    }

    /* If a is NULL, b has to be NULL, too (see previous conditional).
     * If the keycodes differ, it can't be a duplicate. */
    if (!a->symbol.empty() && a->symbol != b->symbol) {
        return false;
    }

    /* Check if the keycodes or modifiers are different. If so, they
     * can't be duplicate */
    if (a->keycode != b->keycode ||
        a->event_state_mask != b->event_state_mask ||
        a->release != b->release) {
        return false;
    }

    return true;
}

/*
 * Checks for duplicate key bindings (the same keycode or keysym is configured
 * more than once). If a duplicate binding is found, a message is printed to
 * stderr and the has_errors variable is set to true, which will start
 * i3-nagbar.
 *
 */
bool has_duplicate_bindings(Config *config) {
    bool has_errors = false;
    for (auto &current : config->current_mode()->bindings) {
        for (auto &bind : config->current_mode()->bindings) {
            /* Abort when we reach the current keybinding, only check the
             * bindings before */
            if (bind == current) {
                break;
            }
            
            if (!binding_same_key(&bind, &current)) {
                continue;
            }

            has_errors = true;
            if (current.keycode != 0) {
                ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keycode %d, command \"%s\"\n",
                                  current.event_state_mask, current.keycode, current.command));
            } else {
                ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keysym %s, command \"%s\"\n",
                                  current.event_state_mask, current.symbol, current.command));
            }
        }
    }

    return has_errors;
}

/*
 * Returns the mode specified by `name` or creates a new mode and adds it to
 * the list of modes.
 *
 */
static void mode_from_name(Config *config, std::string const &name, bool pango_markup, Binding &binding) {
    /* Try to find the mode in the list of modes and return it */
    auto mode_it = config->modes.find(name);
    
    if (mode_it != config->modes.end()) {
        mode_it->second.bindings.push_back(binding);
        return;
    }

    /* If the mode was not found, create a new one */
    Mode mode{name, pango_markup};
    mode.bindings.push_back(binding);

    config->modes.insert_or_assign(name, mode);
}

/*
 * Adds a binding from config parameters given as strings and returns a
 * pointer to the binding structure. Returns NULL if the input code could not
 * be parsed.
 *
 */
void configure_binding(Config *config, std::string const &bindtype, std::string const &modifiers, std::string const &input_code,
                       bool release, bool border, bool whole_window,
                       bool exclude_titlebar, std::string const &command, std::string const &modename,
                       bool pango_markup) {
    DLOG(fmt::sprintf("Binding bindtype %s, modifiers %s, input code %s, release %s\n", bindtype, modifiers, input_code, release ? "true" : "false"));
    
    Binding new_binding{};
    new_binding.release = (release ? B_UPON_KEYRELEASE : B_UPON_KEYPRESS);
    new_binding.border = border;
    new_binding.whole_window = whole_window;
    new_binding.exclude_titlebar = exclude_titlebar;
    if (bindtype == "bindsym") {
        new_binding.input_type = (input_code.starts_with("button") ? B_MOUSE : B_KEYBOARD);

        new_binding.symbol = input_code;
    } else {
        long keycode;
        if (!utils::parse_long(input_code, keycode)) {
            ELOG(std::format("Could not parse \"{}\" as an input code, ignoring this binding.\n", input_code));
            return;
        }

        new_binding.keycode = keycode;
        new_binding.input_type = B_KEYBOARD;
    }
    new_binding.command = command;
    new_binding.event_state_mask = event_state_from_str(modifiers.data());
    int group_bits_set = 0;
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_1) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_2) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_3) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_4) {
        group_bits_set++;
    }
    if (group_bits_set > 1) {
        ELOG("Keybinding has more than one Group specified, but your X server is always in precisely one group. The keybinding can never trigger.\n");
    }

    mode_from_name(config, modename, pango_markup, new_binding);
}

/*
 * Switches the key bindings to the given mode, if the mode exists
 *
 */
void switch_mode(std::string const new_mode) {
    DLOG(fmt::sprintf("Switching to mode %s\n", new_mode));
    
    auto it = global.configManager->config->modes.find(new_mode);
    if (it == global.configManager->config->modes.end()) {
        ELOG("Mode not found\n");
        return;
    }

    auto &mode = it->second;

    ungrab_all_keys(*global.x);
    global.configManager->config->_current_mode = it->first;
    translate_keysyms(global.xkb->keymap());
    grab_all_keys(*global.x);
    regrab_all_buttons(*global.x);

    /* Reset all B_UPON_KEYRELEASE_IGNORE_MODS bindings to avoid possibly
     * activating one of them. */
    for (auto &bind : global.configManager->config->current_mode()->bindings) {
        if (bind.release == B_UPON_KEYRELEASE_IGNORE_MODS) {
            bind.release = B_UPON_KEYRELEASE;
        }
    }

    std::string event_msg = fmt::sprintf(R"({"change":"%s", "pango_markup":%s})", it->first, (mode.pango_markup ? "true" : "false"));

    global.ipcManager->ipc_send_event("mode", i3ipc::EVENT_MODE, event_msg);
}
