/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.c: all config storing functions (see config_parser.c)
 *
 */
module;
struct criteria_state;

#include <cassert>
#include <cerrno>
#include <climits>

#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "i3_ipc/i3-ipc.h"
#include "i3.h"

#include <wordexp.h>

#include <fmt/printf.h>
module i3_config_old;

import utils;
import log;

static bool str_to_bool(const char *s, const char *v) {
    if (s == nullptr) {
        return false;
    } else {
        return strcmp(s, v) >= 0;
    }
}

static const char * null_to_empty_str(const char *s) {
    if (s == nullptr) {
        return "";
    } else {
        return s;
    }
}

namespace cfg {
    void include(criteria_state *criteria_state, ConfigResultIR &result, const char *pattern) {
        DLOG(fmt::sprintf("include %s\n",  pattern));

        wordexp_t p;
        const int ret = wordexp(pattern, &p, 0);
        if (ret != 0) {
            ELOG(fmt::sprintf("wordexp(%s): error %d\n",  pattern, ret));
            result.has_errors = true;
            return;
        }
        char **w = p.we_wordv;
        for (size_t i = 0; i < p.we_wordc; i++) {
            char resolved_path[PATH_MAX] = {'\0'};
            if (realpath(w[i], resolved_path) == nullptr) {
                ELOG(fmt::sprintf("realpath(%s): %s\n",  w[i], strerror(errno)));
                result.has_errors = true;
                continue;
            }

            auto skip = std::ranges::find_if(result.ctx.included_files, [&resolved_path](auto & included_file) {
                return (strcmp(included_file.c_str(), resolved_path) == 0);
            });
            if (skip != result.ctx.included_files.end()) {
                LOG(fmt::sprintf("Skipping file %s (already included)\n",  resolved_path));
                continue;
            }

            LOG(fmt::sprintf("Including config file %s\n",  resolved_path));

            OldParser parser{resolved_path, result.ctx.resourceDatabase, result.ctx, result.ctx.applier};
            try {
                parser.parse_file();
                ELOG(fmt::sprintf("including config file %s: %s\n", resolved_path, strerror(errno)));
                result.ctx.included_files.emplace_back(resolved_path);
            } catch (std::exception &e) {
                result.has_errors = true;
            }
        }
        wordfree(&p);
    }

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(criteria_state *criteria_state, ConfigResultIR &result, int _state) {
        result.ctx.applier.criteria_init(criteria_state, _state);
    }

    void criteria_pop_state(criteria_state *criteria_state, ConfigResultIR &result) {
        result.next_state = result.ctx.applier.criteria_pop_state(criteria_state);
    }
    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(criteria_state *criteria_state, ConfigResultIR &result, const char *ctype, const char *cvalue) {
        result.ctx.applier.criteria_add(criteria_state, ctype, cvalue);
    }

    void font(criteria_state *criteria_state, ConfigResultIR &result, const char *font) {
        result.ctx.applier.font(font);
    }

