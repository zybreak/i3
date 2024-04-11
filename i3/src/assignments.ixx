module;
#include <string>
#include <optional>
#include <ranges>
#include <deque>
#include <memory>
export module i3:assignments;

import :match;
import log;

export {
    enum class workspace_assignment_type {
        WORKSPACE,
        WORKSPACE_NUMBER,
    };

    /**
     * An Assignment makes specific windows go to a specific workspace/output or
     * run a command for that window. With this mechanism, the user can -- for
     * example -- assign their browser to workspace "www". Checking if a window is
     * assigned works by comparing the Match data structure with the window (see
     * match_matches_window()).
     *
     */
    class Assignment {
       public:
        /** the criteria to check if a window matches */
        Match match;

        Assignment(Match &&match) : match(match) {}

        virtual ~Assignment() {

        }
    };

    /**
     * A_COMMAND = run the specified command for the matching window
     */
    class CommandAssignment : public Assignment {
       public:
        std::string command;
        CommandAssignment(Match &&match) : Assignment(std::move(match)) {}
    };

    /**
     * A_TO_WORKSPACE = assign the matching window to the specified workspace
     */
    class WorkspaceAssignment : public Assignment {
       public:
        workspace_assignment_type type;
        /** destination workspace/command/output, depending on the type */
        std::string workspace;
        WorkspaceAssignment(Match &&match) : Assignment(std::move(match)) {}
    };

    class OutputAssignment : public Assignment {
       public:
        std::string output;
        OutputAssignment(Match &&match) : Assignment(std::move(match)) {}
    };

    /**
     * A_NO_FOCUS = don't focus matched window when it is managed
     */
    class NoFocusAssignment : public Assignment {
       public:
        NoFocusAssignment(Match &&match) : Assignment(std::move(match)) {}
    };

    /**
     * Checks the list of assignments for the given window and runs all matching
     * ones (unless they have already been run for this specific window).
     *
     */
    void run_assignments(i3Window * window);

    /**
     * Returns the first matching assignment for the given window.
     *
     */
    template<typename T>
    std::optional<std::reference_wrapper<T>> assignment_for(const std::deque<std::unique_ptr<Assignment>> &assignments, const i3Window *window) {
        auto assignment = std::ranges::find_if(assignments, [&](auto &a) {
            return ((dynamic_cast<T*>(a.get())) && (a->match.match_matches_window(window)));
        });

        if (assignment != assignments.end()) {
            DLOG("got a matching assignment\n");
            return std::reference_wrapper<T>(*static_cast<T*>(assignment->get()));
        }
        return std::nullopt;
    }
}