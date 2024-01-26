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
 * When the user moves the mouse pointer onto a window, this callback gets called.
 *
 */
void PropertyHandlers::handle_enter_notify(xcb_enter_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("enter_notify for %08x, mode = %d, detail %d, serial %d\n",
                      event->event, event->mode, event->detail, event->sequence));
    DLOG(fmt::sprintf("coordinates %d, %d\n", event->event_x, event->event_y));
    if (event->mode != XCB_NOTIFY_MODE_NORMAL) {
        DLOG("This was not a normal notify, ignoring\n");
        return;
    }
    /* Some events are not interesting, because they were not generated
     * actively by the user, but by reconfiguration of windows */
    if (event_is_ignored(event->sequence, XCB_ENTER_NOTIFY)) {
        DLOG("Event ignored\n");
        return;
    }

    bool enter_child = false;
    /* Get container by frame or by child window */
    if ((con = con_by_frame_id(event->event)) == nullptr) {
        con = con_by_window_id(event->event);
        enter_child = true;
    }

    /* If we cannot find the container, the user moved their cursor to the root
     * window. In this case and if they used it to a dock, we need to focus the
     * workspace on the correct output. */
    if (con == nullptr || con->parent->type == CT_DOCKAREA) {
        DLOG(fmt::sprintf("Getting screen at %d x %d\n", event->root_x, event->root_y));
        check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    /* see if the user entered the window on a certain window decoration */
    layout_t layout = (enter_child ? con->parent->layout : con->layout);
    if (layout == L_DEFAULT) {
        for (auto &child : con->nodes_head | std::views::reverse) {
            if (child->deco_rect.rect_contains(event->event_x, event->event_y)) {
                LOG(fmt::sprintf("using child %p / %s instead!\n", (void *)child, child->name));
                con = child;
                break;
            }
        }
    }

    if (config.disable_focus_follows_mouse)
        return;

    /* if this container is already focused, there is nothing to do. */
    if (con == global.focused)
        return;

    /* Get the currently focused workspace to check if the focus change also
     * involves changing workspaces. If so, we need to call workspace_show() to
     * correctly update state and send the IPC event. */
    Con *ws = con->con_get_workspace();
    if (ws != global.focused->con_get_workspace())
        workspace_show(ws);

    x->focused_id = XCB_NONE;
    con_descend_focused(con)->con_focus();
    tree_render();
}
