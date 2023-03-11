/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_parser.c: hand-written parser to parse configuration directives.
 *
 * See also src/commands_parser.c for rationale on why we use a custom parser.
 *
 * This parser works VERY MUCH like src/commands_parser.c, so read that first.
 * The differences are:
 *
 * 1. config_parser supports the 'number' token type (in addition to 'word' and
 *    'string'). Numbers are referred to using &num (like $str).
 *
 * 2. Criteria are not executed immediately, they are just stored.
 *
 * 3. config_parser recognizes \n and \r as 'end' token, while commands_parser
 *    ignores them.
 *
 * 4. config_parser skips the current line on invalid inputs and follows the
 *    nearest <error> token.
 *
 */
#include <cassert>
#include <cerrno>
#include <climits>
#include <sstream>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"

#include "parser_stack.h"
#include "util.h"
#include "i3.h"
#include "configuration.h"
#include "bindings.h"
#include "config_directives.h"
#include "config_parser.h"
#include "nagbar.h"

#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

#include <xcb/xcb_xrm.h>

/*******************************************************************************
 * The data structures used for parsing. Essentially the current state and a
 * list of tokens for that state.
 *
 * The GENERATED_* files are generated by generate-commands-parser.pl with the
 * input parser-specs/configs.spec.
 ******************************************************************************/

#include "GENERATED_config_enums.h"

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

#include "GENERATED_config_tokens.h"

/*******************************************************************************
 * The parser itself.
 ******************************************************************************/

#include "GENERATED_config_call.h"

static void next_state(const cmdp_token *token, struct parser_ctx &ctx) {
    cmdp_state _next_state = token->next_state;

    //printf("token = name %s identifier %s\n", token->name, token->identifier);
    //printf("next_state = %d\n", token->next_state);
    if (token->next_state == __CALL) {
        struct ConfigResultIR subcommand_output = {
            .ctx = ctx,
        };
        GENERATED_call(ctx.criteria_state, ctx.stack, token->extra.call_identifier, subcommand_output);
        if (subcommand_output.has_errors) {
            ctx.has_errors = true;
        }
        _next_state = (cmdp_state)subcommand_output.next_state;
        clear_stack(ctx.stack);
    }

    ctx.state = _next_state;
    if (ctx.state == INITIAL) {
        clear_stack(ctx.stack);
    }

    /* See if we are jumping back to a state in which we were in previously
     * (statelist contains INITIAL) and just move statelist_idx accordingly. */
    for (int i = 0; i < ctx.statelist_idx; i++) {
        if ((cmdp_state)(ctx.statelist[i]) != _next_state) {
            continue;
        }
        ctx.statelist_idx = i + 1;
        return;
    }

    /* Otherwise, the state is new and we add it to the list */
    ctx.statelist[ctx.statelist_idx++] = _next_state;
}

/*
 * Returns a pointer to the start of the line (one byte after the previous \r,
 * \n) or the start of the input, if this is the first line.
 *
 */
static const char *start_of_line(const char *walk, const std::string &beginning) {
    while (walk >= beginning.c_str() && *walk != '\n' && *walk != '\r') {
        walk--;
    }

    return walk + 1;
}

/*
 * Copies the line and terminates it at the next \n, if any.
 *
 * The caller has to free() the result.
 *
 */
static char *single_line(const char *start) {
    char *result = sstrdup(start);
    char *end = strchr(result, '\n');
    if (end != nullptr)
        *end = '\0';
    return result;
}

static std::string get_possible_tokens(const cmdp_token_ptr *ptr) {
    std::string possible_tokens{};
    for (int c = 0; c < ptr->n; c++) {
        auto token = &(ptr->array[c]);
        if (token->name[0] == '\'') {
            /* A literal is copied to the error message enclosed with
             * single quotes. */
            possible_tokens.append(fmt::format("'{}'", token->name + 1));
        } else {
            /* Skip error tokens in error messages, they are used
             * internally only and might confuse users. */
            if (strcmp(token->name, "error") == 0)
                continue;
            /* Any other token is copied to the error message enclosed
             * with angle brackets. */
            possible_tokens.append(fmt::format("<{}>", token->name));
        }
        if (c < (ptr->n - 1)) {
            possible_tokens.append(", ");
        }
    }

    return possible_tokens;
}

