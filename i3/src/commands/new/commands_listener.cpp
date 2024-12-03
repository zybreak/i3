/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
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
struct criteria_state;

#include <fmt/core.h>
#include <antlr4-runtime.h>

#include "parser-specs/commandsLexer.h"
#include "parser-specs/commandsGrammar.h"
#include "parser-specs/commandsGrammarBaseListener.h"
#include <nlohmann/json.hpp>
module i3_commands_new;

import i3_commands_base;

using namespace antlr4;

CommandsListener::CommandsListener(BaseCommandsApplier &commandsApplier, nlohmann::json *json_gen, ipc_client *ipc_client)
    : cmd(commandsApplier) {
    this->command_output = {
        .json_gen = json_gen,
        .client = ipc_client,
        .next_state = 0,
        .needs_tree_render = false};
}

void CommandsListener::enterCommand_move(commandsGrammar::Command_moveContext *ctx) {
    cmd.move_con_to_output(criteria_state, command_output, "", false);
    cmd.move_con_to_workspace(criteria_state, command_output, "");
    cmd.move_con_to_workspace_back_and_forth(criteria_state, command_output);
    cmd.move_con_to_workspace_name(criteria_state, command_output, "", "");
    cmd.move_con_to_workspace_number(criteria_state, command_output, "", "");
}

void CommandsListener::enterCommand_exec(commandsGrammar::Command_execContext *ctx) {
    auto options = ctx->COMMAND_OPTION() | std::views::all | std::views::transform([](auto const v) { return v->getText(); }) | std::ranges::to<std::vector>();

    auto nosn = std::ranges::find(options, "--no-startup-id") != options.end();

    cmd.exec(criteria_state, command_output, nosn ? "--no-startup-id" : "", ctx->COMMAND_STRING()->getText().c_str());
}

void CommandsListener::enterCommand_exit(commandsGrammar::Command_exitContext *ctx) {
    cmd.exit(criteria_state, command_output);
}

void CommandsListener::enterCommand_kill(commandsGrammar::Command_killContext *ctx) {
    auto killMode = ctx->COMMAND_STRING()->getText();
    cmd.kill(criteria_state, command_output, killMode.c_str());
}

void CommandsListener::enterCommand_fullscreen(commandsGrammar::Command_fullscreenContext *ctx) {
    auto action = ctx->COMMAND_STRING(0)->getText();
    auto mode = ctx->COMMAND_STRING(1) == nullptr ? "" : ctx->COMMAND_STRING(1)->getText();
    cmd.fullscreen(criteria_state, command_output, action.c_str(), mode.c_str());
}

void CommandsListener::enterCommand_sticky(commandsGrammar::Command_stickyContext *ctx) {
    cmd.sticky(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_debuglog(commandsGrammar::Command_debuglogContext *ctx) {
    cmd.debuglog(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_border(commandsGrammar::Command_borderContext *ctx) {
    cmd.border(criteria_state, command_output, "", 0);
}

void CommandsListener::enterCommand_layout(commandsGrammar::Command_layoutContext *ctx) {
    cmd.layout(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_append_layout(commandsGrammar::Command_append_layoutContext *ctx) {
    cmd.append_layout(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_workspace(commandsGrammar::Command_workspaceContext *ctx) {
    cmd.workspace(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_focus(commandsGrammar::Command_focusContext *ctx) {
    cmd.focus(criteria_state, command_output, false);
}

void CommandsListener::enterCommand_split(commandsGrammar::Command_splitContext *ctx) {
    cmd.split(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_floating(commandsGrammar::Command_floatingContext *ctx) {
    cmd.floating(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_resize(commandsGrammar::Command_resizeContext *ctx) {
    cmd.resize(criteria_state, command_output, "", "", 0, 0);
}

void CommandsListener::enterCommand_rename(commandsGrammar::Command_renameContext *ctx) {
    cmd.rename_workspace(criteria_state, command_output, "", "");
}

void CommandsListener::enterCommand_nop(commandsGrammar::Command_nopContext *ctx) {
    auto comment = ctx->COMMAND_STRING() != nullptr ? ctx->COMMAND_STRING()->getText() : "";
    cmd.nop(criteria_state, command_output, comment.c_str());
}

void CommandsListener::enterCommand_reload(commandsGrammar::Command_reloadContext *ctx) {
    cmd.reload(criteria_state, command_output);
}

void CommandsListener::enterCommand_restart(commandsGrammar::Command_restartContext *ctx) {
    cmd.restart(criteria_state, command_output);
}

void CommandsListener::enterCommand_mode(commandsGrammar::Command_modeContext *ctx) {
    cmd.mode(criteria_state, command_output, "");
}

void CommandsListener::enterCommand_open(commandsGrammar::Command_openContext *ctx) {
    cmd.open(criteria_state, command_output);
}

void CommandsListener::enterCommand_title_format(commandsGrammar::Command_title_formatContext *ctx) {
    cmd.title_format(criteria_state, command_output, ctx->COMMAND_STRING()->getText().c_str());
}

void CommandsListener::enterCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext *ctx) {
    cmd.title_window_icon(criteria_state, command_output, "", 0);
}
