/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * manage.c: Initially managing new windows (or existing ones on restart).
 *
 */
module;
#include <config.h>
#include <xcb/xproto.h>
export module i3:manage;

import :internal;

class Con;

export {
    /**
     * Go through all existing windows (if the window manager is restarted) and
     * manage them
     *
     */
    void manage_existing_windows(xcb_window_t root);

    /**
     * Restores the geometry of each window by reparenting it to the root window
     * at the position of its frame.
     *
     * This is to be called *only* before exiting/restarting i3 because of evil
     * side-effects which are to be expected when continuing to run i3.
     *
     */
    void restore_geometry();

    /**
     * Do some sanity checks and then reparent the window.
     *
     */
    void manage_window(xcb_window_t window,
                       xcb_get_window_attributes_reply_t * attr,
                       bool needs_to_be_mapped);

    /**
     * Remanages a window: performs a swallow check and runs assignments.
     * Returns con for the window regardless if it updated.
     *
     */
    Con *remanage_window(Con * con);

    void update_motif_hints(xcb_get_property_reply_t * prop, border_style_t * motif_border_style);
}