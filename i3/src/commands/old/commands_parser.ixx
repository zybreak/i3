module;
#include "../command_result.h"
#include <nlohmann/json.hpp>
#include "../base_commands_applier.h"
export module i3_commands_old;

import i3ipc;

export CommandResult parse_command_old(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier &applier);
