module;
#include "../command_result.h"
#include <nlohmann/json.hpp>
#include "../base_commands_applier.h"
export module i3_commands_old;

struct ipc_client;

export namespace i3_commands_old {
    CommandResult parse_command(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier &applier);
}
