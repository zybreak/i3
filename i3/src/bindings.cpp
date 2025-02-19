/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * bindings.c: Functions for configuring, finding and, running bindings.
 */
module;
struct criteria_state;
#include <sys/types.h>

#include <xcb/xcb.h>

#include <ev.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <fmt/core.h>
#include <fmt/printf.h>
module i3;

import std;
import utils;
import log;
import i3_commands_base;
import i3ipc;

using namespace std::literals;

struct resolve {
    /* The binding which we are resolving. */
    Binding *bind;

    /* |bind|’s keysym (translated to xkb_keysym_t), e.g. XKB_KEY_R. */
    xkb_keysym_t keysym;

    /* The xkb state built from the user-provided modifiers and group. */
    xkb_state *state;

    /* Like |xkb_state|, just without the shift modifier, if shift was specified. */
    xkb_state *xkb_state_no_shift;

    /* Like |xkb_state|, but with NumLock. */
    xkb_state *xkb_state_numlock;

    /* Like |xkb_state|, but with NumLock, just without the shift modifier, if shift was specified. */
    xkb_state *xkb_state_numlock_no_shift;
};

/**
 * Launch nagbar to indicate errors in the configuration file.
 */
static void start_config_error_nagbar(bool has_errors) {
    std::string font_pattern = global.configManager->config->font->pattern;
    auto type = has_errors ? bar_type_t::TYPE_ERROR : bar_type_t::TYPE_WARNING;
    std::string text = has_errors ? "You have an error in your i3 config file!" : "Your config is outdated. Please fix the warnings to make sure everything works.";

    std::vector<button_t> buttons{};
    start_nagbar(&global.config_error_nagbar_pid, buttons, text, font_pattern, type, false);
}

/*
 * Returns the mode specified by `name` or creates a new mode and adds it to
 * the list of modes.
 *
 */
static void mode_from_name(Config *config, std::string const &name, bool pango_markup, Binding &binding) {
    /* Try to find the mode in the list of modes and return it */
    auto mode_it = config->modes.find(name);

    if (mode_it != config->modes.end()) {
        mode_it->second.bindings.push_back(binding);
        return;
    }

    /* If the mode was not found, create a new one */
    Mode mode{name, pango_markup};
    mode.bindings.push_back(binding);

    config->modes.insert_or_assign(name, mode);
}

/*
 * Adds a binding from config parameters given as strings and returns a
 * pointer to the binding structure. Returns NULL if the input code could not
 * be parsed.
 *
 */
void configure_binding(Config *config, std::string_view const bindtype, std::string_view const modifiers, std::string_view const input_code,
                       bool release, bool border, bool whole_window,
                       bool exclude_titlebar, std::string_view const command, std::string_view const modename,
                       bool pango_markup) {
    DLOG(fmt::sprintf("Binding bindtype %s, modifiers %s, input code %s, release %s\n", bindtype, modifiers, input_code, release ? "true" : "false"));

    Binding new_binding{};
    new_binding.release = (release ? B_UPON_KEYRELEASE : B_UPON_KEYPRESS);
    new_binding.border = border;
    new_binding.whole_window = whole_window;
    new_binding.exclude_titlebar = exclude_titlebar;
    if (strcmp(bindtype.data(), "bindsym") == 0) {
        new_binding.input_type = (strncasecmp(input_code.data(), "button", (sizeof("button") - 1)) == 0
                                      ? B_MOUSE
                                      : B_KEYBOARD);

        new_binding.symbol = input_code.data();
    } else {
        long keycode;
        if (!utils::parse_long(input_code.data(), &keycode, 10)) {
            ELOG(fmt::sprintf("Could not parse \"%s\" as an input code, ignoring this binding.\n", input_code.data()));
            return;
        }

        new_binding.keycode = keycode;
        new_binding.input_type = B_KEYBOARD;
    }
    new_binding.command = command;
    new_binding.event_state_mask = event_state_from_str(modifiers.data());
    int group_bits_set = 0;
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_1) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_2) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_3) {
        group_bits_set++;
    }
    if ((new_binding.event_state_mask >> 16) & I3_XKB_GROUP_MASK_4) {
        group_bits_set++;
    }
    if (group_bits_set > 1) {
        ELOG("Keybinding has more than one Group specified, but your X server is always in precisely one group. The keybinding can never trigger.\n");
    }

    mode_from_name(config, modename.data(), pango_markup, new_binding);
}

