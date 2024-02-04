#include "config_parser.h"
#include "config_adapter.h"
#include "test_resource_database.h"
#include <err.h>

import i3;

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename);

/*******************************************************************************
 * Code for building the stand-alone binary test.commands_parser which is used
 * by t/187-commands-parser.t.
 ******************************************************************************/
int main(int argc, char *argv[]) {

    std::string content{};
    char c;

    while (std::cin.get(c)) {
        content += c;
    }

    ConfigApplierAdapter applier{std::cout};

    TestResourceDatabase resourceDatabase{};

    parser_ctx ctx{applier, resourceDatabase, config_load_t::C_LOAD};

    parse_config(ctx, content, "<stdin>");
}