module;
struct criteria_state;
export module i3_config_base:base_config_applier;

import std;

export class BaseConfigApplier {
  public:
    virtual ~BaseConfigApplier() = default;

    virtual criteria_state *criteria_create(int _state) = 0;

    virtual void criteria_init(criteria_state *criteria_state, int _state) = 0;

    virtual int criteria_pop_state(criteria_state *criteria_state) = 0;

    virtual void criteria_add(criteria_state *criteria_state, char const *ctype, char const *cvalue) = 0;

    virtual void font(std::string const &font) = 0;

    virtual void exec(std::string const &exectype, bool no_startup_id, std::string const &command) = 0;

    virtual void gaps(std::string const &workspace, std::string const &type, long const value) = 0;

    virtual void smart_borders(std::string const &enable) = 0;

    virtual void smart_gaps(std::string const &enable) = 0;

    virtual void for_window(criteria_state *criteria_state, std::string const &command) = 0;

    virtual void floating_minimum_size(long width, long height) = 0;

    virtual void floating_maximum_size(long width, long height) = 0;

    virtual void default_orientation(std::string const &orientation) = 0;

    virtual void workspace_layout(std::string const &layout) = 0;

    virtual void workspace_back_and_forth(std::string const &value) = 0;

    virtual void focus_follows_mouse(std::string const &value) = 0;

    virtual void mouse_warping(std::string const &value) = 0;

    virtual void focus_wrapping(std::string const &value) = 0;

    virtual void force_focus_wrapping(std::string const &value) = 0;

    virtual void force_display_urgency_hint(long duration_ms) = 0;

    virtual void focus_on_window_activation(std::string const &mode) = 0;

    virtual void title_align(std::string const &alignment) = 0;

    virtual void hide_edge_borders(std::string const &borders) = 0;

    virtual void assign_output(criteria_state *criteria_state, std::string const &output) = 0;

    virtual void assign(criteria_state *criteria_state, std::string const &workspace, bool is_number) = 0;

    virtual void no_focus(criteria_state *criteria_state) = 0;

    virtual void ipc_socket(std::string const &path) = 0;

    virtual void ipc_kill_timeout(long timeout_ms) = 0;

    virtual void tiling_drag(std::string const &value) = 0;

    virtual void restart_state(std::string const &path) = 0;

    virtual void popup_during_fullscreen(std::string const &value) = 0;

    virtual void color(std::string const &colorclass, std::string const &border, std::string const &background, std::string const &text, std::string const &indicator, std::string const &child_border) = 0;

    virtual void color_single(std::string const &colorclass, std::string const &color) = 0;

    virtual void floating_modifier(std::string const &modifiers) = 0;

    virtual void default_border(std::string const &windowtype, std::string const &border, long width) = 0;

    virtual void workspace(std::string const &workspace, std::string const &output) = 0;

    virtual void binding(std::string const &bindtype, std::string const &modifiers, std::string const &key, bool release, bool border, bool whole_window, bool exclude_titlebar, std::string const &command) = 0;

    virtual void enter_mode(bool pango_markup, std::string const &mode) = 0;

    virtual void mode_binding(std::string const &bindtype, std::string const &modifiers, std::string const &key, bool release, bool border, bool whole_window, bool exclude_titlebar, std::string const &command) = 0;

    virtual void bar_height(long const height) = 0;

    virtual void bar_padding_one(long const all) = 0;

    virtual void bar_padding_two(long const top_and_bottom, long const right_and_left) = 0;

    virtual void bar_padding_three(long const top, long const right_and_left, long const bottom) = 0;

    virtual void bar_padding_four(long const top, long const right, long const bottom, long const left) = 0;
};
