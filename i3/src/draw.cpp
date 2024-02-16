/*
 * vim:ts=4:sw=4:expandtab
 *
 * © 2015 Ingo Bürk and contributors (see also: LICENSE)
 *
 * draw.c: Utility for drawing.
 *
 */
module;
#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>
#include <deque>

#include <fmt/printf.h>
#include <cairo.h>
#include <cairo-xcb.h>
module i3;

import utils;
import log;

/* Forward declarations */
static void draw_util_set_source_color(surface_t *surface, color_t color);

/* We need to flush cairo surfaces twice to avoid an assertion bug. See #1989
 * and https://bugs.freedesktop.org/show_bug.cgi?id=92455. */
static void CAIRO_SURFACE_FLUSH(_cairo_surface *surface) {
    cairo_surface_flush(surface);
    cairo_surface_flush(surface);
}

struct Colorpixel {
    char hex[8];
    uint32_t pixel;
};

static std::deque<std::unique_ptr<Colorpixel>> colorpixels{};

static uint8_t RGB_8_TO_16(uint8_t i) {
    return (65535 * ((i) & 0xFF) / 255);
}

/*
 * Returns the colorpixel to use for the given hex color (think of HTML).
 *
 * The hex_color has to start with #, for example #FF00FF.
 *
 * NOTE that get_colorpixel() does _NOT_ check the given color code for validity.
 * This has to be done by the caller.
 *
 */
static uint32_t get_colorpixel(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *hex) {
    char strgroups[3][3] = {
        {hex[1], hex[2], '\0'},
        {hex[3], hex[4], '\0'},
        {hex[5], hex[6], '\0'}};
    uint8_t r = strtol(strgroups[0], nullptr, 16);
    uint8_t g = strtol(strgroups[1], nullptr, 16);
    uint8_t b = strtol(strgroups[2], nullptr, 16);

    /* Shortcut: if our screen is true color, no need to do a roundtrip to X11 */
    if (root_screen == nullptr || root_screen->root_depth == 24 || root_screen->root_depth == 32) {
        return (0xFFUL << 24) | (r << 16 | g << 8 | b);
    }

    /* Lookup this colorpixel in the cache */
    for (const auto &colorpixel : colorpixels) {
        if (strcmp(colorpixel->hex, hex) == 0) {
            return colorpixel->pixel;
        }
    }

    int r16 = RGB_8_TO_16(r);
    int g16 = RGB_8_TO_16(g);
    int b16 = RGB_8_TO_16(b);

    xcb_alloc_color_reply_t *reply;

    reply = xcb_alloc_color_reply(conn, xcb_alloc_color(conn, root_screen->default_colormap, r16, g16, b16),
                                  nullptr);

    if (!reply) {
        LOG("Could not allocate color\n");
        exit(1);
    }

    uint32_t pixel = reply->pixel;
    free(reply);

    /* Store the result in the cache */
    auto cache_pixel = std::make_unique<Colorpixel>();

    strncpy(cache_pixel->hex, hex, 7);
    cache_pixel->hex[7] = '\0';

    cache_pixel->pixel = pixel;

    colorpixels.push_front(std::move(cache_pixel));

    return pixel;
}

/*
 * Initialize the surface to represent the given drawable.
 *
 */
void draw_util_surface_init(xcb_connection_t *conn, surface_t *surface, xcb_drawable_t drawable,
                            xcb_visualtype_t *visual, int width, int height) {
    surface->id = drawable;
    surface->width = width;
    surface->height = height;

    if (visual == nullptr) {
        visual = global.x->visual_type;
    }

    surface->gc = xcb_generate_id(conn);
    xcb_void_cookie_t gc_cookie = xcb_create_gc_checked(conn, surface->gc, surface->id, 0, nullptr);

    xcb_generic_error_t *error = xcb_request_check(conn, gc_cookie);
    if (error != nullptr) {
        ELOG(fmt::sprintf("Could not create graphical context. Error code: %d. Please report this bug.\n", error->error_code));
    }

    surface->surface = cairo_xcb_surface_create(conn, surface->id, visual, width, height);
    surface->cr = cairo_create(surface->surface);
}

/*
 * Destroys the surface.
 *
 */
void draw_util_surface_free(xcb_connection_t *conn, surface_t *surface) {
    xcb_free_gc(conn, surface->gc);
    cairo_surface_destroy(surface->surface);
    cairo_destroy(surface->cr);

    /* We need to explicitly set these to NULL to avoid assertion errors in
     * cairo when calling this multiple times. This can happen, for example,
     * when setting the border of a window to none and then closing it. */
    surface->surface = nullptr;
    surface->cr = nullptr;
}

/*
 * Resize the surface to the given size.
 *
 */
void draw_util_surface_set_size(surface_t *surface, int width, int height) {
    surface->width = width;
    surface->height = height;
    cairo_xcb_surface_set_size(surface->surface, width, height);
}

/*
 * Parses the given color in hex format to an internal color representation.
 * Note that the input must begin with a hash sign, e.g., "#3fbc59".
 *
 */
color_t draw_util_hex_to_color(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *color) {
    if (strlen(color) < 6 || color[0] != '#') {
        ELOG(fmt::sprintf("Could not parse color: %s\n", color));
        return draw_util_hex_to_color(conn, root_screen, "#A9A9A9");
    }

    char alpha[2];
    if (strlen(color) == strlen("#rrggbbaa")) {
        alpha[0] = color[7];
        alpha[1] = color[8];
    } else {
        alpha[0] = alpha[1] = 'F';
    }

    char groups[4][3] = {
        {color[1], color[2], '\0'},
        {color[3], color[4], '\0'},
        {color[5], color[6], '\0'},
        {alpha[0], alpha[1], '\0'}};

    return (color_t){
        .red = strtol(groups[0], nullptr, 16) / 255.0,
        .green = strtol(groups[1], nullptr, 16) / 255.0,
        .blue = strtol(groups[2], nullptr, 16) / 255.0,
        .alpha = strtol(groups[3], nullptr, 16) / 255.0,
        .colorpixel = get_colorpixel(conn, root_screen, color)};
}

