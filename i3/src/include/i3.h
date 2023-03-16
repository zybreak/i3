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

#include <config.h>

#include <sys/time.h>
#include <queue>
#include <memory>
#include <sys/resource.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <X11/XKBlib.h>

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>

#include "bindings.h"
#include "xcb.h"
#include "workspace.h"

#define DEFAULT_DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

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
extern xcb_key_symbols_t *keysyms;
extern char **start_argv;
extern int xkb_current_group;

extern bool xkb_supported, shape_supported;
extern struct ev_loop *main_loop;

/**
 * Puts the given socket file descriptor into non-blocking mode or dies if
 * setting O_NONBLOCK failed. Non-blocking sockets are a good idea for our
 * IPC model because we should by no means block the window manager.
 *
 */
void set_nonblock(int sockfd);

/**
 * Reports whether str represents the enabled state (1, yes, true, …).
 *
 */
bool boolstr(const char *str);
