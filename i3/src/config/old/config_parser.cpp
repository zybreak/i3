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
module;
struct criteria_state;

#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>

#include <xcb/xcb_xrm.h>

#include <fmt/printf.h>
module i3_config_old;

import std;
import utils;
import log;
import i3_config_base;

static void next_state(const cmdp_token &token, parser_ctx &ctx, OldParser &parser) {
    cmdp_state _next_state = token.next_state;

    if (token.next_state == cmdp_state::__CALL) {
        ConfigResultIR subcommand_output = {
            .parser = parser,
            .next_state = 0,
            .has_errors = false
        };
        GENERATED_call(ctx.criteria_state, ctx.stack, token.call_identifier, subcommand_output);
        if (subcommand_output.has_errors) {
            ctx.has_errors = true;
        }
        _next_state = static_cast<cmdp_state>(subcommand_output.next_state);
        clear_stack(ctx.stack);
    }

    ctx.state = std::to_underlying(_next_state);
    if (ctx.state == std::to_underlying(cmdp_state::INITIAL)) {
        clear_stack(ctx.stack);
    }

    /* See if we are jumping back to a state in which we were in previously
     * (statelist contains INITIAL) and just move statelist_idx accordingly. */
    for (int i = 0; i < ctx.statelist_idx; i++) {
        if (static_cast<cmdp_state>(ctx.statelist[i]) != _next_state) {
            continue;
        }
        ctx.statelist_idx = i + 1;
        return;
    }

    /* Otherwise, the state is new and we add it to the list */
    ctx.statelist[ctx.statelist_idx++] = std::to_underlying(_next_state);
}

/*
 * Returns a pointer to the start of the line (one byte after the previous \r,
 * \n) or the start of the input, if this is the first line.
 *
 */
static std::string::const_iterator start_of_line(std::string::const_iterator walk, std::string::const_iterator beginning) {
    while (walk >= beginning && *walk != '\n' && *walk != '\r') {
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
static std::string_view single_line(std::string::const_iterator &start, std::string::const_iterator &end) {
    auto newline_pos = std::find(start, end, '\n');

    return std::string_view(start, newline_pos);
}

static std::string get_possible_tokens(const std::vector<cmdp_token> &ptr) {
    std::string possible_tokens{};
    for (unsigned long c = 0; c < ptr.size(); c++) {
        auto &token = ptr.at(c);
        if (token.name[0] == '\'') {
            /* A literal is copied to the error message enclosed with
             * single quotes. */
            possible_tokens.append(fmt::format("'{}'", token.name.substr(1)));
        } else {
            /* Skip error tokens in error messages, they are used
             * internally only and might confuse users. */
            if (token.name == "error") {
                continue;
            }
            /* Any other token is copied to the error message enclosed
             * with angle brackets. */
            possible_tokens.append(fmt::format("<{}>", token.name));
        }
        if (c < (ptr.size() - 1)) {
            possible_tokens.append(", ");
        }
    }

    return possible_tokens;
}

/* Figure out how much memory we will need to fill in the names of
 * all tokens afterwards. */
void OldParser::unhandled_token(const std::string &input, int linecnt, const std::vector<cmdp_token> &ptr, bool &has_errors, std::string::const_iterator &walk, std::ostream &err_output) {
    /* Build up a decent error message. We include the problem, the
     * full input, and underline the position where the parser
     * currently is. */
    std::string possible_tokens = get_possible_tokens(ptr);
    /* Go back to the beginning of the line */
    auto beginning = input.cbegin();
    auto input_end = input.cend();
    std::string::const_iterator error_line = start_of_line(walk, beginning);

    /* Contains the same amount of characters as 'input' has, but with
     * the unparsable part highlighted using ^ characters. */
    std::string position{};
    //position.reserve(std::distance(error_line, input.end()));
    std::string::const_iterator copywalk;
    for (copywalk = error_line;
         *copywalk != '\n' && *copywalk != '\r' && *copywalk != '\0';
         copywalk++) {
        position.push_back((copywalk >= walk ? '^' : (*copywalk == '\t' ? '\t' : ' ')));
    }

    err_output << fmt::sprintf("CONFIG: Expected one of these tokens: %s\n", possible_tokens);
    err_output << fmt::sprintf("CONFIG: (in file %s)\n", filename.native());
    std::string_view error_copy = single_line(error_line, input_end);

    /* Print context lines *before* the error, if any. */
    if (linecnt > 1) {
        auto context_p1_start = start_of_line(error_line - 2, beginning);
        std::string_view context_p1_line = single_line(context_p1_start, input_end);
        if (linecnt > 2) {
            auto context_p2_start = start_of_line(context_p1_start - 2, beginning);
            std::string_view context_p2_line = single_line(context_p2_start, input_end);
            err_output << fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt - 2, context_p2_line);
        }
        err_output << fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt - 1, context_p1_line);
    }
    err_output << fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt, error_copy);
    err_output << fmt::sprintf("CONFIG:           %s\n",  position);
    /* Print context lines *after* the error, if any. */
    for (int i = 0; i < 2; i++) {
        auto error_line_end = std::find(error_line, input_end, '\n');
        if (error_line_end != input_end && *(error_line_end + 1) != '\0') {
            error_line = error_line_end + 1;
            error_copy = single_line(error_line, input_end);
            err_output << fmt::sprintf("CONFIG: Line %3d: %s\n",  linecnt + i + 1, error_copy);
        }
    }

    has_errors = true;

    /* Skip the rest of this line, but continue parsing. */
    while (walk != input_end && *walk != '\n') {
        walk++;
    }

    clear_stack(ctx.stack);

    /* To figure out in which state to go (e.g. MODE or INITIAL),
     * we find the nearest state which contains an <error> token
     * and follow that one. */
    bool error_token_found = false;
    for (int i = ctx.statelist_idx - 1; (i >= 0) && !error_token_found; i--) {
        std::vector<cmdp_token> errptr = tokens.at(static_cast<cmdp_state>(ctx.statelist[i]));
        for (unsigned long j = 0; j < errptr.size(); j++) {
            if (errptr.at(j).name != "error") {
                continue;
            }
            next_state(errptr.at(j), ctx, *this);
            error_token_found = true;
            break;
        }
        linecnt++;
    }

    if (!error_token_found) {
        std::terminate();
    }
}

