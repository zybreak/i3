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
export module i3_commands_base:base_commands_applier;

import :command_result_ir;

export class BaseCommandsApplier {
  public:
    virtual ~BaseCommandsApplier() = default;

    virtual criteria_state *create_criteria_state(int _state) = 0;

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    virtual void criteria_init(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    virtual void criteria_match_windows(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    virtual void criteria_add(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *ctype, char const *cvalue) = 0;

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    virtual void move_con_to_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) = 0;

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    virtual void move_con_to_workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    virtual void move_con_to_workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *no_auto_back_and_forth) = 0;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    virtual void move_con_to_workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *no_auto_back_and_forth) = 0;

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    virtual void resize(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *way, char const *direction, long resize_px, long resize_ppt) = 0;

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    virtual void resize_set(criteria_state *criteria_state, CommandsResultIR &cmd_output, long cwidth, char const *mode_width, long cheight, char const *mode_height) = 0;

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    virtual void border(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *border_style_str, long border_width) = 0;

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    virtual void nop(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment) = 0;

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    virtual void append_layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *cpath) = 0;

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    virtual void workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) = 0;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    virtual void workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *_no_auto_back_and_forth) = 0;

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    virtual void workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    virtual void workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *_no_auto_back_and_forth) = 0;

    /*
     * Implementation of 'mode <string>'.
     *
     */
    virtual void mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *mode) = 0;

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    virtual void move_con_to_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, bool move_workspace) = 0;

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    virtual void floating(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *floating_mode) = 0;

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    virtual void split(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction) = 0;

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    virtual void kill(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *kill_mode_str) = 0;

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    virtual void exec(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *nosn, char const *command) = 0;

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    virtual void focus_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) = 0;

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    virtual void focus_sibling(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) = 0;

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    virtual void focus_window_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *window_mode) = 0;

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    virtual void focus_level(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *level) = 0;

    /*
     * Implementation of 'focus'.
     *
     */
    virtual void focus(criteria_state *criteria_state, CommandsResultIR &cmd_output, bool focus_workspace) = 0;

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    virtual void fullscreen(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action, char const *fullscreen_mode) = 0;

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    virtual void sticky(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action) = 0;

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    virtual void move_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str, long amount, char const *mode) = 0;

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    virtual void layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *layout_str) = 0;

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    virtual void layout_toggle(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *toggle_mode) = 0;

    /*
     * Implementation of 'exit'.
     *
     */
    virtual void exit(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'reload'.
     *
     */
    virtual void reload(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'restart'.
     *
     */
    virtual void restart(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'open'.
     *
     */
    virtual void open(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    virtual void focus_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name) = 0;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    virtual void move_window_to_position(criteria_state *criteria_state, CommandsResultIR &cmd_output, long x, char const *mode_x, long y, char const *mode_y) = 0;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    virtual void move_window_to_center(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *method) = 0;

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    virtual void move_window_to_mouse(criteria_state *criteria_state, CommandsResultIR &cmd_output) = 0;

    /*
     * Implementation of 'title_format <format>'
     *
     */
    virtual void title_format(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *format) = 0;

    /*
     * Implementation of 'title_window_icon <yes|no|toggle>' and 'title_window_icon <padding|toggle> <px>'
     *
     */
    virtual void title_window_icon(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *enable, int padding) = 0;

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    virtual void rename_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *old_name, char const *new_name) = 0;

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    virtual void bar_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_mode, char const *bar_id) = 0;

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    virtual void bar_hidden_state(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_hidden_state, char const *bar_id) = 0;

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    virtual void debuglog(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *argument) = 0;

    /*
     * Implementation of 'nagbar'
     *
     */
    virtual void nagbar(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *message, char const *font, char const *primary, char const *labels, char const *actions, char const *terminals) = 0;

    /*
     * Implementation of 'gaps inner|outer|top|right|bottom|left|horizontal|vertical current|all set|plus|minus|toggle <px>'
     *
     */
    virtual void gaps(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *scope, char const *mode, char const *value) = 0;
};
