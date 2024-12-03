/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.c: all config storing functions (see config_parser.c)
 *
 */
module;
#include <xcb/xcb.h>

#include <wordexp.h>
export module i3:config_applier;

import std;
import utils;
import i3_config_base;
import :criteria_state;

class Config;

/*******************************************************************************
 * Include functions.
 ******************************************************************************/
export class ConfigApplier : public BaseConfigApplier {
  private:
    std::string current_mode{};
    bool current_mode_pango_markup;
    Config *config;

  public:
    ConfigApplier(Config *config)
        : config(config) {};
    criteria_state *criteria_create(int _state) override;

    void criteria_init(criteria_state *cs, int _state) override;

    int criteria_pop_state(criteria_state *cs) override;

    void criteria_add(criteria_state *cs, char const *ctype, char const *cvalue) override;

    void font(std::string const &font) override;

    void binding(std::string const &bindtype, std::string const &modifiers,
                 std::string const &key, bool release, bool border, bool whole_window,
                 bool exclude_titlebar, std::string const &command) override;

    void mode_binding(std::string const &bindtype, std::string const &modifiers,
                      std::string const &key, bool release, bool border, bool whole_window,
                      bool exclude_titlebar, std::string const &command) override;

    void enter_mode(bool pango_markup, std::string const &modename) override;

    void exec(std::string const &exectype, bool no_startup_id,
              std::string const &command) override;

    void gaps(std::string const &workspace, std::string const &type, long const value) override;

    void smart_borders(std::string const &enable) override;

    void smart_gaps(std::string const &enable) override;

    void for_window(criteria_state *criteria_state, std::string const &command) override;

    void floating_minimum_size(long width, long height) override;

    void floating_maximum_size(long width, long height) override;

    void floating_modifier(std::string const &modifiers) override;
    
    void tiling_drag_swap_modifier(std::string const &modifiers) override;

    void default_orientation(std::string const &orientation) override;

    void workspace_layout(std::string const &layout) override;

    void default_border(std::string const &windowtype, std::string const &border, long width) override;

    void hide_edge_borders(std::string const &borders) override;

    void focus_follows_mouse(std::string const &value) override;

    void mouse_warping(std::string const &value) override;

    void focus_wrapping(std::string const &value) override;

    void force_focus_wrapping(std::string const &value) override;

    void workspace_back_and_forth(std::string const &value) override;

    void force_display_urgency_hint(long duration_ms) override;

    void focus_on_window_activation(std::string const &mode) override;

    void title_align(std::string const &alignment) override;

    void workspace(std::string const &workspace, std::string const &output) override;

    void ipc_socket(std::string const &path) override;

    void restart_state(std::string const &path) override;

    void popup_during_fullscreen(std::string const &value) override;

    void color_single(std::string const &colorclass, std::string const &color) override;

    void color(std::string const &colorclass, std::string const &border, std::string const &background, std::string const &text, std::string const &indicator, std::string const &child_border) override;

    void assign_output(criteria_state *criteria_state, std::string const &output) override;

    void assign(criteria_state *criteria_state, std::string const &workspace, bool is_number) override;

    void no_focus(criteria_state *criteria_state) override;

    void ipc_kill_timeout(long timeout_ms) override;

    void tiling_drag(std::string const &value) override;

    void bar_height(long const height) override;

    void bar_padding_one(long const all) override;

    void bar_padding_two(long const top_and_bottom, long const right_and_left) override;

    void bar_padding_three(long const top, long const right_and_left, long const bottom) override;

    void bar_padding_four(long const top, long const right, long const bottom, long const left) override;
};