/* Dump the entire config file into the debug log. */
static void log_config(const std::string &input) {
    std::istringstream iss(input);

    int linecnt = 1;
    for (std::string line; std::getline(iss, line); linecnt++) {
        DLOG(fmt::sprintf("CONFIG(line %3d): %s\n",  linecnt, line));
    }
}

bool OldParser::handle_literal(std::string::const_iterator &walk, const cmdp_token &token) {
    if (strncasecmp(std::to_address(walk), token.name.substr(1).c_str(), token.name.size() - 1) == 0) {
        if (token.identifier) {
            push_string_append(ctx.stack, token.identifier->c_str(), token.name.c_str() + 1);
        }
        walk += token.name.length() - 1;
        next_state(token, ctx, *this);
        return true;
    }

    return false;
}

bool OldParser::handle_number(std::string::const_iterator &walk, const cmdp_token &token) {
    char *end = nullptr;
    errno = 0;
    long int num = std::strtol(std::to_address(walk), &end, 10);
    if ((errno == ERANGE && (num == LONG_MIN || num == LONG_MAX)) ||
        (errno != 0 && num == 0)) {
        return false;
    }

    /* No valid numbers found */
    if (num == 0) {
        return false;
    }

    if (token.identifier) {
        push_long(ctx.stack, token.identifier->c_str(), num);
    }

    /* Set walk to the first non-number character */
    int dist = end - std::to_address(walk);
    std::advance(walk, dist);
    next_state(token, ctx, *this);
    return true;
}

bool OldParser::handle_word(std::string::const_iterator &walk, const cmdp_token &token) {
    std::string::const_iterator beginning = walk;
    /* Handle quoted strings (or words). */
    if (*walk == '"') {
        beginning++;
        walk++;
        while (*walk != '\0' && (*walk != '"' || *(walk - 1) == '\\')) {
            walk++;
        }
    } else {
        if (token.name[0] == 's') {
            while (*walk != '\0' && *walk != '\r' && *walk != '\n') {
                walk++;
            }
        } else {
            /* For a word, the delimiters are white space (' ' or
                         * '\t'), closing square bracket (]), comma (,) and
                         * semicolon (;). */
            while (*walk != ' ' && *walk != '\t' &&
                   *walk != ']' && *walk != ',' &&
                   *walk != ';' && *walk != '\r' &&
                   *walk != '\n' && *walk != '\0') {
                walk++;
            }
        }
    }
    if (walk != beginning) {
        std::string str{};
        str.reserve(std::distance(beginning, walk)  + 1);
        /* We copy manually to handle escaping of characters. */
        int inpos;
        for (inpos = 0;
             inpos < std::distance(beginning, walk);
             inpos++) {
            /* We only handle escaped double quotes to not break
                         * backwards compatibility with people using \w in
                         * regular expressions etc. */
            if (beginning[inpos] == '\\' && beginning[inpos + 1] == '"') {
                inpos++;
            }
            str += beginning[inpos];
        }
        if (token.identifier) {
            push_string_append(ctx.stack, token.identifier->c_str(), str.c_str());
        }
        /* If we are at the end of a quoted string, skip the ending
                     * double quote. */
        if (*walk == '"') {
            walk++;
        }
        next_state(token, ctx, *this);
        return true;
    }

    return false;
}

