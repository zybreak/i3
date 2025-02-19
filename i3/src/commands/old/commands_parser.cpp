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
#include <err.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <nlohmann/json.hpp>
module i3_commands_old;

import std;
import i3_commands_base;
import utils;

/*******************************************************************************
 * The parser itself.
 ******************************************************************************/

/*******************************************************************************
 * The (small) stack where identified literals are stored during the parsing
 * of a single command (like $workspace).
 ******************************************************************************/
static CommandsResultIR subcommand_output;
static CommandsResultIR command_output;

[[nodiscard("PURE FUN")]]
static cmdp_state next_state(cmdp_token const &token, stack &stack, criteria_state *criteria_state) {
    if (token.next_state == cmdp_state::__CALL) {
        subcommand_output.json_gen = command_output.json_gen;
        subcommand_output.client = command_output.client;
        subcommand_output.applier = command_output.applier;
        subcommand_output.needs_tree_render = false;

        try {
            GENERATED_call(criteria_state, stack, token.call_identifier, subcommand_output);
        } catch (std::exception const &e) {
            if (subcommand_output.json_gen != nullptr) {
                subcommand_output.json_gen->push_back({{"success", false},
                                                       {"error", e.what()}});
            }
        }

        auto state = static_cast<cmdp_state>(subcommand_output.next_state);
        /* If any subcommand requires a tree_render(), we need to make the
         * whole parser result request a tree_render(). */
        if (subcommand_output.needs_tree_render) {
            command_output.needs_tree_render = true;
        }
        clear_stack(stack);
        return state;
    }

    auto state = token.next_state;
    if (state == cmdp_state::INITIAL) {
        clear_stack(stack);
    }
    return state;
}

bool handle_literal(std::string::const_iterator &walk, cmdp_token const &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    if (strncasecmp(std::to_address(walk), token.name.substr(1).c_str(), token.name.size() - 1) == 0) {
        if (token.identifier) {
            push_string(stack, token.identifier->c_str(), token.name.c_str() + 1);
        }
        walk += token.name.length() - 1;
        *state = next_state(token, stack, criteria_state);
        return true;
    }

    return false;
}

bool handle_number(std::string::const_iterator &walk, cmdp_token const &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    /* Handle numbers. We only accept decimal numbers for now. */
    char *end = nullptr;
    errno = 0;
    long int num = strtol(std::to_address(walk), &end, 10);
    if ((errno == ERANGE && (num == LONG_MIN || num == LONG_MAX)) ||
        (errno != 0 && num == 0)) {
        return false;
    }

    /* No valid numbers found */
    if (num == 0) {
        return false;
    }

    if (token.identifier) {
        push_long(stack, token.identifier->c_str(), num);
    }

    /* Set walk to the first non-number character */
    int dist = end - std::to_address(walk);
    std::advance(walk, dist);
    *state = next_state(token, stack, criteria_state);
    return true;
}

bool handle_word(std::string::const_iterator &walk, cmdp_token const &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    auto str = utils::parse_string(walk, (token.name[0] != 's'));
    if (str) {
        if (token.identifier) {
            push_string(stack, token.identifier->c_str(), str->c_str());
        }
        /* If we are at the end of a quoted string, skip the ending
         * double quote. */
        if (*walk == '"') {
            walk++;
        }
        *state = next_state(token, stack, criteria_state);
        return true;
    }
    return false;
}

bool handle_end(std::string::const_iterator &walk, cmdp_token const &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    if (*walk == '\0' || *walk == ',' || *walk == ';') {
        *state = next_state(token, stack, criteria_state);
        /* To make sure we start with an appropriate matching
         * datastructure for commands which do *not* specify any
         * criteria, we re-initialize the criteria system after
         * every command. */
        if (*walk == '\0' || *walk == ';') {
            cmd::criteria_init(criteria_state, command_output);
        }
        walk++;
        return true;
    }

    return false;
}