static bool binding_in_current_group(Binding const &bind) {
    /* If no bits are set, the binding should be installed in every group. */
    if ((bind.event_state_mask >> 16) == I3_XKB_GROUP_MASK_ANY) {
        return true;
    }
    switch (global.xkb->xkb_current_group) {
        case XCB_XKB_GROUP_1:
            return ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_1);
        case XCB_XKB_GROUP_2:
            return ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_2);
        case XCB_XKB_GROUP_3:
            return ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_3);
        case XCB_XKB_GROUP_4:
            return ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_4);
        default:
            ELOG(fmt::sprintf("BUG: xkb_current_group (= %d) outside of [XCB_XKB_GROUP_1..XCB_XKB_GROUP_4]\n", global.xkb->xkb_current_group));
            return false;
    }
}

static void grab_keycode_for_binding(x_connection *conn, Binding &bind, uint32_t keycode) {
    /* Grab the key in all combinations */
    auto mods = (bind.event_state_mask & 0xFFFF);
    DLOG(fmt::sprintf("Binding %p Grabbing keycode %d with event state mask 0x%x (mods 0x%x)\n", fmt::ptr(&bind), keycode, bind.event_state_mask, mods));

    /* Also bind the key with active NumLock */
    /* Also bind the key with active CapsLock */
    /* Also bind the key with active NumLock+CapsLock */
    uint32_t mod_list[] = {mods, mods | global.x->xcb_numlock_mask, mods | XCB_MOD_MASK_LOCK, mods | global.x->xcb_numlock_mask | XCB_MOD_MASK_LOCK};

    std::ranges::for_each(mod_list, [&](uint32_t mod) {
        conn->grab_key(
            0,
            global.x->root,
            mod,
            keycode,
            XCB_GRAB_MODE_ASYNC,
            XCB_GRAB_MODE_ASYNC);
    });
}

/*
 * Ungrabs all keys, to be called before re-grabbing the keys because of a
 * mapping_notify event or a configuration file reload
 *
 */
void ungrab_all_keys(x_connection *conn) {
    DLOG("Ungrabbing all keys\n");

    conn->ungrab_key(XCB_GRAB_ANY, global.x->root, XCB_BUTTON_MASK_ANY);
}

/*
 * Grab the bound keys (tell X to send us keypress events for those keycodes)
 *
 */