/*
 * Set the given color as the source color on the surface.
 *
 */
static void draw_util_set_source_color(surface_t *surface, color_t color) {
    if ((surface)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    cairo_set_source_rgba(surface->cr, color.red, color.green, color.blue, color.alpha);
}

/*
 * Draw the given text using libi3.
 * This function also marks the surface dirty which is needed if other means of
 * drawing are used. This will be the case when using XCB to draw text.
 *
 */
void draw_util_text(xcb_connection_t *conn, i3Font *font, i3String *text, surface_t *surface, color_t fg_color, color_t bg_color, int x, int y, int max_width) {
    if ((surface)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    /* Flush any changes before we draw the text as this might use XCB directly. */
    CAIRO_SURFACE_FLUSH(surface->surface);

    set_font_colors(conn, surface->gc, fg_color, bg_color);
    draw_text(font, conn, text, surface->id, surface->gc, surface->surface, x, y, max_width);

    /* Notify cairo that we (possibly) used another way to draw on the surface. */
    cairo_surface_mark_dirty(surface->surface);
}

/*
 * Draw the given text using libi3.
 * This function also marks the surface dirty which is needed if other means of
 * drawing are used. This will be the case when using XCB to draw text.
 *
 */
void draw_util_text(xcb_connection_t *conn, i3Font *font, std::string &text, surface_t *surface, color_t fg_color, color_t bg_color, int x, int y, int max_width) {
    if ((surface)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    /* Flush any changes before we draw the text as this might use XCB directly. */
    CAIRO_SURFACE_FLUSH(surface->surface);

    set_font_colors(conn, surface->gc, fg_color, bg_color);
    draw_text(font, conn, text, surface->id, surface->gc, surface->surface, x, y, max_width);

    /* Notify cairo that we (possibly) used another way to draw on the surface. */
    cairo_surface_mark_dirty(surface->surface);
}

/*
 * Draws a filled rectangle.
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
void draw_util_rectangle(surface_t *surface, color_t color, double x, double y, double w, double h) {
    if ((surface)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    cairo_save(surface->cr);

    /* Using the SOURCE operator will copy both color and alpha information directly
     * onto the surface rather than blending it. This is a bit more efficient and
     * allows better color control for the user when using opacity. */
    cairo_set_operator(surface->cr, CAIRO_OPERATOR_SOURCE);
    draw_util_set_source_color(surface, color);

    cairo_rectangle(surface->cr, x, y, w, h);
    cairo_fill(surface->cr);

    /* Make sure we flush the surface for any text drawing operations that could follow.
     * Since we support drawing text via XCB, we need this. */
    CAIRO_SURFACE_FLUSH(surface->surface);

    cairo_restore(surface->cr);
}

/*
 * Clears a surface with the given color.
 *
 */
void draw_util_clear_surface(surface_t *surface, color_t color) {
    if ((surface)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    cairo_save(surface->cr);

    /* Using the SOURCE operator will copy both color and alpha information directly
     * onto the surface rather than blending it. This is a bit more efficient and
     * allows better color control for the user when using opacity. */
    cairo_set_operator(surface->cr, CAIRO_OPERATOR_SOURCE);
    draw_util_set_source_color(surface, color);

    cairo_paint(surface->cr);

    /* Make sure we flush the surface for any text drawing operations that could follow.
     * Since we support drawing text via XCB, we need this. */
    CAIRO_SURFACE_FLUSH(surface->surface);

    cairo_restore(surface->cr);
}

/*
 * Copies a surface onto another surface.
 *
 */
void draw_util_copy_surface(surface_t *src, surface_t *dest, double src_x, double src_y,
                            double dest_x, double dest_y, double width, double height) {
    if ((src)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)src));
        return;
    }
    if ((dest)->id == 0L) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)dest));
        return;
    }

    cairo_save(dest->cr);

    /* Using the SOURCE operator will copy both color and alpha information directly
     * onto the surface rather than blending it. This is a bit more efficient and
     * allows better color control for the user when using opacity. */
    cairo_set_operator(dest->cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(dest->cr, src->surface, dest_x - src_x, dest_y - src_y);

    cairo_rectangle(dest->cr, dest_x, dest_y, width, height);
    cairo_fill(dest->cr);

    /* Make sure we flush the surface for any text drawing operations that could follow.
     * Since we support drawing text via XCB, we need this. */
    CAIRO_SURFACE_FLUSH(src->surface);
    CAIRO_SURFACE_FLUSH(dest->surface);

    cairo_restore(dest->cr);
}

/**
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
void draw_util_image(cairo_surface_t *image, surface_t *surface, int x, int y, int width, int height) {
    if ((surface)->id == XCB_NONE) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", (void *)surface));
        return;
    }

    cairo_save(surface->cr);

    cairo_translate(surface->cr, x, y);

    const int src_width = cairo_image_surface_get_width(image);
    const int src_height = cairo_image_surface_get_height(image);
    double scale = std::min((double)width / src_width, (double)height / src_height);
    cairo_scale(surface->cr, scale, scale);

    cairo_set_source_surface(surface->cr, image, 0, 0);
    cairo_paint(surface->cr);

    cairo_restore(surface->cr);
}
