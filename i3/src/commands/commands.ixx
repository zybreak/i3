module;
#include <string>
#include <nlohmann/json.hpp>
#include "base_commands_applier.h"
export module i3:commands;

import i3ipc;
import :ipc;

export {
    /**
     * A struct that contains useful information about the result of a command as a
     * whole (e.g. a compound command like "floating enable, border none").
     * needs_tree_render is true if needs_tree_render of any individual command was
     * true.
     */
    struct CommandResult {
        bool parse_error;
        /* the error_message is currently only set for parse errors */
        std::string error_message;
        bool needs_tree_render;
    };

    /**
     * Parses and executes the given command. If a caller-allocated yajl_gen is
     * passed, a json reply will be generated in the format specified by the ipc
     * protocol. Pass NULL if no json reply is required.
     *
     * Free the returned CommandResult with command_result_free().
     */
    CommandResult parse_command(const std::string &input, nlohmann::json *gen, ipc_client *client, BaseCommandsApplier *applier);
}