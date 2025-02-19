/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * handlers.c: Small handlers for various events (keypresses, focus changes,
 *             …).
 *
 */
module;
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>
#include <libsn/sn-monitor.h>
#undef SN_API_NOT_YET_FROZEN

#include <xcb/randr.h>
#include <fmt/printf.h>
module i3;

import std;
import log;
import rect;

static uint32_t const _NET_WM_STATE_REMOVE = 0;
static uint32_t const _NET_WM_STATE_ADD = 1;
static uint32_t const _NET_WM_STATE_TOGGLE = 2;

enum NET_WM {
    _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0,
    _NET_WM_MOVERESIZE_SIZE_TOP,
    _NET_WM_MOVERESIZE_SIZE_TOPRIGHT,
    _NET_WM_MOVERESIZE_SIZE_RIGHT,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT,
    _NET_WM_MOVERESIZE_SIZE_BOTTOM,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT,
    _NET_WM_MOVERESIZE_SIZE_LEFT,
    _NET_WM_MOVERESIZE_MOVE,          /* movement only */
    _NET_WM_MOVERESIZE_SIZE_KEYBOARD, /* size via keyboard */
    _NET_WM_MOVERESIZE_MOVE_KEYBOARD, /* move via keyboard */
    _NET_WM_MOVERESIZE_CANCEL         /* cancel operation */
};

int const _NET_MOVERESIZE_WINDOW_X = (1 << 8);
int const _NET_MOVERESIZE_WINDOW_Y = (1 << 9);
int const _NET_MOVERESIZE_WINDOW_WIDTH = (1 << 10);
int const _NET_MOVERESIZE_WINDOW_HEIGHT = (1 << 11);

static void handle_net_wm_state_change(ConCon *con, uint32_t change, uint32_t atom) {
    if (atom == 0) {
        return;
    }
    
    const char *debug_change = (change == _NET_WM_STATE_REMOVE ? "remove" : (change == _NET_WM_STATE_ADD ? "add" : "toggle"));
    
    if (atom == i3::atoms[i3::Atom::_NET_WM_STATE_FULLSCREEN]) {
        DLOG(std::format("Received a client message to {} _NET_WM_STATE_FULLSCREEN.\n", debug_change));

        /* Check if the fullscreen state should be toggled */
        if (change == _NET_WM_STATE_TOGGLE ||
            (con->fullscreen_mode != CF_NONE && change == _NET_WM_STATE_REMOVE) ||
            (con->fullscreen_mode == CF_NONE && change == _NET_WM_STATE_ADD)) {
            DLOG("toggling fullscreen\n");
            con_toggle_fullscreen(con, CF_OUTPUT);
        }
    } else if (atom == i3::atoms[i3::Atom::_NET_WM_STATE_DEMANDS_ATTENTION]) {
        DLOG(std::format("Received a client message to {} _NET_WM_STATE_DEMANDS_ATTENTION.\n", debug_change));

        /* Check if the urgent flag must be set or not */
        if (change == _NET_WM_STATE_ADD) {
            con->con_set_urgency(true);
            con = remanage_window(con);
        } else if (change == _NET_WM_STATE_REMOVE) {
            con->con_set_urgency(false);
            con = remanage_window(con);
        } else if (change == _NET_WM_STATE_TOGGLE) {
            con->con_set_urgency(!con->urgent);
            con = remanage_window(con);
        }
    } else if (atom == i3::atoms[i3::Atom::_NET_WM_STATE_STICKY]) {
        DLOG(std::format("Received a client message to {} _NET_WM_STATE_STICKY.\n", debug_change));

        if (change == _NET_WM_STATE_ADD) {
            con->sticky = true;
        } else if (change == _NET_WM_STATE_REMOVE) {
            con->sticky = false;
        } else if (change == _NET_WM_STATE_TOGGLE) {
            con->sticky = !con->sticky;
        }

        DLOG(fmt::sprintf("New sticky status for con = %p is %i.\n", fmt::ptr(con), con->sticky));
        ewmh_update_sticky(con->get_window()->id, con->sticky);
        output_push_sticky_windows(global.focused);
        ewmh_update_wm_desktop();
    } else {
        DLOG(std::format("Unknown atom in ClientMessage to {} type {}\n", debug_change, atom));
        return;
    }

    tree_render();
}

