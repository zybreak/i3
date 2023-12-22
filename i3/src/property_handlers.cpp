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

#include "i3string.h"
#include "log.h"

#include "i3_ipc/include/i3-ipc.h"
#include "manage.h"
#include "i3.h"

#include <ctime>

#include <xcb/randr.h>
#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-monitor.h>
#include <memory>
#include <mutex>
#include <algorithm>
#include <ranges>
#include <span>
#include "global.h"
#include "ipc.h"
#include <config.h>
module i3;

import :output;
import utils;

/* After mapping/unmapping windows, a notify event is generated. However, we don’t want it,
   since it’d trigger an infinite loop of switching between the different windows when
   changing workspaces */
static std::mutex mtx;

/*
 * Adds the given sequence to the list of events which are ignored.
 * If this ignore should only affect a specific response_type, pass
 * response_type, otherwise, pass -1.
 *
 * Every ignored sequence number gets garbage collected after 5 seconds.
 *
 */
void PropertyHandlers::add_ignore_event(const int sequence, const int response_type) {
    std::lock_guard<std::mutex> guard(mtx);
    auto event = std::make_unique<Ignore_Event>();

    event->sequence = sequence;
    event->response_type = response_type;
    event->added = time(nullptr);

    ignore_events.insert(ignore_events.begin(), std::move(event));
}

/*
 * Checks if the given sequence is ignored and returns true if so.
 *
 */
bool PropertyHandlers::event_is_ignored(const int sequence, const int response_type) {
    std::lock_guard<std::mutex> guard(mtx);

    time_t now = time(nullptr);
    ignore_events.erase(std::remove_if(ignore_events.begin(), ignore_events.end(), [now](const auto &it) { return (now - it->added) > 5; }), ignore_events.end());

    return std::any_of(
            ignore_events.begin(),
            ignore_events.end(),
            [&response_type, &sequence](const auto &event) {
                return (event->sequence == sequence && (event->response_type == -1 || event->response_type == response_type));
            });
}

/*
 * Called with coordinates of an enter_notify event or motion_notify event
 * to check if the user crossed virtual screen boundaries and adjust the
 * current workspace, if so.
 *
 */
static void check_crossing_screen_boundary(uint32_t x, uint32_t y) {
    Output *output;

    /* If the user disable focus follows mouse, we have nothing to do here */
    if (config.disable_focus_follows_mouse)
        return;

    if ((output = global.randr->get_output_containing(x, y)) == nullptr) {
        ELOG("ERROR: No such screen\n");
        return;
    }

    if (output->con == nullptr) {
        ELOG("ERROR: The screen is not recognized by i3 (no container associated)\n");
        return;
    }

    /* Focus the output on which the user moved their cursor */
    Con *old_focused = global.focused;
    Con *next = con_descend_focused(output->con->output_get_content());
    /* Since we are switching outputs, this *must* be a different workspace, so
     * call workspace_show() */
    workspace_show(next->con_get_workspace());
    next->con_focus();

    /* If the focus changed, we re-render to get updated decorations */
    if (old_focused != global.focused)
        tree_render();
}

/*
 * When the user moves the mouse pointer onto a window, this callback gets called.
 *
 */