/* Figure out how much memory we will need to fill in the names of
 * all tokens afterwards. */
static void unhandled_token(const std::string &input, const char *filename, int linecnt, const size_t len, const cmdp_token_ptr *ptr, parser_ctx &ctx, bool &has_errors, const char **walk) {
    /* Build up a decent error message. We include the problem, the
     * full input, and underline the position where the parser
     * currently is. */
    std::string possible_tokens = get_possible_tokens(ptr);
    /* Go back to the beginning of the line */
    const char *error_line = start_of_line(*walk, input);

    /* Contains the same amount of characters as 'input' has, but with
     * the unparseable part highlighted using ^ characters. */
    char *position = (char*)scalloc(strlen(error_line) + 1, 1);
    const char *copywalk;
    for (copywalk = error_line;
         *copywalk != '\n' && *copywalk != '\r' && *copywalk != '\0';
         copywalk++)
        position[(copywalk - error_line)] = (copywalk >= *walk ? '^' : (*copywalk == '\t' ? '\t' : ' '));
    position[(copywalk - error_line)] = '\0';

    ELOG(fmt::sprintf("CONFIG: Expected one of these tokens: %s\n", possible_tokens));
    ELOG(fmt::sprintf("CONFIG: (in file %s)\n", filename));
    char *error_copy = single_line(error_line);

    /* Print context lines *before* the error, if any. */
    if (linecnt > 1) {
        const char *context_p1_start = start_of_line(error_line - 2, input);
        char *context_p1_line = single_line(context_p1_start);
        if (linecnt > 2) {
            const char *context_p2_start = start_of_line(context_p1_start - 2, input);
            char *context_p2_line = single_line(context_p2_start);
            ELOG(fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt - 2, context_p2_line));
            free(context_p2_line);
        }
        ELOG(fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt - 1, context_p1_line));
        free(context_p1_line);
    }
    ELOG(fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt, error_copy));
    ELOG(fmt::sprintf("CONFIG:           %s\n",  position));
    free(error_copy);
    /* Print context lines *after* the error, if any. */
    for (int i = 0; i < 2; i++) {
        char *error_line_end = (char*)strchr(error_line, '\n');
        if (error_line_end != nullptr && *(error_line_end + 1) != '\0') {
            error_line = error_line_end + 1;
            error_copy = single_line(error_line);
            ELOG(fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt + i + 1, error_copy));
            free(error_copy);
        }
    }

    has_errors = true;

    /* Skip the rest of this line, but continue parsing. */
    while ((size_t)(*walk - input.c_str()) <= len && **walk != '\n')
        (*walk)++;

    free(position);
    clear_stack(ctx.stack);

    /* To figure out in which state to go (e.g. MODE or INITIAL),
     * we find the nearest state which contains an <error> token
     * and follow that one. */
    bool error_token_found = false;
    for (int i = ctx.statelist_idx - 1; (i >= 0) && !error_token_found; i--) {
        cmdp_token_ptr *errptr = &(tokens[ctx.statelist[i]]);
        for (int j = 0; j < errptr->n; j++) {
            if (strcmp(errptr->array[j].name, "error") != 0)
                continue;
            next_state(&(errptr->array[j]), ctx);
            error_token_found = true;
            break;
        }
        linecnt++;
    }

    assert(error_token_found);
}

/* Dump the entire config file into the debug log. We cannot just use
 * DLOG(fmt::sprintf("%s",  input)); because one log message must not exceed 4 KiB. */
static void log_config(const std::string &input) {
    std::istringstream iss(input);

    int linecnt = 1;
    for (std::string line; std::getline(iss, line); linecnt++) {
        DLOG(fmt::sprintf("CONFIG(line %3d): %s\n",  linecnt, line));
    }
}

static bool handle_literal(const char **walk, const cmdp_token *token, parser_ctx &ctx) {
    if (strncasecmp(*walk, token->name + 1, strlen(token->name) - 1) == 0) {
        if (token->identifier != nullptr) {
            push_string_append(ctx.stack, token->identifier, token->name + 1);
        }
        *walk += strlen(token->name) - 1;
        next_state(token, ctx);
        return true;
    }

    return false;
}

