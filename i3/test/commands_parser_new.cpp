#include "commands_parser.h"
#include "parser_stack.h"
#include <stdio.h>

CommandResult parse_command_new(const std::string &input, nlohmann::json *gen, ipc_client *client);

/*******************************************************************************
 * Code for building the stand-alone binary test.commands_parser which is used
 * by t/187-commands-parser.t.
 ******************************************************************************/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Syntax: %s <command>\n", argv[0]);
        return 1;
    }

    std::string cmd{};

    for (int i = 1; i < argc; i++) {
        cmd.append(" ").append(argv[i]);
    }

    try {
        CommandResult result = parse_command_new(cmd, nullptr, nullptr);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}