module;
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include "xpp/proto/bigreq.hpp"
#include "xpp/proto/xkb.hpp"
#include "xpp/xpp.hpp"
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <fmt/printf.h>
#include <xcb/xcb_keysyms.h>
module i3;

import std;
import utils;
import log;
import :xcb;

static int fill_rmlvo_from_root(xkb_rule_names *xkb_names) {
    size_t content_max_words = 256;

    auto atom_reply = global.x->conn->intern_atom(0, strlen("_XKB_RULES_NAMES"), "_XKB_RULES_NAMES");
    if (atom_reply->length == 0) {
        return -1;
    }

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

    char const *walk = (char const *)xcb_get_property_value(prop_reply.get().get());
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

Keymap::Keymap() {
    if ((context = xkb_context_new(XKB_CONTEXT_NO_FLAGS)) == nullptr) {
        throw std::runtime_error("Could not create xkbcommon context");
    }

    int32_t device_id;
    if (global.xkb->xkb_supported && (device_id = xkb_x11_get_core_keyboard_device_id(**global.x)) > -1) {
        if ((keymap = xkb_x11_keymap_new_from_device(context, **global.x, device_id, XKB_KEYMAP_COMPILE_NO_FLAGS)) == nullptr) {
            throw std::runtime_error("xkb_x11_keymap_new_from_device failed");
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
        keymap = xkb_keymap_new_from_names(context, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
        free((char *)names.rules);
        free((char *)names.model);
        free((char *)names.layout);
        free((char *)names.variant);
        free((char *)names.options);
        if (keymap == nullptr) {
            throw std::runtime_error("xkb_keymap_new_from_names failed");
        }
    }
}

/*
 * Loads the XKB keymap from the X11 server and feeds it to xkbcommon.
 *
 */
void Xkb::load_keymap() {
    this->_keymap = std::make_unique<Keymap>();
}

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
                if (*code != mod_code) {
                    continue;
                }

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

unsigned int Xkb::get_numlock_mask() {
    return aio_get_mod_mask_for(x, XCB_NUM_LOCK, keysyms().keysyms);
}

xcb_keysym_t Xkb::key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state) {
    return xcb_key_press_lookup_keysym(keysyms().keysyms, event, state);
}

void Xkb::refresh_keyboard_mapping(xcb_mapping_notify_event_t *event) {
    xcb_refresh_keyboard_mapping(keysyms().keysyms, event);
}

Xkb::Xkb(X &x)
    : x(x) {
    x_connection *conn = &*x;
    auto xkb_ext = conn->extension<xpp::xkb::extension>();
    xkb_supported = xkb_ext->present;
    if (!xkb_supported) {
        DLOG("xkb is not present on this server");
    } else {
        DLOG("initializing xcb-xkb");
        conn->xkb().use_extension(XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
        conn->xkb().select_events(
            XCB_XKB_ID_USE_CORE_KBD,
            XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
            0,
            XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
            0xff,
            0xff,
            nullptr);

        /* Setting both, XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE and
         * XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED, will lead to the
         * X server sending us the full XKB state in KeyPress and KeyRelease:
         * https://cgit.freedesktop.org/xorg/xserver/tree/xkb/xkbEvents.c?h=xorg-server-1.20.0#n927
         *
         * XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT enable detectable autorepeat:
         * https://www.x.org/releases/current/doc/kbproto/xkbproto.html#Detectable_Autorepeat
         * This affects bindings using the --release flag: instead of getting multiple KeyRelease
         * events we get only one event when the key is physically released by the user.
         */
        uint32_t const mask = XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE |
                              XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED |
                              XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT;
        /* The last three parameters are unset because they are only relevant
         * when using a feature called “automatic reset of boolean controls”:
         * https://www.x.org/releases/X11R7.7/doc/kbproto/xkbproto.html#Automatic_Reset_of_Boolean_Controls
         * */
        auto pcf_reply = conn->xkb().per_client_flags(XCB_XKB_ID_USE_CORE_KBD, mask, mask, 0, 0, 0);

        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE))) {
            ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE");
        }
        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED))) {
            ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED");
        }
        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT))) {
            ELOG("Could not set XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT");
        }

        xkb_base = xkb_ext->first_event;
    }
}

void Xkb::key_symbols_alloc() {
    _keysyms = std::make_unique<Keysyms>(x);
}
