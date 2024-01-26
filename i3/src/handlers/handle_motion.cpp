/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * handlers.c: Small handlers for various events (keypresses, focus changes,
 *             …).
 *
 */
module;
#include <cassert>
#include <climits>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

#include "i3_ipc/i3-ipc.h"
#include "i3.h"
#include "atoms.h"

#include <ctime>

#include <xcb/randr.h>
#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-monitor.h>
#include <memory>
#include <mutex>
#include <algorithm>
#include <ranges>
#include <span>
#include <config.h>
#include <fmt/printf.h>
module i3;

import log;

#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2

#define FREE(pointer)   \
    do {                \
        free(pointer);  \
        pointer = NULL; \
    } while (0)

import :output;
import utils;

/*
 * When the user moves the mouse but does not change the active window
 * (e.g. when having no windows opened but moving mouse on the root screen
 * and crossing virtual screen boundaries), this callback gets called.
 *
 */
void PropertyHandlers::handle_motion_notify(xcb_motion_notify_event_t *event) {
    global.last_timestamp = event->time;

    /* Skip events where the pointer was over a child window, we are only
     * interested in events on the root window. */
    if (event->child != XCB_NONE)
        return;

    Con *con;
    if ((con = con_by_frame_id(event->event)) == nullptr) {
        DLOG("MotionNotify for an unknown container, checking if it crosses screen boundaries.\n");
        this->check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    if (config.disable_focus_follows_mouse)
        return;

    if (con->layout != L_DEFAULT && con->layout != L_SPLITV && con->layout != L_SPLITH)
        return;

    /* see over which rect the user is */
    for (auto &current : con->nodes_head | std::views::reverse) {
        if (!current->deco_rect.rect_contains(event->event_x, event->event_y))
            continue;

        /* We found the rect, let’s see if this window is focused */
        if (con::first(con->focus_head) == current)
            return;

        current->con_focus();
        x_push_changes(global.croot);
        return;
    }
}