    void binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command) {
        result.ctx.applier.binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command);
    }


    void mode_binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                          const char *key, const char *release, const char *border, const char *whole_window,
                          const char *exclude_titlebar, const char *command) {
        result.ctx.applier.mode_binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command);
    }

    void enter_mode(criteria_state *criteria_state, ConfigResultIR &result, const char *pango_markup, const char *modename) {
        result.ctx.applier.enter_mode(str_to_bool(pango_markup, "--pango_markup"), modename);
    }

    void exec(criteria_state *criteria_state, ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command) {
        result.ctx.applier.exec(exectype, str_to_bool(no_startup_id, "--no-startup-id"), command);
    }

    void for_window(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {

        result.ctx.applier.for_window(criteria_state, command);
    }

    void floating_minimum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height) {
        result.ctx.applier.floating_minimum_size(width, height);
    }

    void floating_maximum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height) {
        result.ctx.applier.floating_maximum_size(width, height);
    }

    void floating_modifier(criteria_state *criteria_state, ConfigResultIR &result, const char *modifiers) {
        result.ctx.applier.floating_modifier(modifiers);
    }

    void default_orientation(criteria_state *criteria_state, ConfigResultIR &result, const char *orientation) {
        result.ctx.applier.default_orientation(orientation);
    }

    void workspace_layout(criteria_state *criteria_state, ConfigResultIR &result, const char *layout) {
        result.ctx.applier.workspace_layout(layout);
    }

    void default_border(criteria_state *criteria_state, ConfigResultIR &result, const char *windowtype, const char *border,
                            const long width) {
        result.ctx.applier.default_border(windowtype, border, width);
    }

    void hide_edge_borders(criteria_state *criteria_state, ConfigResultIR &result, const char *borders) {
        result.ctx.applier.hide_edge_borders(borders);
    }

    void focus_follows_mouse(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.focus_follows_mouse(value);
    }

    void mouse_warping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.mouse_warping(value);
    }

    void focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.focus_wrapping(value);
    }

    void force_focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.force_focus_wrapping(value);
    }

    void workspace_back_and_forth(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.workspace_back_and_forth(value);
    }

    void force_display_urgency_hint(criteria_state *criteria_state, ConfigResultIR &result, const long duration_ms) {
        result.ctx.applier.force_display_urgency_hint(duration_ms);
    }

    void focus_on_window_activation(criteria_state *criteria_state, ConfigResultIR &result, const char *mode) {
        result.ctx.applier.focus_on_window_activation(mode);
    }

    void title_align(criteria_state *criteria_state, ConfigResultIR &result, const char *alignment) {
        result.ctx.applier.title_align(alignment);
    }

    void workspace(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, const char *output) {
        result.ctx.applier.workspace(workspace, output);
    }

    void ipc_socket(criteria_state *criteria_state, ConfigResultIR &result, const char *path) {
        result.ctx.applier.ipc_socket(path);
    }

    void restart_state(criteria_state *criteria_state, ConfigResultIR &result, const char *path) {
        result.ctx.applier.restart_state(path);
    }

    void popup_during_fullscreen(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.ctx.applier.popup_during_fullscreen(value);
    }

    void color_single(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *color) {
        result.ctx.applier.color_single(colorclass, color);
    }

    void color(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border) {
        result.ctx.applier.color(colorclass, border, background, text, indicator, child_border);
    }

    void assign_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output) {
        result.ctx.applier.assign_output(criteria_state, output);
    }

    void assign(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, bool is_number) {
        result.ctx.applier.assign(criteria_state, workspace, is_number);
    }

    void no_focus(criteria_state *criteria_state, ConfigResultIR &result) {
        result.ctx.applier.no_focus(criteria_state);
    }

    void ipc_kill_timeout(criteria_state *criteria_state, ConfigResultIR &result, const long timeout_ms) {
        result.ctx.applier.ipc_kill_timeout(timeout_ms);
    }

    void bar_font(criteria_state *criteria_state, ConfigResultIR &result, const char *font) {
    }

    void bar_separator_symbol(criteria_state *criteria_state, ConfigResultIR &result, const char *separator) {
    }

    void bar_mode(criteria_state *criteria_state, ConfigResultIR &result, const char *mode) {
    }

    void bar_hidden_state(criteria_state *criteria_state, ConfigResultIR &result, const char *hidden_state) {
    }

    void bar_id(criteria_state *criteria_state, ConfigResultIR &result, const char *bar_id) {
    }

    void bar_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output) {
    }

    void bar_verbose(criteria_state *criteria_state, ConfigResultIR &result, const char *verbose) {
    }

    void bar_modifier(criteria_state *criteria_state, ConfigResultIR &result, const char *modifiers) {
    }

    void bar_wheel_up_cmd(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {

    }

    void bar_wheel_down_cmd(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {
    }

    void bar_bindsym(criteria_state *criteria_state, ConfigResultIR &result, const char *button, const char *release,
                         const char *command) {
    }

    void bar_position(criteria_state *criteria_state, ConfigResultIR &result, const char *position) {
    }

    void bar_i3bar_command(criteria_state *criteria_state, ConfigResultIR &result, const char *i3bar_command) {
    }

    void bar_color(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *border,
                       const char *background, const char *text) {
    }

    void bar_socket_path(criteria_state *criteria_state, ConfigResultIR &result, const char *socket_path) {
    }

    void bar_tray_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output) {
    }

    void bar_tray_padding(criteria_state *criteria_state, ConfigResultIR &result, const long padding_px) {
    }

    void bar_color_single(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *color) {
    }

    void bar_status_command(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {
    }

    void bar_binding_mode_indicator(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
    }

    void bar_workspace_buttons(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
    }

    void bar_workspace_min_width(criteria_state *criteria_state, ConfigResultIR &result, const long width) {
    }

    void bar_strip_workspace_numbers(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
    }

    void bar_strip_workspace_name(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
    }

    void bar_start(criteria_state *criteria_state, ConfigResultIR &result) {
    }

    void bar_finish(criteria_state *criteria_state, ConfigResultIR &result) {
    }
}