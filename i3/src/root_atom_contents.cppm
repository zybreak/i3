module;
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
export module i3:root_atom_contents;

import std;
import utils;

/**
 * Try to get the contents of the given atom (for example I3_SOCKET_PATH) from
 * the X11 root window and return NULL if it doesn’t work.
 *
 * If the provided XCB connection is NULL, a new connection will be
 * established.
 *
 */
export std::optional<std::string> root_atom_contents(const char *atomname, xcb_connection_t *provided_conn, int screen) {
    xcb_intern_atom_cookie_t atom_cookie;
    xcb_intern_atom_reply_t *atom_reply;
    char *content = nullptr;
    size_t content_max_words = 256;
    xcb_connection_t *conn = provided_conn;

    if (provided_conn == nullptr &&
        ((conn = xcb_connect(nullptr, &screen)) == nullptr ||
         xcb_connection_has_error(conn))) {
        return std::nullopt;
    }

    atom_cookie = xcb_intern_atom(conn, 0, std::strlen(atomname), atomname);

    xcb_screen_t *root_screen = xcb_aux_get_screen(conn, screen);
    xcb_window_t root = root_screen->root;

    atom_reply = xcb_intern_atom_reply(conn, atom_cookie, nullptr);
    if (atom_reply == nullptr) {
        goto out_conn;
    }

    xcb_get_property_cookie_t prop_cookie;
    xcb_get_property_reply_t *prop_reply;
    prop_cookie = xcb_get_property_unchecked(conn, false, root, atom_reply->atom,
                                             XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
    prop_reply = xcb_get_property_reply(conn, prop_cookie, nullptr);
    if (prop_reply == nullptr) {
        goto out_atom;
    }
    if (xcb_get_property_value_length(prop_reply) > 0 && prop_reply->bytes_after > 0) {
        /* We received an incomplete value. Ask again but with a properly
         * adjusted size. */
        content_max_words += std::ceil(prop_reply->bytes_after / 4.0);
        /* Repeat the request, with adjusted size */
        std::free(prop_reply);
        prop_cookie = xcb_get_property_unchecked(conn, false, root, atom_reply->atom,
                                                 XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
        prop_reply = xcb_get_property_reply(conn, prop_cookie, nullptr);
        if (prop_reply == nullptr) {
            goto out_atom;
        }
    }
    if (xcb_get_property_value_length(prop_reply) == 0) {
        goto out;
    }
    if (prop_reply->type == XCB_ATOM_CARDINAL) {
        /* We treat a CARDINAL as a >= 32-bit unsigned int. The only CARDINAL
         * we query is I3_PID, which is 32-bit. */
        sasprintf(&content, "%u", *((unsigned int *)xcb_get_property_value(prop_reply)));
    } else {
        sasprintf(&content, "%.*s", xcb_get_property_value_length(prop_reply),
                  (char *)xcb_get_property_value(prop_reply));
    }

out:
    std::free(prop_reply);
out_atom:
    std::free(atom_reply);
out_conn:
    if (provided_conn == nullptr) {
        xcb_disconnect(conn);
    }
    return std::make_optional<std::string>(content);
}