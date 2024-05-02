/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands_parser.c: hand-written parser to parse commands (commands are what
 * you bind on keys and what you can send to i3 using the IPC interface, like
 * 'move left' or 'workspace 4').
 *
 * We use a hand-written parser instead of lex/yacc because our commands are
 * easy for humans, not for computers. Thus, it’s quite hard to specify a
 * context-free grammar for the commands. A PEG grammar would be easier, but
 * there’s downsides to every PEG parser generator I have come across so far.
 *
 * This parser is basically a state machine which looks for literals or strings
 * and can push either on a stack. After identifying a literal or string, it
 * will either transition to the current state, to a different state, or call a
 * function (like cmd_move()).
 *
 * Special care has been taken that error messages are useful and the code is
 * well testable (when compiled with -DTEST_PARSER it will output to stdout
 * instead of actually calling any function).
 *
 */
module;
class criteria_state;
#include "parser-specs/commandsLexer.h"
#include "parser-specs/commandsGrammar.h"
#include "parser-specs/commandsGrammarBaseListener.h"
#include <nlohmann/json.hpp>
export module i3_commands_new:commands_listener;

import i3_commands_base;
import i3ipc;

export class CommandsListener : public commandsGrammarBaseListener {
   private:
    CommandResultIR command_output;
    criteria_state *criteria_state;
    BaseCommandsApplier &cmd;
    
   public:
    CommandsListener(BaseCommandsApplier &commandsApplier, nlohmann::json *json_gen, ipc_client *ipc_client);
    
    void enterCommand_move(commandsGrammar::Command_moveContext *ctx) override;
    
    void enterCommand_exec(commandsGrammar::Command_execContext * ctx) override;

    void enterCommand_exit(commandsGrammar::Command_exitContext * ctx) override;

    void enterCommand_kill(commandsGrammar::Command_killContext * ctx) override;

    void enterCommand_fullscreen(commandsGrammar::Command_fullscreenContext * ctx) override;

    void enterCommand_sticky(commandsGrammar::Command_stickyContext * ctx) override;

    void enterCommand_debuglog(commandsGrammar::Command_debuglogContext * ctx) override;

    void enterCommand_border(commandsGrammar::Command_borderContext * ctx) override;

    void enterCommand_layout(commandsGrammar::Command_layoutContext * ctx) override;

    void enterCommand_append_layout(commandsGrammar::Command_append_layoutContext * ctx) override;

    void enterCommand_workspace(commandsGrammar::Command_workspaceContext * ctx) override;

    void enterCommand_focus(commandsGrammar::Command_focusContext * ctx) override;

    void enterCommand_split(commandsGrammar::Command_splitContext * ctx) override;

    void enterCommand_floating(commandsGrammar::Command_floatingContext * ctx) override;

    void enterCommand_resize(commandsGrammar::Command_resizeContext * ctx) override;

    void enterCommand_rename(commandsGrammar::Command_renameContext * ctx) override;

    void enterCommand_nop(commandsGrammar::Command_nopContext * ctx) override;

    void enterCommand_reload(commandsGrammar::Command_reloadContext * ctx) override;

    void enterCommand_restart(commandsGrammar::Command_restartContext * ctx) override;

    void enterCommand_mode(commandsGrammar::Command_modeContext * ctx) override;

    void enterCommand_open(commandsGrammar::Command_openContext * ctx) override;

    void enterCommand_title_format(commandsGrammar::Command_title_formatContext * ctx) override;

    void enterCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext * ctx) override;

};
