module;
#include <nlohmann/json.hpp>
export module i3_commands_base;

export import :base_commands_applier;
export import :command_result_ir;
export import :command_result;

class ipc_client;

export struct command_parser_data {
    nlohmann::json *gen{};
    ipc_client *client{};
};
