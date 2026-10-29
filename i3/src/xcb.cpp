/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * xcb.c: Helper functions for easier usage of XCB
 *
 */
module;

#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import std;
import utils;
import log;
import rect;

/*
 * Convenience wrapper around xcb_create_window which takes care of depth, generating an ID and checking
 * for errors.
 *
 */
xcb_window_t create_window(xcb::Connection &conn, Rect dims,
                           uint16_t depth, xcb_visualid_t visual, uint16_t window_class,
                           enum xcursor_cursor_t cursor, bool map, xcb::xproto::CreateWindowValueList const &values) {
    xcb_window_t result = conn.generate_id();

    /* If the window class is XCB_WINDOW_CLASS_INPUT_ONLY, we copy depth and
     * visual id from the parent window. */
    if (window_class == std::to_underlying(xcb::xproto::window_class_t::WINDOW_CLASS_INPUT_ONLY)) {
        depth = xcb::copy_from_parent;
        visual = xcb::copy_from_parent;
    }

    xcb::xproto::create_window(&conn,
                               depth,
                               result,                                  /* the window id */
                               global.x->root,                          /* parent == root */
                               dims.x, dims.y, dims.width, dims.height, /* dimensions */
                               0,                                       /* border = 0, we draw our own */
                               static_cast<xcb::xproto::window_class_t>(window_class),
                               visual,
                               values);

    /* CreateWindow is unchecked, so an error goes to the event loop and request_check() only makes a round trip, as
     * libxcb's xcb_request_check() did here. The connection is used by one thread only, so the last request sent is
     * the CreateWindow. */
    if (auto const error = conn.request_check(xcb::last_request_sent(conn))) {
        ELOG(fmt::sprintf("Could not create window. Error code: %d.\n", error->error_code));
    }

    /* Set the cursor */
    xcb::xproto::change_window_attributes(&conn, result,
                                          xcb::xproto::ChangeWindowAttributesValueList{}.with_cursor(global.x->xcursor_get_cursor(cursor)));

    /* Map the window (= make it visible) */
    if (map) {
        xcb::xproto::map_window(&conn, result);
    }

    return result;
}

/*
 * Generates a configure_notify_event with absolute coordinates (relative to the X root
 * window, not to the client’s frame) for the given client.
 *
 */
void fake_absolute_configure_notify(Con *con) {
    if (con->get_window() == nullptr) {
        return;
    }

    xcb::xproto::configure_notify_event_t generatedEvent{};

    generatedEvent.event = con->get_window()->id;
    generatedEvent.window = con->get_window()->id;
    generatedEvent.response_type = xcb::xproto::CONFIGURE_NOTIFY;

    generatedEvent.x = static_cast<int16_t>(con->rect.x + con->get_window_rect().x);
    generatedEvent.y = static_cast<int16_t>(con->rect.y + con->get_window_rect().y);
    generatedEvent.width = static_cast<uint16_t>(con->get_window_rect().width);
    generatedEvent.height = static_cast<uint16_t>(con->get_window_rect().height);

    generatedEvent.border_width = con->border_width;
    generatedEvent.above_sibling = xcb::none;
    generatedEvent.override_redirect = false;

    DLOG(fmt::sprintf("fake rect = (%d, %d, %d, %d)\n", generatedEvent.x, generatedEvent.y, generatedEvent.width, generatedEvent.height));

    /* Every X11 event is 32 bytes long, the ConfigureNotify fields take 28: the rest is zeros. */
    std::array<char, 32> event{};
    std::memcpy(event.data(), &generatedEvent, sizeof generatedEvent);
    xcb::Connection &conn = global.x->connection();
    xcb::xproto::send_event(&conn, false, con->get_window()->id, xcb::xproto::event_mask_t::EVENT_MASK_STRUCTURE_NOTIFY, event);
    conn.flush();
}

/*
 * Sends the WM_TAKE_FOCUS ClientMessage to the given window
 *
 */
