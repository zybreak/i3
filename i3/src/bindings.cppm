/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * bindings.h: Functions for configuring, finding, and running bindings.
 *
 */
module;
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
export module bindings;

import std;
import i3_commands_base;
import xcb;

export {
    /**
     * The lower 16 bits contain a xcb_key_but_mask_t, the higher 16 bits contain
     * an i3_xkb_group_mask_t. This type is necessary for the fallback logic to
     * work when handling XKB groups (see ticket #1775) and makes the code which
     * locates keybindings upon KeyPress/KeyRelease events simpler.
     */
    using i3_event_state_mask_t = uint32_t;
    
    /**
     * Binding input types. See Binding::input_type.
     */
    enum input_type_t {
        B_KEYBOARD = 0,
        B_MOUSE = 1
    };

    /**
     * The name of the default mode.
     *
     */
    std::string const DEFAULT_BINDING_MODE = "default";

    /**
     * Stores a resolved keycode (from a keysym), including the modifier mask. Will
     * be passed to xcb_grab_key().
     *
     */
    struct Binding_Keycode {
        xcb::xproto::keycode_t keycode{};
        i3_event_state_mask_t modifiers{};

        Binding_Keycode(xcb::xproto::keycode_t keycode, i3_event_state_mask_t modifiers) // NOLINT(bugprone-easily-swappable-parameters)
            : keycode(keycode), modifiers(modifiers) {
        }

        auto operator<=>(Binding_Keycode const &) const = default;
        bool operator==(Binding_Keycode const &) const = default;
    };
    
    /**
     * Bitmask for matching XCB_XKB_GROUP_1 to XCB_XKB_GROUP_4.
     */
    enum i3_xkb_group_mask_t {
        I3_XKB_GROUP_MASK_ANY = 0,
        I3_XKB_GROUP_MASK_1 = (1 << 0),
        I3_XKB_GROUP_MASK_2 = (1 << 1),
        I3_XKB_GROUP_MASK_3 = (1 << 2),
        I3_XKB_GROUP_MASK_4 = (1 << 3)
    };

    enum binding_upon_t {
        /* This binding will only be executed upon KeyPress events */
        B_UPON_KEYPRESS = 0,
        /* This binding will be executed either upon a KeyRelease event, or… */
        B_UPON_KEYRELEASE = 1,
        /* …upon a KeyRelease event, even if the modifiers don’t match. This
         * state is triggered from get_binding() when the corresponding
         * KeyPress (!) happens, so that users can release the modifier keys
         * before releasing the actual key. */
        B_UPON_KEYRELEASE_IGNORE_MODS = 2,
    };

    /**
     * Holds a keybinding, consisting of a keycode combined with modifiers and the
     * command which is executed as soon as the key is pressed (see
     * src/config_parser.c)
     *
     */
    struct Binding {
        /* The type of input this binding is for. (Mouse bindings are not yet
         * implemented. All bindings are currently assumed to be keyboard bindings.) */
        input_type_t input_type{};

        /** If true, the binding should be executed upon a KeyRelease event, not a
         * KeyPress (the default). */
        binding_upon_t release{};

        /** If this is true for a mouse binding, the binding should be executed
         * when the button is pressed over the window border. */
        bool border{};

        /** If this is true for a mouse binding, the binding should be executed
         * when the button is pressed over any part of the window, not just the
         * title bar (default). */
        bool whole_window{};

        /** If this is true for a mouse binding, the binding should only be
         * executed if the button press was not on the titlebar. */
        bool exclude_titlebar{};

        /** Keycode to bind */
        uint32_t keycode{};

        /** Bitmask which is applied against event->state for KeyPress and
         * KeyRelease events to determine whether this binding applies to the
         * current state. */
        i3_event_state_mask_t event_state_mask{};

        /** Symbol the user specified in configfile, if any. This needs to be
         * stored with the binding to be able to re-convert it into a keycode
         * if the keyboard mapping changes (using Xmodmap for example) */
        std::string symbol{};

        /** Only in use if symbol != NULL. Contains keycodes which generate the
         * specified symbol. Useful for unbinding and checking which binding was
         * used when a key press event comes in. */
        std::vector<Binding_Keycode> keycodes{};

        /** Command, like in command mode */
        std::string command{};

        auto operator<=>(Binding const &) const = default;
    };
    
    /*
     * A utility function to convert a string containing the group and modifiers to
     * the corresponding bit mask.
     */
    i3_event_state_mask_t event_state_from_str(char const *str);

    /**
     * Returns a pointer to the Binding that matches the given xcb event or NULL if
     * no such binding exists.
     *
     */
    Binding *get_binding_from_xcb_event(xcb::xproto::key_press_event_t const *event, std::vector<Binding> &bindings);
    Binding *get_binding_from_xcb_event(xcb::xproto::button_press_event_t const *event, std::vector<Binding> &bindings);

    /**
     * Returns a list of buttons that should be grabbed on a window.
     * This list will always contain 1–3, all higher buttons will only be returned
     * if there is a whole-window binding for it on some window in the current
     * config.
     * The list is terminated by a 0.
     */
    std::set<int> bindings_get_buttons_to_grab(std::vector<Binding> &bindings);
}
