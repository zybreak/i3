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
 * Handles ConfigureNotify events for the root window, which are generated when
 * the monitor configuration changed.
 *
 */
void PropertyHandlers::handle_configure_notify(xcb_configure_notify_event_t *event) {
    if (event->event != x->root) {
        DLOG(fmt::sprintf("ConfigureNotify for non-root window 0x%08x, ignoring\n", event->event));
        return;
    }
    DLOG(fmt::sprintf("ConfigureNotify for root window 0x%08x\n", event->event));

    global.randr->randr_query_outputs();
}

