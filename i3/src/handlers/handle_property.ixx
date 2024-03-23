module;
#include <optional>
#include <utility>
#include <string>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <cairo.h>
#include <cmath>
export module i3:handle_property;

import :window;

export namespace handle_property {

    struct window_class {
        std::string class_name;
        std::string class_instance;
    };

    struct window_hints {
        bool doesnt_accept_focus;
        bool urgency_hint;
    };

    /*
     * Updates the WM_CLASS (consisting of the class and instance) for the
     * given window.
     *
     */
    std::optional<window_class> window_update_class(xcb_get_property_reply_t * prop);

    /*
     * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
     * window. Further updates using window_update_name_legacy will be ignored.
     *
     */
    std::optional<std::string> window_update_name(xcb_get_property_reply_t * prop);

    /*
     * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
     * touch what the client sends us but pass it to xcb_image_text_8. To get
     * proper unicode rendering, the application has to use _NET_WM_NAME (see
     * window_update_name()).
     *
     */
    std::optional<std::string> window_update_name_legacy(xcb_get_property_reply_t * prop);

    /*
     * Updates the CLIENT_LEADER (logical parent window).
     *
     */
    std::optional<xcb_window_t> window_update_leader(xcb_get_property_reply_t * prop);

    /*
     * Updates the TRANSIENT_FOR (logical parent window).
     *
     */
    std::optional<xcb_window_t> window_update_transient_for(xcb_get_property_reply_t * prop);

    /*
     * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
     *
     */
    std::optional<reservedpx> window_update_strut_partial(xcb_get_property_reply_t * prop);

    /*
     * Updates the WM_WINDOW_ROLE
     *
     */
    std::optional<std::string> window_update_role(xcb_get_property_reply_t * prop);

    /*
     * Updates the _NET_WM_WINDOW_TYPE property.
     *
     */
    std::optional<xcb_atom_t> window_update_type(xcb_get_property_reply_t * reply);

    /*
     * Updates the WM_HINTS (we only care about the input focus handling part).
     *
     */
    std::optional<window_hints> window_update_hints(xcb_get_property_reply_t * prop);

    /*
     * Updates the MOTIF_WM_HINTS. The container's border style should be set to
     * `motif_border_style' if border style is not BS_NORMAL.
     *
     * i3 only uses this hint when it specifies a window should have no
     * title bar, or no decorations at all, which is how most window managers
     * handle it.
     *
     * The EWMH spec intended to replace Motif hints with _NET_WM_WINDOW_TYPE, but
     * it is still in use by popular widget toolkits such as GTK+ and Java AWT.
     *
     */
    std::optional<uint32_t> window_update_motif_hints(xcb_get_property_reply_t * prop);

    /*
     * Updates the WM_CLIENT_MACHINE
     *
     */
    std::optional<std::string> window_update_machine(xcb_get_property_reply_t * prop);
}
