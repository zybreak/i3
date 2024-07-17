/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.h: all config storing functions (see config_parser.c)
 *
 */
module;
struct criteria_state;
export module i3_config_old:config_directives;

import i3_config_base;

export namespace cfg {
/* The following functions are called by the config parser, see
 * parser-specs/config.spec. They get the parsed parameters and store them in
 * our data structures, e.g. cfg_font gets a font name and stores it in
 * config.font.
 *
 * Since they are so similar, individual comments were omitted. */

    void criteria_init(criteria_state *criteria_state, ConfigResultIR &result, int _state);

    void criteria_add(criteria_state *criteria_state, ConfigResultIR &result, const char *ctype, const char *cvalue);

    void criteria_pop_state(criteria_state *criteria_state, ConfigResultIR &result);

    void include(criteria_state *criteria_state, ConfigResultIR &result, const char *pattern);

    void font(criteria_state *criteria_state, ConfigResultIR &result, const char *font);

    void exec(criteria_state *criteria_state, ConfigResultIR &result, const char *exectype, const char *no_startup_id,
                  const char *command);

    void for_window(criteria_state *criteria_state, ConfigResultIR &result, const char *command);

    void gaps(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, const char *type, const long value);

    void smart_borders(criteria_state *criteria_state, ConfigResultIR &result, const char *enable);

    void smart_gaps(criteria_state *criteria_state, ConfigResultIR &result, const char *enable);

    void
    floating_minimum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height);

    void
    floating_maximum_size(criteria_state *criteria_state, ConfigResultIR &result, const long width, const long height);

    void default_orientation(criteria_state *criteria_state, ConfigResultIR &result, const char *orientation);

    void workspace_layout(criteria_state *criteria_state, ConfigResultIR &result, const char *layout);

    void workspace_back_and_forth(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void focus_follows_mouse(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void mouse_warping(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void force_focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void force_display_urgency_hint(criteria_state *criteria_state, ConfigResultIR &result, const long duration_ms);

    void focus_on_window_activation(criteria_state *criteria_state, ConfigResultIR &result, const char *mode);

    void title_align(criteria_state *criteria_state, ConfigResultIR &result, const char *alignment);

    void hide_edge_borders(criteria_state *criteria_state, ConfigResultIR &result, const char *borders);

    void assign_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output);

    void assign(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, bool is_number);

    void no_focus(criteria_state *criteria_state, ConfigResultIR &result);

    void ipc_socket(criteria_state *criteria_state, ConfigResultIR &result, const char *path);

    void ipc_kill_timeout(criteria_state *criteria_state, ConfigResultIR &result, const long timeout_ms);

    void tiling_drag(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void restart_state(criteria_state *criteria_state, ConfigResultIR &result, const char *path);

    void popup_during_fullscreen(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void color(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *border,
                   const char *background, const char *text, const char *indicator, const char *child_border);

    void
    color_single(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *color);

    void floating_modifier(criteria_state *criteria_state, ConfigResultIR &result, const char *modifiers);

    void
    default_border(criteria_state *criteria_state, ConfigResultIR &result, const char *windowtype, const char *border,
                       const long width);

    void workspace(criteria_state *criteria_state, ConfigResultIR &result, const char *workspace, const char *output);

    void binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command);

    void
    enter_mode(criteria_state *criteria_state, ConfigResultIR &result, const char *pango_markup, const char *mode);

    void
    mode_binding(criteria_state *criteria_state, ConfigResultIR &result, const char *bindtype, const char *modifiers,
                     const char *key, const char *release, const char *border, const char *whole_window,
                     const char *exclude_titlebar, const char *command);

    void bar_font(criteria_state *criteria_state, ConfigResultIR &result, const char *font);

    void bar_separator_symbol(criteria_state *criteria_state, ConfigResultIR &result, const char *separator);

    void bar_mode(criteria_state *criteria_state, ConfigResultIR &result, const char *mode);

    void bar_hidden_state(criteria_state *criteria_state, ConfigResultIR &result, const char *hidden_state);

    void bar_id(criteria_state *criteria_state, ConfigResultIR &result, const char *bar_id);

    void bar_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output);

    void bar_verbose(criteria_state *criteria_state, ConfigResultIR &result, const char *verbose);

    void bar_height(criteria_state *criteria_state, ConfigResultIR &result, const long height);

    void bar_padding_one(criteria_state *criteria_state, ConfigResultIR &result, const long all);

    void bar_padding_two(criteria_state *criteria_state, ConfigResultIR &result, const long top_and_bottom, const long right_and_left);

    void bar_padding_three(criteria_state *criteria_state, ConfigResultIR &result, const long top, const long right_and_left, const long bottom);

    void bar_padding_four(criteria_state *criteria_state, ConfigResultIR &result, const long top, const long right, const long bottom, const long left);

    void bar_modifier(criteria_state *criteria_state, ConfigResultIR &result, const char *modifiers);

    void bar_wheel_up_cmd(criteria_state *criteria_state, ConfigResultIR &result, const char *command);

    void bar_wheel_down_cmd(criteria_state *criteria_state, ConfigResultIR &result, const char *command);

    void bar_bindsym(criteria_state *criteria_state, ConfigResultIR &result, const char *button, const char *release,
                         const char *command);

    void bar_position(criteria_state *criteria_state, ConfigResultIR &result, const char *position);

    void bar_i3bar_command(criteria_state *criteria_state, ConfigResultIR &result, const char *i3bar_command);

    void bar_color(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass, const char *border,
                       const char *background, const char *text);

    void bar_socket_path(criteria_state *criteria_state, ConfigResultIR &result, const char *socket_path);

    void bar_tray_output(criteria_state *criteria_state, ConfigResultIR &result, const char *output);

    void bar_tray_padding(criteria_state *criteria_state, ConfigResultIR &result, const long spacing_px);

    void bar_color_single(criteria_state *criteria_state, ConfigResultIR &result, const char *colorclass,
                         const char *color);

    void bar_status_command(criteria_state *criteria_state, ConfigResultIR &result, const char *command);

    void bar_workspace_command(criteria_state *criteria_state, ConfigResultIR &result, const char *command);

    void bar_binding_mode_indicator(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void bar_workspace_buttons(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void bar_workspace_min_width(criteria_state *criteria_state, ConfigResultIR &result, const long width);

    void bar_strip_workspace_numbers(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void bar_strip_workspace_name(criteria_state *criteria_state, ConfigResultIR &result, const char *value);

    void bar_start(criteria_state *criteria_state, ConfigResultIR &result);

    void bar_finish(criteria_state *criteria_state, ConfigResultIR &result);
}