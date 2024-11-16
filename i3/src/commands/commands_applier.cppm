/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */
export module i3:commands_applier;

import i3_commands_base;
import :criteria_state;

export class CommandsApplier : public BaseCommandsApplier {
  public:
    criteria_state *create_criteria_state(int _state) override;

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *ctype, char const *cvalue) override;

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    void move_con_to_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) override;

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *no_auto_back_and_forth) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    void move_con_to_workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *no_auto_back_and_forth) override;

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *way, char const *direction, long resize_px, long resize_ppt) override;

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(criteria_state *criteria_state, CommandsResultIR &cmd_output, long cwidth, char const *mode_width, long cheight, char const *mode_height) override;

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *border_style_str, long border_width) override;

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment) override;

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *cpath) override;

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    void workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'mode <string>'.
     *
     */
    void mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *mode) override;

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    void move_con_to_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, bool move_workspace) override;

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *floating_mode) override;

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction) override;

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *kill_mode_str) override;

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *nosn, char const *command) override;

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    void focus_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) override;

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) override;

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *window_mode) override;

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *level) override;

    /*
     * Implementation of 'focus'.
     *
     */
    void focus(criteria_state *criteria_state, CommandsResultIR &cmd_output, bool focus_workspace) override;

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    void fullscreen(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action, char const *fullscreen_mode) override;

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action) override;

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str, long amount, char const *mode) override;

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *layout_str) override;

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *toggle_mode) override;

    /*
     * Implementation of 'exit'.
     *
     */
    void exit(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'reload'.
     *
     */
    void reload(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'restart'.
     *
     */
    void restart(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'open'.
     *
     */
    void open(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(criteria_state *criteria_state, CommandsResultIR &cmd_output, long x, char const *mode_x, long y, char const *mode_y) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *method) override;

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *format) override;

    /*
     * Implementation of 'title_window_icon <yes|no|toggle>' and 'title_window_icon <padding|toggle> <px>'
     *
     */
    void title_window_icon(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *enable, int padding) override;

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    void rename_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *old_name, char const *new_name) override;

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_mode, char const *bar_id) override;

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_hidden_state, char const *bar_id) override;

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *argument) override;

    /*
     * Implementation of 'nagbar'
     *
     */
    void nagbar(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *message, char const *font, char const *primary, char const *labels, char const *actions, char const *terminals) override;

    /**
     * Implementation of 'gaps inner|outer|top|right|bottom|left|horizontal|vertical current|all set|plus|minus|toggle <px>'
     *
     */
    void gaps(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *scope, char const *mode, char const *value) override;
};