void PropertyHandlers::handle_enter_notify(xcb_enter_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("enter_notify for %08x, mode = %d, detail %d, serial %d\n",
         event->event, event->mode, event->detail, event->sequence));
    DLOG(fmt::sprintf("coordinates %d, %d\n",  event->event_x, event->event_y));
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
        DLOG(fmt::sprintf("Getting screen at %d x %d\n",  event->root_x, event->root_y));
        check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    /* see if the user entered the window on a certain window decoration */
    layout_t layout = (enter_child ? con->parent->layout : con->layout);
    if (layout == L_DEFAULT) {
        for (auto &child : con->nodes_head | std::views::reverse) {
            if (child->deco_rect.rect_contains(event->event_x, event->event_y)) {
                LOG(fmt::sprintf("using child %p / %s instead!\n",  (void*)child, child->name));
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

    global.x->focused_id = XCB_NONE;
    con_descend_focused(con)->con_focus();
    tree_render();
}

/*
 * When the user moves the mouse but does not change the active window
 * (e.g. when having no windows opened but moving mouse on the root screen
 * and crossing virtual screen boundaries), this callback gets called.
 *
 */
void PropertyHandlers::handle_motion_notify(xcb_motion_notify_event_t *event) {
    global.last_timestamp = event->time;

    /* Skip events where the pointer was over a child window, we are only
     * interested in events on the root window. */
    if (event->child != XCB_NONE)
        return;

    Con *con;
    if ((con = con_by_frame_id(event->event)) == nullptr) {
        DLOG("MotionNotify for an unknown container, checking if it crosses screen boundaries.\n");
        check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    if (config.disable_focus_follows_mouse)
        return;

    if (con->layout != L_DEFAULT && con->layout != L_SPLITV && con->layout != L_SPLITH)
        return;

    /* see over which rect the user is */
    for (auto &current : con->nodes_head | std::views::reverse) {
        if (!current->deco_rect.rect_contains(event->event_x, event->event_y))
            continue;

        /* We found the rect, let’s see if this window is focused */
        if (con::first(con->focus_head) == current)
            return;

        current->con_focus();
        x_push_changes(global.croot);
        return;
    }
}

/*
 * Called when the keyboard mapping changes (for example by using Xmodmap),
 * we need to update our key bindings then (re-translate symbols).
 *
 */
void PropertyHandlers::handle_mapping_notify(xcb_mapping_notify_event_t *event) {
    if (event->request != XCB_MAPPING_KEYBOARD &&
        event->request != XCB_MAPPING_MODIFIER)
        return;

    DLOG("Received mapping_notify for keyboard or modifier mapping, re-grabbing keys\n");
    global.keysyms->refresh_keyboard_mapping(event);

    global.x->xcb_numlock_mask = global.keysyms->get_numlock_mask();

    ungrab_all_keys(*global.x);
    translate_keysyms();
    grab_all_keys(*global.x);
}

/*
 * A new window appeared on the screen (=was mapped), so let’s manage it.
 *
 */
void PropertyHandlers::handle_map_request(xcb_map_request_event_t *event) {

    auto attr = global.x->conn->get_window_attributes_unchecked(event->window);

    DLOG(fmt::sprintf("window = 0x%08x, serial is %d.\n",  event->window, event->sequence));
    add_ignore_event(event->sequence, -1);

    manage_window(event->window, attr.get().get(), false);
}

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
            DLOG(fmt::sprintf("proposed x = %d, new x is %d\n",  event->x, newrect.x));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_Y) {
            newrect.y = event->y + (-1) * bsr.y;
            DLOG(fmt::sprintf("proposed y = %d, new y is %d\n",  event->y, newrect.y));
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
        DLOG(fmt::sprintf("Reconfiguring dock window (con = %p).\n",  (void*)con));
        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
            DLOG(fmt::sprintf("Dock client wants to change height to %d, we can do that.\n",  event->height));

            con->geometry.height = event->height;
            tree_render();
        }

        if (event->value_mask & XCB_CONFIG_WINDOW_X || event->value_mask & XCB_CONFIG_WINDOW_Y) {
            int16_t x = event->value_mask & XCB_CONFIG_WINDOW_X ? event->x : (int16_t)con->geometry.x;
            int16_t y = event->value_mask & XCB_CONFIG_WINDOW_Y ? event->y : (int16_t)con->geometry.y;

            Con *current_output = con->con_get_output();
            Output *target = global.randr->get_output_containing(x, y);
            if (target != nullptr && current_output != target->con) {
                DLOG(fmt::sprintf("Dock client is requested to be moved to output %s, moving it there.\n",  target->output_primary_name()));
                Match *match;
                Con *nc = con_for_window(target->con, con->window, &match);
                DLOG(fmt::sprintf("Dock client will be moved to container %p.\n",  (void*)nc));
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
        DLOG(fmt::sprintf("window 0x%08x wants to be stacked %d\n",  event->window, event->stack_mode));

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
            DLOG(fmt::sprintf("Focusing con = %p\n",  (void*)con));
            workspace_show(workspace);
            con->con_activate_unblock();
            tree_render();
        } else if (config.focus_on_window_activation == FOWA_URGENT || (config.focus_on_window_activation == FOWA_SMART && !workspace_is_visible(workspace))) {
            DLOG(fmt::sprintf("Marking con = %p urgent\n",  (void*)con));
            con_set_urgency(con, true);
            tree_render();
        } else {
            DLOG(fmt::sprintf("Ignoring request for con = %p.\n",  (void*)con));
        }
    }

out:
    fake_absolute_configure_notify(con);
}

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
    DLOG(fmt::sprintf("root geometry reply: (%d, %d) %d x %d\n",  reply->x, reply->y, reply->width, reply->height));

    global.croot->rect.width = reply->width;
    global.croot->rect.height = reply->height;

    global.randr->randr_query_outputs();

    ipc_send_event("output", I3_IPC_EVENT_OUTPUT, R"({"change":"unspecified"})");
}