static bool handle_number(const char **walk, const cmdp_token *token, parser_ctx &ctx) {
    char *end = nullptr;
    errno = 0;
    long int num = strtol(*walk, &end, 10);
    if ((errno == ERANGE && (num == LONG_MIN || num == LONG_MAX)) ||
        (errno != 0 && num == 0))
        return false;

    /* No valid numbers found */
    if (end == *walk)
        return false;

    if (token->identifier != nullptr) {
        push_long(ctx.stack, token->identifier, num);
    }

    /* Set walk to the first non-number character */
    *walk = end;
    next_state(token, ctx);
    return true;
}

static bool handle_word(const char **walk, const cmdp_token *token, parser_ctx &ctx) {
    const char *beginning = *walk;
    /* Handle quoted strings (or words). */
    if (**walk == '"') {
        beginning++;
        (*walk)++;
        while (**walk != '\0' && (**walk != '"' || *(*walk - 1) == '\\'))
            (*walk)++;
    } else {
        if (token->name[0] == 's') {
            while (**walk != '\0' && **walk != '\r' && **walk != '\n')
                (*walk)++;
        } else {
            /* For a word, the delimiters are white space (' ' or
                         * '\t'), closing square bracket (]), comma (,) and
                         * semicolon (;). */
            while (**walk != ' ' && **walk != '\t' &&
                   **walk != ']' && **walk != ',' &&
                   **walk != ';' && **walk != '\r' &&
                   **walk != '\n' && **walk != '\0')
                (*walk)++;
        }
    }
    if (*walk != beginning) {
        char *str = (char*)scalloc(*walk - beginning + 1, 1);
        /* We copy manually to handle escaping of characters. */
        int inpos, outpos;
        for (inpos = 0, outpos = 0;
             inpos < (*walk - beginning);
             inpos++, outpos++) {
            /* We only handle escaped double quotes to not break
                         * backwards compatibility with people using \w in
                         * regular expressions etc. */
            if (beginning[inpos] == '\\' && beginning[inpos + 1] == '"')
                inpos++;
            str[outpos] = beginning[inpos];
        }
        if (token->identifier) {
            push_string_append(ctx.stack, token->identifier, str);
        }
        free(str);
        /* If we are at the end of a quoted string, skip the ending
                     * double quote. */
        if (**walk == '"')
            (*walk)++;
        next_state(token, ctx);
        return true;
    }

    return false;
}

static bool handle_line(const char **walk, const cmdp_token *token, parser_ctx &ctx) {
    while (**walk != '\0' && **walk != '\n' && **walk != '\r')
        (*walk)++;
    next_state(token, ctx);
    (*walk)++;
    return true;
}

static bool handle_end(const char **walk, const cmdp_token *token, parser_ctx &ctx, ConfigResultIR &subcommand_output, int *linecnt) {
    //printf("checking for end: *%s*\n", walk);
    if (**walk == '\0' || **walk == '\n' || **walk == '\r') {
        next_state(token, ctx);
        /* To make sure we start with an appropriate matching
                     * datastructure for commands which do *not* specify any
                     * criteria, we re-initialize the criteria system after
                     * every command. */
        cfg::criteria_init(ctx.criteria_state, subcommand_output, INITIAL);
        (*linecnt)++;
        (*walk)++;

        return true;
    }

    return false;
}

static void reset_statelist(parser_ctx &ctx) {
    ctx.state = INITIAL;
    for (int & i : ctx.statelist) {
        i = INITIAL;
    }
    ctx.statelist_idx = 1;
}

