module;
struct criteria_state;
#include <string>
#include <iostream>
export module i3_config_base:base_config_applier;

export class BaseConfigApplier {
public:

    virtual ~BaseConfigApplier() = default;

    virtual criteria_state* criteria_init(int _state) = 0;

    virtual int criteria_pop_state(criteria_state *criteria_state) = 0;

    virtual void criteria_add(criteria_state *criteria_state, const char *ctype, const char *cvalue) = 0;

    virtual void font(const std::string &font) = 0;

    virtual void exec(const std::string &exectype, bool no_startup_id, const std::string &command) = 0;

    virtual void for_window(criteria_state *criteria_state, const std::string &command) = 0;

    virtual void floating_minimum_size(long width, long height) = 0;

    virtual void floating_maximum_size(long width, long height) = 0;

    virtual void default_orientation(const std::string &orientation) = 0;

    virtual void workspace_layout(const std::string &layout) = 0;

    virtual void workspace_back_and_forth(const std::string &value) = 0;

    virtual void focus_follows_mouse(const std::string &value) = 0;

    virtual void mouse_warping(const std::string &value) = 0;

    virtual void focus_wrapping(const std::string &value) = 0;

    virtual void force_focus_wrapping(const std::string &value) = 0;

    virtual void force_display_urgency_hint(long duration_ms) = 0;

    virtual void focus_on_window_activation(const std::string &mode) = 0;

    virtual void title_align(const std::string &alignment) = 0;

    virtual void hide_edge_borders(const std::string &borders) = 0;

    virtual void assign_output(criteria_state *criteria_state, const std::string &output) = 0;

    virtual void assign(criteria_state *criteria_state, const std::string &workspace, bool is_number) = 0;

    virtual void no_focus(criteria_state *criteria_state) = 0;

    virtual void ipc_socket(const std::string &path) = 0;

    virtual void ipc_kill_timeout(long timeout_ms) = 0;

    virtual void restart_state(const std::string &path) = 0;

    virtual void popup_during_fullscreen(const std::string &value) = 0;

    virtual void color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) = 0;

    virtual void color_single(const std::string &colorclass, const std::string &color) = 0;

    virtual void floating_modifier(const std::string &modifiers) = 0;

    virtual void default_border(const std::string &windowtype, const std::string &border, long width) = 0;

    virtual void workspace(const std::string &workspace, const std::string &output) = 0;

    virtual void binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) = 0;

    virtual void enter_mode(bool pango_markup, const std::string &mode) = 0;

    virtual void mode_binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) = 0;
};
