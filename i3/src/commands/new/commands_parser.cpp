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
#include <fmt/core.h>
#include <antlr4-runtime.h>

#include "i3-ipc.h"
#include "../commands_parser.h"
#include "parser-specs/commandsLexer.h"
#include "parser-specs/commandsGrammar.h"
#include "parser-specs/commandsGrammarBaseListener.h"
#include "fn.hpp"

using namespace antlr4;

namespace fn = rangeless::fn;
using fn::operators::operator%;   // arg % fn   equivalent to fn(std::forward<Arg>(arg))
using fn::operators::operator%=;  // arg %= fn; equivalent to arg = fn( std::move(arg));

class CommandsListener : public commandsGrammarBaseListener {

    CommandResultIR command_output;
    struct criteria_state criteria_state{};

   public:
    CommandsListener(nlohmann::json *json_gen, ipc_client *ipc_client) {
        this->command_output = {
            .json_gen = json_gen,
            .client = ipc_client,
            .next_state = 0,
            .needs_tree_render = false
        };
    }

    void enterCommand_move(commandsGrammar::Command_moveContext *ctx) override {
        cmd::move_con_to_output(criteria_state, command_output, "", false);
        cmd::move_con_to_workspace(criteria_state, command_output, "");
        cmd::move_con_to_workspace_back_and_forth(criteria_state, command_output);
        cmd::move_con_to_workspace_name(criteria_state, command_output, "", "");
        cmd::move_con_to_workspace_number(criteria_state, command_output, "", "");
    }

    void enterCommand_exec(commandsGrammar::Command_execContext * ctx) override {
        auto options = ctx->COMMAND_OPTION()
                       % fn::transform([](auto opt) { return opt->getText(); })
                       % fn::unique_all()
                       % fn::to_vector();

        auto nosn = options
                       % fn::exists_where([](auto text) { return text == "--no-startup-id"; });

        cmd::exec(criteria_state, command_output, nosn ? "--no-startup-id" : "", ctx->COMMAND_STRING()->getText().c_str());
    }

    void enterCommand_exit(commandsGrammar::Command_exitContext * ctx) override {
        cmd::exit(criteria_state, command_output);
    }

    void enterCommand_kill(commandsGrammar::Command_killContext * ctx) override {
        auto killMode = ctx->COMMAND_STRING()->getText();
        cmd::kill(criteria_state, command_output, killMode.c_str());
    }

    void enterCommand_fullscreen(commandsGrammar::Command_fullscreenContext * ctx) override {
        auto action = ctx->COMMAND_STRING(0)->getText();
        auto mode = ctx->COMMAND_STRING(1) == nullptr ? "" : ctx->COMMAND_STRING(1)->getText();
        cmd::fullscreen(criteria_state, command_output, action.c_str(), mode.c_str());
    }

    void enterCommand_sticky(commandsGrammar::Command_stickyContext * ctx) override {
        cmd::sticky(criteria_state, command_output, "");
    }

    void enterCommand_debuglog(commandsGrammar::Command_debuglogContext * ctx) override {
        cmd::debuglog(criteria_state, command_output, "");
    }

    void enterCommand_border(commandsGrammar::Command_borderContext * ctx) override {
        cmd::border(criteria_state, command_output, "", 0);
    }

    void enterCommand_layout(commandsGrammar::Command_layoutContext * ctx) override {
        cmd::layout(criteria_state, command_output, "");
    }

    void enterCommand_append_layout(commandsGrammar::Command_append_layoutContext * ctx) override {
        cmd::append_layout(criteria_state, command_output, "");
    }

    void enterCommand_workspace(commandsGrammar::Command_workspaceContext * ctx) override {
        cmd::workspace(criteria_state, command_output, "");
    }

    void enterCommand_focus(commandsGrammar::Command_focusContext * ctx) override {
        cmd::focus(criteria_state, command_output);
    }

    void enterCommand_split(commandsGrammar::Command_splitContext * ctx) override {
        cmd::split(criteria_state, command_output, "");
    }

    void enterCommand_floating(commandsGrammar::Command_floatingContext * ctx) override {
        cmd::floating(criteria_state, command_output, "");
    }

    void enterCommand_resize(commandsGrammar::Command_resizeContext * ctx) override {
        cmd::resize(criteria_state, command_output, "", "", 0, 0);
    }

    void enterCommand_rename(commandsGrammar::Command_renameContext * ctx) override {
        cmd::rename_workspace(criteria_state, command_output, "", "");
    }

    void enterCommand_nop(commandsGrammar::Command_nopContext * ctx) override {
        auto comment = ctx->COMMAND_STRING() != nullptr ? ctx->COMMAND_STRING()->getText() : "";
        cmd::nop(criteria_state, command_output, comment.c_str());
    }

    void enterCommand_reload(commandsGrammar::Command_reloadContext * ctx) override {
        cmd::reload(criteria_state, command_output);
    }

    void enterCommand_restart(commandsGrammar::Command_restartContext * ctx) override {
        cmd::restart(criteria_state, command_output);
    }

    void enterCommand_mode(commandsGrammar::Command_modeContext * ctx) override {
        cmd::mode(criteria_state, command_output, "");
    }

    void enterCommand_open(commandsGrammar::Command_openContext * ctx) override {
        cmd::open(criteria_state, command_output);
    }

    void enterCommand_title_format(commandsGrammar::Command_title_formatContext * ctx) override {
        cmd::title_format(criteria_state, command_output, ctx->COMMAND_STRING()->getText().c_str());
    }

    void enterCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext * ctx) override {
        cmd::title_window_icon(criteria_state, command_output, "", 0);
    }
};

class ErrorListener : public BaseErrorListener {
    void syntaxError(Recognizer *recognizer, Token * offendingSymbol, size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override {
        std::cout << "ERROR " << fmt::sprintf("(%d:%d): ", line, charPositionInLine) << msg << std::endl;
    }

};

/*
 * Parses and executes the given command. If a caller-allocated yajl_gen is
 * passed, a json reply will be generated in the format specified by the ipc
 * protocol. Pass NULL if no json reply is required.
 *
 * Free the returned CommandResult with command_result_free().
 */
CommandResult parse_command_new(const std::string &input, nlohmann::json *gen, ipc_client *client) {
    std::string padded_input = fmt::sprintf("$(%s)", input);
    ANTLRInputStream stream{padded_input};
    commandsLexer lexer{&stream};

    lexer.removeErrorListeners();
    ErrorListener pListener{};
    lexer.addErrorListener(&pListener);

    CommonTokenStream tokens{&lexer};
    commandsGrammar parser{&tokens};

    //parser.setErrorHandler(new BailErrorStrategy());
    parser.removeErrorListeners();
    parser.addErrorListener(&pListener);

    auto tree = parser.commands();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw std::runtime_error("we got a problem, sir");
    }

    //cout << tree->toStringTree(&parser, true) << endl << endl;

    CommandsListener listener{gen, client};
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    CommandResult result{};

    return result;
}
