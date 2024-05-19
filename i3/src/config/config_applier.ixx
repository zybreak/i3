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
#include <cassert>
#include <cerrno>
#include <climits>

#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "i3.h"

#include <wordexp.h>
export module i3:config_applier;

import std;
import utils;
import i3_config_base;
import :criteria_state;

/*******************************************************************************
 * Include functions.
 ******************************************************************************/
export class ConfigApplier : public BaseConfigApplier {
private:
    std::string current_mode{};
    bool current_mode_pango_markup;

public:
    criteria_state* criteria_create(int _state) override;

    void criteria_init(criteria_state *cs, int _state) override;

    int criteria_pop_state(criteria_state *cs) override;

    void criteria_add(criteria_state *cs, const char *ctype, const char *cvalue) override;

    void font(const std::string &font) override;

    void binding(const std::string &bindtype, const std::string &modifiers,
                     const std::string &key, bool release, bool border, bool whole_window,
                     bool exclude_titlebar, const std::string &command) override;

    void mode_binding(const std::string &bindtype, const std::string &modifiers,
                          const std::string &key, bool release, bool border, bool whole_window,
                          bool exclude_titlebar, const std::string &command) override;

    void enter_mode(bool pango_markup, const std::string &modename) override;

    void exec(const std::string &exectype, bool no_startup_id,
                  const std::string &command) override;

    void gaps(const std::string &workspace, const std::string &type, const long value) override;

    void smart_borders(const std::string &enable) override;

    void smart_gaps(const std::string &enable) override;

    void for_window(criteria_state *criteria_state, const std::string &command) override;

    void floating_minimum_size(long width, long height) override;

    void floating_maximum_size(long width, long height) override;

    void floating_modifier(const std::string &modifiers) override;

    void default_orientation(const std::string &orientation) override;

    void workspace_layout(const std::string &layout) override;

    void default_border(const std::string &windowtype, const std::string &border, long width) override;

    void hide_edge_borders(const std::string &borders) override;

    void focus_follows_mouse(const std::string &value) override;

    void mouse_warping(const std::string &value) override;

    void focus_wrapping(const std::string &value) override;

    void force_focus_wrapping(const std::string &value) override;

    void workspace_back_and_forth(const std::string &value) override;

    void force_display_urgency_hint(long duration_ms) override;

    void focus_on_window_activation(const std::string &mode) override;

    void title_align(const std::string &alignment) override;

    void workspace(const std::string &workspace, const std::string &output) override;

    void ipc_socket(const std::string &path) override;

    void restart_state(const std::string &path) override;

    void popup_during_fullscreen(const std::string &value) override;

    void color_single(const std::string &colorclass, const std::string &color) override;

    void color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) override;

    void assign_output(criteria_state *criteria_state, const std::string &output) override;

    void assign(criteria_state *criteria_state, const std::string &workspace, bool is_number) override;

    void no_focus(criteria_state *criteria_state) override;

    void ipc_kill_timeout(long timeout_ms) override;

    void tiling_drag(const std::string &value) override;

    void bar_height(const long height) override;

    void bar_padding_one(const long all) override;

    void bar_padding_two(const long top_and_bottom, const long right_and_left) override;

    void bar_padding_three(const long top, const long right_and_left, const long bottom) override;

    void bar_padding_four(const long top, const long right, const long bottom, const long left) override;

};