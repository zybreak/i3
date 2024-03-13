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
#include <algorithm>
#include <ranges>
#include <span>
#include <config.h>
#include <fmt/printf.h>
#include <mutex>
#include <map>
module i3;

import log;
import rect;
import :output;
import utils;

static bool window_name_changed(const i3Window *window, std::string &old_name) {
    if ((old_name.empty()) && (window->name.empty())) {
        return false;
    }

    /* Either the old or the new one is NULL, but not both. */
    if ((old_name.empty()) ^ (window->name.empty())) {
        return true;
    }

    return old_name != window->name;
}

/*
 * Called when a window changes its title
 *
 */
static bool handle_windowname_change(Con *con, xcb_get_property_reply_t *prop) {
    std::string old_name = con->window->name;

    con->window->window_update_name(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name)) {
        ipc_send_window_event("title", con);
    }

    return true;
}

/*
 * Handles the WM_HINTS property for extracting the urgency state of the window.
 *
 */
static bool handle_hints(Con *con, xcb_get_property_reply_t *reply) {
    bool urgency_hint;
    con->window->window_update_hints(reply, &urgency_hint);
    con_set_urgency(con, urgency_hint);
    remanage_window(con);
    tree_render();
    return true;
}

/*
 * Handles legacy window name updates (WM_NAME), see also src/window.c,
 * window_update_name_legacy().
 *
 */
static bool handle_windowname_change_legacy(Con *con, xcb_get_property_reply_t *prop) {
    std::string old_name = con->window->name;

    con->window->window_update_name_legacy(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name)) {
        ipc_send_window_event("title", con);
    }

    return true;
}

/*
 * Handles the size hints set by a window, but currently only the part necessary for displaying
 * clients proportionally inside their frames (mplayer for example)
 *
 * See ICCCM 4.1.2.3 for more details
 *
 */
static bool handle_normal_hints(Con *con, xcb_get_property_reply_t *reply) {
    bool changed = con->window->window_update_normal_hints(reply, nullptr);

    if (changed) {
        Con *floating = con->con_inside_floating();
        if (floating) {
            floating_check_size(con, false);
            tree_render();
        }
    }

    return true;
}

/*
 * Handles changes of the WM_CLIENT_LEADER atom which specifies if this is a
 * toolwindow (or similar) and to which window it belongs (logical parent).
 *
 */
static bool handle_clientleader_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_leader(prop);
    return true;
}

/*
 * Handles the transient for hints set by a window, signalizing that this window is a popup window
 * for some other window.
 *
 * See ICCCM 4.1.2.6 for more details
 *
 */
static bool handle_transient_for(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_transient_for(prop);
    return true;
}

/*
 * Called when a window changes its WM_WINDOW_ROLE.
 *
 */
static bool handle_windowrole_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_role(prop);

    con = remanage_window(con);

    return true;
}

/*
 * Handles the WM_CLASS property for assignments and criteria selection.
 *
 */
static bool handle_class_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_class(prop);
    con = remanage_window(con);
    return true;
}

/*
 * Handles the _NET_WM_STRUT_PARTIAL property for allocating space for dock clients.
 *
 */
static bool handle_strut_partial_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_strut_partial(prop);

    /* we only handle this change for dock clients */
    if (con->parent == nullptr || con->parent->type != CT_DOCKAREA) {
        return true;
    }

    Con *search_at = global.croot;
    Con *output = con->con_get_output();
    if (output != nullptr) {
        DLOG(fmt::sprintf("Starting search at output %s\n", output->name));
        search_at = output;
    }

    /* find out the desired position of this dock window */
    if (con->window->reserved.top > 0 && con->window->reserved.bottom == 0) {
        DLOG("Top dock client\n");
        con->window->dock = i3Window::W_DOCK_TOP;
    } else if (con->window->reserved.top == 0 && con->window->reserved.bottom > 0) {
        DLOG("Bottom dock client\n");
        con->window->dock = i3Window::W_DOCK_BOTTOM;
    } else {
        DLOG("Ignoring invalid reserved edges (_NET_WM_STRUT_PARTIAL), using position as fallback:\n");
        if (con->geometry.y < (search_at->rect.height / 2)) {
            DLOG(fmt::sprintf("geom->y = %d < rect.height / 2 = %d, it is a top dock client\n",
                              con->geometry.y, (search_at->rect.height / 2)));
            con->window->dock = i3Window::W_DOCK_TOP;
        } else {
            DLOG(fmt::sprintf("geom->y = %d >= rect.height / 2 = %d, it is a bottom dock client\n",
                              con->geometry.y, (search_at->rect.height / 2)));
            con->window->dock = i3Window::W_DOCK_BOTTOM;
        }
    }

    /* find the dockarea */
    Con *dockarea = con_for_window(search_at, con->window, nullptr);
    assert(dockarea != nullptr);

    /* attach the dock to the dock area */
    con->con_detach();
    con->con_attach(dockarea, true);

    tree_render();

    return true;
}

static bool handle_window_type(Con *con, xcb_get_property_reply_t *reply) {
    if (con->window->window_update_type(reply)) {
        run_assignments(con->window);
    }
    return true;
}

