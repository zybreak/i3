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
import :output;
import utils;

/*
 * Gets triggered upon a RandR screen change event, that is when the user
 * changes the screen configuration in any way (mode, position, …)
 *
 */
void PropertyHandlers::handle_screen_change(xcb_generic_event_t *e) {
    DLOG("RandR screen change\n");

    /* The geometry of the root window is used for “fullscreen global” and
     * changes when new outputs are added. */
    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(**global.x, global.x->root);
    xcb_get_geometry_reply_t *reply = xcb_get_geometry_reply(**global.x, cookie, nullptr);
    if (reply == nullptr) {
        ELOG("Could not get geometry of the root window, exiting\n");
        exit(EXIT_FAILURE);
    }
    DLOG(fmt::sprintf("root geometry reply: (%d, %d) %d x %d\n", reply->x, reply->y, reply->width, reply->height));

    global.croot->rect.width = reply->width;
    global.croot->rect.height = reply->height;

    global.randr->randr_query_outputs();

    ipc_send_event("output", I3_IPC_EVENT_OUTPUT, R"({"change":"unspecified"})");
}
