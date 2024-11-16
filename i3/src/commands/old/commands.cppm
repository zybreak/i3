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
export module i3_commands_old:commands_directives;

import i3_commands_base;

namespace cmd {
    /**
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *ctype, char const *cvalue);

    /**
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output'.
     *
     */
    void move_con_to_workspace(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which);

    /**
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name,
                                    char const *no_auto_back_and_forth);

    /**
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <number>'.
     *
     */
    void move_con_to_workspace_number(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which,
                                      char const *no_auto_back_and_forth);

    /**
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, long cwidth, char const *mode_width,
                    long cheight, char const *mode_height);

    /**
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *way, char const *direction,
                long resize_px, long resize_ppt);

    /**
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *border_style_str, long border_width);

    /**
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment);

    /**
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *path);

    /**
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which);

    /**
     * Implementation of 'workspace [--no-auto-back-and-forth] number <number>'
     *
     */
    void workspace_number(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which,
                          char const *no_auto_back_and_forth);

    /**
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name,
                        char const *no_auto_back_and_forth);

    /**
     * Implementation of 'mode <string>'.
     *
     */
    void mode(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *mode);

    /**
     * Implementation of 'move [window|container] [to] output <str>'.
     *
     */
    void move_con_to_output(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, bool move_workspace);

    /**
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *floating_mode);

    /**
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction);

    /**
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *kill_mode_str);

    /**
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *nosn, char const *command);

    /**
     * Implementation of 'focus left|right|up|down'.
     *
     */
    void focus_direction(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction);

    /**
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction);

    /**
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *window_mode);

    /**
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *level);

    /**
     * Implementation of 'focus'.
     *
     */
    void focus(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, bool focus_workspace);

    /**
     * Implementation of 'fullscreen [enable|disable|toggle] [global]'.
     *
     */
    void fullscreen(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action,
                    char const *fullscreen_mode);

    /**
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action);

    /**
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str, long amount,
                        char const *mode);

    /**
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *layout_str);

    /**
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *toggle_mode);

    /**
     * Implementation of 'exit'.
     *
     */
    void exit(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'reload'.
     *
     */
    void reload(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'restart'.
     *
     */
    void restart(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'open'.
     *
     */
    void open(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name);

    /**
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, long x, char const *mode_x,
                                 long y, char const *mode_y);

    /**
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *method);

    /**
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(struct criteria_state *criteria_state, CommandsResultIR &cmd_output);

    /**
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *format);

    /**
     * Implementation of 'rename workspace <name> to <name>'
     *
     */
    void rename_workspace(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *old_name,
                          char const *new_name);

    /**
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_mode, char const *bar_id);

    /**
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_hidden_state,
                          char const *bar_id);

    /**
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *argument);

    /**
     * Implementation of 'title_window_icon <yes|no|toggle>' and 'title_window_icon <padding|toggle> <px>'
     *
     */
    void title_window_icon(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *enable, int padding);

    /**
     * Implementation of 'nagbar'
     *
     */
    void nagbar(struct criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *message, char const *font, char const *primary, char const *labels, char const *actions, char const *terminals);

    /**
     * Implementation of 'gaps inner|outer|top|right|bottom|left|horizontal|vertical current|all set|plus|minus|toggle <px>'
     *
     */
    void gaps(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *scope, char const *mode, char const *value);

}  // namespace cmd
