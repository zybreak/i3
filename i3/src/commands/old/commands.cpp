/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */
#include "commands.h"

import utils;

namespace cmd {

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->criteria_init(criteria_state, cmd_output);
    }

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->criteria_match_windows(criteria_state, cmd_output);
    }

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *ctype, const char *cvalue) {
        cmd_output.applier->criteria_add(criteria_state, cmd_output, ctype, cvalue);
    }

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    void move_con_to_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) {
        cmd_output.applier->move_con_to_workspace(criteria_state, cmd_output, which);
    }

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->move_con_to_workspace_back_and_forth(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *no_auto_back_and_forth) {
        cmd_output.applier->move_con_to_workspace_name(criteria_state, cmd_output, name, no_auto_back_and_forth);
    }

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    void move_con_to_workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *no_auto_back_and_forth) {
        cmd_output.applier->move_con_to_workspace_number(criteria_state, cmd_output, which, no_auto_back_and_forth);
    }

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *way, const char *direction, long resize_px, long resize_ppt) {
        cmd_output.applier->resize(criteria_state, cmd_output, way, direction, resize_px, resize_ppt);
    }

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long cwidth, const char *mode_width, long cheight, const char *mode_height) {
        cmd_output.applier->resize_set(criteria_state, cmd_output, cwidth, mode_width, cheight, mode_height);
    }

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *border_style_str, long border_width) {
        cmd_output.applier->border(criteria_state, cmd_output, border_style_str, border_width);
    }

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *comment) {
        cmd_output.applier->nop(criteria_state, cmd_output, comment);
    }

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *cpath) {
        cmd_output.applier->append_layout(criteria_state, cmd_output, cpath);
    }

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) {
        cmd_output.applier->workspace(criteria_state, cmd_output, which);
    }

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    void workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *_no_auto_back_and_forth) {
        cmd_output.applier->workspace_number(criteria_state, cmd_output, which, _no_auto_back_and_forth);
    }

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->workspace_back_and_forth(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *_no_auto_back_and_forth) {
        cmd_output.applier->workspace_name(criteria_state, cmd_output, name, _no_auto_back_and_forth);
    }

    /*
     * Implementation of 'mode <string>'.
     *
     */
    void mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *mode) {
        cmd_output.applier->mode(criteria_state, cmd_output, mode);
    }

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    void move_con_to_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, bool move_workspace) {
        cmd_output.applier->move_con_to_output(criteria_state, cmd_output, name, move_workspace);
    }

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *floating_mode) {
        cmd_output.applier->floating(criteria_state, cmd_output, floating_mode);
    }

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction) {
        cmd_output.applier->split(criteria_state, cmd_output, direction);
    }

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *kill_mode_str) {
        cmd_output.applier->kill(criteria_state, cmd_output, kill_mode_str);
    }

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *nosn, const char *command) {
        cmd_output.applier->exec(criteria_state, cmd_output, nosn, command);
    }

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    void focus_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
        cmd_output.applier->focus_direction(criteria_state, cmd_output, direction_str);
    }

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
        cmd_output.applier->focus_sibling(criteria_state, cmd_output, direction_str);
    }

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *window_mode) {
        cmd_output.applier->focus_window_mode(criteria_state, cmd_output, window_mode);
    }

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *level) {
        cmd_output.applier->focus_level(criteria_state, cmd_output, level);
    }

    /*
     * Implementation of 'focus'.
     *
     */
    void focus(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->focus(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    void fullscreen(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action, const char *fullscreen_mode) {
        cmd_output.applier->fullscreen(criteria_state, cmd_output, action, fullscreen_mode);
    }

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action) {
        cmd_output.applier->sticky(criteria_state, cmd_output, action);
    }

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str, long amount, const char *mode) {
        cmd_output.applier->move_direction(criteria_state, cmd_output, direction_str, amount, mode);
    }

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *layout_str) {
        cmd_output.applier->layout(criteria_state, cmd_output, layout_str);
    }

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *toggle_mode) {
        cmd_output.applier->layout_toggle(criteria_state, cmd_output, toggle_mode);
    }

    /*
     * Implementation of 'exit'.
     *
     */
    void exit(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->exit(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'reload'.
     *
     */
    void reload(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->reload(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'restart'.
     *
     */
    void restart(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->restart(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'open'.
     *
     */
    void open(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->open(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name) {
        cmd_output.applier->focus_output(criteria_state, cmd_output, name);
    }

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long x, const char *mode_x, long y, const char *mode_y) {
        cmd_output.applier->move_window_to_position(criteria_state, cmd_output, x,mode_x, y, mode_y);
    }

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *method) {
        cmd_output.applier->move_window_to_center(criteria_state, cmd_output, method);
    }

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
        cmd_output.applier->move_window_to_mouse(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *format) {
        cmd_output.applier->title_format(criteria_state, cmd_output, format);
    }

    /*
     * Implementation of 'title_window_icon <yes|no>' and 'title_window_icon padding <px>'
     *
     */
    void title_window_icon(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *enable, int padding) {
        cmd_output.applier->title_window_icon(criteria_state, cmd_output, enable, padding);
    }

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    void rename_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *old_name, const char *new_name) {
        cmd_output.applier->rename_workspace(criteria_state, cmd_output, old_name, new_name);
    }

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_mode, const char *bar_id) {
        cmd_output.applier->bar_mode(criteria_state, cmd_output, bar_mode, bar_id);
    }

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_hidden_state, const char *bar_id) {
        cmd_output.applier->bar_hidden_state(criteria_state, cmd_output, bar_hidden_state, bar_id);
    }

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *argument) {
        cmd_output.applier->debuglog(criteria_state, cmd_output, argument);
    }

    /*
     * Implementation of 'nagbar'
     *
     */
    void nagbar(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char* type, const char* message, const char* font, const char* primary, const char* labels, const char* actions, const char* terminals) {
        cmd_output.applier->nagbar(criteria_state, cmd_output, type, message, font, primary, labels, actions, terminals);
    }

}
