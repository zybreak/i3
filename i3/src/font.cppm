module;
#include <pango/pango.h>
#include <xcb/xcb.h>
#include <cairo.h>
export module i3:font;

import std;

struct color_t;

void freeFontDesc(PangoFontDescription *desc);

export {
    /**
     * Data structure for cached font information:
     * - font id in X11 (load it once)
     * - font height (multiple calls needed to get it)
     *
     */
    class i3Font {
       private: 
        xcb_visualtype_t *root_visual_type;
        xcb_connection_t *conn;
        xcb_screen_t *root_screen;

        double pango_font_red;
        double pango_font_green;
        double pango_font_blue;
        double pango_font_alpha;

    public:
        /** The height of the font, built from font_ascent + font_descent */
        int height{};

        /** The pattern/name used to load the font. */
        std::string pattern{};

        /** The pango font description */
        std::unique_ptr<PangoFontDescription, decltype(&freeFontDesc)> pango_desc{nullptr, &freeFontDesc};

        i3Font(xcb_connection_t *conn, xcb_screen_t *root_screen, const std::string pattern);

        /**
         * Predict the text width in pixels for the given text. Text must be
         * specified as an i3String.
         *
         */
        int predict_text_width(const std::string &text) const;

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
        void draw_text(const std::string &text, xcb_drawable_t drawable, xcb_gcontext_t gc,
                       cairo_surface_t *surface, int x, int y, int max_width) const;

        /**
         * Defines the colors to be used for the forthcoming draw_text calls.
         *
         */
        void set_font_colors(xcb_gcontext_t gc, color_t foreground, color_t background);
    };

    /**
     * Loads a font for usage, also getting its height. If fallback is true,
     * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
     * font was previously loaded, it will be freed.
     *
     */
    std::unique_ptr<i3Font> load_font(xcb_connection_t *conn, xcb_screen_t *root_screen, const std::string pattern);
    
}