/*
 * Our window decorations were unmapped. That means, the window will be killed
 * now, so we better clean up before.
 *
 */
void PropertyHandlers::handle_unmap_notify_event(xcb_unmap_notify_event_t *event) {
    DLOG(fmt::sprintf("UnmapNotify for 0x%08x (received from 0x%08x), serial %d\n",  event->window, event->event, event->sequence));
    xcb_get_input_focus_cookie_t cookie;
    Con *con = con_by_window_id(event->window);
    if (con == nullptr) {
        /* This could also be an UnmapNotify for the frame. We need to
         * decrement the ignore_unmap counter. */
        con = con_by_frame_id(event->window);
        if (con == nullptr) {
            LOG("Not a managed window, ignoring UnmapNotify event\n");
            return;
        }

        if (con->ignore_unmap > 0)
            con->ignore_unmap--;
        /* See the end of this function. */
        cookie = xcb_get_input_focus(**global.x);
        DLOG(fmt::sprintf("ignore_unmap = %d for frame of container %p\n",  con->ignore_unmap, (void*)con));
        goto ignore_end;
    }

    /* See the end of this function. */
    cookie = xcb_get_input_focus(**global.x);

    if (con->ignore_unmap > 0) {
        DLOG(fmt::sprintf("ignore_unmap = %d, dec\n",  con->ignore_unmap));
        con->ignore_unmap--;
        goto ignore_end;
    }

    /* Since we close the container, we need to unset _NET_WM_DESKTOP and
     * _NET_WM_STATE according to the spec. */
    xcb_delete_property(**global.x, event->window, A__NET_WM_DESKTOP);
    xcb_delete_property(**global.x, event->window, A__NET_WM_STATE);

    tree_close_internal(con, DONT_KILL_WINDOW, false);
    tree_render();

ignore_end:
    /* If the client (as opposed to i3) destroyed or unmapped a window, an
     * EnterNotify event will follow (indistinguishable from an EnterNotify
     * event caused by moving your mouse), causing i3 to set focus to whichever
     * window is now visible.
     *
     * In a complex stacked or tabbed layout (take two v-split containers in a
     * tabbed container), when the bottom window in tab2 is closed, the bottom
     * window of tab1 is visible instead. X11 will thus send an EnterNotify
     * event for the bottom window of tab1, while the focus should be set to
     * the remaining window of tab2.
     *
     * Therefore, we ignore all EnterNotify events which have the same sequence
     * as an UnmapNotify event. */
    add_ignore_event(event->sequence, XCB_ENTER_NOTIFY);

    /* Since we just ignored the sequence of this UnmapNotify, we want to make
     * sure that following events use a different sequence. When putting xterm
     * into fullscreen and moving the pointer to a different window, without
     * using GetInputFocus, subsequent (legitimate) EnterNotify events arrived
     * with the same sequence and thus were ignored (see ticket #609). */
    free(xcb_get_input_focus_reply(**global.x, cookie, nullptr));
}

/*
 * A destroy notify event is sent when the window is not unmapped, but
 * immediately destroyed (for example when starting a window and immediately
 * killing the program which started it).
 *
 * We just pass on the event to the unmap notify handler (by copying the
 * important fields in the event data structure).
 *
 */
void PropertyHandlers::handle_destroy_notify_event(xcb_destroy_notify_event_t *event) {
    DLOG(fmt::sprintf("destroy notify for 0x%08x, 0x%08x\n",  event->event, event->window));

    xcb_unmap_notify_event_t unmap;
    unmap.sequence = event->sequence;
    unmap.event = event->event;
    unmap.window = event->window;

    handle_unmap_notify_event(&unmap);
}

static bool window_name_changed(i3Window *window, char *old_name) {
    if ((old_name == nullptr) && (window->name == nullptr))
        return false;

    /* Either the old or the new one is NULL, but not both. */
    if ((old_name == nullptr) ^ (window->name == nullptr))
        return true;

    return (strcmp(old_name, window->name->get_utf8()) != 0);
}

/*
 * Called when a window changes its title
 *
 */
