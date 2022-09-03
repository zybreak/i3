#pragma once

#include <sys/types.h>
#include <sys/resource.h>
#include <queue>
#include <memory>
#include "main.h"
#include "assignments.h"
#include "workspace.h"

#include <xpp/xpp.hpp>
#include <xpp/proto/randr.hpp>
#define explicit dont_use_cxx_explicit
#include <xpp/proto/xkb.hpp>
#undef explicit
#include <xpp/proto/shape.hpp>
#include <xpp/proto/bigreq.hpp>

using x_connection = xpp::connection<xpp::xkb::extension, xpp::shape::extension, xpp::big_requests::extension, xpp::randr::extension>;

struct global {
    bool run_atexit{true};
    pid_t config_error_nagbar_pid = -1;
    pid_t command_error_nagbar_pid = -1;
    x_connection *a;

    /* The original value of RLIMIT_CORE when i3 was started. We need to restore
     * this before starting any other process, since we set RLIMIT_CORE to
     * RLIM_INFINITY for i3 debugging versions. */
    struct rlimit original_rlimit_core;

    /* The list of assignments */
    std::deque<std::unique_ptr<Assignment>> assignments{};

    /* The list of workspace assignments (which workspace should end up on which
     * output) */
    std::deque<std::unique_ptr<Workspace_Assignment>> ws_assignments{};

    /* The screen (0 when you are using DISPLAY=:0) of the connection 'conn' */
    int conn_screen;

    /* The last timestamp we got from X11 (timestamps are included in some events
     * and are used for some things, like determining a unique ID in startup
     * notification). */
    xcb_timestamp_t last_timestamp = XCB_CURRENT_TIME;

    xcb_screen_t *root_screen;
    xcb_connection_t *conn;

    bool new_parser;
};

extern struct global global;