void grab_all_keys(x_connection *conn) {
    for (auto &bind : global.configManager->config->current_mode()->bindings) {
        if (bind.input_type != B_KEYBOARD) {
            continue;
        }

        if (!binding_in_current_group(bind)) {
            continue;
        }

        /* The easy case: the user specified a keycode directly. */
        if (bind.keycode > 0) {
            grab_keycode_for_binding(conn, bind, bind.keycode);
            continue;
        }

        for (auto const &binding_keycode : bind.keycodes) {
            int const keycode = binding_keycode.keycode;
            auto mods = (binding_keycode.modifiers & 0xFFFF);
            DLOG(fmt::sprintf("Binding %p Grabbing keycode %d with mods %d\n", fmt::ptr(&bind), keycode, mods));
            conn->grab_key(0, global.x->root, mods, keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
        }
    }
}

/*
 * Release the button grabs on all managed windows and regrab them,
 * reevaluating which buttons need to be grabbed.
 *
 */
void regrab_all_buttons(x_connection *conn) {
    auto buttons = bindings_get_buttons_to_grab();
    conn->grab_server();

    for (auto const &con : global.all_cons) {
        if (con->get_window() == nullptr) {
            continue;
        }

        conn->ungrab_button(XCB_BUTTON_INDEX_ANY, con->get_window()->id, XCB_BUTTON_MASK_ANY);
        xcb_grab_buttons(con->get_window()->id, buttons);
    }

    conn->ungrab_server();
}

/*
 * Returns a pointer to the Binding with the specified modifiers and
 * keycode or NULL if no such binding exists.
 *
 */
static Binding *get_binding(i3_event_state_mask_t state_filtered, bool is_release, uint16_t input_code, input_type_t input_type) {
    Binding *result = nullptr;

    if (!is_release) {
        /* On a press event, we first reset all B_UPON_KEYRELEASE_IGNORE_MODS
         * bindings back to B_UPON_KEYRELEASE */
        for (auto &bind : global.configManager->config->current_mode()->bindings) {
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
    for (auto &bind : global.configManager->config->current_mode()->bindings) {
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
            auto input_keycode = static_cast<xcb_keycode_t>(input_code);
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
Binding *get_binding_from_xcb_event(xcb_generic_event_t *event) {
    bool const is_release = (event->response_type == XCB_KEY_RELEASE ||
                             event->response_type == XCB_BUTTON_RELEASE);

    input_type_t const input_type = ((event->response_type == XCB_BUTTON_RELEASE ||
                                      event->response_type == XCB_BUTTON_PRESS)
                                         ? B_MOUSE
                                         : B_KEYBOARD);

    uint16_t const event_state = ((xcb_key_press_event_t *)event)->state;
    uint16_t const event_detail = ((xcb_key_press_event_t *)event)->detail;

    /* Remove the CapsLock bit */
    i3_event_state_mask_t state_filtered = event_state & ~XCB_MOD_MASK_LOCK;
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
    return get_binding(state_filtered, is_release, event_detail, input_type);
}

/*
 * add_keycode_if_matches is called for each keycode in the keymap and will add
 * the keycode to |data->bind| if the keycode can result in the keysym
 * |data->resolving|.
 *
 */
static void add_keycode_if_matches(xkb_keymap *keymap, xkb_keycode_t key, void *data) {
    const struct resolve *resolving = (struct resolve *)data;
    struct xkb_state *numlock_state = resolving->xkb_state_numlock;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(resolving->state, key);
    if (sym != resolving->keysym) {
        /* Check if Shift was specified, and try resolving the symbol without
         * shift, so that “bindsym $mod+Shift+a nop” actually works. */
        xkb_layout_index_t const layout = xkb_state_key_get_layout(resolving->state, key);
        if (layout == XKB_LAYOUT_INVALID) {
            return;
        }
        if (xkb_state_key_get_level(resolving->state, key, layout) > 1) {
            return;
        }
        /* Skip the Shift fallback for keypad keys, otherwise one cannot bind
         * KP_1 independent of KP_End. */
        if (sym >= XKB_KEY_KP_Space && sym <= XKB_KEY_KP_Equal) {
            return;
        }
        numlock_state = resolving->xkb_state_numlock_no_shift;
        sym = xkb_state_key_get_one_sym(resolving->xkb_state_no_shift, key);
        if (sym != resolving->keysym) {
            return;
        }
    }
    Binding *bind = resolving->bind;

    xcb_keycode_t code = key;
    i3_event_state_mask_t mods = bind->event_state_mask;
    bind->keycodes.emplace_back(code, mods);

    /* Also bind the key with active CapsLock */
    xcb_keycode_t code1 = key;
    i3_event_state_mask_t mods1 = bind->event_state_mask | XCB_MOD_MASK_LOCK;
    bind->keycodes.emplace_back(code1, mods1);

    /* If this binding is not explicitly for NumLock, check whether we need to
     * add a fallback. */
    if ((bind->event_state_mask & global.x->xcb_numlock_mask) != global.x->xcb_numlock_mask) {
        /* Check whether the keycode results in the same keysym when NumLock is
         * active. If so, grab the key with NumLock as well, so that users don’t
         * need to duplicate every key binding with an additional Mod2 specified.
         */
        xkb_keysym_t sym_numlock = xkb_state_key_get_one_sym(numlock_state, key);
        if (sym_numlock == resolving->keysym) {
            /* Also bind the key with active NumLock */
            xcb_keycode_t code2 = key;
            i3_event_state_mask_t mods2 = bind->event_state_mask | global.x->xcb_numlock_mask;
            bind->keycodes.emplace_back(code2, mods2);

            /* Also bind the key with active NumLock+CapsLock */
            xcb_keycode_t code3 = key;
            i3_event_state_mask_t mods3 = bind->event_state_mask | global.x->xcb_numlock_mask | XCB_MOD_MASK_LOCK;
            bind->keycodes.emplace_back(code3, mods3);
        } else {
            DLOG(fmt::sprintf("Skipping automatic numlock fallback, key %d resolves to 0x%x with numlock\n",
                              key, sym_numlock));
        }
    }
}

/*
 * Translates keysymbols to keycodes for all bindings which use keysyms.
 *
 */
void translate_keysyms(Keymap const &keymap) {
    try {
        auto resolved = keymap.resolve();

        xkb_state *dummy_state = resolved->dummy_state;
        xkb_state *dummy_state_no_shift = resolved->dummy_state_no_shift;
        xkb_state *dummy_state_numlock = resolved->dummy_state_numlock;
        xkb_state *dummy_state_numlock_no_shift = resolved->dummy_state_numlock_no_shift;
        bool has_errors = false;

        for (auto &bind : global.configManager->config->current_mode()->bindings) {
            if (bind.input_type == B_MOUSE) {
                long button;
                if (!utils::parse_long(bind.symbol.c_str() + (sizeof("button") - 1), &button, 10)) {
                    ELOG(fmt::sprintf("Could not translate string to button: \"%s\"\n", bind.symbol));
                }

                xcb_keycode_t key = button;
                bind.keycode = key;
                DLOG(fmt::sprintf("Binding Mouse button, Keycode = %d\n", key));
            }

            xkb_layout_index_t group = XCB_XKB_GROUP_1;
            if ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_2) {
                group = XCB_XKB_GROUP_2;
            } else if ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_3) {
                group = XCB_XKB_GROUP_3;
            } else if ((bind.event_state_mask >> 16) & I3_XKB_GROUP_MASK_4) {
                group = XCB_XKB_GROUP_4;
            }

            DLOG(fmt::sprintf("Binding %p group = %d, event_state_mask = %d, &2 = %s, &3 = %s, &4 = %s\n",
                              fmt::ptr(&bind),
                              group,
                              bind.event_state_mask,
                              (bind.event_state_mask & I3_XKB_GROUP_MASK_2) ? "yes" : "no",
                              (bind.event_state_mask & I3_XKB_GROUP_MASK_3) ? "yes" : "no",
                              (bind.event_state_mask & I3_XKB_GROUP_MASK_4) ? "yes" : "no"));
            (void)xkb_state_update_mask(
                dummy_state,
                (bind.event_state_mask & 0x1FFF) /* xkb_mod_mask_t base_mods, */,
                0 /* xkb_mod_mask_t latched_mods, */,
                0 /* xkb_mod_mask_t locked_mods, */,
                0 /* xkb_layout_index_t base_group, */,
                0 /* xkb_layout_index_t latched_group, */,
                group /* xkb_layout_index_t locked_group, */);

            (void)xkb_state_update_mask(
                dummy_state_no_shift,
                (bind.event_state_mask & 0x1FFF) ^ XCB_KEY_BUT_MASK_SHIFT /* xkb_mod_mask_t base_mods, */,
                0 /* xkb_mod_mask_t latched_mods, */,
                0 /* xkb_mod_mask_t locked_mods, */,
                0 /* xkb_layout_index_t base_group, */,
                0 /* xkb_layout_index_t latched_group, */,
                group /* xkb_layout_index_t locked_group, */);

            (void)xkb_state_update_mask(
                dummy_state_numlock,
                (bind.event_state_mask & 0x1FFF) | global.x->xcb_numlock_mask /* xkb_mod_mask_t base_mods, */,
                0 /* xkb_mod_mask_t latched_mods, */,
                0 /* xkb_mod_mask_t locked_mods, */,
                0 /* xkb_layout_index_t base_group, */,
                0 /* xkb_layout_index_t latched_group, */,
                group /* xkb_layout_index_t locked_group, */);

            (void)xkb_state_update_mask(
                dummy_state_numlock_no_shift,
                ((bind.event_state_mask & 0x1FFF) | global.x->xcb_numlock_mask) ^ XCB_KEY_BUT_MASK_SHIFT /* xkb_mod_mask_t base_mods, */,
                0 /* xkb_mod_mask_t latched_mods, */,
                0 /* xkb_mod_mask_t locked_mods, */,
                0 /* xkb_layout_index_t base_group, */,
                0 /* xkb_layout_index_t latched_group, */,
                group /* xkb_layout_index_t locked_group, */);

            if (bind.keycode > 0) {
                /* We need to specify modifiers for the keycode binding (numlock
                 * fallback). */
                bind.keycodes.clear();

                Binding *bind1 = &bind;
                xcb_keycode_t code = bind.keycode;
                i3_event_state_mask_t mods = bind.event_state_mask;
                bind1->keycodes.emplace_back(code, mods);

                /* Also bind the key with active CapsLock */
                Binding *bind2 = &bind;
                xcb_keycode_t code1 = bind.keycode;
                i3_event_state_mask_t mods1 = bind.event_state_mask | XCB_MOD_MASK_LOCK;
                bind2->keycodes.emplace_back(code1, mods1);

                /* If this binding is not explicitly for NumLock, check whether we need to
                 * add a fallback. */
                if ((bind.event_state_mask & global.x->xcb_numlock_mask) != global.x->xcb_numlock_mask) {
                    /* Check whether the keycode results in the same keysym when NumLock is
                     * active. If so, grab the key with NumLock as well, so that users don’t
                     * need to duplicate every key binding with an additional Mod2 specified.
                     */
                    xkb_keysym_t sym = xkb_state_key_get_one_sym(dummy_state, bind.keycode);
                    xkb_keysym_t sym_numlock = xkb_state_key_get_one_sym(dummy_state_numlock, bind.keycode);
                    if (sym == sym_numlock) {
                        /* Also bind the key with active NumLock */
                        Binding *bind3 = &bind;
                        xcb_keycode_t code2 = bind.keycode;
                        i3_event_state_mask_t mods2 = bind.event_state_mask | global.x->xcb_numlock_mask;
                        bind3->keycodes.emplace_back(code2, mods2);

                        /* Also bind the key with active NumLock+CapsLock */
                        Binding *bind4 = &bind;
                        xcb_keycode_t code3 = bind.keycode;
                        i3_event_state_mask_t mods3 = bind.event_state_mask | global.x->xcb_numlock_mask | XCB_MOD_MASK_LOCK;
                        bind4->keycodes.emplace_back(code3, mods3);
                    } else {
                        DLOG(fmt::sprintf("Skipping automatic numlock fallback, key %d resolves to 0x%x with numlock\n",
                                          bind.keycode, sym_numlock));
                    }
                }

                continue;
            }

            /* We need to translate the symbol to a keycode */
            xkb_keysym_t const keysym = xkb_keysym_from_name(bind.symbol.c_str(), XKB_KEYSYM_NO_FLAGS);
            if (keysym == XKB_KEY_NoSymbol) {
                ELOG(fmt::sprintf("Could not translate string to key symbol: \"%s\"\n",
                                  bind.symbol));
                continue;
            }

            resolve resolving = {
                .bind = &bind,
                .keysym = keysym,
                .state = dummy_state,
                .xkb_state_no_shift = dummy_state_no_shift,
                .xkb_state_numlock = dummy_state_numlock,
                .xkb_state_numlock_no_shift = dummy_state_numlock_no_shift,
            };

            bind.keycodes.clear();

            xkb_keymap_key_for_each(keymap.keymap, add_keycode_if_matches, &resolving);
            std::string keycodes{};
            int num_keycodes = 0;
            for (auto &binding_keycode : bind.keycodes) {
                keycodes.append(fmt::sprintf(" %d", binding_keycode.keycode));
                num_keycodes++;

                /* check for duplicate bindings */
                for (auto &check : global.configManager->config->current_mode()->bindings) {
                    if (check == bind) {
                        continue;
                    }
                    if (!check.symbol.empty()) {
                        continue;
                    }
                    if (check.keycode != binding_keycode.keycode ||
                        check.event_state_mask != binding_keycode.modifiers ||
                        check.release != bind.release) {
                        continue;
                    }
                    has_errors = true;
                    ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  keysym = %s, keycode = %d, state_mask = 0x%x\n", bind.symbol, check.keycode, bind.event_state_mask));
                }
            }
            DLOG(fmt::sprintf("state=0x%x, cfg=\"%s\", sym=0x%x → keycodes%s (%d)\n",
                              bind.event_state_mask, bind.symbol, keysym, keycodes, num_keycodes));
        }

        if (has_errors) {
            start_config_error_nagbar(true);
        }
    } catch (std::runtime_error &e) {
        ELOG(e.what());
    }
}

/*
 * Switches the key bindings to the given mode, if the mode exists
 *
 */
void switch_mode(std::string const new_mode) {
    DLOG(fmt::sprintf("Switching to mode %s\n", new_mode));

    auto it = global.configManager->config->modes.find(new_mode);
    if (it == global.configManager->config->modes.end()) {
        ELOG("Mode not found\n");
        return;
    }

    auto &mode = it->second;

    ungrab_all_keys(*global.x);
    global.configManager->config->_current_mode = it->first;
    translate_keysyms(global.xkb->keymap());
    grab_all_keys(*global.x);
    regrab_all_buttons(*global.x);

    /* Reset all B_UPON_KEYRELEASE_IGNORE_MODS bindings to avoid possibly
     * activating one of them. */
    for (auto &bind : global.configManager->config->current_mode()->bindings) {
        if (bind.release == B_UPON_KEYRELEASE_IGNORE_MODS) {
            bind.release = B_UPON_KEYRELEASE;
        }
    }

    std::string event_msg = fmt::sprintf(R"({"change":"%s", "pango_markup":%s})", it->first, (mode.pango_markup ? "true" : "false"));

    global.ipcManager->ipc_send_event("mode", i3ipc::EVENT_MODE, event_msg);
}

/*
 * Reorders bindings by event_state_mask descendingly so that get_binding()
 * correctly matches more specific bindings before more generic bindings. Take
 * the following binding configuration as an example:
 *
 *   bindsym n nop lower-case n pressed
 *   bindsym Shift+n nop upper-case n pressed
 *
 * Without reordering, the first binding’s event_state_mask of 0x0 would match
 * the actual event_stat_mask of 0x1 and hence trigger instead of the second
 * keybinding.
 *
 */
void reorder_bindings(Config *config) {
    std::ranges::for_each(config->modes, [](auto &mode) {
        std::ranges::sort(mode.second.bindings, [](auto const &first, auto const &second) {
            if (first.event_state_mask < second.event_state_mask) {
                return 1;
            } else if (first.event_state_mask == second.event_state_mask) {
                return 0;
            } else {
                return -1;
            }
        });
    });
}

/*
 * Returns true if a is a key binding for the same key as b.
 *
 */
static bool binding_same_key(Binding *a, Binding *b) {
    /* Check if the input types are different */
    if (a->input_type != b->input_type) {
        return false;
    }

    /* Check if one is using keysym while the other is using bindsym. */
    if ((a->symbol.empty() && !b->symbol.empty()) ||
        (!a->symbol.empty() && b->symbol.empty())) {
        return false;
    }

    /* If a is NULL, b has to be NULL, too (see previous conditional).
     * If the keycodes differ, it can't be a duplicate. */
    if (!a->symbol.empty() && a->symbol != b->symbol) {
        return false;
    }

    /* Check if the keycodes or modifiers are different. If so, they
     * can't be duplicate */
    if (a->keycode != b->keycode ||
        a->event_state_mask != b->event_state_mask ||
        a->release != b->release) {
        return false;
    }

    return true;
}

/*
 * Checks for duplicate key bindings (the same keycode or keysym is configured
 * more than once). If a duplicate binding is found, a message is printed to
 * stderr and the has_errors variable is set to true, which will start
 * i3-nagbar.
 *
 */
bool has_duplicate_bindings(Config *config) {
    bool has_errors = false;
    for (auto &current : config->current_mode()->bindings) {
        for (auto &bind : config->current_mode()->bindings) {
            /* Abort when we reach the current keybinding, only check the
             * bindings before */
            if (bind == current) {
                break;
            }

            if (!binding_same_key(&bind, &current)) {
                continue;
            }

            has_errors = true;
            if (current.keycode != 0) {
                ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keycode %d, command \"%s\"\n",
                                  current.event_state_mask, current.keycode, current.command));
            } else {
                ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keysym %s, command \"%s\"\n",
                                  current.event_state_mask, current.symbol, current.command));
            }
        }
    }

    return has_errors;
}

