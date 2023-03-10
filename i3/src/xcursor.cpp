/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * xcursor.c: xcursor support for themed cursors.
 *
 */
#include <cassert>
#include <err.h>
#include <cstdint>
#include <cstdlib>

#include <xcb/xcb.h>
#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"
#include "i3.h"
#include "xcursor.h"
#include "global.h"

#include <xcb/xcb_cursor.h>

static xcb_cursor_context_t *ctx;
static std::map<xcursor_cursor_t, xcb_cursor_t> cursors{};

void xcursor_load_cursors() {
    if (xcb_cursor_context_new(global.conn, global.root_screen, &ctx) < 0) {
        errx(EXIT_FAILURE, "Cannot allocate xcursor context");
    }
    cursors[XCURSOR_CURSOR_POINTER] = xcb_cursor_load_cursor(ctx, "left_ptr");;
    cursors[XCURSOR_CURSOR_RESIZE_HORIZONTAL] = xcb_cursor_load_cursor(ctx, "sb_h_double_arrow");;
    cursors[XCURSOR_CURSOR_RESIZE_VERTICAL] = xcb_cursor_load_cursor(ctx, "sb_v_double_arrow");;
    cursors[XCURSOR_CURSOR_WATCH] = xcb_cursor_load_cursor(ctx, "watch");;
    cursors[XCURSOR_CURSOR_MOVE] = xcb_cursor_load_cursor(ctx, "fleur");;
    cursors[XCURSOR_CURSOR_TOP_LEFT_CORNER] = xcb_cursor_load_cursor(ctx, "top_left_corner");;
    cursors[XCURSOR_CURSOR_TOP_RIGHT_CORNER] = xcb_cursor_load_cursor(ctx, "top_right_corner");;
    cursors[XCURSOR_CURSOR_BOTTOM_LEFT_CORNER] = xcb_cursor_load_cursor(ctx, "bottom_left_corner");;
    cursors[XCURSOR_CURSOR_BOTTOM_RIGHT_CORNER] = xcb_cursor_load_cursor(ctx, "bottom_right_corner");;
}

/*
 * Sets the cursor of the root window to the 'pointer' cursor.
 *
 * This function is called when i3 is initialized, because with some login
 * managers, the root window will not have a cursor otherwise.
 *
 */
void xcursor_set_root_cursor(xcursor_cursor_t cursor_id) {
    uint32_t value_list[]{xcursor_get_cursor(cursor_id)};
    xcb_change_window_attributes(global.conn, root, XCB_CW_CURSOR,
                                 value_list);
}

xcb_cursor_t xcursor_get_cursor(xcursor_cursor_t c) {
    return cursors.at(c);
}
