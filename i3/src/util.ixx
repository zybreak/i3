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
#include <err.h>
#include <xcb/xcb_keysyms.h>

export module i3:util;

import std;
import :internal;

char to_lower(char c) {
    return std::tolower(static_cast<unsigned char>(c));
}

export {
    bool STARTS_WITH(const std::string_view str, const std::string_view prefix) {
        if (prefix.length() > str.length()) {
            return false;
        }

        auto str_begin = str.begin();
        auto str_end = str_begin + prefix.size();

        return std::ranges::equal(prefix, std::ranges::subrange(str_begin, str_end),
            [](char a, char b) { return to_lower(a) == to_lower(b); });
    }

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
    bool update_if_necessary(uint32_t * destination, const uint32_t new_value);

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
    uint32_t aio_get_mod_mask_for(uint32_t keysym, xcb_key_symbols_t * symbols);

    /**
     * Returns true if this version of i3 is a debug build (anything which is not a
     * release version), based on the git version number.
     *
     */
    bool is_debug_build() __attribute__((const));

    /*
     * Converts direction to a string representation.
     *
     */
    const char *direction_to_string(direction_t direction);

    /*
     * Converts position to a string representation.
     *
     */
    const char *position_to_string(position_t position);

    /**
     * The lower 16 bits contain a xcb_key_but_mask_t, the higher 16 bits contain
     * an i3_xkb_group_mask_t. This type is necessary for the fallback logic to
     * work when handling XKB groups (see ticket #1775) and makes the code which
     * locates keybindings upon KeyPress/KeyRelease events simpler.
     */
    typedef uint32_t i3_event_state_mask_t;

    /*
     * A utility function to convert a string containing the group and modifiers to
     * the corresponding bit mask.
     */
    i3_event_state_mask_t event_state_from_str(const char *str) {
        /* It might be better to use strtok() here, but the simpler strstr() should
         * do for now. */
        i3_event_state_mask_t result = 0;
        if (str == nullptr || std::strcmp(str, "") == 0) {
            return result;
        }
        if (std::strstr(str, "Mod1") != nullptr) {
            result |= XCB_KEY_BUT_MASK_MOD_1;
        }
        if (std::strstr(str, "Mod2") != nullptr) {
            result |= XCB_KEY_BUT_MASK_MOD_2;
        }
        if (std::strstr(str, "Mod3") != nullptr) {
            result |= XCB_KEY_BUT_MASK_MOD_3;
        }
        if (std::strstr(str, "Mod4") != nullptr) {
            result |= XCB_KEY_BUT_MASK_MOD_4;
        }
        if (std::strstr(str, "Mod5") != nullptr) {
            result |= XCB_KEY_BUT_MASK_MOD_5;
        }
        if (std::strstr(str, "Control") != nullptr ||
            std::strstr(str, "Ctrl") != nullptr) {
            result |= XCB_KEY_BUT_MASK_CONTROL;
        }
        if (std::strstr(str, "Shift") != nullptr) {
            result |= XCB_KEY_BUT_MASK_SHIFT;
        }
        if (std::strstr(str, "Group1") != nullptr) {
            result |= (I3_XKB_GROUP_MASK_1 << 16);
        }
        if (std::strstr(str, "Group2") != nullptr ||
            std::strstr(str, "Mode_switch") != nullptr) {
            result |= (I3_XKB_GROUP_MASK_2 << 16);
        }
        if (std::strstr(str, "Group3") != nullptr) {
            result |= (I3_XKB_GROUP_MASK_3 << 16);
        }
        if (std::strstr(str, "Group4") != nullptr) {
            result |= (I3_XKB_GROUP_MASK_4 << 16);
        }
        return result;
    }
}