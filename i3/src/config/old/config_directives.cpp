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

static std::string_view null_to_empty_str(const char *s) {
    if (s == nullptr) {
        return "";
    } else {
        return s;
    }
}

static bool str_to_bool(const char *s, const char *v) {
    if (s == nullptr) {
        return false;
    } else {
        return strcmp(s, v) >= 0;
    }
}


/*******************************************************************************
 * Include functions.
 ******************************************************************************/
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
                OldParser parser{resolved_path, result.ctx};
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

    /*******************************************************************************
     * Criteria functions.
     ******************************************************************************/

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

    /*******************************************************************************
     * Utility functions
     ******************************************************************************/

    static char *font_pattern;

    void font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font) {
        config.font = load_font(global.conn, global.root_screen, font, true);
        set_font(&config.font);

        /* Save the font pattern for using it as bar font later on */
        FREE(font_pattern);
        font_pattern = sstrdup(font);
    }

    void binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command) {
        configure_binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command, DEFAULT_BINDING_MODE, false);
    }


    /*******************************************************************************
     * Mode handling
     ******************************************************************************/

    static char *current_mode;
    static bool current_mode_pango_markup;

    void mode_binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                          const char *key, const char *release, const char *border, const char *whole_window,
                          const char *exclude_titlebar, const char *command) {
        configure_binding(bindtype, null_to_empty_str(modifiers), key, str_to_bool(release, "release"), str_to_bool(border, "border"), str_to_bool(whole_window, "whole_window"), str_to_bool(exclude_titlebar, "exclude_titlebar"), command, current_mode, current_mode_pango_markup);
    }

    void enter_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *pango_markup, const char *modename) {
        if (strcmp(modename, DEFAULT_BINDING_MODE) == 0) {
            ELOG(fmt::sprintf("You cannot use the name %s for your mode\n",  DEFAULT_BINDING_MODE));
            return;
        }

        for (auto &mode : modes) {
            if (strcmp(mode->name.c_str(), modename) == 0) {
                 ELOG(fmt::sprintf("The binding mode with name \"%s\" is defined at least twice.\n", modename));
            }
        }

        DLOG(fmt::sprintf("\t now in mode %s\n",  modename));
        FREE(current_mode);
        current_mode = sstrdup(modename);
        current_mode_pango_markup = (pango_markup != nullptr);
    }

    void exec(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command) {
        auto n = std::make_unique<Autostart>();
        n->command = std::string(command);
        n->no_startup_id = (no_startup_id != nullptr);
        if (strcmp(exectype, "exec") == 0) {
            autostarts.push_back(std::move(n));
        } else {
            autostarts_always.push_back(std::move(n));
        }
    }

    void for_window(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
        if (match_is_empty(criteria_state.current_match)) {
            ELOG("Match is empty, ignoring this for_window statement\n");
            return;
        }
        DLOG(fmt::sprintf("\t should execute command %s for the criteria mentioned above\n",  command));
        auto assignment = std::make_unique<CommandAssignment>();
        assignment->match = new Match(criteria_state.current_match);
        assignment->command = command;
        global.assignments.push_back(std::move(assignment));
    }

    void floating_minimum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height) {
        config.floating_minimum_width = width;
        config.floating_minimum_height = height;
    }

    void floating_maximum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height) {
        config.floating_maximum_width = width;
        config.floating_maximum_height = height;
    }

    void floating_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers) {
        config.floating_modifier = event_state_from_str(modifiers);
    }

    void default_orientation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *orientation) {
        if (strcmp(orientation, "horizontal") == 0)
            config.default_orientation = HORIZ;
        else if (strcmp(orientation, "vertical") == 0)
            config.default_orientation = VERT;
        else
            config.default_orientation = NO_ORIENTATION;
    }

    void workspace_layout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *layout) {
        if (strcmp(layout, "default") == 0)
            config.default_layout = L_DEFAULT;
        else if (strcmp(layout, "stacking") == 0 ||
                 strcmp(layout, "stacked") == 0)
            config.default_layout = L_STACKED;
        else
            config.default_layout = L_TABBED;
    }

    void default_border(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *windowtype, const char *border,
                            const long width) {
        int border_style;
        int border_width;

        if (strcmp(border, "1pixel") == 0) {
            border_style = BS_PIXEL;
            border_width = 1;
        } else if (strcmp(border, "none") == 0) {
            border_style = BS_NONE;
            border_width = 0;
        } else if (strcmp(border, "pixel") == 0) {
            border_style = BS_PIXEL;
            border_width = width;
        } else {
            border_style = BS_NORMAL;
            border_width = width;
        }

        if ((strcmp(windowtype, "default_border") == 0) ||
            (strcmp(windowtype, "new_window") == 0)) {
            DLOG(fmt::sprintf("default tiled border style = %d and border width = %d (%ld physical px)\n",
                 border_style, border_width, logical_px(global.root_screen, border_width)));
            config.default_border = (border_style_t)border_style;
            config.default_border_width = logical_px(global.root_screen, border_width);
        } else {
            DLOG(fmt::sprintf("default floating border style = %d and border width = %d (%ld physical px)\n",
                 border_style, border_width, logical_px(global.root_screen, border_width)));
            config.default_floating_border = (border_style_t)border_style;
            config.default_floating_border_width = logical_px(global.root_screen, border_width);
        }
    }

    void hide_edge_borders(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *borders) {
        if (strcmp(borders, "smart") == 0)
            config.hide_edge_borders = HEBM_SMART;
        else if (strcmp(borders, "vertical") == 0)
            config.hide_edge_borders = HEBM_VERTICAL;
        else if (strcmp(borders, "horizontal") == 0)
            config.hide_edge_borders = HEBM_HORIZONTAL;
        else if (strcmp(borders, "both") == 0)
            config.hide_edge_borders = HEBM_BOTH;
        else if (strcmp(borders, "none") == 0)
            config.hide_edge_borders = HEBM_NONE;
        else if (boolstr(borders))
            config.hide_edge_borders = HEBM_VERTICAL;
        else
            config.hide_edge_borders = HEBM_NONE;
    }

    void focus_follows_mouse(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        config.disable_focus_follows_mouse = !boolstr(value);
    }

    void mouse_warping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        if (strcmp(value, "none") == 0)
            config.mouse_warping = POINTER_WARPING_NONE;
        else if (strcmp(value, "output") == 0)
            config.mouse_warping = POINTER_WARPING_OUTPUT;
    }

    void focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        if (strcmp(value, "force") == 0) {
            config.focus_wrapping = FOCUS_WRAPPING_FORCE;
        } else if (strcmp(value, "workspace") == 0) {
            config.focus_wrapping = FOCUS_WRAPPING_WORKSPACE;
        } else if (boolstr(value)) {
            config.focus_wrapping = FOCUS_WRAPPING_ON;
        } else {
            config.focus_wrapping = FOCUS_WRAPPING_OFF;
        }
    }

    void force_focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        /* Legacy syntax. */
        if (boolstr(value)) {
            config.focus_wrapping = FOCUS_WRAPPING_FORCE;
        } else {
            /* For "force_focus_wrapping off", don't enable or disable
             * focus wrapping, just ensure it's not forced. */
            if (config.focus_wrapping == FOCUS_WRAPPING_FORCE) {
                config.focus_wrapping = FOCUS_WRAPPING_ON;
            }
        }
    }

    void workspace_back_and_forth(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        config.workspace_auto_back_and_forth = boolstr(value);
    }

    void force_display_urgency_hint(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long duration_ms) {
        config.workspace_urgency_timer = duration_ms / 1000.0;
    }

    void focus_on_window_activation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode) {
        if (strcmp(mode, "smart") == 0)
            config.focus_on_window_activation = FOWA_SMART;
        else if (strcmp(mode, "urgent") == 0)
            config.focus_on_window_activation = FOWA_URGENT;
        else if (strcmp(mode, "focus") == 0)
            config.focus_on_window_activation = FOWA_FOCUS;
        else if (strcmp(mode, "none") == 0)
            config.focus_on_window_activation = FOWA_NONE;
        else {
             ELOG(fmt::sprintf("Unknown focus_on_window_activation mode \"%s\", ignoring it.\n", mode));
            return;
        }

        DLOG(fmt::sprintf("Set new focus_on_window_activation mode = %i.\n",  config.focus_on_window_activation));
    }

    void title_align(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *alignment) {
        if (strcmp(alignment, "left") == 0) {
            config.title_align = Config::ALIGN_LEFT;
        } else if (strcmp(alignment, "center") == 0) {
            config.title_align = Config::ALIGN_CENTER;
        } else if (strcmp(alignment, "right") == 0) {
            config.title_align = Config::ALIGN_RIGHT;
        } else {
            assert(false);
        }
    }

    static char *current_workspace = nullptr;

    void workspace(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *workspace, const char *output) {
        /* When a new workspace line is encountered, for the first output word,
         * $workspace from the config.spec is non-NULL. Afterwards, the parser calls
         * clear_stack() because of the call line. Thus, we have to preserve the
         * workspace string. */
        if (workspace) {
            FREE(current_workspace);

            for (const auto &assignment : global.ws_assignments) {
                if (strcasecmp(assignment->name.c_str(), workspace) == 0) {
                     ELOG(fmt::sprintf("You have a duplicate workspace assignment for workspace \"%s\"\n",
                         workspace));
                    return;
                }
            }

            current_workspace = sstrdup(workspace);
        } else {
            if (!current_workspace) {
                DLOG("Both workspace and current_workspace are NULL, assuming we had an error before\n");
                return;
            }
            workspace = current_workspace;
        }

         DLOG(fmt::sprintf("Assigning workspace \"%s\" to output \"%s\"\n", workspace, output));

        auto assignment = std::make_unique<Workspace_Assignment>();
        assignment->name = std::string(workspace);
        assignment->output = std::string(output);
        global.ws_assignments.push_back(std::move(assignment));
    }

    void ipc_socket(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path) {
        free(config.ipc_socket_path);
        config.ipc_socket_path = sstrdup(path);
    }

    void restart_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path) {
        free(config.restart_state_path);
        config.restart_state_path = sstrdup(path);
    }

    void popup_during_fullscreen(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        if (strcmp(value, "ignore") == 0) {
            config.popup_during_fullscreen = PDF_IGNORE;
        } else if (strcmp(value, "leave_fullscreen") == 0) {
            config.popup_during_fullscreen = PDF_LEAVE_FULLSCREEN;
        } else {
            config.popup_during_fullscreen = PDF_SMART;
        }
    }

    void color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *color) {
        /* used for client.background only currently */
        config.client.background = draw_util_hex_to_color(global.conn, global.root_screen, color);
    }

    void color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border) {
    #define APPLY_COLORS(classname)                                                              \
        do {                                                                                     \
            if (strcmp(colorclass, "client." #classname) == 0) {                                 \
                config.client.classname.border = draw_util_hex_to_color(global.conn, global.root_screen, border);                 \
                config.client.classname.background = draw_util_hex_to_color(global.conn, global.root_screen, background);         \
                config.client.classname.text = draw_util_hex_to_color(global.conn, global.root_screen, text);                     \
                if (indicator != NULL) {                                                         \
                    config.client.classname.indicator = draw_util_hex_to_color(global.conn, global.root_screen, indicator);       \
                }                                                                                \
                if (child_border != NULL) {                                                      \
                    config.client.classname.child_border = draw_util_hex_to_color(global.conn, global.root_screen, child_border); \
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
        auto assignment = std::make_unique<OutputAssignment>();
        assignment->match = new Match(criteria_state.current_match);
        assignment->type = A_TO_OUTPUT;
        assignment->output = sstrdup(output);
        global.assignments.push_back(std::move(assignment));
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

        if (is_number && ws_name_to_number(workspace) == -1) {
             ELOG(fmt::sprintf("Could not parse initial part of \"%s\" as a number.\n", workspace));
            return;
        }

         DLOG(fmt::sprintf("New assignment, using above criteria, to workspace \"%s\".\n", workspace));
        auto assignment = std::make_unique<WorkspaceAssignment>();
        assignment->match = new Match(criteria_state.current_match);
        assignment->type = is_number ? A_TO_WORKSPACE_NUMBER : A_TO_WORKSPACE;
        assignment->workspace = sstrdup(workspace);
        global.assignments.push_back(std::move(assignment));
    }

    void no_focus(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        if (match_is_empty(criteria_state.current_match)) {
            ELOG("Match is empty, ignoring this assignment\n");
            return;
        }

        DLOG("New assignment, using above criteria, to ignore focus on manage.\n");
        auto assignment = std::make_unique<Assignment>();
        assignment->match = new Match(criteria_state.current_match);
        assignment->type = A_NO_FOCUS;
        global.assignments.push_back(std::move(assignment));
    }

    void ipc_kill_timeout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long timeout_ms) {
        ipc_set_kill_timeout(timeout_ms / 1000.0);
    }

    /*******************************************************************************
     * Bar configuration (i3bar)
     ******************************************************************************/

    static std::unique_ptr<Barconfig> current_bar;

    void bar_font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font) {
        FREE(current_bar->font);
        current_bar->font = sstrdup(font);
    }

    void bar_separator_symbol(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *separator) {
        FREE(current_bar->separator_symbol);
        current_bar->separator_symbol = sstrdup(separator);
    }

    void bar_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode) {
        current_bar->mode = (strcmp(mode, "dock") == 0 ? M_DOCK : (strcmp(mode, "hide") == 0 ? M_HIDE : M_INVISIBLE));
    }

    void bar_hidden_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *hidden_state) {
        current_bar->hidden_state = (strcmp(hidden_state, "hide") == 0 ? S_HIDE : S_SHOW);
    }

    void bar_id(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bar_id) {
        current_bar->id = sstrdup(bar_id);
    }

    void bar_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output) {
        current_bar->outputs.emplace_back(output);
    }

    void bar_verbose(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *verbose) {
        current_bar->verbose = boolstr(verbose);
    }

    void bar_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers) {
        current_bar->modifier = modifiers ? event_state_from_str(modifiers) : XCB_NONE;
    }

    static void bar_configure_binding(const char *button, const char *release, const char *command) {
        if (strncasecmp(button, "button", strlen("button")) != 0) {
             ELOG(fmt::sprintf("Bindings for a bar can only be mouse bindings, not \"%s\", ignoring.\n", button));
            return;
        }

        int input_code = atoi(button + strlen("button"));
        if (input_code < 1) {
             ELOG(fmt::sprintf("Button \"%s\" does not seem to be in format 'buttonX'.\n", button));
            return;
        }
        const bool release_bool = release != nullptr;

        for (auto &current : current_bar->bar_bindings) {
            if (current->input_code == input_code && current->release == release_bool) {
                ELOG(fmt::sprintf("command for button %s was already specified, ignoring.\n",  button));
                return;
            }
        }

        auto new_binding = std::make_unique<Barbinding>();
        new_binding->release = release_bool;
        new_binding->input_code = input_code;
        new_binding->command = command;
        current_bar->bar_bindings.push_back(std::move(new_binding));
    }

    void bar_wheel_up_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
        ELOG(fmt::sprintf("'wheel_up_cmd' is deprecated. Please us 'bindsym button4 %s' instead.\n",  command));
        bar_configure_binding("button4", nullptr, command);
    }

    void bar_wheel_down_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
        ELOG(fmt::sprintf("'wheel_down_cmd' is deprecated. Please us 'bindsym button5 %s' instead.\n",  command));
        bar_configure_binding("button5", nullptr, command);
    }

    void bar_bindsym(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *button, const char *release,
                         const char *command) {
        bar_configure_binding(button, release, command);
    }

    void bar_position(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *position) {
        current_bar->position = (strcmp(position, "top") == 0 ? P_TOP : P_BOTTOM);
    }

    void bar_i3bar_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *i3bar_command) {
        FREE(current_bar->i3bar_command);
        current_bar->i3bar_command = sstrdup(i3bar_command);
    }

    void bar_color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                       const char *background, const char *text) {
    #define APPLY_COLORS(classname)                                           \
        do {                                                                  \
            if (strcmp(colorclass, #classname) == 0) {                        \
                if (text != NULL) {                                           \
                    /* New syntax: border, background, text */                \
                    current_bar->colors.classname##_border = sstrdup(border); \
                    current_bar->colors.classname##_bg = sstrdup(background); \
                    current_bar->colors.classname##_text = sstrdup(text);     \
                } else {                                                      \
                    /* Old syntax: text, background */                        \
                    current_bar->colors.classname##_bg = sstrdup(background); \
                    current_bar->colors.classname##_text = sstrdup(border);   \
                }                                                             \
            }                                                                 \
        } while (0)

        APPLY_COLORS(focused_workspace);
        APPLY_COLORS(active_workspace);
        APPLY_COLORS(inactive_workspace);
        APPLY_COLORS(urgent_workspace);
        APPLY_COLORS(binding_mode);

    #undef APPLY_COLORS
    }

    void bar_socket_path(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *socket_path) {
        FREE(current_bar->socket_path);
        current_bar->socket_path = sstrdup(socket_path);
    }

    void bar_tray_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output) {
        current_bar->tray_outputs.emplace_back(output);
    }

    void bar_tray_padding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long padding_px) {
        current_bar->tray_padding = padding_px;
    }

    void bar_color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *color) {
        if (strcmp(colorclass, "background") == 0)
            current_bar->colors.background = sstrdup(color);
        else if (strcmp(colorclass, "separator") == 0)
            current_bar->colors.separator = sstrdup(color);
        else if (strcmp(colorclass, "statusline") == 0)
            current_bar->colors.statusline = sstrdup(color);
        else if (strcmp(colorclass, "focused_background") == 0)
            current_bar->colors.focused_background = sstrdup(color);
        else if (strcmp(colorclass, "focused_separator") == 0)
            current_bar->colors.focused_separator = sstrdup(color);
        else
            current_bar->colors.focused_statusline = sstrdup(color);
    }

    void bar_status_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command) {
        FREE(current_bar->status_command);
        current_bar->status_command = sstrdup(command);
    }

    void bar_binding_mode_indicator(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        current_bar->hide_binding_mode_indicator = !boolstr(value);
    }

    void bar_workspace_buttons(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        current_bar->hide_workspace_buttons = !boolstr(value);
    }

    void bar_workspace_min_width(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width) {
        current_bar->workspace_min_width = width;
    }

    void bar_strip_workspace_numbers(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        current_bar->strip_workspace_numbers = boolstr(value);
    }

    void bar_strip_workspace_name(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value) {
        current_bar->strip_workspace_name = boolstr(value);
    }

    void bar_start(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        current_bar = std::make_unique<Barconfig>();
        current_bar->tray_padding = 2;
        current_bar->modifier = XCB_KEY_BUT_MASK_MOD_4;
    }

    void bar_finish(struct criteria_state &criteria_state, struct ConfigResultIR &result) {
        DLOG("\t new bar configuration finished, saving.\n");
        /* Generate a unique ID for this bar if not already configured */
        if (current_bar->id == nullptr)
            sasprintf(&current_bar->id, "bar-%d", config.number_barconfigs);

        config.number_barconfigs++;

        /* If no font was explicitly set, we use the i3 font as default */
        if (current_bar->font == nullptr && font_pattern != nullptr)
            current_bar->font = sstrdup(font_pattern);

        barconfigs.push_back(std::move(current_bar));
        /* Simply reset the pointer, but don't free the resources. */
        current_bar = nullptr;
    }
}