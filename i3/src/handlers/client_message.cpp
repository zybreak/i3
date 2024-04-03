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
module i3;

import log;
import rect;

static const uint32_t _NET_WM_STATE_REMOVE = 0;
static const uint32_t _NET_WM_STATE_ADD = 1;
static const uint32_t _NET_WM_STATE_TOGGLE = 2;

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

const int _NET_MOVERESIZE_WINDOW_X = (1 << 8);
const int _NET_MOVERESIZE_WINDOW_Y = (1 << 9);
const int _NET_MOVERESIZE_WINDOW_WIDTH = (1 << 10);
const int _NET_MOVERESIZE_WINDOW_HEIGHT = (1 << 11);

/*
* Handle client messages (EWMH)
*
*/
void PropertyHandlers::handle_client_message(xcb_client_message_event_t *event) {
   /* If this is a startup notification ClientMessage, the library will handle
    * it and call our monitor_event() callback. */
   if (sn_xcb_display_process_event(sndisplay, (xcb_generic_event_t *)event))
       return;

   LOG(fmt::sprintf("ClientMessage for window 0x%08x\n", event->window));
   if (event->type == A__NET_WM_STATE) {
       if (event->format != 32 ||
           (event->data.data32[1] != A__NET_WM_STATE_FULLSCREEN &&
            event->data.data32[1] != A__NET_WM_STATE_DEMANDS_ATTENTION &&
            event->data.data32[1] != A__NET_WM_STATE_STICKY)) {
           DLOG(fmt::sprintf("Unknown atom in clientmessage of type %d\n", event->data.data32[1]));
           return;
       }

       Con *con = con_by_window_id(event->window);
       if (con == nullptr) {
           DLOG("Could not get window for client message\n");
           return;
       }

       if (event->data.data32[1] == A__NET_WM_STATE_FULLSCREEN) {
           /* Check if the fullscreen state should be toggled */
           if ((con->fullscreen_mode != CF_NONE &&
                (event->data.data32[0] == _NET_WM_STATE_REMOVE ||
                 event->data.data32[0] == _NET_WM_STATE_TOGGLE)) ||
               (con->fullscreen_mode == CF_NONE &&
                (event->data.data32[0] == _NET_WM_STATE_ADD ||
                 event->data.data32[0] == _NET_WM_STATE_TOGGLE))) {
               DLOG("toggling fullscreen\n");
               con_toggle_fullscreen(con, CF_OUTPUT);
           }
       } else if (event->data.data32[1] == A__NET_WM_STATE_DEMANDS_ATTENTION) {
           /* Check if the urgent flag must be set or not */
           if (event->data.data32[0] == _NET_WM_STATE_ADD)
               con_set_urgency(con, true);
           else if (event->data.data32[0] == _NET_WM_STATE_REMOVE)
               con_set_urgency(con, false);
           else if (event->data.data32[0] == _NET_WM_STATE_TOGGLE)
               con_set_urgency(con, !con->urgent);
       } else if (event->data.data32[1] == A__NET_WM_STATE_STICKY) {
           DLOG("Received a client message to modify _NET_WM_STATE_STICKY.\n");
           if (event->data.data32[0] == _NET_WM_STATE_ADD)
               con->sticky = true;
           else if (event->data.data32[0] == _NET_WM_STATE_REMOVE)
               con->sticky = false;
           else if (event->data.data32[0] == _NET_WM_STATE_TOGGLE)
               con->sticky = !con->sticky;

           DLOG(fmt::sprintf("New sticky status for con = %p is %i.\n", fmt::ptr(con), con->sticky));
           ewmh_update_sticky(con->window->id, con->sticky);
           output_push_sticky_windows(global.focused);
           ewmh_update_wm_desktop();
       }

       tree_render();
   } else if (event->type == A__NET_ACTIVE_WINDOW) {
       if (event->format != 32)
           return;

       DLOG(fmt::sprintf("_NET_ACTIVE_WINDOW: Window 0x%08x should be activated\n", event->window));

       Con *con = con_by_window_id(event->window);
       if (con == nullptr) {
           DLOG("Could not get window for client message\n");
           return;
       }

       Con *ws = con->con_get_workspace();
       if (ws == nullptr) {
           DLOG("Window is not being managed, ignoring _NET_ACTIVE_WINDOW\n");
           return;
       }

       /* data32[0] indicates the source of the request (application or pager) */
       if (event->data.data32[0] == 2) {
           /* Always focus the con if it is from a pager, because this is most
            * likely from some user action */
           DLOG(fmt::sprintf("This request came from a pager. Focusing con = %p\n", fmt::ptr(con)));

           workspace_show(ws);
           /* Re-set focus, even if unchanged from i3’s perspective. */
           global.x->focused_id = XCB_NONE;
           con->con_activate_unblock();
       } else {
           /* Request is from an application. */
           if (config.focus_on_window_activation == FOWA_FOCUS || (config.focus_on_window_activation == FOWA_SMART && workspace_is_visible(ws))) {
               DLOG(fmt::sprintf("Focusing con = %p\n", fmt::ptr(con)));
               con->con_activate_unblock();
           } else if (config.focus_on_window_activation == FOWA_URGENT || (config.focus_on_window_activation == FOWA_SMART && !workspace_is_visible(ws))) {
               DLOG(fmt::sprintf("Marking con = %p urgent\n", fmt::ptr(con)));
               con_set_urgency(con, true);
           } else
               DLOG(fmt::sprintf("Ignoring request for con = %p.\n", fmt::ptr(con)));
       }

       tree_render();
   } else if (event->type == A_I3_SYNC) {
       xcb_window_t window = event->data.data32[0];
       uint32_t rnd = event->data.data32[1];
       sync_respond(global.x, window, rnd);
   } else if (event->type == A__NET_REQUEST_FRAME_EXTENTS) {
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
           static_cast<uint32_t>(config.default_border_width), /* left */
           static_cast<uint32_t>(config.default_border_width), /* right */
           static_cast<uint32_t>(render_deco_height()),        /* top */
           static_cast<uint32_t>(config.default_border_width)  /* bottom */
       };
       xcb_change_property(
           **global.x,
           XCB_PROP_MODE_REPLACE,
           event->window,
           A__NET_FRAME_EXTENTS,
           XCB_ATOM_CARDINAL, 32, 4,
           &r);
       xcb_flush(**global.x);
   } else if (event->type == A_WM_CHANGE_STATE) {
       /* http://tronche.com/gui/x/icccm/sec-4.html#s-4.1.4 */
       if (event->data.data32[0] == XCB_ICCCM_WM_STATE_ICONIC) {
           /* For compatibility reasons, Wine will request iconic state and cannot ensure that the WM has agreed on it;
            * immediately revert to normal to avoid being stuck in a paused state. */
           DLOG(fmt::sprintf("Client has requested iconic state, rejecting. (window = %08x)\n", event->window));
           long data[] = {XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE};
           xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, event->window,
                               A_WM_STATE, A_WM_STATE, 32, 2, data);
       } else {
           DLOG(fmt::sprintf("Not handling WM_CHANGE_STATE request. (window = %08x, state = %d)\n", event->window, event->data.data32[0]));
       }
   } else if (event->type == A__NET_CURRENT_DESKTOP) {
       /* This request is used by pagers and bars to change the current
        * desktop likely as a result of some user action. We interpret this as
        * a request to focus the given workspace. See
        * https://standards.freedesktop.org/wm-spec/latest/ar01s03.html#idm140251368135008
        * */
       DLOG(fmt::sprintf("Request to change current desktop to index %d\n", event->data.data32[0]));
       Con *ws = ewmh_get_workspace_by_index(event->data.data32[0]);
       if (ws == nullptr) {
           ELOG("Could not determine workspace for this index, ignoring request.\n");
           return;
       }

       DLOG(fmt::sprintf("Handling request to focus workspace %s\n", ws->name));
       workspace_show(ws);
       tree_render();
   } else if (event->type == A__NET_WM_DESKTOP) {
       uint32_t index = event->data.data32[0];
       DLOG(fmt::sprintf("Request to move window %d to EWMH desktop index %d\n", event->window, index));

       Con *con = con_by_window_id(event->window);
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
               ewmh_update_sticky(con->window->id, true);
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
   } else if (event->type == A__NET_CLOSE_WINDOW) {
       /*
        * Pagers wanting to close a window MUST send a _NET_CLOSE_WINDOW
        * client message request to the root window.
        * https://standards.freedesktop.org/wm-spec/wm-spec-latest.html#idm140200472668896
        */
       Con *con = con_by_window_id(event->window);
       if (con) {
           DLOG(fmt::sprintf("Handling _NET_CLOSE_WINDOW request (con = %p)\n", fmt::ptr(con)));

           if (event->data.data32[0])
               global.last_timestamp = event->data.data32[0];

           tree_close_internal(con, KILL_WINDOW, false);
           tree_render();
       } else {
           DLOG(fmt::sprintf("Couldn't find con for _NET_CLOSE_WINDOW request. (window = %08x)\n", event->window));
       }
   } else if (event->type == A__NET_WM_MOVERESIZE) {
       /*
        * Client-side decorated Gtk3 windows emit this signal when being
        * dragged by their GtkHeaderBar
        */
       Con *con = con_by_window_id(event->window);
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
   } else if (event->type == A__NET_MOVERESIZE_WINDOW) {
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

       handle_configure_request(&generated_event);
   } else {
       DLOG(fmt::sprintf("Skipping client message for unhandled type %d\n", event->type));
   }
}