static bool handle_windowname_change(Con *con, xcb_get_property_reply_t *prop) {
    char *old_name = (con->window->name != nullptr ? sstrdup(con->window->name->get_utf8()) : nullptr);

    con->window->window_update_name(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name))
        ipc_send_window_event("title", con);

    FREE(old_name);

    return true;
}

/*
 * Handles legacy window name updates (WM_NAME), see also src/window.c,
 * window_update_name_legacy().
 *
 */
static bool handle_windowname_change_legacy(Con *con, xcb_get_property_reply_t *prop) {
    char *old_name = (con->window->name != nullptr ? sstrdup(con->window->name->get_utf8()) : nullptr);

    con->window->window_update_name_legacy(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name))
        ipc_send_window_event("title", con);

    FREE(old_name);

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
 * Expose event means we should redraw our windows (= title bar)
 *
 */
void PropertyHandlers::handle_expose_event(xcb_expose_event_t *event) {
    if (event->count != 0) {
        return;
    }

    Con *parent;

    DLOG(fmt::sprintf("window = %08x\n",  event->window));

    if ((parent = con_by_frame_id(event->window)) == nullptr) {
        LOG("expose event for unknown window, ignoring\n");
        return;
    }

    /* Since we render to our surface on every change anyways, expose events
     * only tell us that the X server lost (parts of) the window contents. */
    draw_util_copy_surface(&(parent->frame_buffer), &(parent->frame),
                           0, 0, 0, 0, parent->rect.width, parent->rect.height);
    xcb_flush(**global.x);
}

enum NET_WM {
    _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0,
    _NET_WM_MOVERESIZE_SIZE_TOP,
    _NET_WM_MOVERESIZE_SIZE_TOPRIGHT,
    _NET_WM_MOVERESIZE_SIZE_RIGHT,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT,
    _NET_WM_MOVERESIZE_SIZE_BOTTOM,
    _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT,
    _NET_WM_MOVERESIZE_SIZE_LEFT,
    _NET_WM_MOVERESIZE_MOVE,           /* movement only */
    _NET_WM_MOVERESIZE_SIZE_KEYBOARD,  /* size via keyboard */
    _NET_WM_MOVERESIZE_MOVE_KEYBOARD,  /* move via keyboard */
    _NET_WM_MOVERESIZE_CANCEL          /* cancel operation */
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

    LOG(fmt::sprintf("ClientMessage for window 0x%08x\n",  event->window));
    if (event->type == A__NET_WM_STATE) {
        if (event->format != 32 ||
            (event->data.data32[1] != A__NET_WM_STATE_FULLSCREEN &&
             event->data.data32[1] != A__NET_WM_STATE_DEMANDS_ATTENTION &&
             event->data.data32[1] != A__NET_WM_STATE_STICKY)) {
            DLOG(fmt::sprintf("Unknown atom in clientmessage of type %d\n",  event->data.data32[1]));
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

            DLOG(fmt::sprintf("New sticky status for con = %p is %i.\n",  (void*)con, con->sticky));
            ewmh_update_sticky(con->window->id, con->sticky);
            output_push_sticky_windows(global.focused);
            ewmh_update_wm_desktop();
        }

        tree_render();
    } else if (event->type == A__NET_ACTIVE_WINDOW) {
        if (event->format != 32)
            return;

        DLOG(fmt::sprintf("_NET_ACTIVE_WINDOW: Window 0x%08x should be activated\n",  event->window));

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
            DLOG(fmt::sprintf("This request came from a pager. Focusing con = %p\n",  (void*)con));

            workspace_show(ws);
            /* Re-set focus, even if unchanged from i3’s perspective. */
            global.x->focused_id = XCB_NONE;
            con->con_activate_unblock();
        } else {
            /* Request is from an application. */
            if (config.focus_on_window_activation == FOWA_FOCUS || (config.focus_on_window_activation == FOWA_SMART && workspace_is_visible(ws))) {
                DLOG(fmt::sprintf("Focusing con = %p\n",  (void*)con));
                con->con_activate_unblock();
            } else if (config.focus_on_window_activation == FOWA_URGENT || (config.focus_on_window_activation == FOWA_SMART && !workspace_is_visible(ws))) {
                DLOG(fmt::sprintf("Marking con = %p urgent\n",  (void*)con));
                con_set_urgency(con, true);
            } else
                DLOG(fmt::sprintf("Ignoring request for con = %p.\n",  (void*)con));
        }

