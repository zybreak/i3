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
    ConfigApplierAdapter applier{};

    NewParser parser{"<stdin>", config_load_t::C_LOAD, applier};
    parser.parse_file();
}