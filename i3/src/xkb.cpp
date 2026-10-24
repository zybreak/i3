module;
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <fmt/printf.h>
#include <xcb/xcb_keysyms.h>
module i3;

import std;
import utils;
import log;
import bindings;

static int fill_rmlvo_from_root(xkb_rule_names *xkb_names) {
    size_t content_max_words = 256;

    xcb::Connection *conn = &global.x->connection();
    auto atom_reply = xcb::xproto::intern_atom(conn, 0, "_XKB_RULES_NAMES").get();
    if (!atom_reply) {
        return -1;
    }

    auto prop_reply = xcb::xproto::get_property(conn, false, global.x->root, atom_reply->atom,
                                                XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words)
                          .get();
    if (!prop_reply) {
        return -1;
    }
    if (!prop_reply->value.empty() && prop_reply->bytes_after > 0) {
        /* We received an incomplete value. Ask again but with a properly
         * adjusted size. */
        content_max_words += ceil(prop_reply->bytes_after / 4.0);
        /* Repeat the request, with adjusted size */
        prop_reply = xcb::xproto::get_property(conn, false, global.x->root, atom_reply->atom,
                                               XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words)
                         .get();
        if (!prop_reply) {
            return -1;
        }
    }
    if (prop_reply->value.empty()) {
        return -1;
    }

    char const *walk = reinterpret_cast<char const *>(prop_reply->value.data());
    size_t remaining = prop_reply->value.size();
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
    if (global.xkb->xkb_supported && (device_id = xkb_x11_get_core_keyboard_device_id(global.x->c_handle())) > -1) {
        if ((keymap = xkb_x11_keymap_new_from_device(context, global.x->c_handle(), device_id, XKB_KEYMAP_COMPILE_NO_FLAGS)) == nullptr) {
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
                                 xcb::xproto::GetModifierMappingReplyObj const &modmap_reply) {
    xcb_keycode_t *codes;
    xcb_keycode_t mod_code;

    xcb_keycode_t const *modmap = modmap_reply.keycodes.data();
    /* The reply has eight modifiers' keycodes. */
    std::size_t const keycodes_per_modifier = modmap_reply.keycodes.size() / 8;

    /* Get the list of keycodes for the given symbol */
    if (!(codes = xcb_key_symbols_get_keycode(symbols, keysym))) {
        return 0;
    }

    /* Loop through all modifiers (Mod1-Mod5, Shift, Control, Lock) */
    for (int mod = 0; mod < 8; mod++) {
        for (std::size_t j = 0; j < keycodes_per_modifier; j++) {
            /* Store the current keycode (for modifier 'mod') */
            mod_code = modmap[(mod * keycodes_per_modifier) + j];
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
    x.connection().flush();

    /* Get the current modifier mapping (this is blocking!) */
    auto const modmap_r = xcb::xproto::get_modifier_mapping(&x.connection()).get();
    if (!modmap_r) {
        return 0;
    }

    return get_mod_mask_for(keysym, symbols, *modmap_r);
}

Keysyms::Keysyms(X &x)
    : x(x) {
    keysyms = xcb_key_symbols_alloc(x.c_handle());
}

Keysyms::~Keysyms() {
    xcb_key_symbols_free(keysyms);
}

unsigned int Xkb::get_numlock_mask() {
    return aio_get_mod_mask_for(x, XCB_NUM_LOCK, keysyms().keysyms);
}

/* xcb-keysyms takes libxcb's event types: these copy xcbpp's events, whose layouts are libxcb's, into them. */
xcb_keysym_t Xkb::key_press_lookup_keysym(xcb::xproto::key_press_event_t const *event, uint16_t state) {
    auto c_event = std::bit_cast<xcb_key_press_event_t>(*event);
    return xcb_key_press_lookup_keysym(keysyms().keysyms, &c_event, state);
}

void Xkb::refresh_keyboard_mapping(xcb::xproto::mapping_notify_event_t const *event) {
    auto c_event = std::bit_cast<xcb_mapping_notify_event_t>(*event);
    xcb_refresh_keyboard_mapping(keysyms().keysyms, &c_event);
}

Xkb::Xkb(X &x)
    : x(x) {
    /* XKB isn't in xcbpp: i3 uses libxcb-xkb, on the libxcb handle. */
    xcb_query_extension_reply_t const *xkb_ext = xcb_get_extension_data(x.c_handle(), &xcb_xkb_id);
    xkb_supported = xkb_ext != nullptr && xkb_ext->present;
    if (!xkb_supported) {
        DLOG("xkb is not present on this server");
    } else {
        DLOG("initializing xcb-xkb");
        free(xcb_xkb_use_extension_reply(x.c_handle(), xcb_xkb_use_extension(x.c_handle(), XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION), nullptr));
        xcb_xkb_select_events(
            x.c_handle(),
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
        std::unique_ptr<xcb_xkb_per_client_flags_reply_t, decltype(&std::free)> pcf_reply{
            xcb_xkb_per_client_flags_reply(x.c_handle(),
                                           xcb_xkb_per_client_flags(x.c_handle(), XCB_XKB_ID_USE_CORE_KBD, mask, mask, 0, 0, 0),
                                           nullptr),
            &std::free};

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
    start_nagbar(nagbar_kind_t::CONFIG_ERROR, buttons, text, font_pattern, type, false);
}

/*
 * add_keycode_if_matches is called for each keycode in the keymap and will add
 * the keycode to |data->bind| if the keycode can result in the keysym
 * |data->resolving|.
 *
 */
static void add_keycode_if_matches(xkb_keymap *keymap, xkb_keycode_t key, void *data) {
    const resolve *resolving = (resolve *)data;
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
                if (!utils::parse_long(bind.symbol.substr(sizeof("button") - 1), button)) {
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
