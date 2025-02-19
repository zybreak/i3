/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * xcb.c: Helper functions for easier usage of XCB
 *
 */
module;
class Con;

#include <config.h>
#include <xcb/xcb.h>
export module i3:xcb;

import std;
import rect;
import :internal;

/* from X11/keysymdef.h */
export uint32_t const XCB_NUM_LOCK = 0xff7f;

/* The event masks are defined here because we don’t only set them once but we
   need to set slight variations of them (without XCB_EVENT_MASK_ENTER_WINDOW
   while rendering the layout) */
/** The XCB_CW_EVENT_MASK for the child (= real window) */
export uint32_t const CHILD_EVENT_MASK = (XCB_EVENT_MASK_PROPERTY_CHANGE |
                                          XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                          XCB_EVENT_MASK_FOCUS_CHANGE);

/** The XCB_CW_EVENT_MASK for its frame */
export uint32_t const FRAME_EVENT_MASK = (XCB_EVENT_MASK_BUTTON_PRESS |          /* …mouse is pressed/released */
                                          XCB_EVENT_MASK_BUTTON_RELEASE |
                                          XCB_EVENT_MASK_POINTER_MOTION |        /* …mouse is moved */
                                          XCB_EVENT_MASK_EXPOSURE |              /* …our window needs to be redrawn */
                                          XCB_EVENT_MASK_STRUCTURE_NOTIFY |      /* …the frame gets destroyed */
                                          XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | /* …the application tries to resize itself */
                                          XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |   /* …subwindows get notifies */
                                          XCB_EVENT_MASK_ENTER_WINDOW);          /* …user moves cursor inside our window */

export uint32_t const ROOT_EVENT_MASK = (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                                         XCB_EVENT_MASK_BUTTON_PRESS |
                                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | /* when the user adds a screen (e.g. video \
                                                                            * projector), the root window gets a      \
                                                                            * ConfigureNotify */
                                         XCB_EVENT_MASK_POINTER_MOTION |
                                         XCB_EVENT_MASK_PROPERTY_CHANGE |
                                         XCB_EVENT_MASK_FOCUS_CHANGE |
                                         XCB_EVENT_MASK_ENTER_WINDOW);

export {
    /**
     * Convenience wrapper around xcb_create_window which takes care of depth,
     * generating an ID and checking for errors.
     *
     */
    xcb_window_t create_window(xcb_connection_t * conn, Rect dims,
                               uint16_t depth, xcb_visualid_t visual, uint16_t window_class,
                               enum xcursor_cursor_t cursor, bool map, uint32_t mask, uint32_t *values);

    /**
     * Generates a configure_notify_event with absolute coordinates (relative to
     * the X root window, not to the client’s frame) for the given client.
     *
     */
    void fake_absolute_configure_notify(Con * con);

    /**
     * Sends the WM_TAKE_FOCUS ClientMessage to the given window
     *
     */
    void send_take_focus(xcb_window_t window, xcb_timestamp_t timestamp);

    /**
     * Configures the given window to have the size/position specified by given rect
     *
     */
    void xcb_set_window_rect(xcb_connection_t * conn, xcb_window_t window, Rect r);

    /**
     * Returns the first supported _NET_WM_WINDOW_TYPE atom.
     *
     */
    xcb_atom_t xcb_get_preferred_window_type(xcb_get_property_reply_t * reply);

    /**
     * Returns true if the given reply contains the given data.
     *
     */
    bool xcb_reply_contains_atom(xcb_get_property_reply_t * prop, xcb_atom_t atom);

    /**
     * Get depth of visual specified by visualid
     *
     */
    uint16_t get_visual_depth(xcb_visualid_t visual_id);

    /**
     * Get visual type specified by visualid
     *
     */
    xcb_visualtype_t *get_visualtype_by_id(xcb_visualid_t visual_id);

    /**
     * Get visualid with specified depth
     *
     */
    xcb_visualid_t get_visualid_by_depth(uint16_t depth);

    /**
     * Add an atom to a list of atoms the given property defines.
     * This is useful, for example, for manipulating _NET_WM_STATE.
     *
     */
    void xcb_add_property_atom(xcb_connection_t * conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom);

    /**
     * Remove an atom from a list of atoms the given property defines without
     * removing any other potentially set atoms.  This is useful, for example, for
     * manipulating _NET_WM_STATE.
     *
     */
    void xcb_remove_property_atom(xcb_connection_t * conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom);

    /**
     * Grab the specified buttons on a window when managing it.
     *
     */
    void xcb_grab_buttons(xcb_window_t window, std::set<int> & buttons);
}
