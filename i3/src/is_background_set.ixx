/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <stdlib.h>
#include <cairo.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
export module i3:is_background_set;

import std;
import :x;

/**
 * Find the region in the given window that is not covered by a mapped child
 * window.
 */
static cairo_region_t *unobscured_region(x_connection *conn, xcb_window_t window,
                                         uint16_t window_width, uint16_t window_height) {
    cairo_rectangle_int_t rectangle;
    cairo_region_t *region;

    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = window_width;
    rectangle.height = window_height;
    region = cairo_region_create_rectangle(&rectangle);

    xcb_query_tree_reply_t *tree = xcb_query_tree_reply(*conn, xcb_query_tree_unchecked(*conn, window), nullptr);
    if (!tree) {
        return region;
    }

    /* Get information about children */
    const uint16_t n_children = tree->children_len;
    xcb_window_t *children = xcb_query_tree_children(tree);

    std::vector<xcb_get_geometry_cookie_t> geometries{};
    geometries.reserve(n_children);
    std::vector<xcb_get_window_attributes_cookie_t> attributes{};
    attributes.reserve(n_children);

    for (int i = 0; i < n_children; i++) {
        geometries.push_back(xcb_get_geometry_unchecked(*conn, children[i]));
        attributes.push_back(xcb_get_window_attributes_unchecked(*conn, children[i]));
    }

    /* Remove every visible child from the region */
    for (int i = 0; i < n_children; i++) {
        xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(*conn, geometries.at(i), nullptr);
        xcb_get_window_attributes_reply_t *attr = xcb_get_window_attributes_reply(*conn, attributes.at(i), nullptr);

        if (geom && attr && attr->map_state == XCB_MAP_STATE_VIEWABLE) {
            rectangle.x = geom->x;
            rectangle.y = geom->y;
            rectangle.width = geom->width;
            rectangle.height = geom->height;
            cairo_region_subtract_rectangle(region, &rectangle);
        }

        free(geom);
        free(attr);
    }

    free(tree);
    return region;
}

static void find_unobscured_pixel(x_connection *conn, xcb_window_t window,
                                  uint16_t window_width, uint16_t window_height,
                                  uint16_t *x, uint16_t *y) {
    cairo_region_t *region = unobscured_region(conn, window, window_width, window_height);
    if (cairo_region_num_rectangles(region) > 0) {
        /* Return the top left pixel of the first rectangle */
        cairo_rectangle_int_t rect;
        cairo_region_get_rectangle(region, 0, &rect);
        *x = rect.x;
        *y = rect.y;
    } else {
        /* No unobscured area found */
        *x = 0;
        *y = 0;
    }
    cairo_region_destroy(region);
}

static uint32_t flicker_window_at(x_connection *conn, xcb_screen_t *screen, uint16_t x, uint16_t y, xcb_window_t window,
                                  uint32_t pixel) {
    uint32_t value_list[]{pixel, 1};
    conn->create_window(XCB_COPY_FROM_PARENT, window, screen->root, x, y, 10, 10,
                        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
                        XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT, value_list);
    conn->map_window(window);
    conn->clear_area(0, window, 0, 0, 0, 0);
    xcb_aux_sync(*conn);
    conn->destroy_window(window);

    auto img = conn->get_image_unchecked(XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root, x, y, 1, 1, ~0);
    uint32_t result = 0;
    if (img) {
        uint8_t *data = xcb_get_image_data(img.get().get());
        uint8_t depth = img->depth;
        for (int i = 0; i < std::min(depth, (uint8_t)4); i++) {
            result = (result << 8) | data[i];
        }
    }
    return result;
}

export bool is_background_set(x_connection *conn, xcb_screen_t *screen) {
    uint16_t x, y;
    find_unobscured_pixel(conn, screen->root, screen->width_in_pixels, screen->height_in_pixels, &x, &y);

    xcb_window_t window = conn->generate_id();

    uint32_t pixel1 = flicker_window_at(conn, screen, x, y, window, screen->black_pixel);
    uint32_t pixel2 = flicker_window_at(conn, screen, x, y, window, screen->white_pixel);
    return pixel1 == pixel2;
}