static void handle_state(xcb_client_message_event_t *event) {
    if (event->format != 32) {
        DLOG(std::format("Unknown format {} in ClientMessage\n", event->format));
        return;
    }
    
    ConCon *con = con_by_window_id(event->window);
    if (con == nullptr) {
        DLOG("Could not get window for client message\n");
        return;
    }

    for (size_t i = 0; i < sizeof(event->data.data32) / sizeof(event->data.data32[0]) - 1; i++) {
        handle_net_wm_state_change(con, event->data.data32[0], event->data.data32[i + 1]);
    }
}

static void handle_active_window(xcb_client_message_event_t *event) {
    if (event->format != 32) {
        return;
    }

    DLOG(fmt::sprintf("_NET_ACTIVE_WINDOW: Window 0x%08x should be activated\n", event->window));

    ConCon *con = con_by_window_id(event->window);
    if (con == nullptr) {
        DLOG("Could not get window for client message\n");
        return;
    }

    WorkspaceCon *ws = con->con_get_workspace();
    if (ws == nullptr) {
        DLOG("Window is not being managed, ignoring _NET_ACTIVE_WINDOW\n");
        return;
    }
    /* data32[0] indicates the source of the request (application or pager) */
    if (event->data.data32[0] == 2) {
        /* Always focus the con if it is from a pager, because this is most
         * likely from some user action */
        DLOG(fmt::sprintf("This request came from a pager. Focusing con = %p\n", fmt::ptr(con)));

        global.workspaceManager->workspace_show(ws);
        /* Re-set focus, even if unchanged from i3’s perspective. */
        global.x->focused_id = XCB_NONE;
        con->con_activate_unblock();
    } else {
        /* Request is from an application. */
        if (global.configManager->config->focus_on_window_activation == FOWA_FOCUS || (global.configManager->config->focus_on_window_activation == FOWA_SMART && workspace_is_visible(ws))) {
            DLOG(fmt::sprintf("Focusing con = %p\n", fmt::ptr(con)));
            con->con_activate_unblock();
        } else if (global.configManager->config->focus_on_window_activation == FOWA_URGENT || (global.configManager->config->focus_on_window_activation == FOWA_SMART && !workspace_is_visible(ws))) {
            DLOG(fmt::sprintf("Marking con = %p urgent\n", fmt::ptr(con)));
            con->con_set_urgency(true);
        } else {
            DLOG(fmt::sprintf("Ignoring request for con = %p.\n", fmt::ptr(con)));
        }
    }

    tree_render();
}

static void handle_frame_extents(xcb_client_message_event_t *event) {
    /*
     * A client can request an estimate for the frame size which the window
     * manager will put around it before actually mapping its window. Java
     * does this (as of openjdk-7).
     *
     * Note that the calculation below is not entirely accurate — once you
     * set a different border type, it’s off. We _could_ request all the
     * window properties (which have to be set up at this point according
     * to EWMH), but that seems rather elaborate. The standard explicitly
     * says the application must cope with an estimate that is not entirely
     * accurate.
     */
    DLOG(fmt::sprintf("_NET_REQUEST_FRAME_EXTENTS for window 0x%08x\n", event->window));

    /* The reply data: approximate frame size */
    Rect r = {
        static_cast<uint32_t>(global.configManager->config->default_border_width), /* left */
        static_cast<uint32_t>(global.configManager->config->default_border_width), /* right */
        static_cast<uint32_t>(render_deco_height()),                               /* top */
        static_cast<uint32_t>(global.configManager->config->default_border_width)  /* bottom */
    };
    xcb_change_property(
        **global.x,
        XCB_PROP_MODE_REPLACE,
        event->window,
        i3::atoms[i3::Atom::_NET_FRAME_EXTENTS],
        XCB_ATOM_CARDINAL, 32, 4,
        &r);
    xcb_flush(**global.x);
}

