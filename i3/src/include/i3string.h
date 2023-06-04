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
class i3String {
   private:
    char *utf8{nullptr};
    size_t num_bytes;
    bool pango_markup;
   public:
    explicit i3String(const std::string &str);
    ~i3String();

    /**
     * Returns the UTF-8 encoded version of the i3String.
     *
     */
    const char *get_utf8();

    /**
     * Returns the number of bytes (UTF-8 encoded) in an i3String.
     *
     */
    size_t get_num_bytes();
    /**
     * Whether the given i3String is in Pango markup.
     */
    bool is_pango_markup();

    /**
     * Set whether the i3String should use Pango markup.
     */
    void set_markup(bool pango_markup);
};
