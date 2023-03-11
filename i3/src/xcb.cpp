/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * xcb.c: Helper functions for easier usage of XCB
 *
 */

#include <cstdint>
#include <cstdlib>

#include <xcb/xcb.h>
#include <main.h>
#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"
#include "util.h"
#include "xcb.h"
#include "i3.h"
#include "configuration.h"
#include "handlers.h"
#include "xcursor.h"
#include "global.h"

unsigned int xcb_numlock_mask;

/*
 * Convenience wrapper around xcb_create_window which takes care of depth, generating an ID and checking
 * for errors.
 *
 */
xcb_window_t create_window(xcb_connection_t *conn, Rect dims,
                           uint16_t depth, xcb_visualid_t visual, uint16_t window_class,
                           enum xcursor_cursor_t cursor, bool map, uint32_t mask, uint32_t *values) {
    xcb_window_t result = xcb_generate_id(conn);

    /* If the window class is XCB_WINDOW_CLASS_INPUT_ONLY, we copy depth and
     * visual id from the parent window. */
    if (window_class == XCB_WINDOW_CLASS_INPUT_ONLY) {
        depth = XCB_COPY_FROM_PARENT;
        visual = XCB_COPY_FROM_PARENT;
    }

    xcb_void_cookie_t gc_cookie = xcb_create_window(conn,
                                                    depth,
                                                    result,                                  /* the window id */
                                                    root,                                    /* parent == root */
                                                    dims.x, dims.y, dims.width, dims.height, /* dimensions */
                                                    0,                                       /* border = 0, we draw our own */
                                                    window_class,
                                                    visual,
                                                    mask,
                                                    values);

    xcb_generic_error_t *error = xcb_request_check(conn, gc_cookie);
    if (error != nullptr) {
        ELOG(fmt::sprintf("Could not create window. Error code: %d.\n",  error->error_code));
    }

    /* Set the cursor */
    uint32_t cursor_values[] = {xcursor_get_cursor(cursor)};
    xcb_change_window_attributes(conn, result, XCB_CW_CURSOR, cursor_values);

    /* Map the window (= make it visible) */
    if (map)
        xcb_map_window(conn, result);

    return result;
}

/*
 * Generates a configure_notify_event with absolute coordinates (relative to the X root
 * window, not to the client’s frame) for the given client.
 *
 */
void fake_absolute_configure_notify(Con *con) {
    if (con->window == nullptr)
        return;

    /* Every X11 event is 32 bytes long. Therefore, XCB will copy 32 bytes.
     * In order to properly initialize these bytes, we allocate 32 bytes even
     * though we only need less for an xcb_configure_notify_event_t */
    xcb_configure_notify_event_t generatedEvent{};

    generatedEvent.event = con->window->id;
    generatedEvent.window = con->window->id;
    generatedEvent.response_type = XCB_CONFIGURE_NOTIFY;

    generatedEvent.x = static_cast<int16_t>(con->rect.x + con->window_rect.x);
    generatedEvent.y = static_cast<int16_t>(con->rect.y + con->window_rect.y);
    generatedEvent.width = static_cast<uint16_t>(con->window_rect.width);
    generatedEvent.height = static_cast<uint16_t>(con->window_rect.height);

    generatedEvent.border_width = con->border_width;
    generatedEvent.above_sibling = XCB_NONE;
    generatedEvent.override_redirect = false;

    DLOG(fmt::sprintf("fake rect = (%d, %d, %d, %d)\n", generatedEvent.x, generatedEvent.y, generatedEvent.width, generatedEvent.height));

    xcb_send_event(*global.a, false, con->window->id, XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char *) &generatedEvent);
    xcb_flush(*global.a);
}

/*
 * Sends the WM_TAKE_FOCUS ClientMessage to the given window
 *
 */
void send_take_focus(xcb_window_t window, xcb_timestamp_t timestamp) {
    /* Every X11 event is 32 bytes long. Therefore, XCB will copy 32 bytes.
     * In order to properly initialize these bytes, we allocate 32 bytes even
     * though we only need less for an xcb_configure_notify_event_t */
    void *event = scalloc(32, 1);
    auto *ev = (xcb_client_message_event_t*)event;

    ev->response_type = XCB_CLIENT_MESSAGE;
    ev->window = window;
    ev->type = A_WM_PROTOCOLS;
    ev->format = 32;
    ev->data.data32[0] = A_WM_TAKE_FOCUS;
    ev->data.data32[1] = timestamp;

    DLOG("Sending WM_TAKE_FOCUS to the client\n");
    xcb_send_event(*global.a, false, window, XCB_EVENT_MASK_NO_EVENT, (char *)ev);
    free(event);
}

/*
 * Configures the given window to have the size/position specified by given rect
 *
 */
void xcb_set_window_rect(xcb_connection_t *conn, xcb_window_t window, Rect r) {
    xcb_void_cookie_t cookie;
    cookie = xcb_configure_window(conn, window,
                                  XCB_CONFIG_WINDOW_X |
                                      XCB_CONFIG_WINDOW_Y |
                                      XCB_CONFIG_WINDOW_WIDTH |
                                      XCB_CONFIG_WINDOW_HEIGHT,
                                  &(r.x));
    /* ignore events which are generated because we configured a window */
    add_ignore_event(cookie.sequence, -1);
}

