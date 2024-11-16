module;
#include <pango/pango.h>
#include <cairo/cairo-xcb.h>
export module i3:draw;

import std;
import utils;

class i3Font;

export {
    /* A wrapper grouping an XCB drawable and both a graphics context
     * and the corresponding cairo objects representing it. */
    class surface_t {
      private:
        /* The connection to the X server. */
        xcb_connection_t *conn;

      public:
        /* The drawable which is being represented. */
        xcb_drawable_t id{};

        /* A classic XCB graphics context. */
        xcb_gcontext_t gc{};
        bool owns_gc;

        int width;
        int height;

        /* A cairo surface representing the drawable. */
        cairo_surface_t *surface{};

        /* The cairo object representing the drawable. In general,
         * this is what one should use for any drawing operation. */
        cairo_t *cr{};
        surface_t(xcb_connection_t *conn, xcb_drawable_t drawable,
                  xcb_visualtype_t *visual, int width, int height);
        ~surface_t();

        surface_t() = delete;
        surface_t(surface_t const &) = delete;
        void operator=(surface_t const &) = delete;
        surface_t(surface_t &&other) {
            auto tmp_owns_gc = this->owns_gc;
            auto tmp_cr = this->cr;
            auto tmp_gc = this->gc;
            auto tmp_surface = this->surface;

            this->conn = other.conn;
            this->id = other.id;
            this->gc = other.gc;
            this->owns_gc = other.owns_gc;
            this->width = other.width;
            this->height = other.height;
            this->surface = other.surface;
            this->cr = other.cr;

            other.surface = tmp_surface;
            other.cr = tmp_cr;
            other.owns_gc = tmp_owns_gc;
            other.gc = tmp_gc;
        }
        void operator=(surface_t &&other) {
            auto tmp_owns_gc = this->owns_gc;
            auto tmp_cr = this->cr;
            auto tmp_gc = this->gc;
            auto tmp_surface = this->surface;

            this->conn = other.conn;
            this->id = other.id;
            this->gc = other.gc;
            this->owns_gc = other.owns_gc;
            this->width = other.width;
            this->height = other.height;
            this->surface = other.surface;
            this->cr = other.cr;

            other.surface = tmp_surface;
            other.cr = tmp_cr;
            other.owns_gc = tmp_owns_gc;
            other.gc = tmp_gc;
        }

        /**
         * Resize the surface to the given size.
         *
         */
        surface_t *draw_util_surface_set_size(int width, int height);

        /**
         * Draw the given text using libi3.
         * This function also marks the surface dirty which is needed if other means of
         * drawing are used. This will be the case when using XCB to draw text.
         *
         */
        surface_t *draw_util_text(i3Font &font, std::string &text, color_t fg_color, color_t bg_color, int x, int y, int max_width);

        /**
         * Clears a surface with the given color.
         *
         */
        surface_t *draw_util_clear_surface(color_t color);

        /**
         * This function is a convenience wrapper and takes care of flushing the
         * surface as well as restoring the cairo state.
         *
         */
        surface_t *draw_util_image(cairo_surface_t *image, int x, int y, int width, int height);

        /**
         * Draws a filled rectangle.
         * This function is a convenience wrapper and takes care of flushing the
         * surface as well as restoring the cairo state.
         *
         */
        surface_t *draw_util_rectangle(color_t color, double x, double y, double w, double h);
    };

    /**
     * Parses the given color in hex format to an internal color representation.
     * Note that the input must begin with a hash sign, e.g., "#3fbc59".
     *
     */
    color_t draw_util_hex_to_color(xcb_connection_t * conn, xcb_screen_t * root_screen, char const *color);

    /**
     * Copies a surface onto another surface.
     *
     */
    void draw_util_copy_surface(surface_t * src, surface_t * dest, double src_x, double src_y,
                                double dest_x, double dest_y, double width, double height);
}