        tree_render();
    } else if (event->type == A_I3_SYNC) {
        xcb_window_t window = event->data.data32[0];
        uint32_t rnd = event->data.data32[1];
        sync_respond(window, rnd);
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
        DLOG(fmt::sprintf("_NET_REQUEST_FRAME_EXTENTS for window 0x%08x\n",  event->window));

        /* The reply data: approximate frame size */
        Rect r = {
                (uint32_t)config.default_border_width, /* left */
                (uint32_t)config.default_border_width, /* right */
                (uint32_t)render_deco_height(),        /* top */
                (uint32_t)config.default_border_width  /* bottom */
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
            /* For compatiblity reasons, Wine will request iconic state and cannot ensure that the WM has agreed on it;
             * immediately revert to normal to avoid being stuck in a paused state. */
            DLOG(fmt::sprintf("Client has requested iconic state, rejecting. (window = %d)\n",  event->window));
            long data[] = {XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE};
            xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, event->window,
                                A_WM_STATE, A_WM_STATE, 32, 2, data);
        } else {
            DLOG(fmt::sprintf("Not handling WM_CHANGE_STATE request. (window = %d, state = %d)\n",  event->window, event->data.data32[0]));
        }
    } else if (event->type == A__NET_CURRENT_DESKTOP) {
        /* This request is used by pagers and bars to change the current
         * desktop likely as a result of some user action. We interpret this as
         * a request to focus the given workspace. See
         * https://standards.freedesktop.org/wm-spec/latest/ar01s03.html#idm140251368135008
         * */
        DLOG(fmt::sprintf("Request to change current desktop to index %d\n",  event->data.data32[0]));
        Con *ws = ewmh_get_workspace_by_index(event->data.data32[0]);
        if (ws == nullptr) {
            ELOG("Could not determine workspace for this index, ignoring request.\n");
            return;
        }

        DLOG(fmt::sprintf("Handling request to focus workspace %s\n",  ws->name));
        workspace_show(ws);
        tree_render();
    } else if (event->type == A__NET_WM_DESKTOP) {
        uint32_t index = event->data.data32[0];
        DLOG(fmt::sprintf("Request to move window %d to EWMH desktop index %d\n",  event->window, index));

        Con *con = con_by_window_id(event->window);
        if (con == nullptr) {
            DLOG(fmt::sprintf("Couldn't find con for window %d, ignoring the request.\n",  event->window));
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
            Con *ws = ewmh_get_workspace_by_index(index);
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
            DLOG(fmt::sprintf("Handling _NET_CLOSE_WINDOW request (con = %p)\n",  (void*)con));

            if (event->data.data32[0])
                global.last_timestamp = event->data.data32[0];

            tree_close_internal(con, KILL_WINDOW, false);
            tree_render();
        } else {
            DLOG(fmt::sprintf("Couldn't find con for _NET_CLOSE_WINDOW request. (window = %d)\n",  event->window));
        }
    } else if (event->type == A__NET_WM_MOVERESIZE) {
        /*
         * Client-side decorated Gtk3 windows emit this signal when being
         * dragged by their GtkHeaderBar
         */
        Con *con = con_by_window_id(event->window);
        if (!con || !con->con_is_floating()) {
            DLOG(fmt::sprintf("Couldn't find con for _NET_WM_MOVERESIZE request, or con not floating (window = %d)\n",  event->window));
            return;
        }
        DLOG(fmt::sprintf("Handling _NET_WM_MOVERESIZE request (con = %p)\n",  (void*)con));
        uint32_t direction = event->data.data32[2];
        uint32_t x_root = event->data.data32[0];
        uint32_t y_root = event->data.data32[1];
        /* construct fake xcb_button_press_event_t */
        xcb_button_press_event_t fake = {
            .root_x = (int16_t)x_root,
            .root_y = (int16_t)y_root,
            .event_x = (int16_t)(x_root - (con->rect.x)),
            .event_y = (int16_t)(y_root - (con->rect.y))};
        switch (direction) {
            case _NET_WM_MOVERESIZE_MOVE:
                floating_drag_window(con->parent, &fake, false);
                break;
            case _NET_WM_MOVERESIZE_SIZE_TOPLEFT ... _NET_WM_MOVERESIZE_SIZE_LEFT:
                floating_resize_window(con->parent, false, &fake);
                break;
            default:
                DLOG(fmt::sprintf("_NET_WM_MOVERESIZE direction %d not implemented\n",  direction));
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
        DLOG(fmt::sprintf("Skipping client message for unhandled type %d\n",  event->type));
    }
}

