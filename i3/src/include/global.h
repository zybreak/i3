#pragma once

#include <sys/types.h>
#include <sys/resource.h>
#include <queue>
#include <memory>
#include "main.h"
#include "assignments.h"
#include "workspace.h"
#include "x.h"
#include "keysyms.h"

struct global {
    Keysyms *keysyms;

    char **start_argv;

    bool xkb_supported{true};
    bool shape_supported{true};
    bool run_atexit{true};
    pid_t config_error_nagbar_pid = -1;
    pid_t command_error_nagbar_pid = -1;
    X *x;
    RandR *randr;

    /* The original value of RLIMIT_CORE when i3 was started. We need to restore
     * this before starting any other process, since we set RLIMIT_CORE to
     * RLIM_INFINITY for i3 debugging versions. */
    struct rlimit original_rlimit_core;

    /* The list of assignments */
    std::deque<std::unique_ptr<Assignment>> assignments{};

    /* The list of workspace assignments (which workspace should end up on which
     * output) */
    std::deque<std::unique_ptr<Workspace_Assignment>> ws_assignments{};

    /* The last timestamp we got from X11 (timestamps are included in some events
     * and are used for some things, like determining a unique ID in startup
     * notification). */
    xcb_timestamp_t last_timestamp = XCB_CURRENT_TIME;

    bool new_parser;
};

extern struct global global;