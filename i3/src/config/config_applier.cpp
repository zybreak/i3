/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.c: all config storing functions (see config_parser.c)
 *
 */
#include <cassert>
#include <cerrno>
#include <climits>

#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "ipc.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "dpi.h"

#include "criteria_state.h"
#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "i3.h"
#include "configuration.h"
#include "match.h"
#include "bindings.h"
#include "config_applier.h"
#include "config_parser.h"
#include "global.h"
#include "base_config_applier.h"
#include "base_parser.h"

#include <wordexp.h>
#include <autostarts.h>

import utils;

using namespace std::literals;

void ConfigApplier::font(const std::string &font) {
    config.font = load_font(**global.x, global.x->root_screen, font.c_str(), true);
    set_font(config.font);
}

void ConfigApplier::binding(const std::string &bindtype, const std::string &modifiers,
                 const std::string &key, bool release, bool border, bool whole_window,
                 bool exclude_titlebar, const std::string &command) {
    configure_binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command, DEFAULT_BINDING_MODE, false);
}

/*******************************************************************************
 * Mode handling
 ******************************************************************************/

void ConfigApplier::mode_binding(const std::string &bindtype, const std::string &modifiers,
                      const std::string &key, bool release, bool border, bool whole_window,
                      bool exclude_titlebar, const std::string &command) {
    configure_binding(bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command, current_mode, current_mode_pango_markup);
}

void ConfigApplier::enter_mode(bool pango_markup, const std::string &modename) {
    if (strcmp(modename.c_str(), DEFAULT_BINDING_MODE) == 0) {
        ELOG(fmt::sprintf("You cannot use the name %s for your mode\n",  DEFAULT_BINDING_MODE));
        return;
    }

    for (auto &mode : modes) {
        if (mode->name == modename) {
             ELOG(fmt::sprintf("The binding mode with name \"%s\" is defined at least twice.\n", modename));
        }
    }

    DLOG(fmt::sprintf("\t now in mode %s\n", modename));
    current_mode = modename;
    current_mode_pango_markup = pango_markup;
}

void ConfigApplier::exec(const std::string &exectype, bool no_startup_id,
              const std::string &command) {
    auto n = std::make_unique<Autostart>();
    n->command = command;
    n->no_startup_id = no_startup_id;
    if (exectype == "exec"s) {
        autostart_add(std::move(n));
    } else {
        autostart_always_add(std::move(n));
    }
}

void ConfigApplier::for_window(Match &current_match, const std::string &command) {
    if (current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this for_window statement\n");
        return;
    }
    DLOG(fmt::sprintf("\t should execute command %s for the criteria mentioned above\n", command));
    auto assignment = std::make_unique<CommandAssignment>();
    assignment->match = new Match(current_match);
    assignment->command = command;
    global.assignments.push_back(std::move(assignment));
}

void ConfigApplier::floating_minimum_size(const long width, const long height) {
    config.floating_minimum_width = width;
    config.floating_minimum_height = height;
}

void ConfigApplier::floating_maximum_size(const long width, const long height) {
    config.floating_maximum_width = width;
    config.floating_maximum_height = height;
}

void ConfigApplier::floating_modifier(const std::string &modifiers) {
    config.floating_modifier = event_state_from_str(modifiers.c_str());
}

void ConfigApplier::default_orientation(const std::string &orientation) {
    if (orientation == "horizontal"s)
        config.default_orientation = HORIZ;
    else if (orientation == "vertical"s)
        config.default_orientation = VERT;
    else
        config.default_orientation = NO_ORIENTATION;
}

void ConfigApplier::workspace_layout(const std::string &layout) {
    if (layout == "default"s)
        config.default_layout = L_DEFAULT;
    else if (layout == "stacking"s || layout == "stacked"s)
        config.default_layout = L_STACKED;
    else
        config.default_layout = L_TABBED;
}

void ConfigApplier::default_border(const std::string &windowtype, const std::string &border, const long width) {
    int border_style;
    int border_width;

    if (border == "1pixel"s) {
        border_style = BS_PIXEL;
        border_width = 1;
    } else if (border == "none"s) {
        border_style = BS_NONE;
        border_width = 0;
    } else if (border == "pixel"s) {
        border_style = BS_PIXEL;
        border_width = width;
    } else {
        border_style = BS_NORMAL;
        border_width = width;
    }

    if (windowtype ==  "default_border"s || windowtype == "new_window"s) {
        DLOG(fmt::sprintf("default tiled border style = %d and border width = %d (%ld physical px)\n",
             border_style, border_width, logical_px(global.x->root_screen, border_width)));
        config.default_border = (border_style_t)border_style;
        config.default_border_width = logical_px(global.x->root_screen, border_width);
    } else {
        DLOG(fmt::sprintf("default floating border style = %d and border width = %d (%ld physical px)\n",
             border_style, border_width, logical_px(global.x->root_screen, border_width)));
        config.default_floating_border = (border_style_t)border_style;
        config.default_floating_border_width = logical_px(global.x->root_screen, border_width);
    }
}

