/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * sync.c: i3 sync protocol: https://i3wm.org/docs/testsuite.html#i3_sync
 *
 */
module;

#include <xcb/xcb.h>

export module i3:sync;

export void sync_respond(xcb_window_t window, uint32_t rnd);
