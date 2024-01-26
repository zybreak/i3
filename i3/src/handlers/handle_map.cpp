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
 * A new window appeared on the screen (=was mapped), so let’s manage it.
 *
 */
void PropertyHandlers::handle_map_request(xcb_map_request_event_t *event) {
    auto attr = x->conn->get_window_attributes_unchecked(event->window);

    DLOG(fmt::sprintf("window = 0x%08x, serial is %d.\n", event->window, event->sequence));
    add_ignore_event(event->sequence, -1);

    manage_window(event->window, attr.get().get(), false);
}
