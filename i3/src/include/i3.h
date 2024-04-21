/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * i3.h: global variables that are used all over i3.
 *
 */
#pragma once

#include <ev.h>

#include <config.h>

#include <sys/time.h>
#include <queue>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>

/** Mouse buttons */
#define XCB_BUTTON_CLICK_LEFT XCB_BUTTON_INDEX_1
#define XCB_BUTTON_CLICK_MIDDLE XCB_BUTTON_INDEX_2
#define XCB_BUTTON_CLICK_RIGHT XCB_BUTTON_INDEX_3
#define XCB_BUTTON_SCROLL_UP XCB_BUTTON_INDEX_4
#define XCB_BUTTON_SCROLL_DOWN XCB_BUTTON_INDEX_5
/* xcb doesn't define constants for these. */
#define XCB_BUTTON_SCROLL_LEFT 6
#define XCB_BUTTON_SCROLL_RIGHT 7

/**
 * The EWMH support window that is used to indicate that an EWMH-compliant
 * window manager is present. This window is created when i3 starts and
 * kept alive until i3 exits.
 * We also use this window as the focused window if no other window is
 * available to be focused on the active workspace in order to prevent
 * keyboard focus issues (see #1378).
 */
extern xcb_window_t ewmh_window;
extern SnDisplay *sndisplay;

//extern struct ev_loop *main_loop;
