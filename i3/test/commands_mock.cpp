#include <iostream>
#include "commands.h"

namespace cmd {

    void move_con_to_workspace(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *which) {
        std::cout << "move_con_to_workspace" << std::endl;
    }

    void move_con_to_workspace_back_and_forth(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "move_con_to_workspace_back_and_forth" << std::endl;
    }

    void move_con_to_workspace_name(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *no_auto_back_and_forth) {
        std::cout << "move_con_to_workspace_name" << std::endl;
    }

    void move_con_to_workspace_number(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *no_auto_back_and_forth) {
        std::cout << "move_con_to_workspace_number" << std::endl;
    }

    void resize(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *way, const char *direction, long resize_px, long resize_ppt) {
        std::cout << "resize" << std::endl;
    }

    void resize_set(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, long cwidth, const char *mode_width, long cheight, const char *mode_height) {
        std::cout << "resize_set" << std::endl;
    }

    void border(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *border_style_str, long border_width) {
        std::cout << "border" << std::endl;
    }

    void nop(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *comment) {
        std::cout << "nop" << std::endl;
    }

    void append_layout(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *cpath) {
        std::cout << "append_layout" << std::endl;
    }

    void workspace(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *which) {
        std::cout << "workspace" << std::endl;
    }

    void workspace_number(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *_no_auto_back_and_forth) {
        std::cout << "workspace_number" << std::endl;
    }

    void workspace_back_and_forth(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "workspace_back_and_forth" << std::endl;
    }

    void workspace_name(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *_no_auto_back_and_forth) {
        std::cout << "workspace_name" << std::endl;
    }

    void mode(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *mode) {
        std::cout << "mode" << std::endl;
    }

    void move_con_to_output(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *name, bool move_workspace) {
        std::cout << "move_con_to_output" << std::endl;
    }

    void floating(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *floating_mode) {
        std::cout << "floating" << std::endl;
    }

    void split(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *direction) {
        std::cout << "split" << std::endl;
    }

    void kill(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *kill_mode_str) {
        std::cout << "kill" << std::endl;
    }

    void exec(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *nosn, const char *command) {
        std::cout << "exec" << std::endl;
    }

    void focus_direction(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
        std::cout << "focus_direction" << std::endl;
    }

    void focus_sibling(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
        std::cout << "focus_sibling" << std::endl;
    }

    void focus_window_mode(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *window_mode) {
        std::cout << "focus_window_mode" << std::endl;
    }

    void focus_level(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *level) {
        std::cout << "focus_level" << std::endl;
    }

    void focus(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "focus" << std::endl;
    }

    void fullscreen(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *action, const char *fullscreen_mode) {
        std::cout << "fullscreen" << std::endl;
    }

    void sticky(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *action) {
        std::cout << "sticky" << std::endl;
    }

    void move_direction(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *direction_str, long amount, const char *mode) {
        std::cout << "move_direction" << std::endl;
    }

    void layout(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *layout_str) {
        std::cout << "layout" << std::endl;
    }

    void layout_toggle(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *toggle_mode) {
        std::cout << "layout_toggle" << std::endl;
    }

    void exit(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "exit" << std::endl;
    }

    void reload(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "reload" << std::endl;
    }

    void restart(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "restart" << std::endl;
    }

    void open(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "open" << std::endl;
    }

    void focus_output(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *name) {
        std::cout << "focus_output" << std::endl;
    }

    void move_window_to_position(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, long x, const char *mode_x, long y, const char *mode_y) {
        std::cout << "move_window_to_position" << std::endl;
    }

    void move_window_to_center(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *method) {
        std::cout << "move_window_to_center" << std::endl;
    }

    void move_window_to_mouse(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "move_window_to_mouse" << std::endl;
    }

    void move_scratchpad(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "move_scratchpad" << std::endl;
    }

    void scratchpad_show(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output) {
        std::cout << "scratchpad_show" << std::endl;
    }

    void title_format(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *format) {
        std::cout << "title_format" << std::endl;
    }

    void title_window_icon(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *enable, int padding) {
        std::cout << "title_window_icon" << std::endl;
    }

    void rename_workspace(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *old_name, const char *new_name) {
        std::cout << "rename_workspace" << std::endl;
    }

    void bar_mode(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *bar_mode, const char *bar_id) {
        std::cout << "bar_mode" << std::endl;
    }

    void bar_hidden_state(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *bar_hidden_state, const char *bar_id) {
        std::cout << "bar_hidden_state" << std::endl;
    }

    void debuglog(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char *argument) {
        std::cout << "debuglog" << std::endl;
    }

    void nagbar(struct criteria_state &criteria_state, struct CommandResultIR &cmd_output, const char* type, const char* message, const char* font, const char* primary, const char* labels, const char* actions, const char* terminals) {
        std::cout << "nagbar" << std::endl;
    }

}
