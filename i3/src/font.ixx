module;
#include <pango/pango.h>
#include <xcb/xcb.h>
#include <cairo.h>
export module i3:font;

struct color_t;
class i3String;

export {
    /**
     * Data structure for cached font information:
     * - font id in X11 (load it once)
     * - font height (multiple calls needed to get it)
     *
     */
    class i3Font {
       public:
        /** The height of the font, built from font_ascent + font_descent */
        int height;

        /** The pattern/name used to load the font. */
        char *pattern;

        /** The pango font description */
        PangoFontDescription *pango_desc;
    };


    /**
     * Predict the text width in pixels for the given text. Text must be
     * specified as an i3String.
     *
     */
    int predict_text_width(xcb_connection_t *conn, xcb_screen_t *root_screen, i3String *text);

    /**
     * Defines the colors to be used for the forthcoming draw_text calls.
     *
     */
    void set_font_colors(xcb_connection_t *conn, xcb_gcontext_t gc, color_t foreground, color_t background);

    /**
     * Loads a font for usage, also getting its height. If fallback is true,
     * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
     * font was previously loaded, it will be freed.
     *
     */
    i3Font* load_font(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *pattern, bool fallback);

    /**
     * Defines the font to be used for the forthcoming calls.
     *
     */
    void set_font(i3Font *font);

    /**
     * Frees the resources taken by the current font. If no font was previously
     * loaded, it simply returns.
     *
     */
    void free_font(xcb_connection_t *conn);

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
}