Variable::~Variable() {
    FREE(key);
    FREE(value);
}

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename) {
    bool has_errors = false;
    const char *walk = input.c_str();
    const size_t len = input.size();
    int linecnt = 1;

    log_config(input);

    reset_statelist(ctx);

    struct ConfigResultIR subcommand_output = {
        .ctx = ctx,
    };

    cfg::criteria_init(ctx.criteria_state, subcommand_output, INITIAL);

    /* The "<=" operator is intentional: We also handle the terminating 0-byte
     * explicitly by looking for an 'end' token. */
    while ((size_t)(walk - input.c_str()) <= len) {
        /* Skip whitespace before every token, newlines are relevant since they
         * separate configuration directives. */
        while ((*walk == ' ' || *walk == '\t') && *walk != '\0')
            walk++;

        //printf("remaining input: %s\n", walk);

        cmdp_token_ptr *ptr = &(tokens[ctx.state]);
        bool token_handled = false;
        for (int c = 0; c < ptr->n && !token_handled; c++) {
            auto token = &(ptr->array[c]);

            /* A literal. */
            if (token->name[0] == '\'') {
                token_handled = handle_literal(&walk, token, ctx);
            } else if (strcmp(token->name, "number") == 0) {
                /* Handle numbers. We only accept decimal numbers for now. */
                token_handled = handle_number(&walk, token, ctx);
            } else if (strcmp(token->name, "string") == 0 || strcmp(token->name, "word") == 0) {
                token_handled = handle_word(&walk, token, ctx);
            } else if (strcmp(token->name, "line") == 0) {
                token_handled = handle_line(&walk, token, ctx);
                linecnt++;
            } else if (strcmp(token->name, "end") == 0) {
               token_handled = handle_end(&walk, token, ctx, subcommand_output, &linecnt);
            }
        }

        if (!token_handled) {
            unhandled_token(input, filename, linecnt, len, ptr, ctx, has_errors, &walk);
        }
    }

    return has_errors;
}

#ifndef TEST_PARSER
/**
 * Launch nagbar to indicate errors in the configuration file.
 */
void start_config_error_nagbar(bool has_errors) {
    const char *font_pattern = config.font.pattern ? config.font.pattern : "fixed";
    auto type = has_errors ? TYPE_ERROR : TYPE_WARNING;
    const char *text = has_errors ? "You have an error in your i3 config file!" : "Your config is outdated. Please fix the warnings to make sure everything works.";

    std::vector<button_t> buttons{};
    start_nagbar(&global.config_error_nagbar_pid, buttons, text, font_pattern, type);
}
#endif

class ResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;
   public:

    explicit ResourceDatabase(xcb_connection_t *conn) : conn(conn) {
    }

    char* get_resource(char *name, const char *fallback) {
        if (conn == nullptr) {
            return nullptr;
        }

        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return strdup(fallback);
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name, nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n",  name, fallback));
            resource = strdup(fallback);
        }

        return resource;
    }

    ~ResourceDatabase() {
        if (database) {
            xcb_xrm_database_free(database);
        }
    }
};

/*
 * Inserts or updates a variable assignment depending on whether it already exists.
 *
 */
static void upsert_variable(std::vector<std::shared_ptr<Variable>> &variables, char *key, char *value) {
    for (auto &current : variables) {
        if (strcmp(current->key, key) != 0) {
            continue;
        }

        DLOG(fmt::sprintf("Updated variable: %s = %s -> %s\n",  key, current->value, value));
        FREE(current->value);
        current->value = sstrdup(value);
        return;
    }

    DLOG(fmt::sprintf("Defined new variable: %s = %s\n",  key, value));
    auto n = std::make_shared<Variable>();
    auto loc = variables.begin();
    n->key = sstrdup(key);
    n->value = sstrdup(value);
    /* ensure that the correct variable is matched in case of one being
     * the prefix of another */
    while (loc != variables.end()) {
        if (strlen(n->key) >= strlen((*loc)->key))
            break;
        ++loc;
    }

    if (loc == variables.end()) {
        variables.insert(variables.begin(), n);
    } else {
        variables.insert(loc, n);
    }
}

/* We need to copy the buffer because we need to invalidate the
 * variables (otherwise we will count them twice, which is bad when
 * 'extra' is negative)
 */
static size_t count_extra_bytes(const char *buf, __off_t size, parser_ctx &ctx) {
    size_t extra_bytes = 0;
    char *bufcopy = sstrdup(buf);
    for (auto &variable : ctx.variables) {
        auto current = variable.get();
        size_t extra = (strlen(current->value) - strlen(current->key));
        char *next;
        for (next = bufcopy;
             next < (bufcopy + size) &&
             (next = strcasestr(next, current->key)) != nullptr;
             next += strlen(current->key)) {
            *next = '_';
            extra_bytes += extra;
        }
    }
    free(bufcopy);

    return extra_bytes;
}

