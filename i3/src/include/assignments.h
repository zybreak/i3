/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * assignments.c: Assignments for specific windows (for_window).
 *
 */
#pragma once

#include <config.h>
#include <memory>

#include "window.h"

class i3Window;
class Match;

enum assignment_type_t : unsigned int {
    A_ANY = 0,
    A_COMMAND = (1 << 0),
    A_TO_WORKSPACE = (1 << 1),
    A_NO_FOCUS = (1 << 2),
    A_TO_WORKSPACE_NUMBER = (1 << 3),
    A_TO_OUTPUT = (1 << 4)
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
    /** type of this assignment:
     *
     * A_COMMAND = run the specified command for the matching window
     * A_TO_WORKSPACE = assign the matching window to the specified workspace
     * A_NO_FOCUS = don't focus matched window when it is managed
     *
     * While the type is a bitmask, only one value can be set at a time. It is
     * a bitmask to allow filtering for multiple types, for example in the
     * assignment_for() function.
     *
     */
    assignment_type_t type;

    /** the criteria to check if a window matches */
    Match *match;

    virtual ~Assignment();
};

class CommandAssignment : public Assignment {
public:
    std::string command;
};

class WorkspaceAssignment : public Assignment {
public:
    /** destination workspace/command/output, depending on the type */
    std::string workspace;
};

class OutputAssignment : public Assignment {
public:
    std::string output;
};

/**
 * Checks the list of assignments for the given window and runs all matching
 * ones (unless they have already been run for this specific window).
 *
 */
void run_assignments(i3Window *window);

/**
 * Returns the first matching assignment for the given window.
 *
 */
std::optional<std::reference_wrapper<Assignment>> assignment_for(i3Window *window, assignment_type_t type);
