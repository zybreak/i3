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
#include "i3string.h"
#include "log.h"
#include "wrapper.h"
#include "dpi.h"

#include <glib.h>
#include <cstdlib>
#include <cstring>
#include <iconv.h>
#include <err.h>

static iconv_t utf8_conversion_descriptor = (iconv_t)-1;
static iconv_t ucs2_conversion_descriptor = (iconv_t)-1;

/*
 * Converts the given string to UTF-8 from UCS-2 big endian. The return value
 * must be freed after use.
 *
 */
static char *convert_ucs2_to_utf8(xcb_char2b_t *text, size_t num_glyphs) {
    /* Allocate the output buffer (UTF-8 is at most 4 bytes per glyph) */
    size_t buffer_size = num_glyphs * 4 + 1;
    char *buffer = (char*)scalloc(buffer_size, 1);

    /* We need to use an additional pointer, because iconv() modifies it */
    char *output = buffer;
    size_t output_size = buffer_size - 1;

    if (utf8_conversion_descriptor == (iconv_t)-1) {
        /* Get a new conversion descriptor */
        utf8_conversion_descriptor = iconv_open("UTF-8", "UCS-2BE");
        if (utf8_conversion_descriptor == (iconv_t)-1)
            err(EXIT_FAILURE, "Error opening the conversion context");
    } else {
        /* Reset the existing conversion descriptor */
        iconv(utf8_conversion_descriptor, nullptr, nullptr, nullptr, nullptr);
    }

    /* Do the conversion */
    size_t input_len = num_glyphs * sizeof(xcb_char2b_t);
    size_t rc = iconv(utf8_conversion_descriptor, (char **)&text,
                      &input_len, &output, &output_size);
    if (rc == (size_t)-1) {
        perror("Converting to UTF-8 failed");
        free(buffer);
        return nullptr;
    }

    return buffer;
}

/*
 * Converts the given string to UCS-2 big endian for use with
 * xcb_image_text_16(). The amount of real glyphs is stored in real_strlen,
 * a buffer containing the UCS-2 encoded string (16 bit per glyph) is
 * returned. It has to be freed when done.
 *
 */
static xcb_char2b_t *convert_utf8_to_ucs2(char *input, size_t *real_strlen) {
    /* Calculate the input buffer size (UTF-8 is strlen-safe) */
    size_t input_size = strlen(input);

    /* Calculate the output buffer size and allocate the buffer */
    size_t buffer_size = input_size * sizeof(xcb_char2b_t);
    auto *buffer = (xcb_char2b_t*)smalloc(buffer_size);

    /* We need to use an additional pointer, because iconv() modifies it */
    size_t output_bytes_left = buffer_size;
    xcb_char2b_t *output = buffer;

    if (ucs2_conversion_descriptor == (iconv_t)-1) {
        /* Get a new conversion descriptor. //IGNORE is a GNU suffix that makes
         * iconv to silently discard characters that cannot be represented in
         * the target character set. */
        ucs2_conversion_descriptor = iconv_open("UCS-2BE//IGNORE", "UTF-8");
        if (ucs2_conversion_descriptor == (iconv_t)-1) {
            ucs2_conversion_descriptor = iconv_open("UCS-2BE", "UTF-8");
        }
        if (ucs2_conversion_descriptor == (iconv_t)-1) {
            err(EXIT_FAILURE, "Error opening the conversion context");
        }
    } else {
        /* Reset the existing conversion descriptor */
        iconv(ucs2_conversion_descriptor, nullptr, nullptr, nullptr, nullptr);
    }

    /* Do the conversion */
    size_t rc = iconv(ucs2_conversion_descriptor, &input, &input_size,
                      (char **)&output, &output_bytes_left);
    if (rc == (size_t)-1) {
        /* Conversion will only be partial. */
        perror("Converting to UCS-2 failed");
    }

    /* If no bytes where converted, this is equivalent to freeing buffer. */
    buffer_size -= output_bytes_left;
    buffer = (xcb_char2b_t*)srealloc(buffer, buffer_size);

    /* Return the resulting string's length */
    if (real_strlen != nullptr) {
        *real_strlen = buffer_size / sizeof(xcb_char2b_t);
    }

    return buffer;
}


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