void send_take_focus(xcb_window_t window, xcb_timestamp_t timestamp) {
    xcb::xproto::client_message_event_t ev{};

    ev.response_type = xcb::xproto::CLIENT_MESSAGE;
    ev.window = window;
    ev.type = i3::atoms[i3::Atom::WM_PROTOCOLS];
    ev.format = 32;
    ev.data.data32[0] = i3::atoms[i3::Atom::WM_TAKE_FOCUS];
    ev.data.data32[1] = timestamp;

    DLOG("Sending WM_TAKE_FOCUS to the client\n");
    xcb::xproto::send_event(&global.x->connection(), false, window, xcb::xproto::event_mask_t::EVENT_MASK_NO_EVENT,
                            std::bit_cast<std::array<char, 32>>(ev));
}

/*
 * Configures the given window to have the size/position specified by given rect
 *
 */
void xcb_set_window_rect(xcb::Connection &conn, xcb_window_t window, Rect r) {
    std::uint64_t const sequence = xcb::xproto::configure_window(&conn, window,
                                                                 xcb::xproto::ConfigureWindowValueList{}
                                                                     .with_x(static_cast<int32_t>(r.x))
                                                                     .with_y(static_cast<int32_t>(r.y))
                                                                     .with_width(r.width)
                                                                     .with_height(r.height));
    /* ignore events which are generated because we configured a window */
    global.handlers->add_ignore_event(sequence, -1);
}

uint32_t property_value32(xcb::xproto::GetPropertyReplyObj const &reply, std::size_t index) {
    uint32_t value = 0;
    if (index < reply.value.size() / 4) {
        std::memcpy(&value, reply.value.data() + (index * 4), sizeof value);
    } else if (index * 4 < reply.value.size()) {
        std::memcpy(&value, reply.value.data() + (index * 4), reply.value.size() - (index * 4));
    }
    return value;
}

/*
 * Returns the first supported _NET_WM_WINDOW_TYPE atom.
 *
 */
xcb_atom_t xcb_get_preferred_window_type(xcb::xproto::GetPropertyReplyObj const *reply) {
    if (reply == nullptr || reply->value.empty()) {
        return xcb::none;
    }

    for (std::size_t i = 0; i < reply->value.size() / (reply->format / 8U); i++) {
        xcb_atom_t atom = property_value32(*reply, i);
        if (atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_MENU] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_POPUP_MENU] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLTIP] ||
            atom == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NOTIFICATION]) {
            return atom;
        }
    }

    return xcb::none;
}

/*
 * Returns true if the given reply contains the given atom.
 *
 */
bool xcb_reply_contains_atom(xcb::xproto::GetPropertyReplyObj const *prop, xcb_atom_t atom) {
    if (prop == nullptr || prop->value.empty()) {
        return false;
    }

    for (std::size_t i = 0; i < prop->value.size() / (prop->format / 8U); i++) {
        if (property_value32(*prop, i) == atom) {
            return true;
        }
    }

    return false;
}

/*
 * Get visual type specified by visualid
 *
 */
xcb_visualtype_t *get_visualtype_by_id(xcb_visualid_t visual_id) {
    xcb_depth_iterator_t depth_iter;

    depth_iter = xcb_screen_allowed_depths_iterator(global.x->root_screen);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter;

        visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
        for (; visual_iter.rem; xcb_visualtype_next(&visual_iter)) {
            if (visual_id == visual_iter.data->visual_id) {
                return visual_iter.data;
            }
        }
    }
    return nullptr;
}

/*
 * Get visualid with specified depth
 *
 */
xcb_visualid_t get_visualid_by_depth(uint16_t depth) {
    auto const &screen = xcb::get_setup(global.x->connection())->get_roots()[static_cast<std::size_t>(global.x->conn_screen)];
    for (auto const &allowed : screen.get_allowed_depths()) {
        if (allowed.depth != depth) {
            continue;
        }

        auto const visuals = allowed.get_visuals();
        if (visuals.empty()) {
            continue;
        }
        return visuals.front().visual_id;
    }
    return 0;
}

/*
 * Returns the screen with the given number, or nullptr if the connection has no such screen, as xcb-util's
 * xcb_aux_get_screen() does.
 *
 */
xcb_screen_t *get_screen(xcb_connection_t *conn, int screen) {
    int number = 0;
    for (xcb_screen_iterator_t it = xcb_setup_roots_iterator(xcb_get_setup(conn)); it.rem; xcb_screen_next(&it), ++number) {
        if (number == screen) {
            return it.data;
        }
    }
    return nullptr;
}

