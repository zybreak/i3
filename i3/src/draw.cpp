/*
 * vim:ts=4:sw=4:expandtab
 *
 * © 2015 Ingo Bürk and contributors (see also: LICENSE)
 *
 * draw.c: Utility for drawing.
 *
 */
module;
#include <xcb/xcb.h>

#include <fmt/printf.h>
#include <cairo.h>
#include <cairo-xcb.h>
module i3;

import std;
import utils;
import log;

/* Forward declarations */
static void draw_util_set_source_color(surface_t *surface, color_t color);

/* We need to flush cairo surfaces twice to avoid an assertion bug. See #1989
 * and https://bugs.freedesktop.org/show_bug.cgi?id=92455. */
inline static void CAIRO_SURFACE_FLUSH(cairo_surface_t *surface) {
    cairo_surface_flush(surface);
    cairo_surface_flush(surface);
}

static bool surface_initialized(surface_t *surface) {
    if (surface == nullptr || surface->id == XCB_NONE) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n", fmt::ptr(surface)));
        return false;
    }
    return true;
}

DrawContext::~DrawContext() {
    /* Null for libxcb's handle of a connection that failed, on which libxcb's calls did nothing. */
    if (xcb::Connection *const c = xcb::xcbcompat::connection(conn)) {
        for (xcb_gcontext_t const gc : gcs | std::views::values) {
            xcb::xproto::free_gc(c, gc);
        }
    }

    /* cairo has no lookup of the device it keeps for a connection: a surface
     * gives it (and makes one if none was made). */
    cairo_surface_t *surface = cairo_xcb_surface_create(conn, root_screen->root, get_visualtype(root_screen), 1, 1);
    cairo_device_t *device = cairo_device_reference(cairo_surface_get_device(surface));
    cairo_surface_destroy(surface);
    cairo_device_finish(device);
    cairo_device_destroy(device);
}

/*
 * Get a GC for the given depth. The given drawable must have this depth.
 *
 * Per the X11 protocol manual for "CreateGC":
 * > The gcontext can be used with any destination drawable having the same root
 * > and depth as the specified drawable;
 */
xcb_gcontext_t DrawContext::get_gc(uint8_t depth, xcb_drawable_t drawable) {
    if (auto const it = gcs.find(depth); it != gcs.end()) {
        return it->second;
    }

    /* The drawing state of a connection exists only on one that was made (X, a nagbar, the placeholders). */
    xcb::Connection &c = *xcb::xcbcompat::connection(conn);
    xcb_gcontext_t gc = c.generate_id();
    /* The drawable is only used to get the root and depth, thus the GC is not
     * tied to the drawable and it can be re-used with different drawables. */
    auto const error = xcb::xproto::create_gc_checked(&c, gc, drawable, xcb::xproto::CreateGcValueList{}).get();
    if (error) {
        ELOG(fmt::sprintf("Could not create graphical context. Error code: %d. Please report this bug.\n", error->error_code));
        return gc;
    }

    gcs.emplace(depth, gc);
    return gc;
}

/*
 * Get depth of visual specified by visualid
 *
 */
uint16_t get_visual_depth(xcb_visualid_t visual_id) {
    return get_visual_depth(global.x->root_screen, visual_id);
}

/*
 * Get depth of visual specified by visualid, among the visuals of the given
 * screen.
 *
 */
uint16_t get_visual_depth(xcb_screen_t *screen, xcb_visualid_t visual_id) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (visual_id == visual_iter.data->visual_id) {
                return depth_iter.data->depth;
            }
        }
    }
    return 0;
}

surface_t::surface_t(DrawContext &context, xcb_drawable_t drawable, xcb_visualtype_t *_visual, int width, int height)
    : conn(context.connection()), id(drawable), width(width), height(height) {
    xcb_visualtype_t *visual;
    if (_visual == nullptr) {
        visual = global.x->visual_type;
    } else {
        visual = _visual;
    }

    gc = context.get_gc(get_visual_depth(context.screen(), visual->visual_id), drawable);
    surface = cairo_xcb_surface_create(conn, id, visual, width, height);
    cr = cairo_create(surface);
}

/*
 * Destroys the surface.
 *
 */
surface_t::~surface_t() {
    cairo_status_t status = CAIRO_STATUS_SUCCESS;
    if (this->cr) {
        status = cairo_status(this->cr);
    }
    if (status != CAIRO_STATUS_SUCCESS) {
        LOG(fmt::sprintf("Found cairo context in an error status while freeing, error %d is %s",
                         std::to_underlying(status), cairo_status_to_string(status)));
    }

    cairo_surface_destroy(this->surface);
    cairo_destroy(this->cr);
}

/*
 * Resize the surface to the given size.
 *
 */
surface_t *surface_t::draw_util_surface_set_size(int width, int height) {
    surface_t *surface = this;

    surface->width = width;
    surface->height = height;
    cairo_xcb_surface_set_size(surface->surface, width, height);

    return this;
}

/*
 * Parses the given color in hex format to an internal color representation.
 * Note that the input must begin with a hash sign, e.g., "#3fbc59".
 *
 */
color_t draw_util_hex_to_color(xcb_connection_t *conn, xcb_screen_t *root_screen, char const *color) {
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
    if (!surface_initialized(surface)) {
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
surface_t *surface_t::draw_util_text(i3Font &font, std::string &text, color_t fg_color, color_t bg_color, int x, int y, int max_width) {
    /* Flush any changes before we draw the text as this might use XCB directly. */
    CAIRO_SURFACE_FLUSH(this->surface);

    font.set_font_colors(this->gc, fg_color, bg_color);
    font.draw_text(text, this->id, this->gc, this->surface, x, y, max_width);

    /* Notify cairo that we (possibly) used another way to draw on the surface. */
    cairo_surface_mark_dirty(this->surface);

    return this;
}

/**
 * Draw the given image using libi3.
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
surface_t *surface_t::draw_util_image(cairo_surface_t *image, int x, int y, int width, int height) {
    surface_t *surface = this;

    cairo_save(surface->cr);

    cairo_translate(surface->cr, x, y);

    int const src_width = cairo_image_surface_get_width(image);
    int const src_height = cairo_image_surface_get_height(image);
    double scale = std::min(static_cast<double>(width) / src_width, static_cast<double>(height) / src_height);
    cairo_scale(surface->cr, scale, scale);

    cairo_set_source_surface(surface->cr, image, 0, 0);
    cairo_paint(surface->cr);

    cairo_restore(surface->cr);

    return this;
}

/*
 * Draws a filled rectangle.
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
surface_t *surface_t::draw_util_rectangle(color_t color, double x, double y, double w, double h) {
    surface_t *surface = this;

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

    return this;
}

/*
 * Clears a surface with the given color.
 *
 */
surface_t *surface_t::draw_util_clear_surface(color_t color) {
    surface_t *surface = this;

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

    return this;
}

/*
 * Copies a surface onto another surface.
 *
 */
void draw_util_copy_surface(surface_t *src, surface_t *dest, double src_x, double src_y,
                            double dest_x, double dest_y, double width, double height) {
    if (!surface_initialized(src) ||
        !surface_initialized(dest)) {
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