/*
 * Runs the given binding and handles parse errors. If con is passed, it will
 * execute the command binding with that container selected by criteria.
 * Returns a CommandResult for running the binding's command. Free with
 * command_result_free().
 *
 */
CommandResult run_binding(Binding *bind, Con *con) {
    std::string command;

    /* We need to copy the binding and command since “reload” may be part of
     * the command, and then the memory that bind points to may not contain the
     * same data anymore. */
    if (con == nullptr) {
        command = bind->command;
    } else {
        command = fmt::format("[con_id=\"{}\"] {}", fmt::ptr(con), bind->command);
    }

    /* The "mode" command might change the current mode, so back it up to
     * correctly produce an event later. */
    std::string &modename = global.configManager->config->current_mode()->name;

    Binding bind_cp = *bind;
    auto commandsApplier = CommandsApplier{};
    CommandResult result = parse_command(command, {}, &commandsApplier);

    if (result.needs_tree_render) {
        tree_render();
    }

    if (result.parse_error) {
        std::vector<button_t> buttons{};
        auto prompt = std::string("The configured command for this shortcut could not be run successfully.");
        start_nagbar(&global.command_error_nagbar_pid, buttons, prompt, global.configManager->config->font->pattern, bar_type_t::TYPE_ERROR, false);
    }

    global.ipcManager->ipc_send_binding_event("run", &bind_cp, modename.c_str());

    return result;
}

/*
 * Returns a list of buttons that should be grabbed on a window.
 * This list will always contain 1–3, all higher buttons will only be returned
 * if there is a whole-window binding for it on some window in the current
 * config.
 * The list is terminated by a 0.
 */
std::set<int> bindings_get_buttons_to_grab() {
    /* We always return buttons 1 through 3. */
    std::set<int> buffer = {1, 2, 3};

    for (auto &bind : global.configManager->config->current_mode()->bindings) {
        /* We are only interested in whole window mouse bindings. */
        if (bind.input_type != B_MOUSE || !bind.whole_window) {
            continue;
        }

        long button;
        if (!utils::parse_long(bind.symbol.c_str() + (sizeof("button") - 1), &button, 10)) {
            ELOG("Could not parse button number, skipping this binding. Please report this bug in i3.\n");
            continue;
        }

        buffer.insert(button);
    }

    return buffer;
}