void unhandled_token(CommandResult &result, nlohmann::json *gen, stack &stack, std::string const &input, size_t const len, std::vector<cmdp_token> const &ptr, std::string::const_iterator &walk) {
    /* Figure out how much memory we will need to fill in the names of
     * all tokens afterwards. */
    size_t tokenlen = 0;
    for (auto &token : ptr) {
        tokenlen += token.name.length() + strlen("'', ");
    }

    /* Build up a decent error message. We include the problem, the
     * full input, and underline the position where the parser
     * currently is. */
    std::string possible_tokens{};
    possible_tokens.reserve(tokenlen + 1);
    for (unsigned long c = 0; c < ptr.size(); c++) {
        auto &token = ptr.at(c);
        if (token.name[0] == '\'') {
            /* A literal is copied to the error message enclosed with
             * single quotes. */
            possible_tokens += '\'';
            possible_tokens += token.name.substr(1);
            possible_tokens += '\'';
        } else {
            /* Any other token is copied to the error message enclosed
             * with angle brackets. */
            possible_tokens += '<';
            possible_tokens += token.name;
            possible_tokens += '>';
        }
        if (c < (ptr.size() - 1)) {
            possible_tokens += ", ";
        }
    }

    auto errormessage = fmt::format("Expected one of these tokens: {}", possible_tokens);

    /* Contains the same amount of characters as 'input' has, but with
     * the unparsable part highlighted using ^ characters. */
    std::string position(len, '\0');
    auto walk_len = strlen(std::to_address(walk));
    std::fill_n(position.begin(), len - walk_len, ' ');
    std::fill_n(position.begin() + (len - walk_len), walk_len, '^');

    // ELOG(fmt::sprintf("%s\n",  errormessage));
    // ELOG(fmt::sprintf("Your command: %s\n",  input));
    // ELOG(fmt::sprintf("              %s\n",  position));

    result.parse_error = true;
    result.error_message = errormessage;

    /* Format this error message as a JSON reply. */
    if (gen != nullptr) {
        gen->push_back({{"success", false},
                        /* We set parse_error to true to distinguish this from other
                         * errors. i3-nagbar is spawned upon keypresses only for parser
                         * errors. */
                        {"parse_error", true},
                        {"error", errormessage},
                        {"input", input},
                        {"errorposition", position}});
    }

    clear_stack(stack);
}

/*
 * Parses and executes the given command. If a caller-allocated yajl_gen is
 * passed, a json reply will be generated in the format specified by the ipc
 * protocol. Pass NULL if no json reply is required.
 *
 * Free the returned CommandResult with command_result_free().
 */
CommandResult i3_commands_old::parse_command(std::string const &input, command_parser_data &&data, BaseCommandsApplier *applier) {
    // DLOG(fmt::sprintf("COMMAND: *%.4000s*\n",  input));
    cmdp_state state = cmdp_state::INITIAL;
    stack stack{};
    struct criteria_state *criteria_state = applier->create_criteria_state(std::to_underlying(state));
    auto result = CommandResult{};

    command_output.applier = applier;
    command_output.client = data.client;

    /* A YAJL JSON generator used for formatting replies. */
    command_output.json_gen = data.gen;

    command_output.needs_tree_render = false;

    std::string::const_iterator walk = input.begin();
    size_t const len = input.length();

    /* The "<=" operator is intentional: We also handle the terminating 0-byte
     * explicitly by looking for an 'end' token. */
    while (walk <= input.cend()) {
        /* skip whitespace and newlines before every token */
        while ((*walk == ' ' || *walk == '\t' ||
                *walk == '\r' || *walk == '\n') &&
               *walk != '\0') {
            walk++;
        }

        std::vector<cmdp_token> &ptr = tokens.at(state);
        bool token_handled = false;
        for (unsigned long c = 0; c < ptr.size() && !token_handled; c++) {
            cmdp_token const &token = ptr.at(c);

            /* A literal. */
            if (token.name[0] == '\'') {
                token_handled = handle_literal(walk, token, &state, stack, criteria_state);
            } else if (token.name == "number") {
                token_handled = handle_number(walk, token, &state, stack, criteria_state);
            } else if (token.name == "string" || token.name == "word") {
                token_handled = handle_word(walk, token, &state, stack, criteria_state);
            } else if (token.name == "end") {
                token_handled = handle_end(walk, token, &state, stack, criteria_state);
            }
        }

        if (!token_handled) {
            unhandled_token(result, data.gen, stack, input, len, ptr, walk);
            break;
        }
    }

    delete criteria_state;

    result.needs_tree_render = command_output.needs_tree_render;
    return result;
}
