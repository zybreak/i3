#ifndef I3_KEYSYMS_H
#define I3_KEYSYMS_H

#include "x.h"
#include "xcb.h"
#include "util.h"

#include <xcb/xcb_keysyms.h>

class Keysyms {
   private:
    X *x;
   public:
    xcb_key_symbols_t *keysyms;

    explicit Keysyms(X *x);
    ~Keysyms();

    unsigned int get_numlock_mask();
    xcb_keysym_t key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state);
    void refresh_keyboard_mapping(xcb_mapping_notify_event_t *event);
    void key_symbols_alloc();
};

#endif