bool OldParser::handle_line(std::string::const_iterator &walk, const cmdp_token &token) {
    while (*walk != '\0' && *walk != '\n' && *walk != '\r') {
        walk++;
    }
    next_state(token, ctx, *this);
    walk++;
    return true;
}

bool OldParser::handle_end(std::string::const_iterator &walk, const cmdp_token &token, ConfigResultIR &subcommand_output, int *linecnt) {
    if (*walk == '\0' || *walk == '\n' || *walk == '\r') {
        next_state(token, ctx, *this);
        /* To make sure we start with an appropriate matching
                     * datastructure for commands which do *not* specify any
                     * criteria, we re-initialize the criteria system after
                     * every command. */
        cfg::criteria_init(ctx.criteria_state, subcommand_output, std::to_underlying(cmdp_state::INITIAL));
        (*linecnt)++;
        walk++;

        return true;
    }

    return false;
}

static void reset_statelist(parser_ctx &ctx) {
    ctx.state = std::to_underlying(cmdp_state::INITIAL);
    for (int & i : ctx.statelist) {
        i = std::to_underlying(cmdp_state::INITIAL);
    }
    ctx.statelist_idx = 1;
}

bool OldParser::parse_config(const std::string &input, std::ostream &err_output) {
    bool has_errors = false;
    std::string::const_iterator walk = input.begin();
    int linecnt = 1;

    log_config(input);

    reset_statelist(ctx);

    ConfigResultIR subcommand_output = {
        .parser = dynamic_cast<BaseParser&>(*this),
        .next_state = 0,
        .has_errors = false
    };

    cfg::criteria_init(ctx.criteria_state, subcommand_output, std::to_underlying(cmdp_state::INITIAL));

    /* The "<=" operator is intentional: We also handle the terminating 0-byte
     * explicitly by looking for an 'end' token. */
    while (walk <= input.cend()) {
        /* Skip whitespace before every token, newlines are relevant since they
         * separate configuration directives. */
        while (walk < input.cend() && (*walk == ' ' || *walk == '\t')) {
            walk++;
        }

        auto &ptr = tokens.at(static_cast<cmdp_state>(ctx.state));
        bool token_handled = false;
        for (unsigned long c = 0; c < ptr.size() && !token_handled; c++) {
            auto &token = ptr.at(c);

            /* A literal. */
            if (token.name[0] == '\'') {
                token_handled = handle_literal(walk, token);
            } else if (token.name == "number") {
                /* Handle numbers. We only accept decimal numbers for now. */
                token_handled = handle_number(walk, token);
            } else if (token.name == "string" || token.name == "word") {
                token_handled = handle_word(walk, token);
            } else if (token.name == "line") {
                token_handled = handle_line(walk, token);
                linecnt++;
            } else if (token.name == "end") {
               token_handled = handle_end(walk, token, subcommand_output, &linecnt);
            }
        }

        if (!token_handled) {
            unhandled_token(input, linecnt, ptr, has_errors, walk, err_output);
        }
    }

    return has_errors;
}

/*
 * Inserts or updates a variable assignment depending on whether it already exists.
 *
 */
static void upsert_variable(std::vector<std::shared_ptr<Variable>> &variables, std::string_view const key, std::string_view const value) {
    for (auto &current : variables) {
        if (strcmp(current->key.c_str(), key.data()) != 0) {
            continue;
        }

        DLOG(fmt::sprintf("Updated variable: %s = %s -> %s\n",  key, current->value, value.data()));
        current->value = value.data();
        return;
    }

    DLOG(fmt::sprintf("Defined new variable: %s = %s\n",  key, value));
    auto n = std::make_shared<Variable>();
    auto loc = variables.begin();
    n->key = key.data();
    n->value = value.data();
    /* ensure that the correct variable is matched in case of one being
     * the prefix of another */
    while (loc != variables.end()) {
        if (n->key.length() >= (*loc)->key.length()) {
            break;
        }
        ++loc;
    }

    if (loc == variables.end()) {
        variables.insert(variables.begin(), n);
    } else {
        variables.insert(loc, n);
    }
}