static bool handle_window_type(Con *con, xcb_get_property_reply_t *reply) {
    con->window->window_update_type(reply);
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
 * Handles the WM_HINTS property for extracting the urgency state of the window.
 *
 */
static bool handle_hints(Con *con, xcb_get_property_reply_t *reply) {
    bool urgency_hint;
    con->window->window_update_hints(reply, &urgency_hint);
    con_set_urgency(con, urgency_hint);
    tree_render();
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
 * Handles changes of the WM_CLIENT_LEADER atom which specifies if this is a
 * toolwindow (or similar) and to which window it belongs (logical parent).
 *
 */
static bool handle_clientleader_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_leader(prop);
    return true;
}

/*
 * Handles FocusIn events which are generated by clients (i3’s focus changes
 * don’t generate FocusIn events due to a different EventMask) and updates the
 * decorations accordingly.
 *
 */
void PropertyHandlers::handle_focus_in(xcb_focus_in_event_t *event) {
    DLOG(fmt::sprintf("focus change in, for window 0x%08x\n",  event->event));

    if (event->event == global.x->root) {
        DLOG("Received focus in for root window, refocusing the focused window.\n");
        global.focused->con_focus();
        global.x->focused_id = XCB_NONE;
        x_push_changes(global.croot);
    }

    Con *con;
    if ((con = con_by_window_id(event->event)) == nullptr || con->window == nullptr)
        return;
    DLOG(fmt::sprintf("That is con %p / %s\n",  (void*)con, con->name));

    if (event->mode == XCB_NOTIFY_MODE_GRAB ||
        event->mode == XCB_NOTIFY_MODE_UNGRAB) {
        DLOG("FocusIn event for grab/ungrab, ignoring\n");
        return;
    }

    if (event->detail == XCB_NOTIFY_DETAIL_POINTER) {
        DLOG("notify detail is pointer, ignoring this event\n");
        return;
    }

    /* Floating windows should be refocused to ensure that they are on top of
     * other windows. */
    if (global.x->focused_id == event->event && !con->con_inside_floating()) {
        DLOG("focus matches the currently focused window, not doing anything\n");
        return;
    }

    /* Skip dock clients, they cannot get the i3 focus. */
    if (con->parent->type == CT_DOCKAREA) {
        DLOG("This is a dock client, not focusing.\n");
        return;
    }

    DLOG("focus is different / refocusing floating window: updating decorations\n");

    con->con_activate_unblock();

    /* We update focused_id because we don’t need to set focus again */
    global.x->focused_id = event->event;
    tree_render();
}

/*
 * Handles ConfigureNotify events for the root window, which are generated when
 * the monitor configuration changed.
 *
 */
void PropertyHandlers::handle_configure_notify(xcb_configure_notify_event_t *event) {
    if (event->event != global.x->root) {
        DLOG(fmt::sprintf("ConfigureNotify for non-root window 0x%08x, ignoring\n",  event->event));
        return;
    }
    DLOG(fmt::sprintf("ConfigureNotify for root window 0x%08x\n",  event->event));

    global.randr->randr_query_outputs();
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
 * Handles the WM_CLIENT_MACHINE property for assignments and criteria selection.
 *
 */
static bool handle_machine_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_machine(prop);
    con = remanage_window(con);
    return true;
}


/*
 * Handles the _MOTIF_WM_HINTS property of specifing window deocration settings.
 *
 */
static bool handle_motif_hints_change(Con *con, xcb_get_property_reply_t *prop) {
    border_style_t motif_border_style;
    update_motif_hints(prop, &motif_border_style);

    if (motif_border_style != con->border_style && motif_border_style != BS_NORMAL) {
        DLOG(fmt::sprintf("Update border style of con %p to %d\n",  (void*)con, motif_border_style));
        con_set_border_style(con, motif_border_style, con->current_border_width);

        x_push_changes(global.croot);
    }

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
        DLOG(fmt::sprintf("Starting search at output %s\n",  output->name));
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
    DLOG(fmt::sprintf("floating change for con %p\n",  (void*)con));

    remanage_window(con);

    return true;
}

static bool handle_windowicon_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_icon(prop);

    x_push_changes(global.croot);

    return true;
}

/*
 * Sets the appropriate atoms for the property handlers after the atoms were
 * received from X11
 *
 */
