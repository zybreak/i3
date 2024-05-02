#include <iostream>

import i3;
import i3_commands_base;

using namespace std::literals;

static std::string bool2str(bool b, const std::string &s) {
    return (b) ? s : "(null)"s;
}

static std::string empty2null(const char *s) {
    return (s == nullptr) ? "(null)"s : s;
}

class CommandsApplierTest : public BaseCommandsApplier {

    void criteria_add(criteria_state *cs, CommandResultIR &cmd_output, const char *ctype, const char *cvalue) override {
        cs->current_match.parse_property(ctype, cvalue);
        std::cout << "cfg::criteria_add(" << ctype << ", " << cvalue << ")" << std::endl;
    }
    
    criteria_state* create_criteria_state(int _state) override {
        criteria_state *cs = new criteria_state(_state);
        
        cs->current_match = Match();

        return cs;
    }
    
    void criteria_init(criteria_state *cs, CommandResultIR &cmd_output) override {
        //cs->criteria_next_state = _state;
        
        cs->current_match = Match();
    }

    void criteria_match_windows(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        
    }

    void move_con_to_workspace(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *which) override {
        std::cout << "cmd::move_con_to_workspace(" << which << ")" << std::endl;
    }

    void move_con_to_workspace_back_and_forth(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::move_con_to_workspace_back_and_forth()" << std::endl;
    }

    void move_con_to_workspace_name(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *name, const char *no_auto_back_and_forth) override {
        std::cout << "cmd::move_con_to_workspace_name(" << name << ", " << no_auto_back_and_forth << ")" << std::endl;
    }

    void move_con_to_workspace_number(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *which, const char *no_auto_back_and_forth) override {
        std::cout << "cmd::move_con_to_workspace_number(" << which << ", " << no_auto_back_and_forth << ")" << std::endl;
    }

    void resize(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *way, const char *direction, long resize_px, long resize_ppt) override {
        std::cout << "cmd::resize(" << way << ", " << direction << ", " << resize_px << ", " << resize_ppt << ")" << std::endl;
    }

    void resize_set(criteria_state *criteria_state, CommandResultIR &cmd_output, long cwidth, const char *mode_width, long cheight, const char *mode_height) override {
        std::cout << "cmd::resize_set(" << cwidth << ", " << mode_width << ", " << cheight << ", " << mode_height << ")" << std::endl;
    }

    void border(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *border_style_str, long border_width) override {
        std::cout << "cmd::border(" << border_style_str << ", " << border_width << ")" << std::endl;
    }

    void nop(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *comment) override {
        std::cout << "cmd::nop(" << empty2null(comment) << ")" << std::endl;
    }

    void append_layout(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *cpath) override {
        std::cout << "cmd::append_layout" << std::endl;
    }

    void workspace(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *which) override {
        std::cout << "cmd::workspace" << std::endl;
    }

    void workspace_number(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *which, const char *_no_auto_back_and_forth) override {
        std::cout << "cmd::workspace_number" << std::endl;
    }

    void workspace_back_and_forth(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::workspace_back_and_forth" << std::endl;
    }

    void workspace_name(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *name, const char *_no_auto_back_and_forth) override {
        std::cout << "cmd::workspace_name" << std::endl;
    }

    void mode(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *mode) override {
        std::cout << "cmd::mode" << std::endl;
    }

    void move_con_to_output(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *name, bool move_workspace) override {
        std::cout << "cmd::move_con_to_output" << std::endl;
    }

    void floating(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *floating_mode) override {
        std::cout << "cmd::floating" << std::endl;
    }

    void split(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *direction) override {
        std::cout << "cmd::split" << std::endl;
    }

    void kill(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *kill_mode_str) override {
        std::cout << "cmd::kill" << std::endl;
    }

    void exec(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *nosn, const char *command) override {
        std::cout << "cmd::exec" << std::endl;
    }

    void focus_direction(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *direction_str) override {
        std::cout << "cmd::focus_direction" << std::endl;
    }

    void focus_sibling(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *direction_str) override {
        std::cout << "cmd::focus_sibling" << std::endl;
    }

    void focus_window_mode(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *window_mode) override {
        std::cout << "cmd::focus_window_mode" << std::endl;
    }

    void focus_level(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *level) override {
        std::cout << "cmd::focus_level" << std::endl;
    }

    void focus(criteria_state *criteria_state, CommandResultIR &cmd_output, bool focus_workspace) override {
        std::cout << "cmd::focus" << std::endl;
    }

    void fullscreen(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *action, const char *fullscreen_mode) override {
        std::cout << "cmd::fullscreen" << std::endl;
    }

    void sticky(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *action) override {
        std::cout << "cmd::sticky" << std::endl;
    }

    void move_direction(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *direction_str, long amount, const char *mode) override {
        std::cout << "cmd::move_direction" << std::endl;
    }

    void layout(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *layout_str) override {
        std::cout << "cmd::layout" << std::endl;
    }

    void layout_toggle(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *toggle_mode) override {
        std::cout << "cmd::layout_toggle" << std::endl;
    }

    void exit(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::exit" << std::endl;
    }

    void reload(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::reload" << std::endl;
    }

    void restart(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::restart" << std::endl;
    }

    void open(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::open" << std::endl;
    }

    void focus_output(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *name) override {
        std::cout << "cmd::focus_output" << std::endl;
    }

    void move_window_to_position(criteria_state *criteria_state, CommandResultIR &cmd_output, long x, const char *mode_x, long y, const char *mode_y) override {
        std::cout << "cmd::move_window_to_position" << std::endl;
    }

    void move_window_to_center(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *method) override {
        std::cout << "cmd::move_window_to_center" << std::endl;
    }

    void move_window_to_mouse(criteria_state *criteria_state, CommandResultIR &cmd_output) override {
        std::cout << "cmd::move_window_to_mouse" << std::endl;
    }

    void title_format(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *format) override {
        std::cout << "cmd::title_format" << std::endl;
    }

    void title_window_icon(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *enable, int padding) override {
        std::cout << "cmd::title_window_icon" << std::endl;
    }

    void rename_workspace(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *old_name, const char *new_name) override {
        std::cout << "cmd::rename_workspace" << std::endl;
    }

    void bar_mode(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *bar_mode, const char *bar_id) override {
        std::cout << "cmd::bar_mode" << std::endl;
    }

    void bar_hidden_state(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *bar_hidden_state, const char *bar_id) override {
        std::cout << "cmd::bar_hidden_state" << std::endl;
    }

    void debuglog(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *argument) override {
        std::cout << "cmd::debuglog" << std::endl;
    }

    void nagbar(criteria_state *criteria_state, CommandResultIR &cmd_output, const char* type, const char* message, const char* font, const char* primary, const char* labels, const char* actions, const char* terminals) override {
        std::cout << "cmd::nagbar" << std::endl;
    }
    
    void gaps(criteria_state *criteria_state, CommandResultIR &cmd_output, const char *type, const char *scope, const char *mode, const char *value) override {
        std::cout << "cmd::gaps" << std::endl;
    }

};
