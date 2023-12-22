module;

#include <pango/pango.h>
#include <cairo/cairo-xcb.h>
#include "i3string.h"
export module i3:draw;

export {

/* Represents a color split by color channel. */
struct color_t {
    double red;
    double green;
    double blue;
    double alpha;

    /* The colorpixel we use for direct XCB calls. */
    uint32_t colorpixel;

    auto operator<=>(const color_t &r) const = default;
};

/* A wrapper grouping an XCB drawable and both a graphics context
 * and the corresponding cairo objects representing it. */
struct surface_t {
    /* The drawable which is being represented. */
    xcb_drawable_t id;

    /* A classic XCB graphics context. */
    xcb_gcontext_t gc;

    int width;
    int height;

    /* A cairo surface representing the drawable. */
    cairo_surface_t *surface;

    /* The cairo object representing the drawable. In general,
     * this is what one should use for any drawing operation. */
    cairo_t *cr;
};

/**
 * Draws text onto the specified X drawable (normally a pixmap) at the
 * specified coordinates (from the top left corner of the leftmost, uppermost
 * glyph) and using the provided gc.
 *
 * The given cairo surface must refer to the specified X drawable.
 *
 * Text must be specified as an i3String.
 *
 */
void draw_text(xcb_connection_t *conn, i3String *text, xcb_drawable_t drawable, xcb_gcontext_t gc,
               cairo_surface_t *surface, int x, int y, int max_width);

/**
 * Initialize the surface to represent the given drawable.
 *
 */
void draw_util_surface_init(xcb_connection_t *conn, surface_t *surface, xcb_drawable_t drawable,
                            xcb_visualtype_t *visual, int width, int height);

/**
 * Resize the surface to the given size.
 *
 */
void draw_util_surface_set_size(surface_t *surface, int width, int height);

/**
 * Destroys the surface.
 *
 */
void draw_util_surface_free(xcb_connection_t *conn, surface_t *surface);

/**
 * Parses the given color in hex format to an internal color representation.
 * Note that the input must begin with a hash sign, e.g., "#3fbc59".
 *
 */
color_t draw_util_hex_to_color(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *color);

/**
 * Draw the given text using libi3.
 * This function also marks the surface dirty which is needed if other means of
 * drawing are used. This will be the case when using XCB to draw text.
 *
 */
void draw_util_text(xcb_connection_t *conn, i3String *text, surface_t *surface, color_t fg_color, color_t bg_color, int x, int y, int max_width);

/**
 * Draws a filled rectangle.
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
void draw_util_rectangle(surface_t *surface, color_t color, double x, double y, double w, double h);

/**
 * Clears a surface with the given color.
 *
 */
void draw_util_clear_surface(surface_t *surface, color_t color);

/**
 * Copies a surface onto another surface.
 *
 */
void draw_util_copy_surface(surface_t *src, surface_t *dest, double src_x, double src_y,
                            double dest_x, double dest_y, double width, double height);

/**
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
void draw_util_image(cairo_surface_t *image, surface_t *surface, int x, int y, int width, int height);
}