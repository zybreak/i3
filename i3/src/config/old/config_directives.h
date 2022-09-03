/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.h: all config storing functions (see config_parser.c)
 *
 */
#pragma once

#include <config.h>

#include "config_parser.h"

namespace cfg {
/* The following functions are called by the config parser, see
 * parser-specs/config.spec. They get the parsed parameters and store them in
 * our data structures, e.g. cfg_font gets a font name and stores it in
 * config.font.
 *
 * Since they are so similar, individual comments were omitted. */

    void criteria_init(struct criteria_state &criteria_state, struct ConfigResultIR &result, int _state);

    void criteria_add(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *ctype, const char *cvalue);

    void criteria_pop_state(struct criteria_state &criteria_state, struct ConfigResultIR &result);

    void include(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *pattern);

    void font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font);

    void exec(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command);

    void for_window(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command);

    void
    floating_minimum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height);

    void
    floating_maximum_size(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width, const long height);

    void default_orientation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *orientation);

    void workspace_layout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *layout);

    void workspace_back_and_forth(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void focus_follows_mouse(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void mouse_warping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void force_focus_wrapping(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void force_display_urgency_hint(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long duration_ms);

    void focus_on_window_activation(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode);

    void title_align(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *alignment);

    void hide_edge_borders(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *borders);

    void assign_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output);

    void assign(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *workspace, bool is_number);

    void no_focus(struct criteria_state &criteria_state, struct ConfigResultIR &result);

    void ipc_socket(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path);

    void ipc_kill_timeout(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long timeout_ms);

    void restart_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *path);

    void popup_during_fullscreen(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border);

    void
    color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *color);

    void floating_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers);

    void
    default_border(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *windowtype, const char *border,
                       const long width);

    void workspace(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *workspace, const char *output);

    void binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command);

    void
    enter_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *pango_markup, const char *mode);

    void
    mode_binding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command);

    void bar_font(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *font);

    void bar_separator_symbol(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *separator);

    void bar_mode(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *mode);

    void bar_hidden_state(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *hidden_state);

    void bar_id(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *bar_id);

    void bar_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output);

    void bar_verbose(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *verbose);

    void bar_modifier(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *modifiers);

    void bar_wheel_up_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command);

    void bar_wheel_down_cmd(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command);

    void bar_bindsym(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *button, const char *release,
                         const char *command);

    void bar_position(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *position);

    void bar_i3bar_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *i3bar_command);

    void bar_color(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass, const char *border,
                       const char *background, const char *text);

    void bar_socket_path(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *socket_path);

    void bar_tray_output(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *output);

    void bar_tray_padding(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long spacing_px);

    void bar_color_single(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *colorclass,
                         const char *color);

    void bar_status_command(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *command);

    void bar_binding_mode_indicator(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void bar_workspace_buttons(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void bar_workspace_min_width(struct criteria_state &criteria_state, struct ConfigResultIR &result, const long width);

    void bar_strip_workspace_numbers(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void bar_strip_workspace_name(struct criteria_state &criteria_state, struct ConfigResultIR &result, const char *value);

    void bar_start(struct criteria_state &criteria_state, struct ConfigResultIR &result);

    void bar_finish(struct criteria_state &criteria_state, struct ConfigResultIR &result);
}