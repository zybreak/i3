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
static inline void CAIRO_SURFACE_FLUSH(cairo_surface_t *surface) {
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

/*
 * Get a GC for the given depth. The given drawable must have this depth.
 *
 * Per the X11 protocol manual for "CreateGC":
 * > The gcontext can be used with any destination drawable having the same root
 * > and depth as the specified drawable;
 */
static xcb_gcontext_t get_gc(xcb_connection_t *conn, uint8_t depth, xcb_drawable_t drawable, bool *should_free) {
    static struct {
        uint8_t depth;
        xcb_gcontext_t gc;
    } gc_cache[2] = {
        {0, 0},
    };

    size_t index = 0;
    bool cache = false;

    *should_free = false;
    for (; index < sizeof(gc_cache) / sizeof(gc_cache[0]); index++) {
        if (gc_cache[index].depth == depth) {
            return gc_cache[index].gc;
        }
        if (gc_cache[index].depth == 0) {
            cache = true;
            break;
        }
    }

    xcb_gcontext_t gc = xcb_generate_id(conn);
    /* The drawable is only used to get the root and depth, thus the GC is not
     * tied to the drawable and it can be re-used with different drawables. */
    xcb_void_cookie_t gc_cookie = xcb_create_gc_checked(conn, gc, drawable, 0, nullptr);

    xcb_generic_error_t *error = xcb_request_check(conn, gc_cookie);
    if (error != nullptr) {
        ELOG(fmt::sprintf("Could not create graphical context. Error code: %d. Please report this bug.\n", error->error_code));
        free(error);
        return gc;
    }

    if (cache) {
        gc_cache[index].depth = depth;
        gc_cache[index].gc = gc;
    } else {
        *should_free = true;
    }

    return gc;
}

/*
 * Get depth of visual specified by visualid
 *
 */
uint16_t get_visual_depth(xcb_visualid_t visual_id) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(global.x->root_screen);
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

surface_t::surface_t(xcb_connection_t *conn, xcb_drawable_t drawable, xcb_visualtype_t *_visual, int width, int height) : conn(conn), id(drawable), width(width), height(height) {
    xcb_visualtype_t *visual;
    if (_visual == nullptr) {
        visual = global.x->visual_type;
    } else {
        visual = _visual;
    }
    
    gc = get_gc(conn, get_visual_depth(visual->visual_id), drawable, &owns_gc);
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

    if (this->owns_gc) {
        xcb_free_gc(conn, this->gc);
    }
    cairo_surface_destroy(this->surface);
    cairo_destroy(this->cr);
}

/*
 * Resize the surface to the given size.
 *
 */
surface_t* surface_t::draw_util_surface_set_size(int width, int height) {
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
surface_t* surface_t::draw_util_text(i3Font *font, std::string &text, color_t fg_color, color_t bg_color, int x, int y, int max_width) {
    /* Flush any changes before we draw the text as this might use XCB directly. */
    CAIRO_SURFACE_FLUSH(this->surface);

    font->set_font_colors(this->gc, fg_color, bg_color);
    font->draw_text(text, this->id, this->gc, this->surface, x, y, max_width);

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
surface_t* surface_t::draw_util_image(cairo_surface_t *image, int x, int y, int width, int height) {
    surface_t *surface = this;

    cairo_save(surface->cr);

    cairo_translate(surface->cr, x, y);

    const int src_width = cairo_image_surface_get_width(image);
    const int src_height = cairo_image_surface_get_height(image);
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
surface_t* surface_t::draw_util_rectangle(color_t color, double x, double y, double w, double h) {
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
surface_t* surface_t::draw_util_clear_surface(color_t color) {
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
