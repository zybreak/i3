#pragma once

struct i3Font;
struct xcb_connection_t;
struct xcb_screen_t;
struct color_t;

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
 * Returns true if and only if the current font is a pango font.
 *
 */
bool font_is_pango();

/**
 * Loads a font for usage, also getting its height. If fallback is true,
 * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
 * font was previously loaded, it will be freed.
 *
 */
i3Font load_font(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *pattern, const bool fallback);

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
 * Defines the colors to be used for the forthcoming draw_text calls.
 *
 */
void set_font_colors(xcb_connection_t *conn, xcb_gcontext_t gc, color_t foreground, color_t background);

/**
 * Returns true if and only if the current font is a pango font.
 *
 */
bool font_is_pango();
