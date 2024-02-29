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

/**
 * Initiates a mouse drag operation on a tiled window.
 *
 */
export void tiling_drag(Con *con, xcb_button_press_event_t *event);
