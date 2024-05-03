module;
#include <nlohmann/json.hpp>
export module i3_commands_old:commands_parser;

import i3ipc;
import i3_commands_base;

export namespace i3_commands_old {
    CommandResult parse_command(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier *applier);
};
