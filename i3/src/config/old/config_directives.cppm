/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
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

    void criteria_add(criteria_state *criteria_state, ConfigResultIR &result, char const *ctype, char const *cvalue);

    void criteria_pop_state(criteria_state *criteria_state, ConfigResultIR &result);

    void include(criteria_state *criteria_state, ConfigResultIR &result, char const *pattern);

    void font(criteria_state *criteria_state, ConfigResultIR &result, char const *font);

    void exec(criteria_state *criteria_state, ConfigResultIR &result, char const *exectype, char const *no_startup_id,
              char const *command);

    void for_window(criteria_state *criteria_state, ConfigResultIR &result, char const *command);

    void gaps(criteria_state *criteria_state, ConfigResultIR &result, char const *workspace, char const *type, long const value);

    void smart_borders(criteria_state *criteria_state, ConfigResultIR &result, char const *enable);

    void smart_gaps(criteria_state *criteria_state, ConfigResultIR &result, char const *enable);

    void
    floating_minimum_size(criteria_state *criteria_state, ConfigResultIR &result, long const width, long const height);

    void
    floating_maximum_size(criteria_state *criteria_state, ConfigResultIR &result, long const width, long const height);

    void default_orientation(criteria_state *criteria_state, ConfigResultIR &result, char const *orientation);

    void workspace_layout(criteria_state *criteria_state, ConfigResultIR &result, char const *layout);

    void workspace_back_and_forth(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void focus_follows_mouse(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void mouse_warping(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void force_focus_wrapping(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void force_display_urgency_hint(criteria_state *criteria_state, ConfigResultIR &result, long const duration_ms);

    void focus_on_window_activation(criteria_state *criteria_state, ConfigResultIR &result, char const *mode);

    void title_align(criteria_state *criteria_state, ConfigResultIR &result, char const *alignment);

    void hide_edge_borders(criteria_state *criteria_state, ConfigResultIR &result, char const *borders);

    void assign_output(criteria_state *criteria_state, ConfigResultIR &result, char const *output);

    void assign(criteria_state *criteria_state, ConfigResultIR &result, char const *workspace, bool is_number);

    void no_focus(criteria_state *criteria_state, ConfigResultIR &result);

    void ipc_socket(criteria_state *criteria_state, ConfigResultIR &result, char const *path);

    void ipc_kill_timeout(criteria_state *criteria_state, ConfigResultIR &result, long const timeout_ms);

    void tiling_drag(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void restart_state(criteria_state *criteria_state, ConfigResultIR &result, char const *path);

    void popup_during_fullscreen(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void color(criteria_state *criteria_state, ConfigResultIR &result, char const *colorclass, char const *border,
               char const *background, char const *text, char const *indicator, char const *child_border);

    void
    color_single(criteria_state *criteria_state, ConfigResultIR &result, char const *colorclass, char const *color);

    void floating_modifier(criteria_state *criteria_state, ConfigResultIR &result, char const *modifiers);

    void tiling_drag_swap_modifier(criteria_state *criteria_state, ConfigResultIR &result, char const *modifiers);
    
    void
    default_border(criteria_state *criteria_state, ConfigResultIR &result, char const *windowtype, char const *border,
                   long const width);

    void workspace(criteria_state *criteria_state, ConfigResultIR &result, char const *workspace, char const *output);

    void binding(criteria_state *criteria_state, ConfigResultIR &result, char const *bindtype, char const *modifiers,
                 char const *key, char const *release, char const *border, char const *whole_window,
                 char const *exclude_titlebar, char const *command);

    void
    enter_mode(criteria_state *criteria_state, ConfigResultIR &result, char const *pango_markup, char const *mode);

    void
    mode_binding(criteria_state *criteria_state, ConfigResultIR &result, char const *bindtype, char const *modifiers,
                 char const *key, char const *release, char const *border, char const *whole_window,
                 char const *exclude_titlebar, char const *command);

    void bar_font(criteria_state *criteria_state, ConfigResultIR &result, char const *font);

    void bar_separator_symbol(criteria_state *criteria_state, ConfigResultIR &result, char const *separator);

    void bar_mode(criteria_state *criteria_state, ConfigResultIR &result, char const *mode);

    void bar_hidden_state(criteria_state *criteria_state, ConfigResultIR &result, char const *hidden_state);

    void bar_id(criteria_state *criteria_state, ConfigResultIR &result, char const *bar_id);

    void bar_output(criteria_state *criteria_state, ConfigResultIR &result, char const *output);

    void bar_verbose(criteria_state *criteria_state, ConfigResultIR &result, char const *verbose);

    void bar_height(criteria_state *criteria_state, ConfigResultIR &result, long const height);

    void bar_padding_one(criteria_state *criteria_state, ConfigResultIR &result, long const all);

    void bar_padding_two(criteria_state *criteria_state, ConfigResultIR &result, long const top_and_bottom, long const right_and_left);

    void bar_padding_three(criteria_state *criteria_state, ConfigResultIR &result, long const top, long const right_and_left, long const bottom);

    void bar_padding_four(criteria_state *criteria_state, ConfigResultIR &result, long const top, long const right, long const bottom, long const left);

    void bar_modifier(criteria_state *criteria_state, ConfigResultIR &result, char const *modifiers);

    void bar_wheel_up_cmd(criteria_state *criteria_state, ConfigResultIR &result, char const *command);

    void bar_wheel_down_cmd(criteria_state *criteria_state, ConfigResultIR &result, char const *command);

    void bar_bindsym(criteria_state *criteria_state, ConfigResultIR &result, char const *button, char const *release,
                     char const *command);

    void bar_position(criteria_state *criteria_state, ConfigResultIR &result, char const *position);

    void bar_i3bar_command(criteria_state *criteria_state, ConfigResultIR &result, char const *i3bar_command);

    void bar_color(criteria_state *criteria_state, ConfigResultIR &result, char const *colorclass, char const *border,
                   char const *background, char const *text);

    void bar_socket_path(criteria_state *criteria_state, ConfigResultIR &result, char const *socket_path);

    void bar_tray_output(criteria_state *criteria_state, ConfigResultIR &result, char const *output);

    void bar_tray_padding(criteria_state *criteria_state, ConfigResultIR &result, long const spacing_px);

    void bar_color_single(criteria_state *criteria_state, ConfigResultIR &result, char const *colorclass,
                          char const *color);

    void bar_status_command(criteria_state *criteria_state, ConfigResultIR &result, char const *command);

    void bar_workspace_command(criteria_state *criteria_state, ConfigResultIR &result, char const *command);

    void bar_binding_mode_indicator(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void bar_workspace_buttons(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void bar_workspace_min_width(criteria_state *criteria_state, ConfigResultIR &result, long const width);

    void bar_strip_workspace_numbers(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void bar_strip_workspace_name(criteria_state *criteria_state, ConfigResultIR &result, char const *value);

    void bar_start(criteria_state *criteria_state, ConfigResultIR &result);

    void bar_finish(criteria_state *criteria_state, ConfigResultIR &result);
}  // namespace cfg