static bool read_file(FILE *fstr, char *buf, parser_ctx &ctx) {
    ResourceDatabase resourceDatabase{*global.a};
    bool invalid_sets = false;
    char buffer[4096], key[512], value[4096], *continuation = nullptr;

    while (!feof(fstr)) {
        if (!continuation)
            continuation = buffer;
        if (fgets(continuation, sizeof(buffer) - (continuation - buffer), fstr) == nullptr) {
            if (feof(fstr))
                break;
            throw std::runtime_error("Unexpected EOF");
        }
        if (buffer[strlen(buffer) - 1] != '\n' && !feof(fstr)) {
            ELOG(fmt::sprintf("Your line continuation is too long, it exceeds %zd bytes\n",  sizeof(buffer)));
        }

        /* sscanf implicitly strips whitespace. */
        value[0] = '\0';
        const bool skip_line = (sscanf(buffer, "%511s %4095[^\n]", key, value) < 1 || strlen(key) < 3);
        const bool comment = (key[0] == '#');
        value[4095] = '\n';

        continuation = strstr(buffer, "\\\n");
        if (continuation) {
            if (!comment) {
                continue;
            }
            DLOG(fmt::sprintf("line continuation in comment is ignored: \"%.*s\"\n", (int)strlen(buffer) - 1, buffer));
            continuation = nullptr;
        }

        strncpy(buf + strlen(buf), buffer, strlen(buffer) + 1);

        /* Skip comments and empty lines. */
        if (skip_line || comment) {
            continue;
        }

        if (strcasecmp(key, "set") == 0 && *value != '\0') {
            char v_key[512];
            char v_value[4096] = {'\0'};

            if (sscanf(value, "%511s %4095[^\n]", v_key, v_value) < 1) {
                ELOG(fmt::sprintf("Failed to parse variable specification '%s', skipping it.\n",  value));
                invalid_sets = true;
                continue;
            }

            if (v_key[0] != '$') {
                ELOG("Malformed variable assignment, name has to start with $\n");
                invalid_sets = true;
                continue;
            }

            upsert_variable(ctx.variables, v_key, v_value);
            continue;
        } else if (strcasecmp(key, "set_from_resource") == 0) {
            char res_name[512] = {'\0'};
            char v_key[512];
            char fallback[4096] = {'\0'};

            /* Ensure that this string is terminated. For example, a user might
             * want a variable to be empty if the resource can't be found and
             * uses
             *   set_from_resource $foo i3wm.foo
             * Without explicitly terminating the string first, sscanf() will
             * leave it uninitialized, causing garbage in the config.*/
            fallback[0] = '\0';

            if (sscanf(value, "%511s %511s %4095[^\n]", v_key, res_name, fallback) < 1) {
                ELOG(fmt::sprintf("Failed to parse resource specification '%s', skipping it.\n",  value));
                invalid_sets = true;
                continue;
            }

            if (v_key[0] != '$') {
                ELOG("Malformed variable assignment, name has to start with $\n");
                invalid_sets = true;
                continue;
            }
#ifndef TEST_PARSER
            char *res_value = resourceDatabase.get_resource(res_name, fallback);

            upsert_variable(ctx.variables, v_key, res_value);
            free(res_value);
#endif
            continue;
        }
    }

    return invalid_sets;
}

/* Allocate a new buffer and copy the file over to the new one,
 * and replace occurrences of our variables */
static char* replace_variables(char *n, char *buf, __off_t size, parser_ctx &ctx) {
    char *walk = buf, *destwalk;
    destwalk = n;
    while (walk < (buf + size)) {
        std::map<char*, char*> next_match{};
        struct Variable *nearest = nullptr;
        /* Find the next variable */
        for (auto &variable : ctx.variables) {
            char *match = strcasestr(walk, variable->key);
            if (match != nullptr) {
                next_match[variable->key] = match;
            }
        }
        long distance = size;
        for (auto &variable : ctx.variables) {
            if (!next_match.contains(variable->key))
                continue;
            if ((next_match[variable->key] - walk) < distance) {
                distance = (next_match[variable->key] - walk);
                nearest = variable.get();
            }
        }
        if (nearest == nullptr) {
            /* If there are no more variables, we just copy the rest */
            strncpy(destwalk, walk, (buf + size) - walk);
            destwalk += (buf + size) - walk;
            *destwalk = '\0';
            break;
        } else {
            /* Copy until the next variable, then copy its value */
            strncpy(destwalk, walk, distance);
            strncpy(destwalk + distance, nearest->value, strlen(nearest->value));
            walk += distance + strlen(nearest->key);
            destwalk += distance + strlen(nearest->value);
        }
    }

    return n;
}


