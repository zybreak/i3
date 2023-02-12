/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * string.c: Define an i3String type to automagically handle UTF-8/UCS-2
 *           conversions. Some font backends need UCS-2 (X core fonts),
 *           others want UTF-8 (Pango).
 *
 */
#include "libi3.h"

#include <glib.h>
#include <cstdlib>
#include <cstring>

/*
 * Build an i3String from an UTF-8 encoded string.
 * Returns the newly-allocated i3String.
 *
 */
i3String *i3string_from_utf8(const std::string &from_utf8) {
    return new i3String{from_utf8};
}

i3String::i3String(const std::string &str) {
    /* g_utf8_make_valid NULL-terminates the string. */
    this->utf8 = g_utf8_make_valid(str.c_str(), str.length());

    /* num_bytes < 0 means NULL-terminated string, need to calculate length */
    this->num_bytes = strlen(this->utf8);
}

/*
 * Free an i3String.
 *
 */
i3String::~i3String() {
    if (this->utf8) {
        free(this->utf8);
    }

    if (this->ucs2) {
        free(this->ucs2);
    }
}

static void i3string_ensure_utf8(i3String *str) {
    if (str->utf8 != nullptr)
        return;
    if ((str->utf8 = convert_ucs2_to_utf8(str->ucs2, str->num_glyphs)) != nullptr)
        str->num_bytes = strlen(str->utf8);
}

static void i3string_ensure_ucs2(i3String *str) {
    if (str->ucs2 != nullptr)
        return;
    str->ucs2 = convert_utf8_to_ucs2(str->utf8, &str->num_glyphs);
}

/*
 * Returns the UTF-8 encoded version of the i3String.
 *
 */
const char *i3string_as_utf8(i3String *str) {
    i3string_ensure_utf8(str);
    return str->utf8;
}

/*
 * Returns the UCS-2 encoded version of the i3String.
 *
 */
const xcb_char2b_t *i3string_as_ucs2(i3String *str) {
    i3string_ensure_ucs2(str);
    return str->ucs2;
}

/*
 * Returns the number of bytes (UTF-8 encoded) in an i3String.
 *
 */
size_t i3string_get_num_bytes(i3String *str) {
    i3string_ensure_utf8(str);
    return str->num_bytes;
}

/*
 * Whether the given i3String is in Pango markup.
 */
bool i3string_is_markup(i3String *str) {
    return str->pango_markup;
}

/*
 * Set whether the i3String should use Pango markup.
 */
void i3string_set_markup(i3String *str, bool pango_markup) {
    str->pango_markup = pango_markup;
}

/*
 * Returns the number of glyphs in an i3String.
 *
 */
size_t i3string_get_num_glyphs(i3String *str) {
    i3string_ensure_ucs2(str);
    return str->num_glyphs;
}
