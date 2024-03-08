/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * tiling_drag.h: Reposition tiled windows by dragging.
 *
 */
module;
class Con;
#include <xcb/xproto.h>
export module i3:tiling_drag;

export {

    /**
     * Returns whether there currently are any drop targets.
     * Used to only initiate a drag when there is something to drop onto.
     *
     */
    bool has_drop_targets();

    /**
     * Initiates a mouse drag operation on a tiled window.
     *
     */
    void tiling_drag(Con * con, xcb_button_press_event_t * event, bool use_threshold);
}