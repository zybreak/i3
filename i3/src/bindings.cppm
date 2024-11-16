/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * bindings.h: Functions for configuring, finding, and running bindings.
 *
 */
module;
#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
export module i3:bindings;

import std;
import :x;
import :util;
import i3_commands_base;

class Keymap;

export {
    class Config;
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
        xcb_keycode_t keycode{};
        i3_event_state_mask_t modifiers{};

        Binding_Keycode(xcb_keycode_t keycode, i3_event_state_mask_t modifiers)
            : keycode(keycode), modifiers(modifiers) {
        }

        auto operator<=>(Binding_Keycode const &) const = default;
        bool operator==(Binding_Keycode const &) const = default;
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

    /**
     * Adds a binding from config parameters given as strings and returns a
     * pointer to the binding structure. Returns NULL if the input code could not
     * be parsed.
     *
     */
    void configure_binding(Config * config, std::string_view bindtype, std::string_view modifiers, std::string_view input_code,
                           bool release, bool border, bool whole_window,
                           bool exclude_titlebar, std::string_view command, std::string_view modename,
                           bool pango_markup);

    /**
     * Ungrabs all keys, to be called before re-grabbing the keys because of a
     * mapping_notify event or a configuration file reload
     *
     */
    void ungrab_all_keys(x_connection * conn);

    /**
     * Grab the bound keys (tell X to send us keypress events for those keycodes)
     *
     */
    void grab_all_keys(x_connection * conn);

    /**
     * Release the button grabs on all managed windows and regrab them,
     * reevaluating which buttons need to be grabbed.
     *
     */
    void regrab_all_buttons(x_connection * conn);

    /**
     * Returns a pointer to the Binding that matches the given xcb event or NULL if
     * no such binding exists.
     *
     */
    Binding *get_binding_from_xcb_event(xcb_generic_event_t * event);

    /**
     * Translates keysymbols to keycodes for all bindings which use keysyms.
     *
     */
    void translate_keysyms(Keymap const &keymap);

    /**
     * Switches the key bindings to the given mode, if the mode exists
     *
     */
    void switch_mode(std::string const new_mode);

    /**
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
    void reorder_bindings(Config * config);

    /**
     * Checks for duplicate key bindings (the same keycode or keysym is configured
     * more than once). If a duplicate binding is found, a message is printed to
     * stderr and the has_errors variable is set to true, which will start
     * i3-nagbar.
     *
     */
    bool has_duplicate_bindings(Config * config);

    /**
     * Runs the given binding and handles parse errors. If con is passed, it will
     * execute the command binding with that container selected by criteria.
     * Returns a CommandResult for running the binding's command. Caller should
     * render tree if needs_tree_render is true. Free with command_result_free().
     *
     */
    CommandResult run_binding(Binding * bind, Con * con);

    /**
     * Returns a list of buttons that should be grabbed on a window.
     * This list will always contain 1–3, all higher buttons will only be returned
     * if there is a whole-window binding for it on some window in the current
     * config.
     * The list is terminated by a 0.
     */
    std::set<int> bindings_get_buttons_to_grab();
}
