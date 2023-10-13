/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
#include "get_visualtype.h"
#include "draw.h"
#include "dpi.h"
#include "log.h"
#include "font.h"

#include <cassert>
#include <cairo/cairo-xcb.h>
#include <err.h>
#include <pango/pangocairo.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

import utils;

static const i3Font *savedFont = nullptr;

static xcb_visualtype_t *root_visual_type;
static double pango_font_red;
static double pango_font_green;
static double pango_font_blue;

static PangoLayout *create_layout_with_dpi(cairo_t *cr) {
    PangoLayout *layout;
    PangoContext *context;

    context = pango_cairo_create_context(cr);
    pango_cairo_context_set_resolution(context, get_dpi_value());
    layout = pango_layout_new(context);
    g_object_unref(context);

    return layout;
}

/*
 * Loads a Pango font description into an i3Font structure. Returns true
 * on success, false otherwise.
 *
 */
static bool load_pango_font(xcb_connection_t *conn, xcb_screen_t *root_screen, i3Font *font, const char *desc) {
    /* Load the font description */
    font->pango_desc = pango_font_description_from_string(desc);
    if (!font->pango_desc) {
        ELOG(fmt::sprintf("Could not open font %s with Pango, fallback to X font.\n",  desc));
        return false;
    }

    LOG(fmt::sprintf("Using Pango font %s, size %d\n",
        pango_font_description_get_family(font->pango_desc),
        pango_font_description_get_size(font->pango_desc) / PANGO_SCALE));

    /* We cache root_visual_type here, since you must call
     * load_pango_font before any other pango function
     * that would need root_visual_type */
    root_visual_type = get_visualtype(root_screen);

    /* Create a dummy Pango layout to compute the font height */
    cairo_surface_t *surface = cairo_xcb_surface_create(conn, root_screen->root, root_visual_type, 1, 1);
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);
    pango_layout_set_font_description(layout, font->pango_desc);

    /* Get the font height */
    gint height;
    pango_layout_get_pixel_size(layout, nullptr, &height);
    font->height = height;

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return true;
}

/*
 * Draws text using Pango rendering.
 *
 */
static void draw_text_pango(const char *text, size_t text_len,
                            xcb_drawable_t drawable, cairo_surface_t *surface,
                            int x, int y, int max_width, bool pango_markup) {
    /* Create the Pango layout */
    /* root_visual_type is cached in load_pango_font */
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);
    gint height;

    pango_layout_set_font_description(layout, savedFont->pango_desc);
    pango_layout_set_width(layout, max_width * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_CHAR);
    pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);

    if (pango_markup)
        pango_layout_set_markup(layout, text, text_len);
    else
        pango_layout_set_text(layout, text, text_len);

    /* Do the drawing */
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, pango_font_red, pango_font_green, pango_font_blue);
    pango_cairo_update_layout(cr, layout);
    pango_layout_get_pixel_size(layout, nullptr, &height);
    /* Center the piece of text vertically. */
    int yoffset = (height - savedFont->height) / 2;
    cairo_move_to(cr, x, y - yoffset);
    pango_cairo_show_layout(cr, layout);

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
}

/*
 * Calculate the text width using Pango rendering.
 *
 */
static int predict_text_width_pango(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *text, size_t text_len, bool pango_markup) {
    /* Create a dummy Pango layout */
    /* root_visual_type is cached in load_pango_font */
    cairo_surface_t *surface = cairo_xcb_surface_create(conn, root_screen->root, root_visual_type, 1, 1);
    cairo_t *cr = cairo_create(surface);
    PangoLayout *layout = create_layout_with_dpi(cr);

    /* Get the font width */
    gint width;
    pango_layout_set_font_description(layout, savedFont->pango_desc);

    if (pango_markup)
        pango_layout_set_markup(layout, text, text_len);
    else
        pango_layout_set_text(layout, text, text_len);

    pango_cairo_update_layout(cr, layout);
    pango_layout_get_pixel_size(layout, &width, nullptr);

    /* Free resources */
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    return width;
}

/*
 * Loads a font for usage, also getting its metrics. If fallback is true,
 * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
 * font was previously loaded, it will be freed.
 *
 */
i3Font* load_font(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *pattern, const bool fallback) {
    /* if any font was previously loaded, free it now */
    free_font(conn);

    auto *font = new i3Font{};
    font->pattern = nullptr;

    /* No XCB connction, return early because we're just validating the
     * configuration file. */
    if (conn == nullptr) {
        return font;
    }

    /* Try to load a pango font if specified */
    if (strlen(pattern) > strlen("pango:") && !strncmp(pattern, "pango:", strlen("pango:"))) {
        const char *font_pattern = pattern + strlen("pango:");
        if (load_pango_font(conn, root_screen, font, font_pattern)) {
            font->pattern = sstrdup(pattern);
            return font;
        }
    } else if (strlen(pattern) > strlen("xft:") && !strncmp(pattern, "xft:", strlen("xft:"))) {
        const char *font_pattern = pattern + strlen("xft:");
        if (load_pango_font(conn, root_screen, font, font_pattern)) {
            font->pattern = sstrdup(pattern);
            return font;
        }
    } else {
        const char *font_pattern = pattern;
        if (load_pango_font(conn, root_screen, font, font_pattern)) {
            font->pattern = sstrdup(pattern);
            return font;
        }
    }
}

/*
 * Defines the font to be used for the forthcoming calls.
 *
 */
void set_font(i3Font *font) {
    savedFont = font;
}

/*
 * Frees the resources taken by the current font. If no font was previously
 * loaded, it simply returns.
 *
 */
void free_font(xcb_connection_t *conn) {
    /* if there is no saved font, simply return */
    if (savedFont == nullptr)
        return;

    free(savedFont->pattern);
    /* Free the font description */
    pango_font_description_free(savedFont->pango_desc);

    savedFont = nullptr;
}

/*
 * Defines the colors to be used for the forthcoming draw_text calls.
 *
 */
void set_font_colors(xcb_connection_t *conn, xcb_gcontext_t gc, color_t foreground, color_t background) {
    assert(savedFont != nullptr);

    /* Save the foreground font */
    pango_font_red = foreground.red;
    pango_font_green = foreground.green;
    pango_font_blue = foreground.blue;
}

/*
 * Draws text onto the specified X drawable (normally a pixmap) at the
 * specified coordinates (from the top left corner of the leftmost, uppermost
 * glyph) and using the provided gc.
 *
 * Text must be specified as an i3String.
 *
 */
void draw_text(xcb_connection_t *conn, i3String *text, xcb_drawable_t drawable, xcb_gcontext_t gc,
               cairo_surface_t *surface, int x, int y, int max_width) {
    assert(savedFont != nullptr);

    /* Render the text using Pango */
    draw_text_pango(text->get_utf8(), text->get_num_bytes(),
                    drawable, surface, x, y, max_width, text->is_pango_markup());
}

/*
 * Predict the text width in pixels for the given text. Text must be
 * specified as an i3String.
 *
 */
int predict_text_width(xcb_connection_t *conn, xcb_screen_t *root_screen, i3String *text) {
    assert(savedFont != nullptr);

    /* Calculate extents using Pango */
    return predict_text_width_pango(conn, root_screen, text->get_utf8(), text->get_num_bytes(),
                                    text->is_pango_markup());
}
