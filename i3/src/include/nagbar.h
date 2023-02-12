#pragma once

#include "unistd.h"
#include "sys/types.h"

enum bar_type_t {
    TYPE_ERROR = 0,
    TYPE_WARNING = 1
};

struct button_t {
    i3String *label;
    char *action;
    int16_t x;
    uint16_t width;
    bool terminal;
};

/**
 * Starts an i3-nagbar instance with the given parameters. Takes care of
 * handling SIGCHLD and killing i3-nagbar when i3 exits.
 *
 * The resulting PID will be stored in *nagbar_pid and can be used with
 * kill_nagbar() to kill the bar later on.
 *
 */
void start_nagbar(pid_t *nagbar_pid,
                  std::vector<button_t> &buttons,
                  const char *prompt,
                  const char *pattern = "pango:monospace 8",
                  bar_type_t bar_type = TYPE_ERROR,
                  bool position_on_primary = false);

/**
 * Kills the i3-nagbar process, if nagbar_pid != -1.
 *
 * If wait_for_it is set (restarting i3), this function will waitpid(),
 * otherwise, ev is assumed to handle it (reloading).
 *
 */
void kill_nagbar(pid_t nagbar_pid, bool wait_for_it);
