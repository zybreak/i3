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
#include <fmt/core.h>
#include <fmt/printf.h>
#include <antlr4-runtime.h>
#include <nlohmann/json.hpp>

#include "parser-specs/commandsLexer.h"
#include "parser-specs/commandsGrammar.h"
#include "parser-specs/commandsGrammarBaseListener.h"
module i3_commands_new;

import i3_commands_base;

using namespace antlr4;

class ipc_client;

class ErrorListener : public BaseErrorListener {
    void syntaxError(Recognizer *recognizer, Token *offendingSymbol, size_t line, size_t charPositionInLine,
                     std::string const &msg, std::exception_ptr e) override {
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
CommandResult i3_commands_new::parse_command(std::string const &input, command_parser_data &&data, BaseCommandsApplier &applier) {
    std::string padded_input = fmt::sprintf("$(%s)", input);
    ANTLRInputStream stream{padded_input};
    commandsLexer lexer{&stream};

    lexer.removeErrorListeners();
    ErrorListener pListener{};
    lexer.addErrorListener(&pListener);

    CommonTokenStream tokens{&lexer};
    commandsGrammar parser{&tokens};

    // parser.setErrorHandler(new BailErrorStrategy());
    parser.removeErrorListeners();
    parser.addErrorListener(&pListener);

    auto tree = parser.commands();

    if (parser.getNumberOfSyntaxErrors() > 0) {
        throw std::runtime_error("we got a problem, sir");
    }

    // cout << tree->toStringTree(&parser, true) << endl << endl;

    CommandsListener listener{applier, data.gen, data.client};
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

    CommandResult result{};

    return result;
}
