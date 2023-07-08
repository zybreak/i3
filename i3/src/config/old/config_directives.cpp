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

#include "i3string.h"
#include "log.h"
#include "ipc.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"

#include "criteria_state.h"
#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "i3.h"
#include "configuration.h"
#include "match.h"
#include "bindings.h"
#include "config_directives.h"
#include "config_parser.h"

#include <wordexp.h>
#include <autostarts.h>

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
    void include(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *pattern) {
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

            auto skip = std::ranges::find_if(included_files, [&resolved_path](auto & included_file) {
                return (strcmp(included_file.c_str(), resolved_path) == 0);
            });
            if (skip != included_files.end()) {
                LOG(fmt::sprintf("Skipping file %s (already included)\n",  resolved_path));
                continue;
            }

            LOG(fmt::sprintf("Including config file %s\n",  resolved_path));

            try {
                OldParser parser{resolved_path, result.ctx, result.ctx.applier};
                switch (parser.parse_file()) {
                    case PARSE_FILE_SUCCESS:
                        included_files.emplace_back(resolved_path);
                        break;

                    case PARSE_FILE_FAILED:
                        ELOG(fmt::sprintf("including config file %s: %s\n", resolved_path, strerror(errno)));
                        /* fallthrough */

                    case PARSE_FILE_CONFIG_ERRORS:
                        result.has_errors = true;
                        break;

                    default:
                        /* missing case statement */
                        assert(false);
                        break;
                }
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
    void criteria_init(struct criteria_state &criteria_state, struct ConfigResultIR &result, int _state) {
        criteria_state.criteria_next_state = _state;

        DLOG(fmt::sprintf("Initializing criteria, current_match = %p, state = %d\n",  (void*)&(criteria_state.current_match), _state));
        criteria_state.current_match = Match();
    }

    void criteria_pop_state(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        result.next_state = criteria_state.criteria_next_state;
    }
    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *ctype, const char *cvalue) {
        criteria_state.current_match.parse_property(ctype, cvalue);
    }

    void font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font) {
        result.ctx.applier.font(font);
    }

    void binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command) {
        result.ctx.applier.binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command);
    }


    void mode_binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                          const char *key, const char *release, const char *border, const char *whole_window,
                          const char *exclude_titlebar, const char *command) {
        result.ctx.applier.mode_binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command);
    }

    void enter_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *pango_markup, const char *modename) {
        result.ctx.applier.enter_mode(str_to_bool(pango_markup, "--pango_markup"), modename);
    }

    void exec(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command) {
        result.ctx.applier.exec(exectype, str_to_bool(no_startup_id, "--no-startup-id"), command);
    }

    void for_window(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
        if (match_is_empty(criteria_state.current_match)) {
            //ELOG("Match is empty, ignoring this for_window statement\n");
            return;
        }
        //DLOG(fmt::sprintf("\t should execute command %s for the criteria mentioned above\n",  command));
        Match match(criteria_state.current_match);
        result.ctx.applier.for_window(match, command);
    }

    void floating_minimum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height) {
        result.ctx.applier.floating_minimum_size(width, height);
    }

    void floating_maximum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height) {
        result.ctx.applier.floating_maximum_size(width, height);
    }

    void floating_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers) {
        result.ctx.applier.floating_modifier(modifiers);
    }

    void default_orientation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *orientation) {
        result.ctx.applier.default_orientation(orientation);
    }

    void workspace_layout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *layout) {
        result.ctx.applier.workspace_layout(layout);
    }

    void default_border(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *windowtype, const char *border,
                            const long width) {
        result.ctx.applier.default_border(windowtype, border, width);
    }

    void hide_edge_borders(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *borders) {
        result.ctx.applier.hide_edge_borders(borders);
    }

    void focus_follows_mouse(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.focus_follows_mouse(value);
    }

    void mouse_warping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.mouse_warping(value);
    }

    void focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.focus_wrapping(value);
    }

    void force_focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.force_focus_wrapping(value);
    }

    void workspace_back_and_forth(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.workspace_back_and_forth(value);
    }

    void force_display_urgency_hint(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long duration_ms) {
        result.ctx.applier.force_display_urgency_hint(duration_ms);
    }

    void focus_on_window_activation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode) {
        result.ctx.applier.focus_on_window_activation(mode);
    }

    void title_align(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *alignment) {
        result.ctx.applier.title_align(alignment);
    }

    void workspace(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *workspace, const char *output) {
        result.ctx.applier.workspace(workspace, output);
    }

    void ipc_socket(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path) {
        result.ctx.applier.ipc_socket(path);
    }

    void restart_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path) {
        result.ctx.applier.restart_state(path);
    }

    void popup_during_fullscreen(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        result.ctx.applier.popup_during_fullscreen(value);
    }

    void color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *color) {
        result.ctx.applier.color_single(colorclass, color);
    }

    void color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border) {
        result.ctx.applier.color(colorclass, border, background, text, indicator, child_border);
    }

    void assign_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output) {
        if (match_is_empty(criteria_state.current_match)) {
            ELOG("Match is empty, ignoring this assignment\n");
            return;
        }

        if (criteria_state.current_match.window_mode != WM_ANY) {
            ELOG("Assignments using window mode (floating/tiling) is not supported\n");
            return;
        }

        DLOG(fmt::sprintf("New assignment, using above criteria, to output \"%s\".\n", output));
        Match match(criteria_state.current_match);
        result.ctx.applier.assign_output(match, output);
    }

    void assign(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *workspace, bool is_number) {
        if (match_is_empty(criteria_state.current_match)) {
            ELOG("Match is empty, ignoring this assignment\n");
            return;
        }

        if (criteria_state.current_match.window_mode != WM_ANY) {
            ELOG("Assignments using window mode (floating/tiling) is not supported\n");
            return;
        }
        Match match(criteria_state.current_match);
        result.ctx.applier.assign(match, workspace, is_number);
    }

    void no_focus(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        if (match_is_empty(criteria_state.current_match)) {
            ELOG("Match is empty, ignoring this assignment\n");
            return;
        }
        Match match(criteria_state.current_match);
        result.ctx.applier.no_focus(match);
    }

    void ipc_kill_timeout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long timeout_ms) {
        result.ctx.applier.ipc_kill_timeout(timeout_ms);
    }

    void bar_font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font) {
    }

    void bar_separator_symbol(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *separator) {
    }

    void bar_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode) {
    }

    void bar_hidden_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *hidden_state) {
    }

    void bar_id(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bar_id) {
    }

    void bar_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output) {
    }

    void bar_verbose(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *verbose) {
    }

    void bar_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers) {
    }

    void bar_wheel_up_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {

    }

    void bar_wheel_down_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
    }

    void bar_bindsym(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *button, const char *release,
                         const char *command) {
    }

    void bar_position(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *position) {
    }

    void bar_i3bar_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *i3bar_command) {
    }

    void bar_color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                       const char *background, const char *text) {
    }

    void bar_socket_path(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *socket_path) {
    }

    void bar_tray_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output) {
    }

    void bar_tray_padding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long padding_px) {
    }

    void bar_color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *color) {
    }

    void bar_status_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
    }

    void bar_binding_mode_indicator(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
    }

    void bar_workspace_buttons(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
    }

    void bar_workspace_min_width(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width) {
    }

    void bar_strip_workspace_numbers(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
    }

    void bar_strip_workspace_name(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
    }

    void bar_start(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
    }

    void bar_finish(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
    }
}