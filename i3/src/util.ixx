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

/* TODO: move stuff used by more than one module into utils/ */
export {

    std::vector<std::string> add_argument(std::vector<std::string> &original, const char *opt_char, const char *opt_arg, const char *opt_name);
    
    /**
     * Restart i3 in-place
     * appends -a to argument list to disable autostart
     *
     */
    void i3_restart(bool forget_layout);

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