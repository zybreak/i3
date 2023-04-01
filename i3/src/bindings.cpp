/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * bindings.c: Functions for configuring, finding and, running bindings.
 */
#include <cassert>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

#include <xcb/xcb.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "wrapper.h"

#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "xcb.h"
#include "i3.h"
#include "commands_parser.h"
#include "bindings.h"
#include "global.h"
#include "config_parser.h"
#include "nagbar.h"

#include <cmath>

#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <fmt/core.h>
#include <set>

static struct xkb_context *xkb_context;
static struct xkb_keymap *xkb_keymap;

/*
 * The name of the default mode.
 *
 */
const char *DEFAULT_BINDING_MODE = "default";

/*
 * Returns the mode specified by `name` or creates a new mode and adds it to
 * the list of modes.
 *
 */
static struct Mode *mode_from_name(const std::string &name, bool pango_markup) {
    /* Try to find the mode in the list of modes and return it */
    for (auto &mode : modes) {
        if (strcmp(mode->name.c_str(), name.c_str()) == 0) {
            return mode.get();
        }
    }

    /* If the mode was not found, create a new one */
    Mode *ptr;
    auto mode = std::make_unique<Mode>(name);
    mode->pango_markup = pango_markup;
    ptr = mode.get();
    modes.insert(modes.begin(), std::move(mode));

    return ptr;
}

/*
 * Adds a binding from config parameters given as strings and returns a
 * pointer to the binding structure. Returns NULL if the input code could not
 * be parsed.
 *
 */
void configure_binding(const std::string_view &bindtype, const std::string_view &modifiers, const std::string_view &input_code,
                           bool release, bool border, bool whole_window,
                           bool exclude_titlebar, const std::string_view &command, const std::string_view &modename,
                           bool pango_markup) {
    auto new_binding = std::make_unique<Binding>();
    DLOG(fmt::sprintf("Binding %p bindtype %s, modifiers %s, input code %s, release %s\n",  (void*)new_binding.get(), bindtype, modifiers, input_code, release));
    new_binding->release = (release ? B_UPON_KEYRELEASE : B_UPON_KEYPRESS);
    new_binding->border = border;
    new_binding->whole_window = whole_window;
    new_binding->exclude_titlebar = exclude_titlebar;
    if (strcmp(bindtype.data(), "bindsym") == 0) {
        new_binding->input_type = (strncasecmp(input_code.data(), "button", (sizeof("button") - 1)) == 0
                                       ? B_MOUSE
                                       : B_KEYBOARD);

        new_binding->symbol = sstrdup(input_code.data());
    } else {
        long keycode;
        if (!parse_long(input_code.data(), &keycode, 10)) {
             ELOG(fmt::sprintf("Could not parse \"%s\" as an input code, ignoring this binding.\n", input_code.data()));
            return;
        }

        new_binding->keycode = keycode;
        new_binding->input_type = B_KEYBOARD;
    }
    new_binding->command = command;
    new_binding->event_state_mask = event_state_from_str(modifiers.data());
    int group_bits_set = 0;
    if ((new_binding->event_state_mask >> 16) & I3_XKB_GROUP_MASK_1)
        group_bits_set++;
    if ((new_binding->event_state_mask >> 16) & I3_XKB_GROUP_MASK_2)
        group_bits_set++;
    if ((new_binding->event_state_mask >> 16) & I3_XKB_GROUP_MASK_3)
        group_bits_set++;
    if ((new_binding->event_state_mask >> 16) & I3_XKB_GROUP_MASK_4)
        group_bits_set++;
    if (group_bits_set > 1)
        ELOG("Keybinding has more than one Group specified, but your X server is always in precisely one group. The keybinding can never trigger.\n");

    struct Mode *mode = mode_from_name(modename.data(), pango_markup);

    mode->bindings.push_back(std::move(new_binding));
}

