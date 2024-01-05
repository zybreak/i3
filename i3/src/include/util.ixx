/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * util.c: Utility functions, which can be useful everywhere within i3 (see
 *         also libi3).
 *
 */
module;
#include <config.h>

#include <string>

#include <err.h>
#include <xcb/xcb_keysyms.h>

export module i3:util;

import :internal;

export {

bool STARTS_WITH(const char *string, const char *needle) {
        return (strncasecmp((string), (needle), strlen((needle))) == 0);
}

#define FREE(pointer)   \
    do {                \
        free(pointer);  \
        pointer = NULL; \
    } while (0)

/**
 * Returns true if the name consists of only digits.
 *
 */
__attribute__((pure)) bool name_is_digits(const char *name);

/**
 * Set 'out' to the layout_t value for the given layout. The function
 * returns true on success or false if the passed string is not a valid
 * layout name.
 *
 */
bool layout_from_name(const char *layout_str, layout_t *out);

/**
 * Parses the workspace name as a number. Returns -1 if the workspace should be
 * interpreted as a "named workspace".
 *
 */
int ws_name_to_number(const std::string &name);

/**
 * Updates *destination with new_value and returns true if it was changed or false
 * if it was the same
 *
 */
bool update_if_necessary(uint32_t *destination, const uint32_t new_value);

/**
 * Restart i3 in-place
 * appends -a to argument list to disable autostart
 *
 */
void i3_restart(bool forget_layout);

/**
 * Escapes the given string if a pango font is currently used.
 * If the string has to be escaped, the input string will be free'd.
 *
 */
std::string pango_escape_markup(std::string input);

/**
 * Converts a string into a long using strtol().
 * This is a convenience wrapper checking the parsing result. It returns true
 * if the number could be parsed.
 */
bool parse_long(const char *str, long *out, int base);

/**
 * Slurp reads path in its entirety into buf, returning the length of the file
 * or -1 if the file could not be read. buf is set to a buffer of appropriate
 * size, or NULL if -1 is returned.
 *
 */
std::string slurp(const std::string &path);

/**
 * Convert a direction to its corresponding orientation.
 *
 */
orientation_t orientation_from_direction(direction_t direction);

/**
 * Convert a direction to its corresponding position.
 *
 */
position_t position_from_direction(direction_t direction);

/**
 * Convert orientation and position to the corresponding direction.
 *
 */
direction_t direction_from_orientation_position(orientation_t orientation, position_t position);

/**
 * Like writeall, but instead of retrying upon EAGAIN (returned when a write
 * would block), the function stops and returns the total number of bytes
 * written so far.
 *
 */
ssize_t writeall_nonblock(int fd, const void *buf, size_t count);

/**
 * All-in-one function which returns the modifier mask (XCB_MOD_MASK_*) for the
 * given keysymbol, for example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function initiates one round-trip. Use get_mod_mask_for() directly if
 * you already have the modifier mapping and key symbols.
 *
 */
uint32_t aio_get_mod_mask_for(uint32_t keysym, xcb_key_symbols_t *symbols);

/**
 * Returns true if this version of i3 is a debug build (anything which is not a
 * release version), based on the git version number.
 *
 */
bool is_debug_build() __attribute__((const));
}