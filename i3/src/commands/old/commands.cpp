/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */
module;
struct criteria_state;
module i3_commands_old;

import i3_commands_base;
import utils;

namespace cmd {

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->criteria_init(criteria_state, cmd_output);
    }

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->criteria_match_windows(criteria_state, cmd_output);
    }

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *ctype, char const *cvalue) {
        cmd_output.applier->criteria_add(criteria_state, cmd_output, ctype, cvalue);
    }

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    void move_con_to_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) {
        cmd_output.applier->move_con_to_workspace(criteria_state, cmd_output, which);
    }

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->move_con_to_workspace_back_and_forth(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *no_auto_back_and_forth) {
        cmd_output.applier->move_con_to_workspace_name(criteria_state, cmd_output, name, no_auto_back_and_forth);
    }

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    void move_con_to_workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *no_auto_back_and_forth) {
        cmd_output.applier->move_con_to_workspace_number(criteria_state, cmd_output, which, no_auto_back_and_forth);
    }

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *way, char const *direction, long resize_px, long resize_ppt) {
        cmd_output.applier->resize(criteria_state, cmd_output, way, direction, resize_px, resize_ppt);
    }

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(criteria_state *criteria_state, CommandsResultIR &cmd_output, long cwidth, char const *mode_width, long cheight, char const *mode_height) {
        cmd_output.applier->resize_set(criteria_state, cmd_output, cwidth, mode_width, cheight, mode_height);
    }

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *border_style_str, long border_width) {
        cmd_output.applier->border(criteria_state, cmd_output, border_style_str, border_width);
    }

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment) {
        cmd_output.applier->nop(criteria_state, cmd_output, comment);
    }

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *cpath) {
        cmd_output.applier->append_layout(criteria_state, cmd_output, cpath);
    }

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) {
        cmd_output.applier->workspace(criteria_state, cmd_output, which);
    }

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    void workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *_no_auto_back_and_forth) {
        cmd_output.applier->workspace_number(criteria_state, cmd_output, which, _no_auto_back_and_forth);
    }

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->workspace_back_and_forth(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *_no_auto_back_and_forth) {
        cmd_output.applier->workspace_name(criteria_state, cmd_output, name, _no_auto_back_and_forth);
    }

    /*
     * Implementation of 'mode <string>'.
     *
     */
    void mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *mode) {
        cmd_output.applier->mode(criteria_state, cmd_output, mode);
    }

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    void move_con_to_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, bool move_workspace) {
        cmd_output.applier->move_con_to_output(criteria_state, cmd_output, name, move_workspace);
    }

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *floating_mode) {
        cmd_output.applier->floating(criteria_state, cmd_output, floating_mode);
    }

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction) {
        cmd_output.applier->split(criteria_state, cmd_output, direction);
    }

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *kill_mode_str) {
        cmd_output.applier->kill(criteria_state, cmd_output, kill_mode_str);
    }

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *nosn, char const *command) {
        cmd_output.applier->exec(criteria_state, cmd_output, nosn, command);
    }

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    void focus_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) {
        cmd_output.applier->focus_direction(criteria_state, cmd_output, direction_str);
    }

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) {
        cmd_output.applier->focus_sibling(criteria_state, cmd_output, direction_str);
    }

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *window_mode) {
        cmd_output.applier->focus_window_mode(criteria_state, cmd_output, window_mode);
    }

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *level) {
        cmd_output.applier->focus_level(criteria_state, cmd_output, level);
    }

    /*
     * Implementation of 'focus'.
     *
     */
    void focus(criteria_state *criteria_state, CommandsResultIR &cmd_output, bool focus_workspace) {
        cmd_output.applier->focus(criteria_state, cmd_output, focus_workspace);
    }

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    void fullscreen(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action, char const *fullscreen_mode) {
        cmd_output.applier->fullscreen(criteria_state, cmd_output, action, fullscreen_mode);
    }

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action) {
        cmd_output.applier->sticky(criteria_state, cmd_output, action);
    }

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str, long amount, char const *mode) {
        cmd_output.applier->move_direction(criteria_state, cmd_output, direction_str, amount, mode);
    }

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *layout_str) {
        cmd_output.applier->layout(criteria_state, cmd_output, layout_str);
    }

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *toggle_mode) {
        cmd_output.applier->layout_toggle(criteria_state, cmd_output, toggle_mode);
    }

    /*
     * Implementation of 'exit'.
     *
     */
    void exit(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->exit(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'reload'.
     *
     */
    void reload(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->reload(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'restart'.
     *
     */
    void restart(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->restart(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'open'.
     *
     */
    void open(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->open(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name) {
        cmd_output.applier->focus_output(criteria_state, cmd_output, name);
    }

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(criteria_state *criteria_state, CommandsResultIR &cmd_output, long x, char const *mode_x, long y, char const *mode_y) {
        cmd_output.applier->move_window_to_position(criteria_state, cmd_output, x, mode_x, y, mode_y);
    }

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *method) {
        cmd_output.applier->move_window_to_center(criteria_state, cmd_output, method);
    }

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(criteria_state *criteria_state, CommandsResultIR &cmd_output) {
        cmd_output.applier->move_window_to_mouse(criteria_state, cmd_output);
    }

    /*
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *format) {
        cmd_output.applier->title_format(criteria_state, cmd_output, format);
    }

    /*
     * Implementation of 'title_window_icon <yes|no|toggle>' and 'title_window_icon <padding|toggle> <px>'
     *
     */
    void title_window_icon(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *enable, int padding) {
        cmd_output.applier->title_window_icon(criteria_state, cmd_output, enable, padding);
    }

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    void rename_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *old_name, char const *new_name) {
        cmd_output.applier->rename_workspace(criteria_state, cmd_output, old_name, new_name);
    }

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_mode, char const *bar_id) {
        cmd_output.applier->bar_mode(criteria_state, cmd_output, bar_mode, bar_id);
    }

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_hidden_state, char const *bar_id) {
        cmd_output.applier->bar_hidden_state(criteria_state, cmd_output, bar_hidden_state, bar_id);
    }

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *argument) {
        cmd_output.applier->debuglog(criteria_state, cmd_output, argument);
    }

    /*
     * Implementation of 'nagbar'
     *
     */
    void nagbar(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *message, char const *font, char const *primary, char const *labels, char const *actions, char const *terminals) {
        cmd_output.applier->nagbar(criteria_state, cmd_output, type, message, font, primary, labels, actions, terminals);
    }
    /**
     * Implementation of 'gaps inner|outer|top|right|bottom|left|horizontal|vertical current|all set|plus|minus|toggle <px>'
     *
     */
    void gaps(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *scope, char const *mode, char const *value) {
        cmd_output.applier->gaps(criteria_state, cmd_output, type, scope, mode, value);
    }
}  // namespace cmd
