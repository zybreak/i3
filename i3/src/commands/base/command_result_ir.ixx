module;
class BaseCommandsApplier;
class ipc_client;
#include <nlohmann/json.hpp>
export module i3_commands_base:command_result_ir;

/**
 * Holds an intermediate representation of the result of a call to any command.
 * When calling parse_command("floating enable, border none"), the parser will
 * internally use this struct when calling cmd_floating and cmd_border.
 */
export class CommandResultIR {
   public:
    BaseCommandsApplier *applier;

    /* The JSON generator to append a reply to (may be NULL). */
    nlohmann::json *json_gen;

    /* The IPC client connection which sent this command (may be NULL, e.g. for
       key bindings). */
    ipc_client *client;

    /* The next state to transition to. Passed to the function so that we can
     * determine the next state as a result of a function call, like
     * cfg_criteria_pop_state() does. */
    int next_state;

    /* Whether the command requires calling tree_render. */
    bool needs_tree_render;
};