/*
 * Handles the _I3_FLOATING_WINDOW property to properly run assignments for
 * floating window changes.
 *
 * This is needed to correctly run the assignments after changes in floating
 * windows which are triggered by user commands (floating enable | disable). In
 * that case, we can't call run_assignments because it will modify the parser
 * state when it needs to parse the user-specified action, breaking the parser
 * state for the original command.
 *
 */
static bool handle_i3_floating(Con *con, xcb_get_property_reply_t *prop) {
    DLOG(fmt::sprintf("floating change for con %p\n", fmt::ptr(con)));

    remanage_window(con);

    return true;
}

/*
 * Handles the WM_CLIENT_MACHINE property for assignments and criteria selection.
 *
 */
static bool handle_machine_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_machine(prop);
    con = remanage_window(con);
    return true;
}

/*
 * Handles the _MOTIF_WM_HINTS property of specifyng window decoration settings.
 *
 */
static bool handle_motif_hints_change(Con *con, xcb_get_property_reply_t *prop) {
    border_style_t motif_border_style;
    bool has_mwm_hints = con->window->window_update_motif_hints(prop, &motif_border_style);

    if (has_mwm_hints && motif_border_style != con->border_style) {
        DLOG(fmt::sprintf("Update border style of con %p to %d\n", fmt::ptr(con), motif_border_style));
        con_set_border_style(con, motif_border_style, con->current_border_width);

        x_push_changes(global.croot);
    }

    return true;
}

static bool handle_windowicon_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_icon(prop);

    x_push_changes(global.croot);

    return true;
}

/* Returns false if the event could not be processed (e.g. the window could not
     * be found), true otherwise */
using cb_property_handler_t = bool (*)(Con *con, xcb_get_property_reply_t *property);

struct property_handler_t {
    xcb_atom_t atom;
    uint32_t long_len;
    cb_property_handler_t cb;

    property_handler_t(
        xcb_atom_t _atom,
        uint32_t _long_len,
        cb_property_handler_t _cb)
        : atom(_atom), long_len(_long_len), cb(_cb) {
    }
};

void PropertyHandlers::property_notify(xcb_property_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    uint8_t state = event->state;
    xcb_window_t window = event->window;
    xcb_atom_t atom = event->atom;

    static std::map<xcb_atom_t, property_handler_t> property_handlers{
        {A__NET_WM_NAME, {A__NET_WM_NAME, 128, handle_windowname_change}},
        {XCB_ATOM_WM_HINTS, {XCB_ATOM_WM_HINTS, UINT_MAX, handle_hints}},
        {XCB_ATOM_WM_NAME, {XCB_ATOM_WM_NAME, 128, handle_windowname_change_legacy}},
        {XCB_ATOM_WM_NORMAL_HINTS, {XCB_ATOM_WM_NORMAL_HINTS, UINT_MAX, handle_normal_hints}},
        {A_WM_CLIENT_LEADER, {A_WM_CLIENT_LEADER, UINT_MAX, handle_clientleader_change}},
        {XCB_ATOM_WM_TRANSIENT_FOR, {XCB_ATOM_WM_TRANSIENT_FOR, UINT_MAX, handle_transient_for}},
        {A_WM_WINDOW_ROLE, {A_WM_WINDOW_ROLE, 128, handle_windowrole_change}},
        {XCB_ATOM_WM_CLASS, {XCB_ATOM_WM_CLASS, 128, handle_class_change}},
        {A__NET_WM_STRUT_PARTIAL, {A__NET_WM_STRUT_PARTIAL, UINT_MAX, handle_strut_partial_change}},
        {A__NET_WM_WINDOW_TYPE, {A__NET_WM_WINDOW_TYPE, UINT_MAX, handle_window_type}},
        {A_I3_FLOATING_WINDOW, {A_I3_FLOATING_WINDOW, UINT_MAX, handle_i3_floating}},
        {XCB_ATOM_WM_CLIENT_MACHINE, {XCB_ATOM_WM_CLIENT_MACHINE, 128, handle_machine_change}},
        {A__MOTIF_WM_HINTS, {A__MOTIF_WM_HINTS, 5 * sizeof(uint64_t), handle_motif_hints_change}},
        {A__NET_WM_ICON, {A__NET_WM_ICON, UINT_MAX, handle_windowicon_change}}
    };

    auto it = property_handlers.find(atom);

    if (it == property_handlers.end()) {
        // LOG(fmt::sprintf("Unhandled property notify for atom %d (0x%08x)\n",  atom, atom));
        return;
    }

    auto &handler = it->second;

    if ((con = con_by_window_id(window)) == nullptr || con->window == nullptr) {
        DLOG(fmt::sprintf("Received property for atom %d for unknown client\n", atom));
        return;
    }

    if (state != XCB_PROPERTY_DELETE) {
        try {
            auto propr = global.x->conn->get_property(0, window, atom, XCB_GET_PROPERTY_TYPE_ANY, 0, handler.long_len);

            handler.cb(con, propr.get().get());
        } catch (std::exception &e) {
            DLOG(fmt::sprintf("got error %s when getting property of atom %d\n", e.what(), atom));
        }
    } else {
        handler.cb(con, nullptr);
    }
}