static std::string read_file(std::istream &fstr, BaseResourceDatabase &resourceDatabase, parser_ctx &ctx) {
    char key[512], value[4096];
    std::string buf{};

    for (std::string buffer; std::getline(fstr, buffer);) {

        /* sscanf implicitly strips whitespace. */
        value[0] = '\0';
        const bool skip_line = (sscanf(buffer.c_str(), "%511s %4095[^\n]", key, value) < 1 || strlen(key) < 3);
        const bool comment = (key[0] == '#');
        value[4095] = '\n';

        bool continuation = strstr(buffer.c_str(), "\\\n");
        if (continuation) {
            if (!comment) {
                continue;
            }
            DLOG(fmt::sprintf("line continuation in comment is ignored: \"%s\"\n", buffer));
            continuation = false;
        }

        buf.append(buffer);
        buf.append("\n");

        /* Skip comments and empty lines. */
        if (skip_line || comment) {
            continue;
        }

        if (strcasecmp(key, "set") == 0 && *value != '\0') {
            char v_key[512];
            char v_value[4096] = {'\0'};

            if (sscanf(value, "%511s %4095[^\n]", v_key, v_value) < 1) {
                throw std::domain_error(fmt::sprintf("Failed to parse variable specification '%s', skipping it.\n",  value));
            }

            if (v_key[0] != '$') {
                throw std::domain_error("Malformed variable assignment, name has to start with $\n");
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
                throw std::domain_error(fmt::sprintf("Failed to parse resource specification '%s', skipping it.\n",  value));
            }

            if (v_key[0] != '$') {
                throw std::domain_error("Malformed variable assignment, name has to start with $\n");
            }
            std::string res_value = resourceDatabase.get_resource(res_name, fallback);

            upsert_variable(ctx.variables, v_key, res_value);
            continue;
        }
    }
    return buf;
}

static std::string::const_iterator caseInsensitiveSearch(const std::string& str, std::string::const_iterator &pos, const std::string& substr) {
    return std::search(pos, str.cend(), substr.cbegin(), substr.cend(), [](char a, char b) {
      return std::tolower(a) == std::tolower(b);
    });
}

/* Allocate a new buffer and copy the file over to the new one,
 * and replace occurrences of our variables */
static std::string replace_variables(std::string &buf, parser_ctx &ctx) {
    std::string::const_iterator walk = buf.cbegin();
    std::string destwalk{};
    while (walk != buf.cend()) {
        std::map<std::string, std::string::const_iterator> next_match{};
        Variable *nearest = nullptr;
        /* Find the next variable */
        for (auto &variable : ctx.variables) {
            auto pos = caseInsensitiveSearch(buf, walk, variable->key);
            if (pos != buf.cend()) {
                next_match[variable->key] = pos;
            }
        }
        std::string::const_iterator distance = buf.cend();
        for (auto &variable : ctx.variables) {
            if (!next_match.contains(variable->key)) {
                continue;
            }
            if (next_match[variable->key] < distance) {
                distance = next_match[variable->key];
                nearest = variable.get();
            }
        }
        if (nearest == nullptr) {
            /* If there are no more variables, we just copy the rest */
            destwalk.append(walk, buf.cend());
            break;
        } else {
            /* Copy until the next variable, then copy its value */
            destwalk.append(walk, distance);
            destwalk.append(nearest->value);
            walk = distance;
            std::advance(walk, nearest->key.length());
        }
    }

    return destwalk;
}

OldParser::OldParser(const std::filesystem::path filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, parser_ctx &parent_ctx, BaseConfigApplier &applier) : OldParser(filename, stream, resourceDatabase, applier) {
    this->parent_ctx = &parent_ctx;
    this->ctx.variables = parent_ctx.variables;
}

OldParser::OldParser(const std::filesystem::path filename, std::istream &stream, BaseResourceDatabase &resourceDatabase, BaseConfigApplier &applier) : BaseParser(applier, resourceDatabase), filename(filename), stream(stream), ctx(this) {
    this->old_dir = getcwd(nullptr, 0);

    std::filesystem::path f(filename);
    f.remove_filename();

    LOG(fmt::sprintf("Changing working directory to config file directory %s\n", f.c_str()));
    if (chdir(f.c_str()) == -1) {
        throw std::runtime_error(fmt::sprintf("chdir(%s) failed: %s\n", f.c_str(), strerror(errno)));
    }

}

OldParser::~OldParser() {
    chdir(this->old_dir.c_str());
}

/*
 * Parses the given file by first replacing the variables, then calling
 * parse_config and possibly launching i3-nagbar.
 *
 */
void OldParser::parse_file() {
    IncludedFile included_file{filename};

    while (!stream.eof()) {
        std::string line;
        std::getline(stream, line);
        included_file.raw_contents.append(line);
        included_file.raw_contents.append("\n");
    }

    stream.clear();
    stream.seekg(0);

    std::string buf = read_file(stream, resourceDatabase, ctx);

    /* Then, allocate a new buffer and copy the file over to the new one,
     * but replace occurrences of our variables */
    std::string n = replace_variables(buf, ctx);

    included_file.variable_replaced_contents = n;

    included_files.push_back(std::move(included_file));

    std::stringbuf err_output_buf;
    std::ostream err_output{&err_output_buf};

    bool has_errors = parse_config(n, err_output);
    if (ctx.has_errors) {
        has_errors = true;
    }

    if (has_errors) {
        throw std::domain_error(err_output_buf.str());
    }
}
