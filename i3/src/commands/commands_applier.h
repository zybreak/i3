/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */

#include "base_commands_applier.h"

class CommandsApplier : public BaseCommandsApplier {
   public:

    criteria_state* create_criteria_state();

    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *ctype, const char *cvalue) override;

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    void move_con_to_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) override;

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *no_auto_back_and_forth) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    void move_con_to_workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *no_auto_back_and_forth) override;

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *way, const char *direction, long resize_px, long resize_ppt) override;

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long cwidth, const char *mode_width, long cheight, const char *mode_height) override;

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *border_style_str, long border_width) override;

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *comment) override;

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *cpath) override;

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    void workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'mode <string>'.
     *
     */
    void mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *mode) override;

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    void move_con_to_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, bool move_workspace) override;

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *floating_mode) override;

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction) override;

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *kill_mode_str) override;

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *nosn, const char *command) override;

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    void focus_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) override;

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) override;

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *window_mode) override;

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *level) override;

    /*
     * Implementation of 'focus'.
     *
     */
    void focus(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    void fullscreen(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action, const char *fullscreen_mode) override;

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action) override;

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str, long amount, const char *mode) override;

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *layout_str) override;

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *toggle_mode) override;

    /*
     * Implementation of 'exit'.
     *
     */
    void exit(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'reload'.
     *
     */
    void reload(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'restart'.
     *
     */
    void restart(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'open'.
     *
     */
    void open(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long x, const char *mode_x, long y, const char *mode_y) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *method) override;

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) override;

    /*
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *format) override;

    /*
     * Implementation of 'title_window_icon <yes|no>' and 'title_window_icon padding <px>'
     *
     */
    void title_window_icon(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *enable, int padding) override;

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    void rename_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *old_name, const char *new_name) override;

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_mode, const char *bar_id) override;

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_hidden_state, const char *bar_id) override;

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *argument) override;

    /*
     * Implementation of 'nagbar'
     *
     */
    void nagbar(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char* type, const char* message, const char* font, const char* primary, const char* labels, const char* actions, const char* terminals) override;

};