static void handle_current_desktop(xcb_client_message_event_t *event) {
    /* This request is used by pagers and bars to change the current
     * desktop likely as a result of some user action. We interpret this as
     * a request to focus the given workspace. See
     * https://standards.freedesktop.org/wm-spec/latest/ar01s03.html#idm140251368135008
     * */
    DLOG(fmt::sprintf("Request to change current desktop to index %d\n", event->data.data32[0]));
    WorkspaceCon *ws = ewmh_get_workspace_by_index(event->data.data32[0]);
    if (ws == nullptr) {
        ELOG("Could not determine workspace for this index, ignoring request.\n");
        return;
    }

    DLOG(fmt::sprintf("Handling request to focus workspace %s\n", ws->name));
    global.workspaceManager->workspace_show(ws);
    tree_render();
}

static void handle_change_state(xcb_client_message_event_t *event) {
    /* http://tronche.com/gui/x/icccm/sec-4.html#s-4.1.4 */
    if (event->data.data32[0] == XCB_ICCCM_WM_STATE_ICONIC) {
        /* For compatibility reasons, Wine will request iconic state and cannot ensure that the WM has agreed on it;
         * immediately revert to normal to avoid being stuck in a paused state. */
        DLOG(fmt::sprintf("Client has requested iconic state, rejecting. (window = %08x)\n", event->window));
        long data[] = {XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE};
        xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, event->window,
                            i3::atoms[i3::Atom::WM_STATE], i3::atoms[i3::Atom::WM_STATE], 32, 2, data);
    } else {
        DLOG(fmt::sprintf("Not handling WM_CHANGE_STATE request. (window = %08x, state = %d)\n", event->window, event->data.data32[0]));
    }
}

static void handle_desktop(xcb_client_message_event_t *event) {
    uint32_t index = event->data.data32[0];
    DLOG(fmt::sprintf("Request to move window %d to EWMH desktop index %d\n", event->window, index));

    ConCon *con = con_by_window_id(event->window);
    if (con == nullptr) {
        DLOG(fmt::sprintf("Couldn't find con for window %d, ignoring the request.\n", event->window));
        return;
    }

    if (index == NET_WM_DESKTOP_ALL) {
        /* The window is requesting to be visible on all workspaces, so
         * let's float it and make it sticky. */
        DLOG("The window was requested to be visible on all workspaces, making it sticky and floating.\n");

        if (floating_enable(con, false)) {
            con->floating = FLOATING_AUTO_ON;

            con->sticky = true;
            ewmh_update_sticky(con->get_window()->id, true);
            output_push_sticky_windows(global.focused);
        }
    } else {
        WorkspaceCon *ws = ewmh_get_workspace_by_index(index);
        if (ws == nullptr) {
            ELOG("Could not determine workspace for this index, ignoring request.\n");
            return;
        }

        con_move_to_workspace(con, ws, true, false, false);
    }

    tree_render();
    ewmh_update_wm_desktop();
}

static void handle_close_window(xcb_client_message_event_t *event) {
    /*
     * Pagers wanting to close a window MUST send a _NET_CLOSE_WINDOW
     * client message request to the root window.
     * https://standards.freedesktop.org/wm-spec/wm-spec-latest.html#idm140200472668896
     */
    ConCon *con = con_by_window_id(event->window);
    if (con) {
        DLOG(fmt::sprintf("Handling _NET_CLOSE_WINDOW request (con = %p)\n", fmt::ptr(con)));

        if (event->data.data32[0]) {
            global.last_timestamp = event->data.data32[0];
        }

        tree_close_internal(con, kill_window_t::KILL_WINDOW, false);
        tree_render();
    } else {
        DLOG(fmt::sprintf("Couldn't find con for _NET_CLOSE_WINDOW request. (window = %08x)\n", event->window));
    }
}

