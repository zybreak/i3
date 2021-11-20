/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
#include "libi3.h"

void set_screenshot_as_wallpaper(xcb_connection_t *conn, xcb_screen_t *screen) {
    uint16_t width = screen->width_in_pixels;
    uint16_t height = screen->height_in_pixels;
    xcb_pixmap_t pixmap = xcb_generate_id(conn);
    xcb_gcontext_t gc = xcb_generate_id(conn);

    xcb_create_pixmap(conn, screen->root_depth, pixmap, screen->root, width, height);

    uint32_t list[]{XCB_GX_COPY, (uint32_t)~0, XCB_FILL_STYLE_SOLID, XCB_SUBWINDOW_MODE_INCLUDE_INFERIORS};
    xcb_create_gc(conn, gc, screen->root,
                  XCB_GC_FUNCTION | XCB_GC_PLANE_MASK | XCB_GC_FILL_STYLE | XCB_GC_SUBWINDOW_MODE,
                  list);

    xcb_copy_area(conn, screen->root, pixmap, gc, 0, 0, 0, 0, width, height);
    uint32_t value_list[]{pixmap};
    xcb_change_window_attributes(conn, screen->root, XCB_CW_BACK_PIXMAP, value_list);
    xcb_free_gc(conn, gc);
    xcb_free_pixmap(conn, pixmap);
    xcb_flush(conn);
}
