/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * bindings.c: Functions for configuring, finding and, running bindings.
 */
module;
#include <sys/types.h>

#include <ev.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <fmt/core.h>
#include <fmt/printf.h>
module bindings;

import std;
import utils;
import log;
import i3_commands_base;
import i3ipc;

using namespace std::literals;

/*
 * A utility function to convert a string containing the group and modifiers to
 * the corresponding bit mask.
 */
i3_event_state_mask_t event_state_from_str(char const *str) {
    /* It might be better to use strtok() here, but the simpler strstr() should
         * do for now. */
    i3_event_state_mask_t result = 0;
    if (str == nullptr || std::strcmp(str, "") == 0) {
        return result;
    }
    if (std::strstr(str, "Mod1") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_1);
    }
    if (std::strstr(str, "Mod2") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_2);
    }
    if (std::strstr(str, "Mod3") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_3);
    }
    if (std::strstr(str, "Mod4") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_4);
    }
    if (std::strstr(str, "Mod5") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_MOD_5);
    }
    if (std::strstr(str, "Control") != nullptr ||
        std::strstr(str, "Ctrl") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_CONTROL);
    }
    if (std::strstr(str, "Shift") != nullptr) {
        result |= static_cast<std::uint32_t>(xcb::xproto::key_but_mask_t::KEY_BUT_MASK_SHIFT);
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

/*
 * Returns a pointer to the Binding with the specified modifiers and
 * keycode or NULL if no such binding exists.
 *
 */
static Binding *get_binding(i3_event_state_mask_t state_filtered, bool is_release, uint16_t input_code, input_type_t input_type, std::vector<Binding> &bindings) {
    Binding *result = nullptr;

    if (!is_release) {
        /* On a press event, we first reset all B_UPON_KEYRELEASE_IGNORE_MODS
         * bindings back to B_UPON_KEYRELEASE */
        for (auto &bind : bindings) {
            if (bind.input_type != input_type) {
                continue;
            }
            if (bind.release == B_UPON_KEYRELEASE_IGNORE_MODS) {
                bind.release = B_UPON_KEYRELEASE;
            }
        }
    }

    uint32_t const xkb_group_state = (state_filtered & 0xFFFF0000);
    uint32_t const modifiers_state = (state_filtered & 0x0000FFFF);
    for (auto &bind : bindings) {
        if (bind.input_type != input_type) {
            continue;
        }

        uint32_t const xkb_group_mask = (bind.event_state_mask & 0xFFFF0000);
        bool const groups_match = ((xkb_group_state & xkb_group_mask) == xkb_group_mask);
        if (!groups_match) {
            DLOG(fmt::sprintf("skipping binding %p because XKB groups do not match\n", fmt::ptr(&bind)));
            continue;
        }

        /* For keyboard bindings where a symbol was specified by the user, we
         * need to look in the array of translated keycodes for the event’s
         * keycode */
        bool found_keycode = false;
        if (input_type == B_KEYBOARD && !bind.symbol.empty()) {
            auto input_keycode = static_cast<xcb::xproto::keycode_t>(input_code);
            for (auto &binding_keycode : bind.keycodes) {
                uint32_t const modifiers_mask = (binding_keycode.modifiers & 0x0000FFFF);
                bool const mods_match = (modifiers_mask == modifiers_state);
                DLOG(fmt::sprintf("binding_keycode->modifiers = %d, modifiers_mask = %d, modifiers_state = %d, mods_match = %s\n",
                                  binding_keycode.modifiers, modifiers_mask, modifiers_state, (mods_match ? "yes" : "no")));
                if (binding_keycode.keycode == input_keycode &&
                    (mods_match || (bind.release == B_UPON_KEYRELEASE_IGNORE_MODS && is_release))) {
                    found_keycode = true;
                    break;
                }
            }
        } else {
            /* This case is easier: The user specified a keycode */
            if (bind.keycode != input_code) {
                continue;
            }

            for (auto &binding_keycode : bind.keycodes) {
                uint32_t const modifiers_mask = (binding_keycode.modifiers & 0x0000FFFF);
                bool const mods_match = (modifiers_mask == modifiers_state);
                DLOG(fmt::sprintf("binding_keycode->modifiers = %d, modifiers_mask = %d, modifiers_state = %d, mods_match = %s\n",
                                  binding_keycode.modifiers, modifiers_mask, modifiers_state, (mods_match ? "yes" : "no")));
                if (mods_match || (bind.release == B_UPON_KEYRELEASE_IGNORE_MODS && is_release)) {
                    found_keycode = true;
                    break;
                }
            }
        }
        if (!found_keycode) {
            continue;
        }

        /* If this binding is a release binding, it matches the key which the
         * user pressed. We therefore mark it as B_UPON_KEYRELEASE_IGNORE_MODS
         * for later, so that the user can release the modifiers before the
         * actual key or button and the release event will still be matched. */
        if (bind.release == B_UPON_KEYRELEASE && !is_release) {
            bind.release = B_UPON_KEYRELEASE_IGNORE_MODS;
            DLOG(fmt::sprintf("marked bind %p as B_UPON_KEYRELEASE_IGNORE_MODS\n", fmt::ptr(&bind)));
            if (result) {
                break;
            }
            continue;
        }

        /* Check if the binding is for a press or a release event */
        if ((bind.release == B_UPON_KEYPRESS && is_release)) {
            continue;
        }

        if (is_release) {
            return &bind;
        } else if (!result) {
            /* Continue looping to mark needed B_UPON_KEYRELEASE_IGNORE_MODS. */
            result = &bind;
        }
    }

    return result;
}

/*
 * Returns a pointer to the Binding that matches the given xcb button or key
 * event or NULL if no such binding exists.
 *
 */
template <typename Event>
static Binding *get_binding_from_event(Event const *event, std::vector<Binding> &bindings) {
    bool const is_release = (event->response_type == xcb::xproto::KEY_RELEASE ||
                             event->response_type == xcb::xproto::BUTTON_RELEASE);

    input_type_t const input_type = ((event->response_type == xcb::xproto::BUTTON_RELEASE ||
                                      event->response_type == xcb::xproto::BUTTON_PRESS)
                                         ? B_MOUSE
                                         : B_KEYBOARD);

    uint16_t const event_state = event->state;
    uint16_t const event_detail = event->detail;

    /* Remove the CapsLock bit */
    i3_event_state_mask_t state_filtered = event_state & ~static_cast<std::uint32_t>(xcb::xproto::mod_mask_t::MOD_MASK_LOCK);
    DLOG(fmt::sprintf("(removed capslock, state = 0x%x)\n", state_filtered));
    /* Transform the keyboard_group from bit 13 and bit 14 into an
     * i3_xkb_group_mask_t, so that get_binding() can just bitwise AND the
     * configured bindings against |state_filtered|.
     *
     * These bits are only set because we set the XKB client flags
     * XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE and
     * XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED. See also doc/kbproto
     * section 2.2.2:
     * https://www.x.org/releases/X11R7.7/doc/kbproto/xkbproto.html#Computing_A_State_Field_from_an_XKB_State */
    switch ((event_state & 0x6000) >> 13) {
        case XCB_XKB_GROUP_1:
            state_filtered |= (I3_XKB_GROUP_MASK_1 << 16);
            break;
        case XCB_XKB_GROUP_2:
            state_filtered |= (I3_XKB_GROUP_MASK_2 << 16);
            break;
        case XCB_XKB_GROUP_3:
            state_filtered |= (I3_XKB_GROUP_MASK_3 << 16);
            break;
        case XCB_XKB_GROUP_4:
            state_filtered |= (I3_XKB_GROUP_MASK_4 << 16);
            break;
    }
    state_filtered &= ~0x6000;
    DLOG(fmt::sprintf("(transformed keyboard group, state = 0x%x)\n", state_filtered));
    return get_binding(state_filtered, is_release, event_detail, input_type, bindings);
}

Binding *get_binding_from_xcb_event(xcb::xproto::key_press_event_t const *event, std::vector<Binding> &bindings) {
    return get_binding_from_event(event, bindings);
}

Binding *get_binding_from_xcb_event(xcb::xproto::button_press_event_t const *event, std::vector<Binding> &bindings) {
    return get_binding_from_event(event, bindings);
}

/*
 * Returns a list of buttons that should be grabbed on a window.
 * This list will always contain 1–3, all higher buttons will only be returned
 * if there is a whole-window binding for it on some window in the current
 * config.
 * The list is terminated by a 0.
 */
std::set<int> bindings_get_buttons_to_grab(std::vector<Binding> &bindings) {
    /* We always return buttons 1 through 3. */
    std::set<int> buffer = {1, 2, 3};

    for (auto &bind : bindings) {
        /* We are only interested in whole window mouse bindings. */
        if (bind.input_type != B_MOUSE || !bind.whole_window) {
            continue;
        }

        long button;
        if (!utils::parse_long(bind.symbol.substr(sizeof("button") - 1), button)) {
            ELOG("Could not parse button number, skipping this binding. Please report this bug in i3.\n");
            continue;
        }

        buffer.insert(button);
    }

    return buffer;
}
