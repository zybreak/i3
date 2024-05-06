module;
#include <xcb/xcb_keysyms.h>
export module i3:keysyms;

import :xcb;
import :util;
import :x;

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

Keysyms::Keysyms(X &x)
    : x(x) {
    keysyms = xcb_key_symbols_alloc(*x);
}

Keysyms::~Keysyms() {
    xcb_key_symbols_free(keysyms);
}

unsigned int Keysyms::get_numlock_mask() const {
    return aio_get_mod_mask_for(XCB_NUM_LOCK, keysyms);
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