void ConfigApplier::hide_edge_borders(const std::string &borders) {
    if (borders == "smart"s)
        config.hide_edge_borders = HEBM_SMART;
    else if (borders == "vertical"s)
        config.hide_edge_borders = HEBM_VERTICAL;
    else if (borders == "horizontal"s)
        config.hide_edge_borders = HEBM_HORIZONTAL;
    else if (borders == "both"s)
        config.hide_edge_borders = HEBM_BOTH;
    else if (borders == "none"s)
        config.hide_edge_borders = HEBM_NONE;
    else if (boolstr(borders.c_str()))
        config.hide_edge_borders = HEBM_VERTICAL;
    else
        config.hide_edge_borders = HEBM_NONE;
}

void ConfigApplier::focus_follows_mouse(const std::string &value) {
    config.disable_focus_follows_mouse = !boolstr(value.c_str());
}

void ConfigApplier::mouse_warping(const std::string &value) {
    if (value == "none"s)
        config.mouse_warping = POINTER_WARPING_NONE;
    else if (value == "output"s)
        config.mouse_warping = POINTER_WARPING_OUTPUT;
}

void ConfigApplier::focus_wrapping(const std::string &value) {
    if (value == "force"s) {
        config.focus_wrapping = FOCUS_WRAPPING_FORCE;
    } else if (value == "workspace"s) {
        config.focus_wrapping = FOCUS_WRAPPING_WORKSPACE;
    } else if (boolstr(value.c_str())) {
        config.focus_wrapping = FOCUS_WRAPPING_ON;
    } else {
        config.focus_wrapping = FOCUS_WRAPPING_OFF;
    }
}

void ConfigApplier::force_focus_wrapping(const std::string &value) {
    /* Legacy syntax. */
    if (boolstr(value.c_str())) {
        config.focus_wrapping = FOCUS_WRAPPING_FORCE;
    } else {
        /* For "force_focus_wrapping off", don't enable or disable
         * focus wrapping, just ensure it's not forced. */
        if (config.focus_wrapping == FOCUS_WRAPPING_FORCE) {
            config.focus_wrapping = FOCUS_WRAPPING_ON;
        }
    }
}

void ConfigApplier::workspace_back_and_forth(const std::string &value) {
    config.workspace_auto_back_and_forth = boolstr(value.c_str());
}

void ConfigApplier::force_display_urgency_hint(const long duration_ms) {
    config.workspace_urgency_timer = duration_ms / 1000.0;
}

void ConfigApplier::focus_on_window_activation(const std::string &mode) {
    if (mode == "smart"s)
        config.focus_on_window_activation = FOWA_SMART;
    else if (mode == "urgent"s)
        config.focus_on_window_activation = FOWA_URGENT;
    else if (mode == "focus"s)
        config.focus_on_window_activation = FOWA_FOCUS;
    else if (mode == "none"s)
        config.focus_on_window_activation = FOWA_NONE;
    else {
         ELOG(fmt::sprintf("Unknown focus_on_window_activation mode \"%s\", ignoring it.\n", mode));
        return;
    }

    DLOG(fmt::sprintf("Set new focus_on_window_activation mode = %i.\n",  config.focus_on_window_activation));
}

void ConfigApplier::title_align(const std::string &alignment) {
    if (alignment == "left"s) {
        config.title_align = Config::ALIGN_LEFT;
    } else if (alignment == "center"s) {
        config.title_align = Config::ALIGN_CENTER;
    } else if (alignment == "right"s) {
        config.title_align = Config::ALIGN_RIGHT;
    } else {
        assert(false);
    }
}

void ConfigApplier::workspace(const std::string &workspace, const std::string &output) {
    for (const auto &assignment : global.ws_assignments) {
        if (strcasecmp(assignment->name.c_str(), workspace.c_str()) == 0) {
            ELOG(fmt::sprintf("You have a duplicate workspace assignment for workspace \"%s\"\n", workspace));
            return;
        }
    }

     DLOG(fmt::sprintf("Assigning workspace \"%s\" to output \"%s\"\n", workspace, output));

    auto assignment = std::make_unique<Workspace_Assignment>();
    assignment->name = workspace;
    assignment->output = output;
    global.ws_assignments.push_back(std::move(assignment));
}

