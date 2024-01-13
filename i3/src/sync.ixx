/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * sync.c: i3 sync protocol: https://i3wm.org/docs/testsuite.html#i3_sync
 *
 */
module;
#include <cstdint>
#include <cstdlib>

#include <xcb/xcb.h>

#include <fmt/printf.h>

#include "atoms.h"

#include <xcb/xcb.h>
export module i3:sync;

import utils;
import :log;
import :x;

export void sync_respond(X *x, xcb_window_t window, uint32_t rnd) {
    DLOG(fmt::sprintf("[i3 sync protocol] Sending random value %d back to X11 window 0x%08x\n",  rnd, window));

    xcb_client_message_event_t ev{};

    ev.response_type = XCB_CLIENT_MESSAGE;
    ev.window = window;
    ev.type = A_I3_SYNC;
    ev.format = 32;
    ev.data.data32[0] = window;
    ev.data.data32[1] = rnd;

    xcb_send_event(**x, false, window, XCB_EVENT_MASK_NO_EVENT, (char *)&ev);
    xcb_flush(**x);
}