static bool binding_in_current_group(const Binding *bind) {
    /* If no bits are set, the binding should be installed in every group. */
    if ((bind->event_state_mask >> 16) == I3_XKB_GROUP_MASK_ANY)
        return true;
    switch (xkb_current_group) {
        case XCB_XKB_GROUP_1:
            return ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_1);
        case XCB_XKB_GROUP_2:
            return ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_2);
        case XCB_XKB_GROUP_3:
            return ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_3);
        case XCB_XKB_GROUP_4:
            return ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_4);
        default:
            ELOG(fmt::sprintf("BUG: xkb_current_group (= %d) outside of [XCB_XKB_GROUP_1..XCB_XKB_GROUP_4]\n",  xkb_current_group));
            return false;
    }
}

static void grab_keycode_for_binding(x_connection *conn, Binding *bind, uint32_t keycode) {
    /* Grab the key in all combinations */
    auto mods = (bind->event_state_mask & 0xFFFF);
    DLOG(fmt::sprintf("Binding %p Grabbing keycode %d with event state mask 0x%x (mods 0x%x)\n", (void*)bind, keycode, bind->event_state_mask, mods));

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
                     XCB_GRAB_MODE_SYNC,
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
    for (auto &bind : current_mode->bindings) {
        if (bind->input_type != B_KEYBOARD)
            continue;

        if (!binding_in_current_group(bind.get()))
            continue;

        /* The easy case: the user specified a keycode directly. */
        if (bind->keycode > 0) {
            grab_keycode_for_binding(conn, bind.get(), bind->keycode);
            continue;
        }

        for (auto &binding_keycode : bind->keycodes_head) {
            const int keycode = binding_keycode->keycode;
            auto mods = (binding_keycode->modifiers & 0xFFFF);
            DLOG(fmt::sprintf("Binding %p Grabbing keycode %d with mods %d\n",  (void*)bind.get(), keycode, mods));
            conn->grab_key(0, global.x->root, mods, keycode, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC);
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

    for (const auto &con : all_cons) {
        if (con->window == nullptr)
            continue;

        conn->ungrab_button(XCB_BUTTON_INDEX_ANY, con->window->id, XCB_BUTTON_MASK_ANY);
        xcb_grab_buttons(con->window->id, buttons);
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
        for (auto &bind : current_mode->bindings) {
            if (bind->input_type != input_type)
                continue;
            if (bind->release == B_UPON_KEYRELEASE_IGNORE_MODS)
                bind->release = B_UPON_KEYRELEASE;
        }
    }

    const uint32_t xkb_group_state = (state_filtered & 0xFFFF0000);
    const uint32_t modifiers_state = (state_filtered & 0x0000FFFF);
    for (auto &bind : current_mode->bindings) {
        if (bind->input_type != input_type) {
            continue;
        }

        const uint32_t xkb_group_mask = (bind->event_state_mask & 0xFFFF0000);
        const bool groups_match = ((xkb_group_state & xkb_group_mask) == xkb_group_mask);
        if (!groups_match) {
            DLOG(fmt::sprintf("skipping binding %p because XKB groups do not match\n",  (void*)bind.get()));
            continue;
        }

        /* For keyboard bindings where a symbol was specified by the user, we
         * need to look in the array of translated keycodes for the event’s
         * keycode */
        bool found_keycode = false;
        if (input_type == B_KEYBOARD && bind->symbol != nullptr) {
            auto input_keycode = (xcb_keycode_t)input_code;
            for (auto &binding_keycode : bind->keycodes_head) {
                const uint32_t modifiers_mask = (binding_keycode->modifiers & 0x0000FFFF);
                const bool mods_match = (modifiers_mask == modifiers_state);
                DLOG(fmt::sprintf("binding_keycode->modifiers = %d, modifiers_mask = %d, modifiers_state = %d, mods_match = %s\n",
                     binding_keycode->modifiers, modifiers_mask, modifiers_state, (mods_match ? "yes" : "no")));
                if (binding_keycode->keycode == input_keycode &&
                    (mods_match || (bind->release == B_UPON_KEYRELEASE_IGNORE_MODS && is_release))) {
                    found_keycode = true;
                    break;
                }
            }
        } else {
            /* This case is easier: The user specified a keycode */
            if (bind->keycode != input_code) {
                continue;
            }

            for (auto &binding_keycode : bind->keycodes_head) {
                const uint32_t modifiers_mask = (binding_keycode->modifiers & 0x0000FFFF);
                const bool mods_match = (modifiers_mask == modifiers_state);
                DLOG(fmt::sprintf("binding_keycode->modifiers = %d, modifiers_mask = %d, modifiers_state = %d, mods_match = %s\n",
                     binding_keycode->modifiers, modifiers_mask, modifiers_state, (mods_match ? "yes" : "no")));
                if (mods_match || (bind->release == B_UPON_KEYRELEASE_IGNORE_MODS && is_release)) {
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
        if (bind->release == B_UPON_KEYRELEASE && !is_release) {
            bind->release = B_UPON_KEYRELEASE_IGNORE_MODS;
            DLOG(fmt::sprintf("marked bind %p as B_UPON_KEYRELEASE_IGNORE_MODS\n",  (void*)bind.get()));
            if (result) {
                break;
            }
            continue;
        }

        /* Check if the binding is for a press or a release event */
        if ((bind->release == B_UPON_KEYPRESS && is_release)) {
            continue;
        }

        if (is_release) {
            return bind.get();
        } else if (!result) {
            /* Continue looping to mark needed B_UPON_KEYRELEASE_IGNORE_MODS. */
            result = bind.get();
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
    const bool is_release = (event->response_type == XCB_KEY_RELEASE ||
                             event->response_type == XCB_BUTTON_RELEASE);

    const input_type_t input_type = ((event->response_type == XCB_BUTTON_RELEASE ||
                                      event->response_type == XCB_BUTTON_PRESS)
                                         ? B_MOUSE
                                         : B_KEYBOARD);

    const uint16_t event_state = ((xcb_key_press_event_t *)event)->state;
    const uint16_t event_detail = ((xcb_key_press_event_t *)event)->detail;

    /* Remove the CapsLock bit */
    i3_event_state_mask_t state_filtered = event_state & ~XCB_MOD_MASK_LOCK;
    DLOG(fmt::sprintf("(removed capslock, state = 0x%x)\n",  state_filtered));
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
    DLOG(fmt::sprintf("(transformed keyboard group, state = 0x%x)\n",  state_filtered));
    return get_binding(state_filtered, is_release, event_detail, input_type);
}

struct resolve {
    /* The binding which we are resolving. */
    Binding *bind;

    /* |bind|’s keysym (translated to xkb_keysym_t), e.g. XKB_KEY_R. */
    xkb_keysym_t keysym;

    /* The xkb state built from the user-provided modifiers and group. */
    struct xkb_state *xkb_state;

    /* Like |xkb_state|, just without the shift modifier, if shift was specified. */
    struct xkb_state *xkb_state_no_shift;

    /* Like |xkb_state|, but with NumLock. */
    struct xkb_state *xkb_state_numlock;

    /* Like |xkb_state|, but with NumLock, just without the shift modifier, if shift was specified. */
    struct xkb_state *xkb_state_numlock_no_shift;
};

void ADD_TRANSLATED_KEY(Binding *bind, xcb_keycode_t code, i3_event_state_mask_t mods) {
    auto *binding_keycode = new Binding_Keycode();
    binding_keycode->modifiers = (mods);
    binding_keycode->keycode = (code);
    bind->keycodes_head.push_back(binding_keycode);
}

/*
 * add_keycode_if_matches is called for each keycode in the keymap and will add
 * the keycode to |data->bind| if the keycode can result in the keysym
 * |data->resolving|.
 *
 */
static void add_keycode_if_matches(struct xkb_keymap *keymap, xkb_keycode_t key, void *data) {
    const struct resolve *resolving = (struct resolve*)data;
    struct xkb_state *numlock_state = resolving->xkb_state_numlock;
    xkb_keysym_t sym = xkb_state_key_get_one_sym(resolving->xkb_state, key);
    if (sym != resolving->keysym) {
        /* Check if Shift was specified, and try resolving the symbol without
         * shift, so that “bindsym $mod+Shift+a nop” actually works. */
        const xkb_layout_index_t layout = xkb_state_key_get_layout(resolving->xkb_state, key);
        if (layout == XKB_LAYOUT_INVALID)
            return;
        if (xkb_state_key_get_level(resolving->xkb_state, key, layout) > 1)
            return;
        /* Skip the Shift fallback for keypad keys, otherwise one cannot bind
         * KP_1 independent of KP_End. */
        if (sym >= XKB_KEY_KP_Space && sym <= XKB_KEY_KP_Equal)
            return;
        numlock_state = resolving->xkb_state_numlock_no_shift;
        sym = xkb_state_key_get_one_sym(resolving->xkb_state_no_shift, key);
        if (sym != resolving->keysym)
            return;
    }
    Binding *bind = resolving->bind;

    ADD_TRANSLATED_KEY(bind, key, bind->event_state_mask);

    /* Also bind the key with active CapsLock */
    ADD_TRANSLATED_KEY(bind, key, bind->event_state_mask | XCB_MOD_MASK_LOCK);

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
            ADD_TRANSLATED_KEY(bind, key, bind->event_state_mask | global.x->xcb_numlock_mask);

            /* Also bind the key with active NumLock+CapsLock */
            ADD_TRANSLATED_KEY(bind, key, bind->event_state_mask | global.x->xcb_numlock_mask | XCB_MOD_MASK_LOCK);
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
void translate_keysyms() {
    struct xkb_state *dummy_state = nullptr;
    struct xkb_state *dummy_state_no_shift = nullptr;
    struct xkb_state *dummy_state_numlock = nullptr;
    struct xkb_state *dummy_state_numlock_no_shift = nullptr;
    bool has_errors = false;

    if ((dummy_state = xkb_state_new(xkb_keymap)) == nullptr ||
        (dummy_state_no_shift = xkb_state_new(xkb_keymap)) == nullptr ||
        (dummy_state_numlock = xkb_state_new(xkb_keymap)) == nullptr ||
        (dummy_state_numlock_no_shift = xkb_state_new(xkb_keymap)) == nullptr) {
        ELOG("Could not create XKB state, cannot translate keysyms.\n");
        goto out;
    }

    for (auto &bind : current_mode->bindings) {
        if (bind->input_type == B_MOUSE) {
            long button;
            if (!parse_long(bind->symbol + (sizeof("button") - 1), &button, 10)) {
                 ELOG(fmt::sprintf("Could not translate string to button: \"%s\"\n", bind->symbol));
            }

            xcb_keycode_t key = button;
            bind->keycode = key;
            DLOG(fmt::sprintf("Binding Mouse button, Keycode = %d\n",  key));
        }

        xkb_layout_index_t group = XCB_XKB_GROUP_1;
        if ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_2)
            group = XCB_XKB_GROUP_2;
        else if ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_3)
            group = XCB_XKB_GROUP_3;
        else if ((bind->event_state_mask >> 16) & I3_XKB_GROUP_MASK_4)
            group = XCB_XKB_GROUP_4;

        DLOG(fmt::sprintf("Binding %p group = %d, event_state_mask = %d, &2 = %s, &3 = %s, &4 = %s\n",
             (void*)bind.get(),
             group,
             bind->event_state_mask,
             (bind->event_state_mask & I3_XKB_GROUP_MASK_2) ? "yes" : "no",
             (bind->event_state_mask & I3_XKB_GROUP_MASK_3) ? "yes" : "no",
             (bind->event_state_mask & I3_XKB_GROUP_MASK_4) ? "yes" : "no"));
        (void)xkb_state_update_mask(
            dummy_state,
            (bind->event_state_mask & 0x1FFF) /* xkb_mod_mask_t base_mods, */,
            0 /* xkb_mod_mask_t latched_mods, */,
            0 /* xkb_mod_mask_t locked_mods, */,
            0 /* xkb_layout_index_t base_group, */,
            0 /* xkb_layout_index_t latched_group, */,
            group /* xkb_layout_index_t locked_group, */);

        (void)xkb_state_update_mask(
            dummy_state_no_shift,
            (bind->event_state_mask & 0x1FFF) ^ XCB_KEY_BUT_MASK_SHIFT /* xkb_mod_mask_t base_mods, */,
            0 /* xkb_mod_mask_t latched_mods, */,
            0 /* xkb_mod_mask_t locked_mods, */,
            0 /* xkb_layout_index_t base_group, */,
            0 /* xkb_layout_index_t latched_group, */,
            group /* xkb_layout_index_t locked_group, */);

        (void)xkb_state_update_mask(
            dummy_state_numlock,
            (bind->event_state_mask & 0x1FFF) | global.x->xcb_numlock_mask /* xkb_mod_mask_t base_mods, */,
            0 /* xkb_mod_mask_t latched_mods, */,
            0 /* xkb_mod_mask_t locked_mods, */,
            0 /* xkb_layout_index_t base_group, */,
            0 /* xkb_layout_index_t latched_group, */,
            group /* xkb_layout_index_t locked_group, */);

        (void)xkb_state_update_mask(
            dummy_state_numlock_no_shift,
            ((bind->event_state_mask & 0x1FFF) | global.x->xcb_numlock_mask) ^ XCB_KEY_BUT_MASK_SHIFT /* xkb_mod_mask_t base_mods, */,
            0 /* xkb_mod_mask_t latched_mods, */,
            0 /* xkb_mod_mask_t locked_mods, */,
            0 /* xkb_layout_index_t base_group, */,
            0 /* xkb_layout_index_t latched_group, */,
            group /* xkb_layout_index_t locked_group, */);

        if (bind->keycode > 0) {
            /* We need to specify modifiers for the keycode binding (numlock
             * fallback). */
            while (!bind->keycodes_head.empty()) {
                auto *first = bind->keycodes_head.front();
                bind->keycodes_head.pop_front();
                delete first;
            }

            ADD_TRANSLATED_KEY(bind.get(), bind->keycode, bind->event_state_mask);

            /* Also bind the key with active CapsLock */
            ADD_TRANSLATED_KEY(bind.get(), bind->keycode, bind->event_state_mask | XCB_MOD_MASK_LOCK);

            /* If this binding is not explicitly for NumLock, check whether we need to
             * add a fallback. */
            if ((bind->event_state_mask & global.x->xcb_numlock_mask) != global.x->xcb_numlock_mask) {
                /* Check whether the keycode results in the same keysym when NumLock is
                 * active. If so, grab the key with NumLock as well, so that users don’t
                 * need to duplicate every key binding with an additional Mod2 specified.
                 */
                xkb_keysym_t sym = xkb_state_key_get_one_sym(dummy_state, bind->keycode);
                xkb_keysym_t sym_numlock = xkb_state_key_get_one_sym(dummy_state_numlock, bind->keycode);
                if (sym == sym_numlock) {
                    /* Also bind the key with active NumLock */
                    ADD_TRANSLATED_KEY(bind.get(), bind->keycode, bind->event_state_mask | global.x->xcb_numlock_mask);

                    /* Also bind the key with active NumLock+CapsLock */
                    ADD_TRANSLATED_KEY(bind.get(), bind->keycode, bind->event_state_mask | global.x->xcb_numlock_mask | XCB_MOD_MASK_LOCK);
                } else {
                    DLOG(fmt::sprintf("Skipping automatic numlock fallback, key %d resolves to 0x%x with numlock\n",
                         bind->keycode, sym_numlock));
                }
            }

            continue;
        }

        /* We need to translate the symbol to a keycode */
        const xkb_keysym_t keysym = xkb_keysym_from_name(bind->symbol, XKB_KEYSYM_NO_FLAGS);
        if (keysym == XKB_KEY_NoSymbol) {
             ELOG(fmt::sprintf("Could not translate string to key symbol: \"%s\"\n",
                 bind->symbol));
            continue;
        }

        struct resolve resolving = {
            .bind = bind.get(),
            .keysym = keysym,
            .xkb_state = dummy_state,
            .xkb_state_no_shift = dummy_state_no_shift,
            .xkb_state_numlock = dummy_state_numlock,
            .xkb_state_numlock_no_shift = dummy_state_numlock_no_shift,
        };
        while (!bind->keycodes_head.empty()) {
            auto *first = bind->keycodes_head.front();
            bind->keycodes_head.pop_front();
            delete first;
        }
        xkb_keymap_key_for_each(xkb_keymap, add_keycode_if_matches, &resolving);
        char *keycodes = sstrdup("");
        int num_keycodes = 0;
        for (auto &binding_keycode : bind->keycodes_head) {
            char *tmp;
            sasprintf(&tmp, "%s %d", keycodes, binding_keycode->keycode);
            free(keycodes);
            keycodes = tmp;
            num_keycodes++;

            /* check for duplicate bindings */
            for (auto &check : current_mode->bindings) {
                if (check == bind)
                    continue;
                if (check->symbol != nullptr)
                    continue;
                if (check->keycode != binding_keycode->keycode ||
                    check->event_state_mask != binding_keycode->modifiers ||
                    check->release != bind->release)
                    continue;
                has_errors = true;
                ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  keysym = %s, keycode = %d, state_mask = 0x%x\n",  bind->symbol, check->keycode, bind->event_state_mask));
            }
        }
         DLOG(fmt::sprintf("state=0x%x, cfg=\"%s\", sym=0x%x → keycodes%s (%d)\n",
             bind->event_state_mask, bind->symbol, keysym, keycodes, num_keycodes));
        free(keycodes);
    }

out:
    xkb_state_unref(dummy_state);
    xkb_state_unref(dummy_state_no_shift);
    xkb_state_unref(dummy_state_numlock);
    xkb_state_unref(dummy_state_numlock_no_shift);

    if (has_errors) {
        start_config_error_nagbar(true);
    }
}

#undef ADD_TRANSLATED_KEY

/*
 * Switches the key bindings to the given mode, if the mode exists
 *
 */
void switch_mode(const char *new_mode) {
    DLOG(fmt::sprintf("Switching to mode %s\n",  new_mode));

    for (auto &mode : modes) {
        if (strcmp(mode->name.c_str(), new_mode) != 0)
            continue;

        ungrab_all_keys(*global.x);
        current_mode = mode.get();
        translate_keysyms();
        grab_all_keys(*global.x);

        /* Reset all B_UPON_KEYRELEASE_IGNORE_MODS bindings to avoid possibly
         * activating one of them. */
        for (auto &bind : current_mode->bindings) {
            if (bind->release == B_UPON_KEYRELEASE_IGNORE_MODS)
                bind->release = B_UPON_KEYRELEASE;
        }

        char *event_msg;
        sasprintf(&event_msg, R"({"change":"%s", "pango_markup":%s})",
                  mode->name.c_str(), (mode->pango_markup ? "true" : "false"));

        ipc_send_event("mode", I3_IPC_EVENT_MODE, event_msg);
        FREE(event_msg);

        return;
    }

    ELOG("Mode not found\n");
}

struct reorder_binding_cmp {
    inline int operator()(const std::unique_ptr<Binding> &first, const std::unique_ptr<Binding> &second) {
        if (first->event_state_mask < second->event_state_mask) {
            return 1;
        } else if (first->event_state_mask == second->event_state_mask) {
            return 0;
        } else {
            return -1;
        }
    }
};

static void reorder_bindings_of_mode(struct Mode *mode) {
    std::sort(mode->bindings.begin(), mode->bindings.end(), reorder_binding_cmp());
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
void reorder_bindings() {
    for (auto &mode : modes) {
        reorder_bindings_of_mode(mode.get());
    }
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
    if ((a->symbol == nullptr && b->symbol != nullptr) ||
        (a->symbol != nullptr && b->symbol == nullptr)) {
        return false;
    }

    /* If a is NULL, b has to be NULL, too (see previous conditional).
     * If the keycodes differ, it can't be a duplicate. */
    if (a->symbol != nullptr &&
        strcasecmp(a->symbol, b->symbol) != 0) {
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
bool has_duplicate_bindings() {
    bool has_errors = false;
    for (auto &current : current_mode->bindings) {
        for (auto &bind : current_mode->bindings) {
            /* Abort when we reach the current keybinding, only check the
             * bindings before */
            if (bind == current) {
                break;
            }

            if (!binding_same_key(bind.get(), current.get())) {
                continue;
            }

            has_errors = true;
            if (current->keycode != 0) {
                 ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keycode %d, command \"%s\"\n",
                     current->event_state_mask, current->keycode, current->command));
            } else {
                 ELOG(fmt::sprintf("Duplicate keybinding in config file:\n  state mask 0x%x with keysym %s, command \"%s\"\n",
                     current->event_state_mask, current->symbol, current->command));
            }
        }
    }

    return has_errors;
}

/*
 * Creates a dynamically allocated copy of bind.
 */
Binding::Binding(const Binding &bind) {
    input_type = bind.input_type;
    release = bind.release;
    border = bind.border;
    whole_window = bind.whole_window;
    exclude_titlebar = bind.exclude_titlebar;
    keycode = bind.keycode;
    event_state_mask = bind.event_state_mask;

    if (bind.symbol != nullptr) {
        this->symbol = sstrdup(bind.symbol);
    }

    this->command = bind.command;

    for (auto &binding_keycode : bind.keycodes_head) {
        auto *ret_binding_keycode = new Binding_Keycode();
        *ret_binding_keycode = *binding_keycode;
        this->keycodes_head.push_back(ret_binding_keycode);
    }
}

/*
 * Frees the binding. If bind is null, it simply returns.
 */
Binding::~Binding() {
    auto bind = this;
    if (bind == nullptr) {
        return;
    }

    while (!bind->keycodes_head.empty()) {
        auto *first = bind->keycodes_head.front();
        bind->keycodes_head.pop_front();
        delete first;
    }

    FREE(bind->symbol);
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
    if (con == nullptr)
        command = bind->command;
    else
        command = fmt::format("[con_id=\"{}\"] {}", (void*)con, bind->command);

    //"exec i3-nagbar -t warning -m 'You pressed the exit shortcut. Do you really want to exit i3? This will end your X session.' -B 'Yes, exit i3' 'i3 exit'"
    //"exec \"i3-nagbar -t warning -m 'You pressed the exit shortcut. Do you really want to exit i3? This will end your X session.' -B 'Yes, exit i3' 'i3 exit'\""

    Binding bind_cp = *bind;
    CommandResult result = parse_command(command, nullptr, nullptr);

    if (result.needs_tree_render)
        tree_render();

    if (result.parse_error) {
        std::vector<button_t> buttons{};
        start_nagbar(&global.command_error_nagbar_pid, buttons, "The configured command for this shortcut could not be run successfully.", config.font->pattern);
    }

    ipc_send_binding_event("run", &bind_cp);

    return result;
}

static int fill_rmlvo_from_root(struct xkb_rule_names *xkb_names) {
    size_t content_max_words = 256;

    auto atom_reply = global.x->conn->intern_atom(0, strlen("_XKB_RULES_NAMES"), "_XKB_RULES_NAMES");
    if (atom_reply->length == 0)
        return -1;

    auto prop_reply = global.x->conn->get_property(false, global.x->root, atom_reply->atom,
                                                        XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
    if (prop_reply->length == 0) {
        return -1;
    }
    if (prop_reply->value_len > 0 && prop_reply->bytes_after > 0) {
        /* We received an incomplete value. Ask again but with a properly
         * adjusted size. */
        content_max_words += ceil(prop_reply->bytes_after / 4.0);
        /* Repeat the request, with adjusted size */
        prop_reply = global.x->conn->get_property(false, global.x->root, atom_reply->atom,
                                                      XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
        if (prop_reply->length == 0) {
            return -1;
        }
    }
    if (prop_reply->value_len == 0) {
        return -1;
    }

    const char *walk = (const char *)xcb_get_property_value(prop_reply.get().get());
    size_t remaining = prop_reply->value_len;
    for (int i = 0; i < 5 && remaining > 0; i++) {
        auto len = strnlen(walk, remaining);
        switch (i) {
            case 0:
                sasprintf((char **)&(xkb_names->rules), "%.*s", len, walk);
                break;
            case 1:
                sasprintf((char **)&(xkb_names->model), "%.*s", len, walk);
                break;
            case 2:
                sasprintf((char **)&(xkb_names->layout), "%.*s", len, walk);
                break;
            case 3:
                sasprintf((char **)&(xkb_names->variant), "%.*s", len, walk);
                break;
            case 4:
                sasprintf((char **)&(xkb_names->options), "%.*s", len, walk);
                break;
        }
        DLOG(fmt::sprintf("component %d of _XKB_RULES_NAMES is \"%.*s\"\n", i, len, walk));
        walk += (len + 1);
        remaining -= (len + 1);
    }

    return 0;
}

/*
 * Loads the XKB keymap from the X11 server and feeds it to xkbcommon.
 *
 */
bool load_keymap() {
    if (xkb_context == nullptr) {
        if ((xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS)) == nullptr) {
            ELOG("Could not create xkbcommon context\n");
            return false;
        }
    }

    struct xkb_keymap *new_keymap = nullptr;
    int32_t device_id;
    if (xkb_supported && (device_id = xkb_x11_get_core_keyboard_device_id(**global.x)) > -1) {
        if ((new_keymap = xkb_x11_keymap_new_from_device(xkb_context, **global.x, device_id, XKB_KEYMAP_COMPILE_NO_FLAGS)) == nullptr) {
            ELOG("xkb_x11_keymap_new_from_device failed\n");
            return false;
        }
    } else {
        /* Likely there is no XKB support on this server, possibly because it
         * is a VNC server. */
        LOG("No XKB / core keyboard device? Assembling keymap from local RMLVO.\n");
        struct xkb_rule_names names = {
            .rules = nullptr,
            .model = nullptr,
            .layout = nullptr,
            .variant = nullptr,
            .options = nullptr};
        if (fill_rmlvo_from_root(&names) == -1) {
            ELOG("Could not get _XKB_RULES_NAMES atom from root window, falling back to defaults.\n");
            /* Using NULL for the fields of xkb_rule_names. */
        }
        new_keymap = xkb_keymap_new_from_names(xkb_context, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
        free((char *)names.rules);
        free((char *)names.model);
        free((char *)names.layout);
        free((char *)names.variant);
        free((char *)names.options);
        if (new_keymap == nullptr) {
            ELOG("xkb_keymap_new_from_names failed\n");
            return false;
        }
    }
    xkb_keymap_unref(xkb_keymap);
    xkb_keymap = new_keymap;

    return true;
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
    std::set<int> buffer = { 1, 2, 3 };

    for (auto &bind : current_mode->bindings) {
        /* We are only interested in whole window mouse bindings. */
        if (bind->input_type != B_MOUSE || !bind->whole_window)
            continue;

        long button;
        if (!parse_long(bind->symbol + (sizeof("button") - 1), &button, 10)) {
            ELOG("Could not parse button number, skipping this binding. Please report this bug in i3.\n");
            continue;
        }

        buffer.insert(button);
    }

    return buffer;
}
