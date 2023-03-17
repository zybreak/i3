#include <stdio.h>
#include <string>
#include "config_parser.h"
#include "config_adapter.h"

bool parse_config(struct parser_ctx &ctx, const std::string &input, const char *filename);

/*******************************************************************************
 * Code for building the stand-alone binary test.commands_parser which is used
 * by t/187-commands-parser.t.
 ******************************************************************************/
int main(int argc, char *argv[]) {
    std::stringbuf output;
    ConfigApplierAdapter applier{&output};

    ResourceDatabase resourceDatabase{nullptr};

    NewParser parser{resourceDatabase, &std::cin, config_load_t::C_LOAD, applier};
    parser.parse_file();

    std::cout << output.str() << std::endl;
}