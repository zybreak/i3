module;
#include <nlohmann/json.hpp>
export module i3_commands_old:commands_parser;

import i3ipc;
import i3_commands_base;

export CommandResult parse_command_old(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier *applier);