void PropertyHandlers::handle_move_resize(xcb_client_message_event_t *event) {
    /*
     * Client-side decorated Gtk3 windows emit this signal when being
     * dragged by their GtkHeaderBar
     */
    ConCon *con = con_by_window_id(event->window);
    if (!con || !con->con_is_floating()) {
        DLOG(fmt::sprintf("Couldn't find con for _NET_WM_MOVERESIZE request, or con not floating (window = %08x)\n", event->window));
        return;
    }
    DLOG(fmt::sprintf("Handling _NET_WM_MOVERESIZE request (con = %p)\n", fmt::ptr(con)));
    uint32_t direction = event->data.data32[2];
    uint32_t x_root = event->data.data32[0];
    uint32_t y_root = event->data.data32[1];
    /* construct fake xcb_button_press_event_t */
    xcb_button_press_event_t fake = {
        .root_x = static_cast<int16_t>(x_root),
        .root_y = static_cast<int16_t>(y_root),
        .event_x = static_cast<int16_t>(x_root - (con->rect.x)),
        .event_y = static_cast<int16_t>(y_root - (con->rect.y))};
    switch (direction) {
        case _NET_WM_MOVERESIZE_MOVE:
            floating_drag_window(con->parent, &fake, false);
            break;
        case _NET_WM_MOVERESIZE_SIZE_TOPLEFT ... _NET_WM_MOVERESIZE_SIZE_LEFT:
            floating_resize_window(con->parent, false, &fake);
            break;
        default:
            DLOG(fmt::sprintf("_NET_WM_MOVERESIZE direction %d not implemented\n", direction));
            break;
    }
}

static xcb_configure_request_event_t handle_move_resize_window(xcb_client_message_event_t *event) {
    DLOG("Received _NET_MOVE_RESIZE_WINDOW. Handling by faking a configure request.\n");

    xcb_configure_request_event_t generated_event{};

    generated_event.window = event->window;
    generated_event.response_type = XCB_CONFIGURE_REQUEST;

    generated_event.value_mask = 0;
    if (event->data.data32[0] & _NET_MOVERESIZE_WINDOW_X) {
        generated_event.value_mask |= XCB_CONFIG_WINDOW_X;
        generated_event.x = event->data.data32[1];
    }
    if (event->data.data32[0] & _NET_MOVERESIZE_WINDOW_Y) {
        generated_event.value_mask |= XCB_CONFIG_WINDOW_Y;
        generated_event.y = event->data.data32[2];
    }
    if (event->data.data32[0] & _NET_MOVERESIZE_WINDOW_WIDTH) {
        generated_event.value_mask |= XCB_CONFIG_WINDOW_WIDTH;
        generated_event.width = event->data.data32[3];
    }
    if (event->data.data32[0] & _NET_MOVERESIZE_WINDOW_HEIGHT) {
        generated_event.value_mask |= XCB_CONFIG_WINDOW_HEIGHT;
        generated_event.height = event->data.data32[4];
    }

    return generated_event;
}

/*
 * Handle client messages (EWMH)
 *
 */
void PropertyHandlers::handle_client_message(xcb_client_message_event_t *event) {
    /* If this is a startup notification ClientMessage, the library will handle
     * it and call our monitor_event() callback. */
    if (sn_xcb_display_process_event(global.applicationLauncher->sndisplay, (xcb_generic_event_t *)event)) {
        return;
    }

    LOG(fmt::sprintf("ClientMessage for window 0x%08x\n", event->window));
    if (event->type == i3::atoms[i3::Atom::_NET_WM_STATE]) {
        handle_state(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_ACTIVE_WINDOW]) {
        handle_active_window(event);
    } else if (event->type == i3::atoms[i3::Atom::I3_SYNC]) {
        xcb_window_t window = event->data.data32[0];
        uint32_t rnd = event->data.data32[1];
        sync_respond(global.x, window, rnd);
    } else if (event->type == i3::atoms[i3::Atom::_NET_REQUEST_FRAME_EXTENTS]) {
        handle_frame_extents(event);
    } else if (event->type == i3::atoms[i3::Atom::WM_CHANGE_STATE]) {
        handle_change_state(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_CURRENT_DESKTOP]) {
        handle_current_desktop(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_WM_DESKTOP]) {
        handle_desktop(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_CLOSE_WINDOW]) {
        handle_close_window(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_WM_MOVERESIZE]) {
        handle_move_resize(event);
    } else if (event->type == i3::atoms[i3::Atom::_NET_MOVERESIZE_WINDOW]) {
        xcb_configure_request_event_t generated_event = handle_move_resize_window(event);
        handle_configure_request(&generated_event);
    } else {
        DLOG(fmt::sprintf("Skipping client message for unhandled type %d\n", event->type));
    }
}
