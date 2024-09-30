module;
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include "xpp/proto/bigreq.hpp"
#include "xpp/proto/xkb.hpp"
#include "xpp/xpp.hpp"
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <xcb/xcb_keysyms.h>
export module i3:xkb;

import :x;
import log;

export class Keysyms {
    friend class Xkb;
  private:
    X &x;
    xcb_key_symbols_t *keysyms;
    
  public:
    Keysyms() = delete;
    explicit Keysyms(X &x);
    Keysyms(Keysyms &&other) : x(other.x) {
        std::swap(keysyms, other.keysyms);
    }
    
    void operator=(const Keysyms &) = delete;
    void operator=(Keysyms && other) {
        std::swap(keysyms, other.keysyms);
    }
    ~Keysyms();

};

export class ResolvedKeymap {
    friend class Keymap;
  private:
    
   ResolvedKeymap(xkb_keymap *keymap) {
       dummy_state = xkb_state_new(keymap);
       dummy_state_no_shift = xkb_state_new(keymap);
       dummy_state_numlock = xkb_state_new(keymap);
       dummy_state_numlock_no_shift = xkb_state_new(keymap);    
       
       if (dummy_state == nullptr ||
           dummy_state_no_shift == nullptr ||
           dummy_state_numlock == nullptr ||
           dummy_state_numlock_no_shift == nullptr) {
           xkb_state_unref(dummy_state);
           xkb_state_unref(dummy_state_no_shift);
           xkb_state_unref(dummy_state_numlock);
           xkb_state_unref(dummy_state_numlock_no_shift);
           throw std::runtime_error("Could not create XKB state, cannot translate keysyms.");
       }
   }
   
  public:
    xkb_state *dummy_state{nullptr};
    xkb_state *dummy_state_no_shift{nullptr};
    xkb_state *dummy_state_numlock{nullptr};
    xkb_state *dummy_state_numlock_no_shift{nullptr};
  
    
    ResolvedKeymap(ResolvedKeymap const &) = delete;
    ResolvedKeymap& operator=(ResolvedKeymap const &) = delete;
    ResolvedKeymap(ResolvedKeymap &&other) {
        std::swap(dummy_state, other.dummy_state);
        std::swap(dummy_state_no_shift, other.dummy_state_no_shift);
        std::swap(dummy_state_numlock, other.dummy_state_numlock);
        std::swap(dummy_state_numlock_no_shift, other.dummy_state_numlock_no_shift);
    }
    
    ResolvedKeymap& operator=(ResolvedKeymap &&other) {
        std::swap(dummy_state, other.dummy_state);
        std::swap(dummy_state_no_shift, other.dummy_state_no_shift);
        std::swap(dummy_state_numlock, other.dummy_state_numlock);
        std::swap(dummy_state_numlock_no_shift, other.dummy_state_numlock_no_shift);
        return *this;
    }
    
    ~ResolvedKeymap() {
        xkb_state_unref(dummy_state);
        xkb_state_unref(dummy_state_no_shift);
        xkb_state_unref(dummy_state_numlock);
        xkb_state_unref(dummy_state_numlock_no_shift);
    }
};

export class Keymap {
    friend class Xkb;
    friend void translate_keysyms(Keymap const & keymap);
  private:
    xkb_context *context = nullptr;
    xkb_keymap *keymap = nullptr;
    
  public:
    std::unique_ptr<ResolvedKeymap> resolve() const {
        return std::unique_ptr<ResolvedKeymap>(new ResolvedKeymap(keymap));
    }
    Keymap();
    Keymap(Keymap &other) = delete;
    void operator=(Keymap &other) = delete;
        
    Keymap& operator=(Keymap &&other) {
        std::swap(keymap, other.keymap);
        std::swap(context, other.context);
        return *this;
    }
       
    Keymap(Keymap &&other) {
        std::swap(keymap, other.keymap);
        std::swap(context, other.context);
    }
        
    ~Keymap() {
        if (keymap) {
            xkb_keymap_unref(keymap);
        }
        if (context) {
            xkb_context_unref(context);
        }
    }
};

export class Xkb {
  private:
    X &x;
    std::unique_ptr<Keysyms> _keysyms{};
    std::unique_ptr<Keymap> _keymap{};
   public:
    bool xkb_supported{true};
    int xkb_base{-1};
    int xkb_current_group;

    /**
     * Loads the XKB keymap from the X11 server and feeds it to xkbcommon.
     *
     */
    void load_keymap();
    void key_symbols_alloc();
    Keysyms& keysyms() {
        if (!_keysyms) {
            _keysyms = std::make_unique<Keysyms>(x);
        }
        
        return *_keysyms;
    }
    
    Keymap& keymap() {
        if (!_keymap) {
            _keymap = std::make_unique<Keymap>();
        }
        
        return *_keymap;
    }

    Xkb() = delete;
    explicit Xkb(X &x);
    
    
    unsigned int get_numlock_mask();
    xcb_keysym_t key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state);
    void refresh_keyboard_mapping(xcb_mapping_notify_event_t *event);
};
