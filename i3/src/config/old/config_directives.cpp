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

#include <xcb/xcb.h>

#include <wordexp.h>

#include <fmt/printf.h>
module i3_config_old;

import i3_config_base;
import std;
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

            auto skip = std::ranges::find_if(result.parser.included_files, [&resolved_path](auto & included_file) {
                return (strcmp(included_file->path.c_str(), resolved_path) == 0);
            });
            if (skip != result.parser.included_files.end()) {
                LOG(fmt::sprintf("Skipping file %s (already included)\n",  resolved_path));
                continue;
            }

            LOG(fmt::sprintf("Including config file %s\n",  resolved_path));

            std::ifstream input{resolved_path};
            OldParser parser{resolved_path, input, result.parser.resourceDatabase, dynamic_cast<OldParser&>(result.parser).ctx, result.parser.applier};
            try {
                parser.parse_file();
            } catch (std::exception &e) {
                ELOG(fmt::sprintf("including config file %s: %s\n", resolved_path, e.what()));
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
        result.parser.applier.criteria_init(criteria_state, _state);
    }

    void criteria_pop_state(criteria_state *criteria_state, ConfigResultIR &result) {
        result.next_state = result.parser.applier.criteria_pop_state(criteria_state);
    }
    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(criteria_state *criteria_state, ConfigResultIR &result, const char *ctype, const char *cvalue) {
        result.parser.applier.criteria_add(criteria_state, ctype, cvalue);
    }

    void font(criteria_state *criteria_state, ConfigResultIR &result, const char *font) {
        result.parser.applier.font(font);
    }

    void binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command) {
        result.parser.applier.binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "--release"), str_to_bool(border, "--border"), str_to_bool(whole_window, "--whole-window"), str_to_bool(exclude_titlebar, "--exclude-titlebar"), command);
    }


    void mode_binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                          const char *key, const char *release, const char *border, const char *whole_window,
                          const char *exclude_titlebar, const char *command) {
        result.parser.applier.mode_binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "--release"), str_to_bool(border, "--border"), str_to_bool(whole_window, "--whole-window"), str_to_bool(exclude_titlebar, "--exclude-titlebar"), command);
    }

    void enter_mode(criteria_state *criteria_state, ConfigResultIR &result, const char *pango_markup, const char *modename) {
        result.parser.applier.enter_mode(str_to_bool(pango_markup, "--pango_markup"), modename);
    }

    void exec(criteria_state *criteria_state, ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command) {
        result.parser.applier.exec(exectype, str_to_bool(no_startup_id, "--no-startup-id"), command);
    }

    void for_window(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {

        result.parser.applier.for_window(criteria_state, command);
    }

    void gaps(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, const char *type, const long value) {
        result.parser.applier.gaps(workspace, type, value);
    }

    void smart_borders(criteria_state *criteria_state, ConfigResultIR &result, const char *enable) {
        result.parser.applier.smart_borders(enable);
    }

    void smart_gaps(criteria_state *criteria_state, ConfigResultIR &result, const char *enable) {
        result.parser.applier.smart_gaps(enable);
    }

    void floating_minimum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height) {
        result.parser.applier.floating_minimum_size(width, height);
    }

    void floating_maximum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height) {
        result.parser.applier.floating_maximum_size(width, height);
    }

    void floating_modifier(criteria_state *criteria_state, ConfigResultIR &result, const char *modifiers) {
        result.parser.applier.floating_modifier(modifiers);
    }

    void default_orientation(criteria_state *criteria_state, ConfigResultIR &result, const char *orientation) {
        result.parser.applier.default_orientation(orientation);
    }

    void workspace_layout(criteria_state *criteria_state, ConfigResultIR &result, const char *layout) {
        result.parser.applier.workspace_layout(layout);
    }

    void default_border(criteria_state *criteria_state, ConfigResultIR &result, const char *windowtype, const char *border,
                            const long width) {
        result.parser.applier.default_border(windowtype, border, width);
    }

    void hide_edge_borders(criteria_state *criteria_state, ConfigResultIR &result, const char *borders) {
        result.parser.applier.hide_edge_borders(borders);
    }

    void focus_follows_mouse(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.focus_follows_mouse(value);
    }

    void mouse_warping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.mouse_warping(value);
    }

    void focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.focus_wrapping(value);
    }

    void force_focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.force_focus_wrapping(value);
    }

    void workspace_back_and_forth(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.workspace_back_and_forth(value);
    }

    void force_display_urgency_hint(criteria_state *criteria_state, ConfigResultIR &result, const long duration_ms) {
        result.parser.applier.force_display_urgency_hint(duration_ms);
    }

    void focus_on_window_activation(criteria_state *criteria_state, ConfigResultIR &result, const char *mode) {
        result.parser.applier.focus_on_window_activation(mode);
    }

    void title_align(criteria_state *criteria_state, ConfigResultIR &result, const char *alignment) {
        result.parser.applier.title_align(alignment);
    }

    void workspace(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, const char *output) {
        result.parser.applier.workspace(null_to_empty_str(workspace), output);
    }

    void ipc_socket(criteria_state *criteria_state, ConfigResultIR &result, const char *path) {
        result.parser.applier.ipc_socket(path);
    }

    void restart_state(criteria_state *criteria_state, ConfigResultIR &result, const char *path) {
        result.parser.applier.restart_state(path);
    }

    void popup_during_fullscreen(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.popup_during_fullscreen(value);
    }

    void color_single(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *color) {
        result.parser.applier.color_single(colorclass, color);
    }

    void color(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border) {
        result.parser.applier.color(colorclass, border, background, text, indicator, null_to_empty_str(child_border));
    }

    void assign_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output) {
        result.parser.applier.assign_output(criteria_state, output);
    }

    void assign(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, bool is_number) {
        result.parser.applier.assign(criteria_state, workspace, is_number);
    }

    void no_focus(criteria_state *criteria_state, ConfigResultIR &result) {
        result.parser.applier.no_focus(criteria_state);
    }

    void ipc_kill_timeout(criteria_state *criteria_state, ConfigResultIR &result, const long timeout_ms) {
        result.parser.applier.ipc_kill_timeout(timeout_ms);
    }

    void tiling_drag(criteria_state *criteria_state, ConfigResultIR &result, const char *value) {
        result.parser.applier.tiling_drag(value);
    }

    void bar_height(criteria_state *criteria_state, ConfigResultIR &result, const long height) {
        result.parser.applier.bar_height(height);
    }

    void bar_padding_one(criteria_state *criteria_state, ConfigResultIR &result, const long all) {
        result.parser.applier.bar_padding_one(all);
    }

    void bar_padding_two(criteria_state *criteria_state, ConfigResultIR &result, const long top_and_bottom, const long right_and_left) {
        result.parser.applier.bar_padding_two(top_and_bottom, right_and_left);
    }

    void bar_padding_three(criteria_state *criteria_state, ConfigResultIR &result, const long top, const long right_and_left, const long bottom) {
        result.parser.applier.bar_padding_three(top, right_and_left, bottom);
    }

    void bar_padding_four(criteria_state *criteria_state, ConfigResultIR &result, const long top, const long right, const long bottom, const long left) {
        result.parser.applier.bar_padding_four(top, right, bottom, left);
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

    void bar_workspace_command(criteria_state *criteria_state, ConfigResultIR &result, const char *command) {

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