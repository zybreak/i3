#pragma once
#include <fmt/core.h>

import i3;
import i3_config_base;
import i3_config_new;

using namespace std::literals;

class ConfigApplierAdapter : public BaseConfigApplier {
   private:
    std::ostream &out;

    static std::string bool2str(bool b, const std::string &s) {
        return (b) ? s : "(null)"s;
    }

    static std::string empty2null(const std::string &s) {
        return (s.empty()) ? "(null)"s : s;
    }
   public:
    explicit ConfigApplierAdapter(std::ostream &out) : out(out) {
    }

    criteria_state* criteria_create(int _state) override {
        criteria_state *cs = new criteria_state(_state);

        cs->current_match = Match();

        return cs;
    }

    void criteria_init(criteria_state *cs, int _state) override {
        cs->criteria_next_state = _state;

        cs->current_match = Match();
    }

    int criteria_pop_state(criteria_state *cs) override {
        return cs->criteria_next_state;
    }

    void criteria_add(criteria_state *cs, const char *ctype, const char *cvalue) override {
        cs->current_match.parse_property(ctype, cvalue);
        out << "cfg::criteria_add(" << ctype << ", " << cvalue << ")" << std::endl;
    }

    void font(const std::string &font) override {
        out << "cfg::font(" << font << ")" << std::endl;
    }

    void exec(const std::string &exectype, bool no_startup_id, const std::string &command) override {
        out << "cfg::exec(" << exectype << ", " << bool2str(no_startup_id, "--no-startup-id") << ", " << command << ")" << std::endl;
    }

    void gaps(const std::string &workspace, const std::string &type, const long value) override {

    }

    void smart_borders(const std::string &enable) override {

    }

    void smart_gaps(const std::string &enable) override {

    }

    void for_window(criteria_state *criteria_state, const std::string &command) override {
        out << "cfg::for_window(" << command << ")" << std::endl;
    }

    void floating_minimum_size(long width, long height) override {
        out << "cfg::floating_minimum_size(" << width << ", " << height << ")" << std::endl;
    }

    void floating_maximum_size(long width, long height) override {
        out << "cfg::floating_maximum_size(" << width << ", " << height << ")" << std::endl;
    }

    void default_orientation(const std::string &orientation) override {}

    void workspace_layout(const std::string &layout) override {
        out << "cfg::workspace_layout(" << layout << ")" << std::endl;
    }

    void workspace_back_and_forth(const std::string &value) override {
        out << "workspace_back_and_forth " << value << std::endl;
    }

    void focus_follows_mouse(const std::string &value) override {
        out << "cfg::focus_follows_mouse(" << value << ")" << std::endl;
    }

    void mouse_warping(const std::string &value) override {
        out << "cfg::mouse_warping(" << value << ")" << std::endl;
    }

    void focus_wrapping(const std::string &value) override {
        out << "cfg::focus_wrapping(" << value << ")" << std::endl;
    }

    void force_focus_wrapping(const std::string &value) override {
        out << "cfg::force_focus_wrapping(" << value << ")" << std::endl;
    }

    void force_display_urgency_hint(long duration_ms) override {
        out << "cfg::force_display_urgency_hint(" << duration_ms << ")" << std::endl;
    }

    void focus_on_window_activation(const std::string &mode) override {
        out << "cfg::focus_on_window_activation(" << mode << ")" << std::endl;
    }

    void title_align(const std::string &alignment) override {
        out << "cfg::title_align(" << alignment << ")" << std::endl;
    }

    void hide_edge_borders(const std::string &borders) override {
        out << "cfg::hide_edge_borders(" << borders << ")" << std::endl;
    }

    void assign_output(criteria_state *criteria_state, const std::string &output) override {
        out << "cfg::assign_output(" << output << ")" << std::endl;
    }

    void assign(criteria_state *criteria_state, const std::string &workspace, bool is_number) override {
        out << "cfg::assign(" << workspace << ", " << is_number << ")" << std::endl;
    }

    void no_focus(criteria_state *criteria_state) override {
        out << "cfg::no_focus()" << std::endl;
    }

    void ipc_socket(const std::string &path) override {
        out << "cfg::ipc_socket(" << path << ")" << std::endl;
    }

    void ipc_kill_timeout(long timeout_ms) override {
        out << "cfg::ipc_kill_timeout(" << timeout_ms << ")" << std::endl;
    }

    void tiling_drag(const std::string &value) override {
        out << "cfg::tiling_drag(" << value << ")" << std::endl;
    }

    void restart_state(const std::string &path) override {
        out << "cfg::restart_state(" << path << ")" << std::endl;
    }

    void popup_during_fullscreen(const std::string &value) override {
        out << "cfg::popup_during_fullscreen(" << value << ")" << std::endl;
    }

    void color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) override {
        out << "cfg::color(" << colorclass << ", " << border << ", " << background << ", " << text << ", " << indicator << ", " << empty2null(child_border) << ")" << std::endl;
    }

    void color_single(const std::string &colorclass, const std::string &color) override {
        out << "cfg::color_single(" << colorclass << ", " << color << ")" << std::endl;
    }

    void floating_modifier(const std::string &modifiers) override {
        out << "cfg::floating_modifier(" << modifiers << ")" << std::endl;
    }

    void default_border(const std::string &windowtype, const std::string &border, long width) override {
        out << "cfg::default_border(" << windowtype << ", " << border << ", " << width << ")" << std::endl;
    }

    void workspace(const std::string &workspace, const std::string &output) override {
        out << "cfg::workspace(" << empty2null(workspace) << ", " << output << ")" << std::endl;
    }

    void binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) override {
        out
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
        out << "cfg::enter_mode(" << bool2str(pango_markup, "--pango-markup") << ", " << mode << ")" << std::endl;
    }

    void mode_binding(const std::string &bindtype, const std::string &modifiers, const std::string &key, bool release, bool border, bool whole_window, bool exclude_titlebar, const std::string &command) override {
        out << fmt::format("cfg::mode_binding({}, {}, {}, {}, {}, {}, {}, {})",
            bindtype,
            empty2null(modifiers),
            key,
            bool2str(release, "--release"),
            bool2str(border, "--border"),
            bool2str(whole_window, "--whole-window"),
            bool2str(exclude_titlebar, "--exclude-titlebar"),
            command) << std::endl;
    }

    void bar_height(const long height) override {

    }

    void bar_padding_one(const long all) override {

    }

    void bar_padding_two(const long top_and_bottom, const long right_and_left) override {

    }

    void bar_padding_three(const long top, const long right_and_left, const long bottom) override {

    }

    void bar_padding_four(const long top, const long right, const long bottom, const long left) override {

    }
};
