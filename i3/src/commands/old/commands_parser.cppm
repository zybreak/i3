module;
#include <nlohmann/json.hpp>
export module i3_commands_old:commands_parser;

import i3_commands_base;

class ipc_client;

export namespace i3_commands_old {
    CommandResult parse_command(std::string const &input, command_parser_data &&data, BaseCommandsApplier *applier);
};
