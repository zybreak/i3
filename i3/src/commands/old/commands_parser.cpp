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
#include <cerrno>
#include <climits>

#include <err.h>
#include <cstdlib>
#include <cstring>
#include <regex>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <nlohmann/json.hpp>

#include "parser_stack.h"
#include "commands.h"
#include "../command_result_ir.h"
#include "../command_result.h"
#include "../base_commands_applier.h"
#include "../commands_parser.h"

import utils;

/*******************************************************************************
 * The data structures used for parsing. Essentially the current state and a
 * list of tokens for that state.
 *
 * The GENERATED_* files are generated by generate-commands-parser.pl with the
 * input parser-specs/commands.spec.
 ******************************************************************************/

#include "GENERATED_command_enums.h"

struct ipc_client;

struct cmdp_token {
    const char *name;
    const char *identifier;
    /* This might be __CALL */
    cmdp_state next_state;
    union {
        uint16_t call_identifier;
    } extra;
};

struct cmdp_token_ptr {
    cmdp_token *array;
    int n;
};

#include "GENERATED_command_tokens.h"

/*******************************************************************************
 * The parser itself.
 ******************************************************************************/

/*******************************************************************************
 * The (small) stack where identified literals are stored during the parsing
 * of a single command (like $workspace).
 ******************************************************************************/
static struct CommandResultIR subcommand_output;
static struct CommandResultIR command_output;

//#include "criteria_state.h"
#include "GENERATED_command_call.h"

[[nodiscard("PURE FUN")]]
static cmdp_state next_state(const cmdp_token &token, stack &stack, struct criteria_state *criteria_state) {
    if (token.next_state == __CALL) {
        subcommand_output.json_gen = command_output.json_gen;
        subcommand_output.client = command_output.client;
        subcommand_output.applier = command_output.applier;
        subcommand_output.needs_tree_render = false;

        try {
            GENERATED_call(criteria_state, stack, token.extra.call_identifier, subcommand_output);
        } catch (const std::exception &e) {
            if (subcommand_output.json_gen != nullptr) {
                subcommand_output.json_gen->push_back({
                        { "success", false },
                        { "error", e.what() }
                });
            }
        }

        auto state = (cmdp_state)subcommand_output.next_state;
        /* If any subcommand requires a tree_render(), we need to make the
         * whole parser result request a tree_render(). */
        if (subcommand_output.needs_tree_render)
            command_output.needs_tree_render = true;
        clear_stack(stack);
        return state;
    }

    auto state = token.next_state;
    if (state == INITIAL) {
        clear_stack(stack);
    }
    return state;
}


