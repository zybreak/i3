module;
#include <xcb/xcb_keysyms.h>
export module i3:keysyms;

import :xcb;
import :util;
import :x;
import std;

export class Keysyms {
   private:
    X &x;

   public:
    xcb_key_symbols_t *keysyms;

    Keysyms() = delete;
    explicit Keysyms(X &x);
    ~Keysyms();

    unsigned int get_numlock_mask() const;
    xcb_keysym_t key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state) const;
    void refresh_keyboard_mapping(xcb_mapping_notify_event_t *event) const;
    void key_symbols_alloc();
};

/*
 * Returns the modifier mask (XCB_MOD_MASK_*) for the given keysymbol, for
 * example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function does not initiate any round-trips.
 *
 */
static uint32_t get_mod_mask_for(uint32_t keysym,
                                 xcb_key_symbols_t *symbols,
                                 xcb_get_modifier_mapping_reply_t *modmap_reply) {
    xcb_keycode_t *codes, *modmap;
    xcb_keycode_t mod_code;

    modmap = xcb_get_modifier_mapping_keycodes(modmap_reply);

    /* Get the list of keycodes for the given symbol */
    if (!(codes = xcb_key_symbols_get_keycode(symbols, keysym))) {
        return 0;
    }

    /* Loop through all modifiers (Mod1-Mod5, Shift, Control, Lock) */
    for (int mod = 0; mod < 8; mod++) {
        for (int j = 0; j < modmap_reply->keycodes_per_modifier; j++) {
            /* Store the current keycode (for modifier 'mod') */
            mod_code = modmap[(mod * modmap_reply->keycodes_per_modifier) + j];
            /* Check if that keycode is in the list of previously resolved
             * keycodes for our symbol. If so, return the modifier mask. */
            for (xcb_keycode_t *code = codes; *code; code++) {
                if (*code != mod_code)
                    continue;

                std::free(codes);
                /* This corresponds to the XCB_MOD_MASK_* constants */
                return (1 << mod);
            }
        }
    }

    return 0;
}

/*
 * All-in-one function which returns the modifier mask (XCB_MOD_MASK_*) for the
 * given keysymbol, for example for XCB_NUM_LOCK (usually configured to mod2).
 *
 * This function initiates one round-trip. Use get_mod_mask_for() directly if
 * you already have the modifier mapping and key symbols.
 *
 */
static uint32_t aio_get_mod_mask_for(X &x, uint32_t keysym, xcb_key_symbols_t *symbols) {
    xcb_get_modifier_mapping_cookie_t cookie;
    xcb_get_modifier_mapping_reply_t *modmap_r;

    xcb_flush(*x);

    /* Get the current modifier mapping (this is blocking!) */
    cookie = xcb_get_modifier_mapping(*x);
    if (!(modmap_r = xcb_get_modifier_mapping_reply(*x, cookie, nullptr))) {
        return 0;
    }

    uint32_t result = get_mod_mask_for(keysym, symbols, modmap_r);
    std::free(modmap_r);
    return result;
}


Keysyms::Keysyms(X &x)
    : x(x) {
    keysyms = xcb_key_symbols_alloc(*x);
}

Keysyms::~Keysyms() {
    xcb_key_symbols_free(keysyms);
}

unsigned int Keysyms::get_numlock_mask() const {
    return aio_get_mod_mask_for(x, XCB_NUM_LOCK, keysyms);
}

xcb_keysym_t Keysyms::key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state) const {
    return xcb_key_press_lookup_keysym(keysyms, event, state);
}

void Keysyms::refresh_keyboard_mapping(xcb_mapping_notify_event_t *event) const {
    xcb_refresh_keyboard_mapping(keysyms, event);
}
void Keysyms::key_symbols_alloc() {
    xcb_key_symbols_free(keysyms);
    keysyms = xcb_key_symbols_alloc(*x);
}
