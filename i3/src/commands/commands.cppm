module;
#include <nlohmann/json.hpp>
export module i3:commands;

import std;
import i3ipc;
import :ipc;
import i3_commands_base;

export {
    /**
     * Parses and executes the given command. If a caller-allocated yajl_gen is
     * passed, a json reply will be generated in the format specified by the ipc
     * protocol. Pass NULL if no json reply is required.
     *
     * Free the returned CommandResult with command_result_free().
     */
    CommandResult parse_command(std::string const &input, command_parser_data &&data, BaseCommandsApplier *applier);
}