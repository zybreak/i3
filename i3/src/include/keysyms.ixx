module;

#include <xcb/xcb_keysyms.h>
export module i3:keysyms;

class X;

export class Keysyms {
   private:
    X *x;
   public:
    xcb_key_symbols_t *keysyms;

    explicit Keysyms(X *x);
    ~Keysyms();

    unsigned int get_numlock_mask() const;
    xcb_keysym_t key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state) const;
    void refresh_keyboard_mapping(xcb_mapping_notify_event_t *event) const;
    void key_symbols_alloc();
};
