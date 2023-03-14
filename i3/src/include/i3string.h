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
#include <fmt/core.h>
#include <fmt/printf.h>
#include <iostream>
//#include <source_location>

#include <pango/pango.h>
#include <cairo/cairo-xcb.h>

/**
 * Data structure for storing strings.
 *
 */
struct i3String {
    char *utf8{nullptr};
    size_t num_bytes;
    bool pango_markup;
    explicit i3String(const std::string &str);
    ~i3String();
};


/**
 * Data structure for cached font information:
 * - font id in X11 (load it once)
 * - font height (multiple calls needed to get it)
 *
 */
struct i3Font {
    /** The height of the font, built from font_ascent + font_descent */
    int height;

    /** The pattern/name used to load the font. */
    char *pattern;

    /** The pango font description */
    PangoFontDescription *pango_desc;
};

/**
 * Build an i3String from an UTF-8 encoded string.
 * Returns the newly-allocated i3String.
 *
 */
i3String *i3string_from_utf8(const std::string &from_utf8);

/**
 * Returns the UTF-8 encoded version of the i3String.
 *
 */
const char *i3string_as_utf8(i3String *str);

/**
 * Returns the number of bytes (UTF-8 encoded) in an i3String.
 *
 */
size_t i3string_get_num_bytes(i3String *str);

/**
 * Whether the given i3String is in Pango markup.
 */
bool i3string_is_markup(i3String *str);

/**
 * Set whether the i3String should use Pango markup.
 */
void i3string_set_markup(i3String *str, bool pango_markup);