OldParser::OldParser(const char *filename, struct parser_ctx &parent_ctx) : OldParser(filename, parent_ctx.load_type) {
    this->parent_ctx = &parent_ctx;
    this->ctx.variables = parent_ctx.variables;
}

OldParser::OldParser(const char *filename, config_load_t load_type) : filename(filename) {
    this->ctx.load_type = load_type;
    this->old_dir = get_current_dir_name();
    char *dir = nullptr;
    /* dirname(3) might modify the buffer, so make a copy: */
    char *dirbuf = sstrdup(filename);
    if ((dir = dirname(dirbuf)) != nullptr) {
        LOG(fmt::sprintf("Changing working directory to config file directory %s\n",  dir));
        if (chdir(dir) == -1) {
            throw std::runtime_error(fmt::sprintf("chdir(%s) failed: %s\n", dir, strerror(errno)));
        }
    }
    free(dirbuf);

    if ((fd = open(filename, O_RDONLY)) == -1) {
        throw std::runtime_error(fmt::sprintf("cant open file: %s\n", filename));
    }

    if ((fstr = fdopen(fd, "r")) == nullptr) {
        throw std::runtime_error(fmt::sprintf("cant open file: %s\n", filename));
    }
}

OldParser::~OldParser() {
    chdir(this->old_dir);
    free(this->old_dir);
    fclose(fstr);
}

/*
 * Parses the given file by first replacing the variables, then calling
 * parse_config and possibly launching i3-nagbar.
 *
 */
parse_file_result_t OldParser::parse_file() {
    struct stat stbuf{};

    if (fstat(fd, &stbuf) == -1) {
        return PARSE_FILE_FAILED;
    }

    char *buf = (char*)scalloc(stbuf.st_size + 1, 1);

#ifndef TEST_PARSER
    if (current_config == nullptr) {
        current_config = (char*)scalloc(stbuf.st_size + 1, 1);
        if ((ssize_t)fread(current_config, 1, stbuf.st_size, fstr) != stbuf.st_size) {
            return PARSE_FILE_FAILED;
        }
        rewind(fstr);
    }
#endif

    bool invalid_sets;

    try {
        invalid_sets = read_file(fstr, buf, ctx);
    } catch (const std::exception &e) {
        ELOG(fmt::sprintf("Failed to read config file: %s\n", e.what()));
        return PARSE_FILE_FAILED;
    }

    /* For every custom variable, see how often it occurs in the file and
     * how much extra bytes it requires when replaced. */
    auto extra_bytes = count_extra_bytes(buf, stbuf.st_size, ctx);

    char *n = (char*)scalloc(stbuf.st_size + extra_bytes + 1, 1);

    /* Then, allocate a new buffer and copy the file over to the new one,
     * but replace occurrences of our variables */
    replace_variables(n, buf, stbuf.st_size, ctx);

    bool has_errors = parse_config(ctx, n, filename);
    if (ctx.has_errors) {
        has_errors = true;
    }

#ifndef TEST_PARSER
    if (has_duplicate_bindings()) {
        has_errors = true;
    }
#endif

    auto use_nagbar = (ctx.load_type != config_load_t::C_VALIDATE);

    if (use_nagbar && (has_errors || invalid_sets)) {
#ifndef TEST_PARSER
        ELOG(fmt::sprintf("FYI: You are using i3 version %s\n",  i3_version));

        start_config_error_nagbar(has_errors || invalid_sets);
#endif
    }

    free(n);
    free(buf);

    if (has_errors) {
        return PARSE_FILE_CONFIG_ERRORS;
    }
    return PARSE_FILE_SUCCESS;
}

