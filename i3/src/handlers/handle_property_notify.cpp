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

void PropertyHandlers::property_notify(xcb_property_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    uint8_t state = event->state;
    xcb_window_t window = event->window;
    xcb_atom_t atom = event->atom;

    auto it = std::ranges::find_if(property_handlers, [&atom](auto &property_handler) { return property_handler.atom == atom; });

    if (it == property_handlers.end()) {
        // LOG(fmt::sprintf("Unhandled property notify for atom %d (0x%08x)\n",  atom, atom));
        return;
    }

    auto &handler = *it;

    if ((con = con_by_window_id(window)) == nullptr || con->window == nullptr) {
        DLOG(fmt::sprintf("Received property for atom %d for unknown client\n", atom));
        return;
    }

    if (state != XCB_PROPERTY_DELETE) {
        try {
            auto propr = x->conn->get_property(0, window, atom, XCB_GET_PROPERTY_TYPE_ANY, 0, handler.long_len);

            handler.cb(con, propr.get().get());
        } catch (std::exception &e) {
            DLOG(fmt::sprintf("got error %s when getting property of atom %d\n", e.what(), atom));
        }
    } else {
        handler.cb(con, nullptr);
    }
}