void ConfigApplier::ipc_socket(const std::string &path) {
    free(config.ipc_socket_path);
    config.ipc_socket_path = sstrdup(path.c_str());
}

void ConfigApplier::restart_state(const std::string &path) {
    free(config.restart_state_path);
    config.restart_state_path = sstrdup(path.c_str());
}

void ConfigApplier::popup_during_fullscreen(const std::string &value) {
    if (value == "ignore"s) {
        config.popup_during_fullscreen = PDF_IGNORE;
    } else if (value == "leave_fullscreen"s) {
        config.popup_during_fullscreen = PDF_LEAVE_FULLSCREEN;
    } else {
        config.popup_during_fullscreen = PDF_SMART;
    }
}

void ConfigApplier::color_single(const std::string &colorclass, const std::string &color) {
    /* used for client.background only currently */
    config.client.background = draw_util_hex_to_color(**global.x, global.x->root_screen, color.c_str());
}

void ConfigApplier::color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) {
#define APPLY_COLORS(classname)                                                              \
    do {                                                                                     \
        if (strcmp(colorclass.c_str(), "client." #classname) == 0) {                                 \
            config.client.classname.border = draw_util_hex_to_color(**global.x, global.x->root_screen, border.c_str());                 \
            config.client.classname.background = draw_util_hex_to_color(**global.x, global.x->root_screen, background.c_str());         \
            config.client.classname.text = draw_util_hex_to_color(**global.x, global.x->root_screen, text.c_str());                     \
            if (!indicator.empty()) {                                                         \
                config.client.classname.indicator = draw_util_hex_to_color(**global.x, global.x->root_screen, indicator.c_str());       \
            }                                                                                \
            if (!child_border.empty()) {                                                      \
                config.client.classname.child_border = draw_util_hex_to_color(**global.x, global.x->root_screen, child_border.c_str()); \
            } else {                                                                         \
                config.client.classname.child_border = config.client.classname.background;   \
            }                                                                                \
        }                                                                                    \
    } while (0)

    APPLY_COLORS(focused_inactive);
    APPLY_COLORS(focused);
    APPLY_COLORS(unfocused);
    APPLY_COLORS(urgent);
    APPLY_COLORS(placeholder);

#undef APPLY_COLORS
}

void ConfigApplier::assign_output(Match &current_match, const std::string &output) {
    if (current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    if (current_match.window_mode != WM_ANY) {
        ELOG("Assignments using window mode (floating/tiling) is not supported\n");
        return;
    }

     DLOG(fmt::sprintf("New assignment, using above criteria, to output \"%s\".\n", output));
    auto assignment = std::make_unique<OutputAssignment>();
    assignment->match = new Match(current_match);
    assignment->type = A_TO_OUTPUT;
    assignment->output = output;
    global.assignments.push_back(std::move(assignment));
}

void ConfigApplier::assign(Match &current_match, const std::string &workspace, bool is_number) {
    if (current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    if (current_match.window_mode != WM_ANY) {
        ELOG("Assignments using window mode (floating/tiling) is not supported\n");
        return;
    }

    if (is_number && ws_name_to_number(workspace) == -1) {
         ELOG(fmt::sprintf("Could not parse initial part of \"%s\" as a number.\n", workspace));
        return;
    }

     DLOG(fmt::sprintf("New assignment, using above criteria, to workspace \"%s\".\n", workspace));
    auto assignment = std::make_unique<WorkspaceAssignment>();
    assignment->match = new Match(current_match);
    assignment->type = is_number ? A_TO_WORKSPACE_NUMBER : A_TO_WORKSPACE;
    assignment->workspace = workspace;
    global.assignments.push_back(std::move(assignment));
}

void ConfigApplier::no_focus(Match &current_match) {
    if (current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    DLOG("New assignment, using above criteria, to ignore focus on manage.\n");
    auto assignment = std::make_unique<Assignment>();
    assignment->match = new Match(current_match);
    assignment->type = A_NO_FOCUS;
    global.assignments.push_back(std::move(assignment));
}

void ConfigApplier::ipc_kill_timeout(const long timeout_ms) {
    ipc_set_kill_timeout(timeout_ms / 1000.0);
}
