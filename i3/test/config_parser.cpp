#include "config_parser.h"
#include "parser_stack.h"
#include <stdio.h>
#include <configuration.h>

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename);

/*******************************************************************************
 * Code for building the stand-alone binary test.commands_parser which is used
 * by t/187-commands-parser.t.
 ******************************************************************************/
int main(int argc, char *argv[]) {
    struct parser_ctx ctx = {
        .load_type = config_load_t::C_LOAD,
    };

    std::string content{};
    char c;

    while (std::cin.get(c)) {
        content += c;
    }

    parse_config(ctx, content, "<stdin>");
}