/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * libi3: contains functions which are used by i3 *and* accompanying tools such
 * as i3-config-wizard, …
 *
 */
#pragma once

#include <config.h>

#include <string>
#include <cstdarg>
#include <cstdio>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <iostream>

#include <pango/pango.h>
#include <cairo/cairo-xcb.h>
#include "i3string.h"
#include "log.h"

#define DEFAULT_DIR_MODE (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)

/** Mouse buttons */
#define XCB_BUTTON_CLICK_LEFT XCB_BUTTON_INDEX_1
#define XCB_BUTTON_CLICK_MIDDLE XCB_BUTTON_INDEX_2
#define XCB_BUTTON_CLICK_RIGHT XCB_BUTTON_INDEX_3
#define XCB_BUTTON_SCROLL_UP XCB_BUTTON_INDEX_4
#define XCB_BUTTON_SCROLL_DOWN XCB_BUTTON_INDEX_5
/* xcb doesn't define constants for these. */
#define XCB_BUTTON_SCROLL_LEFT 6
#define XCB_BUTTON_SCROLL_RIGHT 7

/**
 * Try to get the contents of the given atom (for example I3_SOCKET_PATH) from
 * the X11 root window and return NULL if it doesn’t work.
 *
 * If the provided XCB connection is NULL, a new connection will be
 * established.
 *
 * The memory for the contents is dynamically allocated and has to be
 * free()d by the caller.
 *
 */
char *root_atom_contents(const char *atomname, xcb_connection_t *provided_conn, int screen);

/**
 * Safe-wrapper around malloc which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
void *smalloc(size_t size);

/**
 * Safe-wrapper around calloc which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
void *scalloc(size_t num, size_t size);

/**
 * Safe-wrapper around realloc which exits if realloc returns NULL (meaning
 * that there is no more memory available).
 *
 */
void *srealloc(void *ptr, size_t size);

/**
 * Safe-wrapper around strdup which exits if malloc returns NULL (meaning that
 * there is no more memory available)
 *
 */
char *sstrdup(const char *str);

/**
 * Safe-wrapper around strndup which exits if strndup returns NULL (meaning that
 * there is no more memory available)
 *
 */
char *sstrndup(const char *str, size_t size);

/**
 * Safe-wrapper around asprintf which exits if it returns -1 (meaning that
 * there is no more memory available)
 *
 */
int sasprintf(char **strp, const char *fmt, ...);

/**
 * Returns the colorpixel to use for the given hex color (think of HTML). Only
 * works for true-color (vast majority of cases) at the moment, avoiding a
 * roundtrip to X11.
 *
 * The hex_color has to start with #, for example #FF00FF.
 *
 * NOTE that get_colorpixel() does _NOT_ check the given color code for validity.
 * This has to be done by the caller.
 *
 * NOTE that this function may in the future rely on a global xcb_connection_t
 * variable called 'conn' to be present.
 *
 */
uint32_t get_colorpixel(xcb_connection_t *conn, xcb_screen_t *root_screen, const char *hex) __attribute__((const));

/**
 * Returns the modifier mask (XCB_MOD_MASK_*) for the given keysymbol, for
 * example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function does not initiate any round-trips.
 *
 */
uint32_t get_mod_mask_for(uint32_t keysym,
                          xcb_key_symbols_t *symbols,
                          xcb_get_modifier_mapping_reply_t *modmap_reply);

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
 * Converts the given string to UTF-8 from UCS-2 big endian. The return value
 * must be freed after use.
 *
 */
char *convert_ucs2_to_utf8(xcb_char2b_t *text, size_t num_glyphs);

/**
 * Converts the given string to UCS-2 big endian for use with
 * xcb_image_text_16(). The amount of real glyphs is stored in real_strlen,
 * a buffer containing the UCS-2 encoded string (16 bit per glyph) is
 * returned. It has to be freed when done.
 *
 */
xcb_char2b_t *convert_utf8_to_ucs2(char *input, size_t *real_strlen);

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
 * Predict the text width in pixels for the given text. Text must be
 * specified as an i3String.
 *
 */
int predict_text_width(xcb_connection_t *conn, xcb_screen_t *root_screen, i3String *text);

/**
 * Returns the visual type associated with the given screen.
 *
 */
xcb_visualtype_t *get_visualtype(xcb_screen_t *screen);

/**
 * Returns the name of a temporary file with the specified prefix.
 *
 */
char *get_process_filename(const char *prefix);

/**
 * Initialize the DPI setting.
 * This will use the 'Xft.dpi' X resource if available and fall back to
 * guessing the correct value otherwise.
 */
void init_dpi(xcb_connection_t *conn, xcb_screen_t *root_screen);

/**
 * This function returns the value of the DPI setting.
 *
 */
double get_dpi_value();

/**
 * Convert a logical amount of pixels (e.g. 2 pixels on a “standard” 96 DPI
 * screen) to a corresponding amount of physical pixels on a standard or retina
 * screen, e.g. 5 pixels on a 227 DPI MacBook Pro 13" Retina screen.
 *
 */
long logical_px(xcb_screen_t *root_screen, const long logical);

/**
 * This function resolves ~ in pathnames.
 * It may resolve wildcards in the first part of the path, but if no match
 * or multiple matches are found, it just returns a copy of path as given.
 *
 */
std::string resolve_tilde(const std::string_view &path);

/**
 * Get the path of the first configuration file found. If override_configpath is
 * specified, that path is returned and saved for further calls. Otherwise,
 * checks the home directory first, then the system directory, always taking
 * into account the XDG Base Directory Specification ($XDG_CONFIG_HOME,
 * $XDG_CONFIG_DIRS).
 *
 */
std::string get_config_path(const std::string *override_configpath, bool use_system_paths);

/**
 * Emulates mkdir -p (creates any missing folders)
 *
 */
int mkdirp(const char *path, mode_t mode);

/** Helper structure for usage in format_placeholders(). */
struct placeholder_t {
    /* The placeholder to be replaced, e.g., "%title". */
    const char *name;
    /* The value this placeholder should be replaced with. */
    const char *value;
};

/**
 * Replaces occurrences of the defined placeholders in the format string.
 *
 */
std::string format_placeholders(const std::string &format, placeholder_t *placeholders, int num);

/* We need to flush cairo surfaces twice to avoid an assertion bug. See #1989
 * and https://bugs.freedesktop.org/show_bug.cgi?id=92455. */
#define CAIRO_SURFACE_FLUSH(surface)  \
    do {                              \
        cairo_surface_flush(surface); \
        cairo_surface_flush(surface); \
    } while (0)

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
