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
module;
#include <glib.h>

#include <pango/pango.h>
#include <cairo/cairo-xcb.h>

#include <string>

module i3;

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
}

/*
 * Returns the UTF-8 encoded version of the i3String.
 *
 */
char *i3String::get_utf8() {
    return utf8;
}

size_t i3String::get_num_bytes() {
    return num_bytes;
}

bool i3String::is_pango_markup() {
    return pango_markup;
}

void i3String::set_markup(bool pango_markup) {
    this->pango_markup = pango_markup;
}