/*
 * Returns the first supported _NET_WM_WINDOW_TYPE atom.
 *
 */
xcb_atom_t xcb_get_preferred_window_type(xcb_get_property_reply_t *reply) {
    if (reply == nullptr || xcb_get_property_value_length(reply) == 0)
        return XCB_NONE;

    xcb_atom_t *atoms;
    if ((atoms = (xcb_atom_t*)xcb_get_property_value(reply)) == nullptr)
        return XCB_NONE;

    for (int i = 0; i < xcb_get_property_value_length(reply) / (reply->format / 8); i++) {
        xcb_atom_t atom = atoms[i];
        if (atom == A__NET_WM_WINDOW_TYPE_NORMAL ||
            atom == A__NET_WM_WINDOW_TYPE_DIALOG ||
            atom == A__NET_WM_WINDOW_TYPE_UTILITY ||
            atom == A__NET_WM_WINDOW_TYPE_TOOLBAR ||
            atom == A__NET_WM_WINDOW_TYPE_SPLASH ||
            atom == A__NET_WM_WINDOW_TYPE_MENU ||
            atom == A__NET_WM_WINDOW_TYPE_DROPDOWN_MENU ||
            atom == A__NET_WM_WINDOW_TYPE_POPUP_MENU ||
            atom == A__NET_WM_WINDOW_TYPE_TOOLTIP ||
            atom == A__NET_WM_WINDOW_TYPE_NOTIFICATION) {
            return atom;
        }
    }

    return XCB_NONE;
}

/*
 * Returns true if the given reply contains the given atom.
 *
 */
bool xcb_reply_contains_atom(xcb_get_property_reply_t *prop, xcb_atom_t atom) {
    if (prop == nullptr || xcb_get_property_value_length(prop) == 0)
        return false;

    xcb_atom_t *atoms;
    if ((atoms = (xcb_atom_t*)xcb_get_property_value(prop)) == nullptr)
        return false;

    for (int i = 0; i < xcb_get_property_value_length(prop) / (prop->format / 8); i++)
        if (atoms[i] == atom)
            return true;

    return false;
}

/*
 * Get depth of visual specified by visualid
 *
 */
uint16_t get_visual_depth(xcb_visualid_t visual_id) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(global.root_screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (visual_id == visual_iter.data->visual_id) {
                return depth_iter.data->depth;
            }
        }
    }
    return 0;
}

/*
 * Get visual type specified by visualid
 *
 */
xcb_visualtype_t *get_visualtype_by_id(xcb_visualid_t visual_id) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(global.root_screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (visual_id == visual_iter.data->visual_id) {
                return visual_iter.data;
            }
        }
    }
    return nullptr;
}

/*
 * Get visualid with specified depth
 *
 */
xcb_visualid_t get_visualid_by_depth(uint16_t depth) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(global.root_screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        if (depth_iter.data->depth != depth)
            continue;

        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        if (!visual_iter.rem)
            continue;
        return visual_iter.data->visual_id;
    }
    return 0;
}

/*
 * Add an atom to a list of atoms the given property defines.
 * This is useful, for example, for manipulating _NET_WM_STATE.
 *
 */
void xcb_add_property_atom(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom) {
    uint32_t data[]{atom};
    xcb_change_property(conn, XCB_PROP_MODE_APPEND, window, property, XCB_ATOM_ATOM, 32, 1, data);
}

/*
 * Remove an atom from a list of atoms the given property defines without
 * removing any other potentially set atoms.  This is useful, for example, for
 * manipulating _NET_WM_STATE.
 *
 */
void xcb_remove_property_atom(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom) {
    xcb_grab_server(conn);

    xcb_get_property_reply_t *reply =
            xcb_get_property_reply(conn,
                                   xcb_get_property(conn, false, window, property, XCB_GET_PROPERTY_TYPE_ANY, 0, 4096),
                                 nullptr);
    if (reply == nullptr || xcb_get_property_value_length(reply) == 0) {
        FREE(reply);
        xcb_ungrab_server(conn);
        return;
    }
    auto *atoms = (xcb_atom_t*)xcb_get_property_value(reply);
    if (atoms == nullptr) {
        goto release_grab;
    }

    {
        int num = 0;
        const int current_size = xcb_get_property_value_length(reply) / (reply->format / 8);
        xcb_atom_t values[current_size];
        for (int i = 0; i < current_size; i++) {
            if (atoms[i] != atom)
                values[num++] = atoms[i];
        }

        xcb_change_property(conn, XCB_PROP_MODE_REPLACE, window, property, XCB_ATOM_ATOM, 32, num, values);
    }

release_grab:
    FREE(reply);
    xcb_ungrab_server(conn);
}

/*
 * Grab the specified buttons on a window when managing it.
 *
 */
void xcb_grab_buttons(xcb_window_t window, std::set<int> &buttons) {
    for (int i : buttons) {
        global.a->grab_button(false, window, XCB_EVENT_MASK_BUTTON_PRESS, XCB_GRAB_MODE_SYNC,
                        XCB_GRAB_MODE_ASYNC, root, XCB_NONE, i, XCB_BUTTON_MASK_ANY);
    }
}