PropertyHandlers::PropertyHandlers(X *x): x{x} {
    sn_monitor_context_new(sndisplay, x->conn->default_screen(), startup_monitor_event, nullptr, nullptr);

    property_handlers.emplace_back(A__NET_WM_NAME, 128, handle_windowname_change);
    property_handlers.emplace_back(XCB_ATOM_WM_HINTS, UINT_MAX, handle_hints);
    property_handlers.emplace_back(XCB_ATOM_WM_NAME, 128, handle_windowname_change_legacy);
    property_handlers.emplace_back(XCB_ATOM_WM_NORMAL_HINTS, UINT_MAX, handle_normal_hints);
    property_handlers.emplace_back(A_WM_CLIENT_LEADER, UINT_MAX, handle_clientleader_change);
    property_handlers.emplace_back(XCB_ATOM_WM_TRANSIENT_FOR, UINT_MAX, handle_transient_for);
    property_handlers.emplace_back(A_WM_WINDOW_ROLE, 128, handle_windowrole_change);
    property_handlers.emplace_back(XCB_ATOM_WM_CLASS, 128, handle_class_change);
    property_handlers.emplace_back(A__NET_WM_STRUT_PARTIAL, UINT_MAX, handle_strut_partial_change);
    property_handlers.emplace_back(A__NET_WM_WINDOW_TYPE, UINT_MAX, handle_window_type);
    property_handlers.emplace_back(A_I3_FLOATING_WINDOW, UINT_MAX, handle_i3_floating);
    property_handlers.emplace_back(XCB_ATOM_WM_CLIENT_MACHINE, 128, handle_machine_change);
    property_handlers.emplace_back(A__MOTIF_WM_HINTS, 5 * sizeof(uint64_t), handle_motif_hints_change);
    property_handlers.emplace_back(A__NET_WM_ICON, UINT_MAX, handle_windowicon_change);
}

void PropertyHandlers::property_notify(xcb_property_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    uint8_t state = event->state;
    xcb_window_t window = event->window;
    xcb_atom_t atom = event->atom;

    auto it = std::ranges::find_if(property_handlers, [&atom](auto &property_handler){ return property_handler.atom == atom; });

    if (it == property_handlers.end()) {
        // LOG(fmt::sprintf("Unhandled property notify for atom %d (0x%08x)\n",  atom, atom));
        return;
    }

    auto &handler = *it;

    if ((con = con_by_window_id(window)) == nullptr || con->window == nullptr) {
        DLOG(fmt::sprintf("Received property for atom %d for unknown client\n",  atom));
        return;
    }

    if (state != XCB_PROPERTY_DELETE) {
        try {
            auto propr = global.x->conn->get_property(0, window, atom, XCB_GET_PROPERTY_TYPE_ANY, 0, handler.long_len);

            handler.cb(con, propr.get().get());
        } catch (std::exception &e) {
            DLOG(fmt::sprintf("got error %s when getting property of atom %d\n",  e.what(), atom));
        }
    } else {
        handler.cb(con, nullptr);
    }
}

/*
 * There was a KeyPress or KeyRelease (both events have the same fields). We
 * compare this key code with our bindings table and pass the bound action to
 * parse_command().
 *
 */
void PropertyHandlers::handle_key_press(xcb_key_press_event_t *event) {
    const bool key_release = (event->response_type == XCB_KEY_RELEASE);

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("%s %d, state raw = 0x%x\n",  (key_release ? "KeyRelease" : "KeyPress"), event->detail, event->state));

    Binding *bind = get_binding_from_xcb_event((xcb_generic_event_t *)event);

    /* if we couldn't find a binding, we are done */
    if (bind == nullptr)
        return;

    run_binding(bind, nullptr);
}

/*
 * Takes an xcb_generic_event_t and calls the appropriate handler, based on the
 * event type.
 *
 */
