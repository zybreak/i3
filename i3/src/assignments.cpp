/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * assignments.c: Assignments for specific windows (for_window).
 *
 */
module;
#include <fmt/core.h>
#include <optional>

#include "i3string.h"
#include "log.h"

#include "commands_parser.h"
#include "global.h"
#include "commands_applier.h"
module i3;

Assignment::~Assignment() {
    delete this->match;
}

/*
 * Checks the list of assignments for the given window and runs all matching
 * ones (unless they have already been run for this specific window).
 *
 */
void run_assignments(i3Window *window) {
    DLOG("Checking if any assignments match this window\n");

    bool needs_tree_render = false;

    /* Check if any assignments match */
    for (const auto &current : global.assignments) {
        if (current->type != A_COMMAND || (current->match == nullptr || !current->match->match_matches_window(window)))
            continue;


        bool skip = false;
        for (const auto &c : window->ran_assignments) {
            if (c != current.get())
                continue;

            DLOG("This assignment already ran for the given window, not executing it again.\n");
            skip = true;
            break;
        }

        if (skip)
            continue;

        /* Store that we ran this assignment to not execute it again. We have
         * to do this before running the actual command to prevent infinite
         * loops. */
        window->ran_assignments.push_back(current.get());

        auto command = dynamic_cast<CommandAssignment*>(current.get());

        DLOG(fmt::sprintf("matching assignment, execute command %s\n",  command->command));
        std::string full_command = fmt::format("[id=\"{}\"] {}", std::to_string(window->id), command->command);
        auto commandsApplier = CommandsApplier{};
        CommandResult result = parse_command(full_command, nullptr, nullptr, commandsApplier);

        if (result.needs_tree_render)
            needs_tree_render = true;
    }

    /* If any of the commands required re-rendering, we will do that now. */
    if (needs_tree_render)
        tree_render();
}

/*
 * Returns the first matching assignment for the given window.
 *
 */
std::optional<std::reference_wrapper<Assignment>> assignment_for(i3Window *window, assignment_type_t type) {
    for (const auto &assignment : global.assignments) {
        if ((type != A_ANY && (assignment->type & type) == 0) ||
                (assignment->match == nullptr || !assignment->match->match_matches_window(window)))
            continue;
        DLOG("got a matching assignment\n");
        return *assignment;
    }

    return std::nullopt;
}
