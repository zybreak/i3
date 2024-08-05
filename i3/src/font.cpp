/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <fmt/printf.h>
#include <cairo/cairo-xcb.h>
#include <err.h>
#include <pango/pangocairo.h>
module i3;

import utils;
import log;

static PangoLayout *create_layout_with_dpi(cairo_t *cr) {
    PangoLayout *layout;
    PangoContext *context;

    context = pango_cairo_create_context(cr);
    pango_cairo_context_set_resolution(context, get_dpi_value());
    layout = pango_layout_new(context);
    g_object_unref(context);

    return layout;
}

void freeFontDesc(PangoFontDescription *desc) {
    if (desc != nullptr) {
        pango_font_description_free(desc);
    }
}

i3Font::i3Font(xcb_connection_t *conn, xcb_screen_t *root_screen, const std::string pattern) : conn(conn), root_screen(root_screen), pattern(pattern), pango_desc(pango_font_description_from_string(pattern.c_str()), &freeFontDesc) {
    /* Load the font description */
    //this->pango_desc = std::make_unique<PangoFontDescription, &freeFontDesc>(pango_font_description_from_string(pattern.c_str()));
    if (!this->pango_desc) {
        throw std::runtime_error(fmt::format("Could not open font {} with Pango", pattern));
    }

    LOG(fmt::sprintf("Using Pango font %s, size %d\n",
            pango_font_description_get_family(this->pango_desc.get()),
            pango_font_description_get_size(this->pango_desc.get()) / PANGO_SCALE));

    /* We cache root_visual_type here, since you must call
     * load_pango_font before any other pango function
     * that would need root_visual_type */
    this->root_visual_type = get_visualtype(root_screen);

    /* Create a dummy Pango layout to compute the font height */
    cairo_surface_t *surface = cairo_xcb_surface_create(conn, root_screen->root, this->root_visual_type, 1, 1);
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);
    pango_layout_set_font_description(layout, pango_desc.get());

    /* Get the font height */
    pango_layout_get_pixel_size(layout, nullptr, &this->height);

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

/*
 * Loads a font for usage, also getting its metrics. If fallback is true,
 * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
 * font was previously loaded, it will be freed.
 *
 */
std::unique_ptr<i3Font> load_font(xcb_connection_t *conn, xcb_screen_t *root_screen, const std::string pattern) {
    return std::make_unique<i3Font>(conn, root_screen, pattern);
}

/*
 * Defines the colors to be used for the forthcoming draw_text calls.
 *
 */
void i3Font::set_font_colors(xcb_gcontext_t gc, color_t foreground, color_t background) {
    /* Save the foreground font */
    pango_font_red = foreground.red;
    pango_font_green = foreground.green;
    pango_font_blue = foreground.blue;
    pango_font_alpha = foreground.alpha;
}

/*
 * Draws text onto the specified X drawable (normally a pixmap) at the
 * specified coordinates (from the top left corner of the leftmost, uppermost
 * glyph) and using the provided gc.
 *
 * Text must be specified as an i3String.
 *
 */
void i3Font::draw_text(const std::string &text, xcb_drawable_t drawable, xcb_gcontext_t gc,
               cairo_surface_t *surface, int x, int y, int max_width) const {
    size_t text_len = text.length();
    /* Render the text using Pango */
    /* Create the Pango layout */
    /* root_visual_type is cached in load_pango_font */
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);
    gint height;

    pango_layout_set_font_description(layout, this->pango_desc.get());
    pango_layout_set_width(layout, max_width * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_CHAR);
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);

    pango_layout_set_text(layout, text.c_str(), text_len);

    /* Do the drawing */
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, pango_font_red, pango_font_green, pango_font_blue, pango_font_alpha);
    pango_cairo_update_layout(cr, layout);
    pango_layout_get_pixel_size(layout, nullptr, &height);
    /* Center the piece of text vertically. */
    int yoffset = (height - this->height) / 2;
    cairo_move_to(cr, x, y - yoffset);
    pango_cairo_show_layout(cr, layout);

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
}

/*
 * Predict the text width in pixels for the given text. Text must be
 * specified as an i3String.
 *
 */
int i3Font::predict_text_width(const std::string &text) const {
    size_t text_len = text.length();
    
    /* Create a dummy Pango layout */
    /* root_visual_type is cached in load_pango_font */
    cairo_surface_t *surface = cairo_xcb_surface_create(conn, root_screen->root, root_visual_type, 1, 1);
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);

    /* Get the font width */
    int width;
    pango_layout_set_font_description(layout, this->pango_desc.get());

    pango_layout_set_text(layout, text.c_str(), text_len);

    pango_cairo_update_layout(cr, layout);
    pango_layout_get_pixel_size(layout, &width, nullptr);

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return width;
}