void PropertyHandlers::handle_event(int type, xcb_generic_event_t *event) {
    x_connection *conn = *x;
    if (type != XCB_MOTION_NOTIFY)
        DLOG(fmt::sprintf("event type %d, xkb_base %d\n",  type, global.xkb->xkb_base));

    if (global.randr->randr_base > -1 &&
        type == global.randr->randr_base + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
        handle_screen_change(event);
        return;
    }

    if (global.xkb->xkb_base > -1 && type == global.xkb->xkb_base) {
        DLOG("xkb event, need to handle it.\n");

        auto *state = (xcb_xkb_state_notify_event_t *)event;
        if (state->xkbType == XCB_XKB_NEW_KEYBOARD_NOTIFY) {
            DLOG(fmt::sprintf("xkb new keyboard notify, sequence %d, time %d\n",  state->sequence, state->time));
            global.keysyms->key_symbols_alloc();
            if (((xcb_xkb_new_keyboard_notify_event_t *)event)->changed & XCB_XKB_NKN_DETAIL_KEYCODES) {
                load_keymap();
            }
            ungrab_all_keys(conn);
            translate_keysyms();
            grab_all_keys(conn);
        } else if (state->xkbType == XCB_XKB_MAP_NOTIFY) {
            if (event_is_ignored(event->sequence, type)) {
                DLOG(fmt::sprintf("Ignoring map notify event for sequence %d.\n",  state->sequence));
            } else {
                DLOG(fmt::sprintf("xkb map notify, sequence %d, time %d\n",  state->sequence, state->time));
                add_ignore_event(event->sequence, type);
                global.keysyms->key_symbols_alloc();
                ungrab_all_keys(conn);
                translate_keysyms();
                grab_all_keys(conn);
                load_keymap();
            }
        } else if (state->xkbType == XCB_XKB_STATE_NOTIFY) {
            DLOG(fmt::sprintf("xkb state group = %d\n",  state->group));
            if (global.xkb_current_group == state->group) {
                return;
            }
            global.xkb_current_group = state->group;
            ungrab_all_keys(conn);
            grab_all_keys(conn);
        }

        return;
    }

    if (global.shape->shape_supported && type == global.shape->shape_base + XCB_SHAPE_NOTIFY) {
        auto *shape = (xcb_shape_notify_event_t *)event;

        DLOG(fmt::sprintf("shape_notify_event for window 0x%08x, shape_kind = %d, shaped = %d\n",
             shape->affected_window, shape->shape_kind, shape->shaped));

        Con *con = con_by_window_id(shape->affected_window);
        if (con == nullptr) {
            LOG(fmt::sprintf("Not a managed window 0x%08x, ignoring shape_notify_event\n",
                shape->affected_window));
            return;
        }

        if (shape->shape_kind == XCB_SHAPE_SK_BOUNDING ||
            shape->shape_kind == XCB_SHAPE_SK_INPUT) {
            x_set_shape(con, (xcb_shape_sk_t)shape->shape_kind, shape->shaped);
        }

        return;
    }

    switch (type) {
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
            handle_key_press((xcb_key_press_event_t *)event);
            break;

        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            handle_button_press((xcb_button_press_event_t *)event);
            break;

        case XCB_MAP_REQUEST:
            handle_map_request((xcb_map_request_event_t *)event);
            break;

        case XCB_UNMAP_NOTIFY:
            handle_unmap_notify_event((xcb_unmap_notify_event_t *)event);
            break;

        case XCB_DESTROY_NOTIFY:
            handle_destroy_notify_event((xcb_destroy_notify_event_t *)event);
            break;

        case XCB_EXPOSE:
            handle_expose_event((xcb_expose_event_t *)event);
            break;

        case XCB_MOTION_NOTIFY:
            handle_motion_notify((xcb_motion_notify_event_t *)event);
            break;

        /* Enter window = user moved their mouse over the window */
        case XCB_ENTER_NOTIFY:
            handle_enter_notify((xcb_enter_notify_event_t *)event);
            break;

        /* Client message are sent to the root window. The only interesting
         * client message for us is _NET_WM_STATE, we honour
         * _NET_WM_STATE_FULLSCREEN and _NET_WM_STATE_DEMANDS_ATTENTION */
        case XCB_CLIENT_MESSAGE:
            handle_client_message((xcb_client_message_event_t *)event);
            break;

        /* Configure request = window tried to change size on its own */
        case XCB_CONFIGURE_REQUEST:
            handle_configure_request((xcb_configure_request_event_t *)event);
            break;

        /* Mapping notify = keyboard mapping changed (Xmodmap), re-grab bindings */
        case XCB_MAPPING_NOTIFY:
            handle_mapping_notify((xcb_mapping_notify_event_t *)event);
            break;

        case XCB_FOCUS_IN:
            handle_focus_in((xcb_focus_in_event_t *)event);
            break;

        case XCB_PROPERTY_NOTIFY: {
            property_notify((xcb_property_notify_event_t *)event);
            break;
        }

        case XCB_CONFIGURE_NOTIFY:
            handle_configure_notify((xcb_configure_notify_event_t *)event);
            break;

        default:
            // LOG(fmt::sprintf("Unhandled event of type %d\n",  type));
            break;
    }
}
