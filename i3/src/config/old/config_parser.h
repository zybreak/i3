/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_parser.h: config parser-related definitions
 *
 */
#pragma once

#include <config.h>

#include "parser_stack.h"
#include "../base_parser.h"

/**
 * Holds a user-assigned variable for parsing the configuration file. The key
 * is replaced by value in every following line of the file.
 *
 */
struct Variable {
    char *key;
    char *value;

    ~Variable();
};

class parser_ctx {
   public:
    config_load_t load_type;
    BaseConfigApplier &applier;
    int state;
    struct criteria_state criteria_state{};

    /* A list which contains the states that lead to the current state, e.g.
   * INITIAL, WORKSPACE_LAYOUT.
   * When jumping back to INITIAL, statelist_idx will simply be set to 1
   * (likewise for other states, e.g. MODE or BAR).
   * This list is used to process the nearest error token. */
    int statelist[10];
    /* NB: statelist_idx points to where the next entry will be inserted */
    int statelist_idx;

    /*******************************************************************************
   * The (small) stack where identified literals are stored during the parsing
   * of a single config directive (like $workspace).
   ******************************************************************************/
    struct stack stack{};

    std::vector<std::shared_ptr<Variable>> variables{};

    bool has_errors{false};

    parser_ctx(BaseConfigApplier &applier, config_load_t load_type);

    ~parser_ctx() = default;
};

/**
 * An intermediate reprsentation of the result of a parse_config call.
 * Currently unused, but the JSON output will be useful in the future when we
 * implement a config parsing IPC command.
 *
 */
struct ConfigResultIR {
    struct parser_ctx ctx;

    /* The next state to transition to. Passed to the function so that we can
     * determine the next state as a result of a function call, like
     * cfg_criteria_pop_state() does. */
    int next_state;

    /* Whether any error happened while processing this config directive. */
    bool has_errors;
};

/**
 * launch nagbar to indicate errors in the configuration file.
 */
void start_config_error_nagbar(bool has_errors);

/**
 * Parses the given file by first replacing the variables, then calling
 * parse_config and launching i3-nagbar if use_nagbar is true.
 *
 * The return value is a boolean indicating whether there were errors during
 * parsing.
 *
 */
class OldParser : public BaseParser {
private:
    const char *filename;
    char *old_dir;
    int fd;
    FILE *fstr;
public:
    struct parser_ctx *parent_ctx = nullptr;
    struct parser_ctx ctx;
    OldParser(const char *filename, struct parser_ctx &parent_ctx, BaseConfigApplier &applier);
    OldParser(const char *filename, config_load_t load_type, BaseConfigApplier &applier);
    ~OldParser() override;
    parse_file_result_t parse_file() override;
};
