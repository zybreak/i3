/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */
module;
#include <string>
export module i3_commands_base:command_result;

/**
 * A struct that contains useful information about the result of a command as a
 * whole (e.g. a compound command like "floating enable, border none").
 * needs_tree_render is true if needs_tree_render of any individual command was
 * true.
 */
export class CommandResult {
   public:
    bool parse_error;
    /* the error_message is currently only set for parse errors */
    std::string error_message;
    bool needs_tree_render;
};