/*
 * Returns the first visual of the screen with the given class and depth, -1 meaning any, or nullptr if there is none,
 * as xcb-util's xcb_aux_find_visual_by_attrs() does. Its parameters are xcb-util's: the class and the depth are int8_t,
 * which it compares with the setup's uint8_t values as ints.
 *
 */
xcb::xproto::visualtype_t *find_visual_by_attrs(xcb::xproto::screen_t const &screen, std::int8_t visual_class, std::int8_t depth) { // NOLINT(bugprone-easily-swappable-parameters)
    for (auto const &allowed : screen.get_allowed_depths()) {
        if (depth != -1 && int{allowed.depth} != int{depth}) {
            continue;
        }
        for (auto &visual : allowed.get_visuals()) {
            if (visual_class == -1 || int{visual._class} == int{visual_class}) {
                return &visual;
            }
        }
    }
    return nullptr;
}

/*
 * Returns the depth of the screen's visual with the given id, or 0 if the screen has no such visual, as xcb-util's
 * xcb_aux_get_depth_of_visual() does.
 *
 */
std::uint8_t get_depth_of_visual(xcb::xproto::screen_t const &screen, xcb::xproto::visualid_t id) {
    for (auto const &allowed : screen.get_allowed_depths()) {
        if (std::ranges::any_of(allowed.get_visuals(), [id](auto const &visual) { return visual.visual_id == id; })) {
            return allowed.depth;
        }
    }
    return 0;
}

/*
 * Returns the name of a screen's selection atom, as xcb-util's xcb_atom_name_by_screen() makes it ("%s_S%u").
 *
 */
std::string atom_name_by_screen(std::string_view base, std::uint8_t screen) {
    return std::format("{}_S{}", base, screen);
}

/*
 * Add an atom to a list of atoms the given property defines.
 * This is useful, for example, for manipulating _NET_WM_STATE.
 *
 */
void xcb_add_property_atom(xcb::Connection &conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom) {
    x_change_property(conn, xcb::xproto::prop_mode_t::PROP_MODE_APPEND, window, property, std::to_underlying(xcb::xproto::atom_enum_t::ATOM_ATOM), std::span{&atom, 1});
}

/*
 * Remove an atom from a list of atoms the given property defines without
 * removing any other potentially set atoms.  This is useful, for example, for
 * manipulating _NET_WM_STATE.
 *
 */
void xcb_remove_property_atom(xcb::Connection &conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom) {
    xcb::xproto::grab_server(&conn);

    auto const reply = xcb::xproto::get_property(&conn, false, window, property, std::to_underlying(xcb::xproto::get_property_type_t::GET_PROPERTY_TYPE_ANY), 0, 4096).get();
    if (!reply || reply->value.empty()) {
        xcb::xproto::ungrab_server(&conn);
        return;
    }

    {
        /* The value's atoms (libxcb's version read value_len atoms whatever the format, past the value's end for
         * formats 8 and 16). */
        std::size_t const current_size = reply->value.size() / sizeof(xcb_atom_t);
        std::vector<xcb_atom_t> values{};
        values.reserve(current_size);
        for (std::size_t i = 0; i < current_size; i++) {
            xcb_atom_t value{};
            std::memcpy(&value, reply->value.data() + (i * sizeof(xcb_atom_t)), sizeof(xcb_atom_t));
            if (value != atom) {
                values.push_back(value);
            }
        }

        x_change_property(conn, xcb::xproto::prop_mode_t::PROP_MODE_REPLACE, window, property, std::to_underlying(xcb::xproto::atom_enum_t::ATOM_ATOM), std::span{values});
    }

    xcb::xproto::ungrab_server(&conn);
}

/*
 * Grab the specified buttons on a window when managing it.
 *
 */
void xcb_grab_buttons(xcb_window_t window, std::set<int> &buttons) {
    for (int i : buttons) {
        xcb::xproto::grab_button(&global.x->connection(), false, window, xcb::xproto::event_mask_t::EVENT_MASK_BUTTON_PRESS,
                                 xcb::xproto::grab_mode_t::GRAB_MODE_SYNC, xcb::xproto::grab_mode_t::GRAB_MODE_ASYNC,
                                 global.x->root, xcb::none, static_cast<xcb::xproto::button_index_t>(i),
                                 xcb::xproto::mod_mask_t::MOD_MASK_ANY);
    }
}
