#pragma once

#include <string>
#include <iostream>
#include "../i3/src/include/match.h"
#include "../i3/src/config/new/base_config_applier.h"

using namespace std::literals;


class ConfigApplierAdapter : public BaseConfigApplier {
private:
    static std::string bool2str(bool b, const std::string &s) {
        return (b) ? s : "(null)"s;
    }

    static std::string empty2null(const std::string &s) {
        return (s.empty()) ? "(null)"s : s;
    }
public:

    void font(const std::string &font) override {
        std::cout << "cfg::font(" << font << ")" << std::endl;
    }

    void exec(const std::string &exectype, bool no_startup_id, const std::string &command) override {
        std::cout << "cfg::exec(" << exectype << ", " << bool2str(no_startup_id, "--no-startup-id") << ", " << command << ")" << std::endl;
    }

    void for_window(Match &current_match, const std::string &command) override {
        std::cout << "cfg::for_window(" << command << ")" << std::endl;
    }

    void floating_minimum_size(long width, long height) override {
        std::cout << "cfg::floating_minimum_size(" << width << ", " << height << ")" << std::endl;
    }

    void floating_maximum_size(long width, long height) override {
        std::cout << "cfg::floating_maximum_size(" << width << ", " << height << ")" << std::endl;
    }

    void default_orientation(const std::string &orientation) override {}

    void workspace_layout(const std::string &layout) override {
        std::cout << "cfg::workspace_layout(" << layout << ")" << std::endl;
    }

    void workspace_back_and_forth(const std::string &value) override {
        std::cout << "workspace_back_and_forth " << value << std::endl;
    }

    void focus_follows_mouse(const std::string &value) override {
        std::cout << "cfg::focus_follows_mouse(" << value << ")" << std::endl;
    }

    void mouse_warping(const std::string &value) override {
        std::cout << "cfg::mouse_warping(" << value << ")" << std::endl;
    }

    void focus_wrapping(const std::string &value) override {
        std::cout << "cfg::focus_wrapping(" << value << ")" << std::endl;
    }

    void force_focus_wrapping(const std::string &value) override {
        std::cout << "cfg::force_focus_wrapping(" << value << ")" << std::endl;
    }

    void force_display_urgency_hint(long duration_ms) override {
        std::cout << "cfg::force_display_urgency_hint(" << duration_ms << ")" << std::endl;
    }

    void focus_on_window_activation(const std::string &mode) override {
        std::cout << "cfg::focus_on_window_activation(" << mode << ")" << std::endl;
    }

    void title_align(const std::string &alignment) override {
        std::cout << "cfg::title_align(" << alignment << ")" << std::endl;
    }

    void hide_edge_borders(const std::string &borders) override {
        std::cout << "cfg::hide_edge_borders(" << borders << ")" << std::endl;
    }

    void assign_output(Match &current_match, const std::string &output) override {
        std::cout << "cfg::assign_output(" << output << ")" << std::endl;
    }

    void assign(Match &current_match, const std::string &workspace, bool is_number) override {
        std::cout << "cfg::assign(" << workspace << ", " << is_number << ")" << std::endl;
    }

    void no_focus(Match &current_match) override {
        std::cout << "cfg::no_focus()" << std::endl;
    }

    void ipc_socket(const std::string &path) override {
        std::cout << "cfg::ipc_socket(" << path << ")" << std::endl;
    }

    void ipc_kill_timeout(long timeout_ms) override {
        std::cout << "cfg::ipc_kill_timeout(" << timeout_ms << ")" << std::endl;
    }

    void restart_state(const std::string &path) override {
        std::cout << "cfg::restart_state(" << path << ")" << std::endl;
    }

    void popup_during_fullscreen(const std::string &value) override {
        std::cout << "cfg::popup_during_fullscreen(" << value << ")" << std::endl;
    }

    void color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) override {
        std::cout << "color " << std::endl;
    }

    void color_single(const std::string &colorclass, const std::string &color) override {
        std::cout << "cfg::color_single(" << colorclass << ", " << color << ")" << std::endl;
    }

    void floating_modifier(const std::string &modifiers) override {
        std::cout << "cfg::floating_modifier(" << modifiers << ")" << std::endl;
    }

    void default_border(const std::string &windowtype, const std::string &border, long width) override {
        std::cout << "cfg::default_border(" << windowtype << ", " << border << ", " << width << ")" << std::endl;
    }

    void workspace(const std::string &workspace, const std::string &output) override {
        std::cout << "cfg::workspace(" << workspace << ", " << output << ")" << std::endl;
    }

    void binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) override {
        std::cout
            << "cfg::binding("
            << bindtype << ", "
            << empty2null(modifiers) << ", "
            << key << ", "
            << bool2str(release, "--release") << ", "
            << bool2str(border, "--border") << ", "
            << bool2str(whole_window, "--whole-window") << ", "
            << bool2str(exclude_titlebar, "--exclude-titlebar") << ", "
            << command
            << ")"
            << std::endl;
    }

    void enter_mode(bool pango_markup, const std::string &mode) override {
        std::cout << "cfg::enter_mode(" << bool2str(pango_markup, "--pango-markup") << ", " << mode << ")" << std::endl;
    }

    void mode_binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) override {
        std::cout
            << "cfg::mode_binding("
            << bindtype << ", "
            << empty2null(modifiers) << ", "
            << key << ", "
            << bool2str(release, "--release") << ", "
            << bool2str(border, "--border") << ", "
            << bool2str(whole_window, "--whole-window") << ", "
            << bool2str(exclude_titlebar, "--exclude-titlebar") << ", "
            << command
            << ")"
            << std::endl;
    }
};