bool handle_literal(const char **walk, const cmdp_token &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {

    if (strncasecmp(*walk, token.name + 1, strlen(token.name) - 1) == 0) {
        if (token.identifier != nullptr) {
            push_string(stack, token.identifier, sstrdup(token.name + 1));
        }
        *walk += strlen(token.name) - 1;
        *state = next_state(token, stack, criteria_state);
        return true;
    }

    return false;
}

bool handle_number(const char **walk, const cmdp_token &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    /* Handle numbers. We only accept decimal numbers for now. */
    char *end = nullptr;
    errno = 0;
    long int num = strtol(*walk, &end, 10);
    if ((errno == ERANGE && (num == LONG_MIN || num == LONG_MAX)) ||
        (errno != 0 && num == 0))
        return false;

    /* No valid numbers found */
    if (end == *walk)
        return false;

    if (token.identifier != nullptr) {
        push_long(stack, token.identifier, num);
    }

    /* Set walk to the first non-number character */
    *walk = end;
    *state = next_state(token, stack, criteria_state);
    return true;
}

bool handle_word(const char **walk, const cmdp_token &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    char *str = utils::parse_string(walk, (token.name[0] != 's'));
    if (str != nullptr) {
        if (token.identifier) {
            push_string(stack, token.identifier, str);
        }
        /* If we are at the end of a quoted string, skip the ending
                     * double quote. */
        if (**walk == '"')
            (*walk)++;
        *state = next_state(token, stack, criteria_state);
        return true;
    }
    return false;
}

bool handle_end(const char **walk, const cmdp_token &token, cmdp_state *state, stack &stack, criteria_state *criteria_state) {
    if (**walk == '\0' || **walk == ',' || **walk == ';') {
        *state = next_state(token, stack, criteria_state);
        /* To make sure we start with an appropriate matching
                     * datastructure for commands which do *not* specify any
                     * criteria, we re-initialize the criteria system after
                     * every command. */
        if (**walk == '\0' || **walk == ';')
            cmd::criteria_init(criteria_state, subcommand_output);
        (*walk)++;
        return true;
    }

    return false;
}

void unhandled_token(CommandResult &result, nlohmann::json *gen, stack &stack, const std::string &input, const size_t len, cmdp_token_ptr &ptr, const char **walk) {
    /* Figure out how much memory we will need to fill in the names of
             * all tokens afterwards. */
    size_t tokenlen = 0;
    for (int c = 0; c < ptr.n; c++)
        tokenlen += strlen(ptr.array[c].name) + strlen("'', ");

    /* Build up a decent error message. We include the problem, the
             * full input, and underline the position where the parser
             * currently is. */
    std::string possible_tokens{};
    possible_tokens.reserve(tokenlen + 1);
    for (int c = 0; c < ptr.n; c++) {
        auto &token = ptr.array[c];
        if (token.name[0] == '\'') {
            /* A literal is copied to the error message enclosed with
                     * single quotes. */
            possible_tokens += '\'';
            possible_tokens += (token.name + 1);
            possible_tokens += '\'';
        } else {
            /* Any other token is copied to the error message enclosed
                     * with angle brackets. */
            possible_tokens += '<';
            possible_tokens += token.name;
            possible_tokens += '>';
        }
        if (c < (ptr.n - 1)) {
            possible_tokens += ", ";
        }
    }

    auto errormessage = fmt::format("Expected one of these tokens: {}", possible_tokens);

    /* Contains the same amount of characters as 'input' has, but with
             * the unparseable part highlighted using ^ characters. */
    std::string position(len, '\0');
    std::fill_n(position.begin(), len - strlen(*walk), ' ');
    std::fill_n(position.begin() + (len - strlen(*walk)), strlen(*walk), '^');

    //ELOG(fmt::sprintf("%s\n",  errormessage));
    //ELOG(fmt::sprintf("Your command: %s\n",  input));
    //ELOG(fmt::sprintf("              %s\n",  position));

    result.parse_error = true;
    result.error_message = errormessage;

    /* Format this error message as a JSON reply. */
    if (gen != nullptr) {
        gen->push_back({
            {"success",       false},
            /* We set parse_error to true to distinguish this from other
                     * errors. i3-nagbar is spawned upon keypresses only for parser
                     * errors. */
            {"parse_error",   true},
            {"error",         errormessage},
            {"input",         input},
            {"errorposition", position}
        });
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
CommandResult parse_command_old(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier &applier) {
    //DLOG(fmt::sprintf("COMMAND: *%.4000s*\n",  input));
    cmdp_state state = INITIAL;
    stack stack{};
    struct criteria_state* criteria_state = applier.create_criteria_state();
    auto result = CommandResult{};

    command_output.applier = &applier;
    command_output.client = client;

    /* A YAJL JSON generator used for formatting replies. */
    command_output.json_gen = gen;

    command_output.needs_tree_render = false;

    const char *walk = input.c_str();
    const size_t len = input.length();
    int c;

    /* The "<=" operator is intentional: We also handle the terminating 0-byte
     * explicitly by looking for an 'end' token. */
    while ((size_t)(walk - input.c_str()) <= len) {
        /* skip whitespace and newlines before every token */
        while ((*walk == ' ' || *walk == '\t' ||
                *walk == '\r' || *walk == '\n') &&
               *walk != '\0')
            walk++;

        cmdp_token_ptr &ptr = tokens[state];
        bool token_handled = false;
        for (c = 0; c < ptr.n && !token_handled; c++) {
            const cmdp_token &token = ptr.array[c];

            /* A literal. */
            if (token.name[0] == '\'') {
                token_handled = handle_literal(&walk, token, &state, stack, criteria_state);
            } else if (strcmp(token.name, "number") == 0) {
                token_handled = handle_number(&walk, token, &state, stack, criteria_state);
            } else if (strcmp(token.name, "string") == 0 || strcmp(token.name, "word") == 0) {
                token_handled = handle_word(&walk, token, &state, stack, criteria_state);
            } else if (strcmp(token.name, "end") == 0) {
                token_handled = handle_end(&walk, token, &state, stack, criteria_state);
            }
        }

        if (!token_handled) {
            unhandled_token(result, gen, stack, input, len, ptr, &walk);
            break;
        }
    }

    result.needs_tree_render = command_output.needs_tree_render;
    return result;
}
