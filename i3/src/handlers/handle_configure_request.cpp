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
import :x;
import utils;

/*
 * Configure requests are received when the application wants to resize windows
 * on their own.
 *
 * We generate a synthethic configure notify event to signalize the client its
 * "new" position.
 *
 */
void PropertyHandlers::handle_configure_request(xcb_configure_request_event_t *event) {
    Con *con;

    DLOG(fmt::sprintf("window 0x%08x wants to be at %dx%d with %dx%d\n",
                      event->window, event->x, event->y, event->width, event->height));

    /* For unmanaged windows, we just execute the configure request. As soon as
     * it gets mapped, we will take over anyways. */
    if ((con = con_by_window_id(event->window)) == nullptr) {
        DLOG("Configure request for unmanaged window, can do that.\n");

        uint32_t mask = 0;
        uint32_t values[7];
        int c = 0;
#define COPY_MASK_MEMBER(mask_member, event_member) \
    do {                                            \
        if (event->value_mask & mask_member) {      \
            mask |= mask_member;                    \
            values[c++] = event->event_member;      \
        }                                           \
    } while (0)

        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_X, x);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_Y, y);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_WIDTH, width);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_HEIGHT, height);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_SIBLING, sibling);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_STACK_MODE, stack_mode);

        xcb_configure_window(**global.x, event->window, mask, values);
        xcb_flush(**global.x);

        return;
    }

    DLOG("Configure request!\n");

    Con *workspace = con->con_get_workspace();
    Con *fullscreen = workspace ? workspace->con_get_fullscreen_covering_ws() : nullptr;

    if (fullscreen != con && con->con_is_floating() && con->con_is_leaf()) {
        /* find the height for the decorations */
        int deco_height = con->deco_rect.height;
        /* we actually need to apply the size/position changes to the *parent*
         * container */
        Rect bsr = con_border_style_rect(con);
        if (con->border_style == BS_NORMAL) {
            bsr.y += deco_height;
            bsr.height -= deco_height;
        }
        Con *floatingcon = con->parent;
        Rect newrect = floatingcon->rect;

        if (event->value_mask & XCB_CONFIG_WINDOW_X) {
            newrect.x = event->x + (-1) * bsr.x;
            DLOG(fmt::sprintf("proposed x = %d, new x is %d\n", event->x, newrect.x));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_Y) {
            newrect.y = event->y + (-1) * bsr.y;
            DLOG(fmt::sprintf("proposed y = %d, new y is %d\n", event->y, newrect.y));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
            newrect.width = event->width + (-1) * bsr.width;
            newrect.width += con->border_width * 2;
            DLOG(fmt::sprintf("proposed width = %d, new width is %d (x11 border %d)\n",
                              event->width, newrect.width, con->border_width));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
            newrect.height = event->height + (-1) * bsr.height;
            newrect.height += con->border_width * 2;
            DLOG(fmt::sprintf("proposed height = %d, new height is %d (x11 border %d)\n",
                              event->height, newrect.height, con->border_width));
        }

        DLOG("Container is a floating leaf node, will do that.\n");
        floating_reposition(floatingcon, newrect);
        return;
    }

    /* Dock windows can be reconfigured in their height and moved to another output. */
    if (con->parent && con->parent->type == CT_DOCKAREA) {
        DLOG(fmt::sprintf("Reconfiguring dock window (con = %p).\n", (void *)con));
        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
            DLOG(fmt::sprintf("Dock client wants to change height to %d, we can do that.\n", event->height));

            con->geometry.height = event->height;
            tree_render();
        }

        if (event->value_mask & XCB_CONFIG_WINDOW_X || event->value_mask & XCB_CONFIG_WINDOW_Y) {
            int16_t x = event->value_mask & XCB_CONFIG_WINDOW_X ? event->x : (int16_t)con->geometry.x;
            int16_t y = event->value_mask & XCB_CONFIG_WINDOW_Y ? event->y : (int16_t)con->geometry.y;

            Con *current_output = con->con_get_output();
            Output *target = global.randr->get_output_containing(x, y);
            if (target != nullptr && current_output != target->con) {
                DLOG(fmt::sprintf("Dock client is requested to be moved to output %s, moving it there.\n", target->output_primary_name()));
                Match *match;
                Con *nc = con_for_window(target->con, con->window, &match);
                DLOG(fmt::sprintf("Dock client will be moved to container %p.\n", (void *)nc));
                con->con_detach();
                con->con_attach(nc, false);

                tree_render();
            } else {
                DLOG("Dock client will not be moved, we only support moving it to another output.\n");
            }
        }
        goto out;
    }

    if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        DLOG(fmt::sprintf("window 0x%08x wants to be stacked %d\n", event->window, event->stack_mode));

        /* Emacs and IntelliJ Idea “request focus” by stacking their window
         * above all others. */
        if (event->stack_mode != XCB_STACK_MODE_ABOVE) {
            DLOG("stack_mode != XCB_STACK_MODE_ABOVE, ignoring ConfigureRequest\n");
            goto out;
        }

        if (fullscreen || !con->con_is_leaf()) {
            DLOG("fullscreen or not a leaf, ignoring ConfigureRequest\n");
            goto out;
        }

        if (workspace == nullptr) {
            DLOG("Window is not being managed, ignoring ConfigureRequest\n");
            goto out;
        }

        if (config.focus_on_window_activation == FOWA_FOCUS || (config.focus_on_window_activation == FOWA_SMART && workspace_is_visible(workspace))) {
            DLOG(fmt::sprintf("Focusing con = %p\n", (void *)con));
            workspace_show(workspace);
            con->con_activate_unblock();
            tree_render();
        } else if (config.focus_on_window_activation == FOWA_URGENT || (config.focus_on_window_activation == FOWA_SMART && !workspace_is_visible(workspace))) {
            DLOG(fmt::sprintf("Marking con = %p urgent\n", (void *)con));
            con_set_urgency(con, true);
            tree_render();
        } else {
            DLOG(fmt::sprintf("Ignoring request for con = %p.\n", (void *)con));
        }
    }

out:
    fake_absolute_configure_notify(con);
}
