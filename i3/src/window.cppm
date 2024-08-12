/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * window.c: Updates window attributes (X11 hints/properties).
 *
 */
module;

#include <cairo.h>
#include <config.h>
#include <xcb/xcb.h>
export module i3:window;

import std;
import :internal;

class Assignment;

export {

    /**
     * Stores the reserved pixels on each screen edge read from a
     * _NET_WM_STRUT_PARTIAL.
     *
     */
    struct reservedpx {
        uint32_t left;
        uint32_t right;
        uint32_t top;
        uint32_t bottom;
    };

    /**
     * A 'Window' is a type which contains an xcb_window_t and all the related
     * information (hints like _NET_WM_NAME for that window).
     *
     */
    class i3Window {
       public:
        xcb_window_t id;

        /** Holds the xcb_window_t (just an ID) for the leader window (logical
         * parent for toolwindows and similar floating windows) */
        xcb_window_t leader;
        xcb_window_t transient_for;

        /** Pointers to the Assignments which were already ran for this Window
         * (assignments run only once) */
        std::deque<Assignment *> ran_assignments{};

        std::string class_class{};
        std::string class_instance{};

        /** The name of the window. */
        std::string name{};

        /** The WM_WINDOW_ROLE of this window (for example, the pidgin buddy window
         * sets "buddy list"). Useful to match specific windows in assignments or
         * for_window. */
        std::string role{};

        /** WM_CLIENT_MACHINE of the window */
        std::string machine{};

        /** Flag to force re-rendering the decoration upon changes */
        bool name_x_changed;

        /** Whether the application used _NET_WM_NAME */
        bool uses_net_wm_name;

        /** Whether the application needs to receive WM_TAKE_FOCUS */
        bool needs_take_focus;

        /** Whether this window accepts focus. We store this inverted so that the
         * default will be 'accepts focus'. */
        bool doesnt_accept_focus;

        /** The _NET_WM_WINDOW_TYPE for this window. */
        xcb_atom_t window_type;

        /** The _NET_WM_DESKTOP for this window. */
        uint32_t wm_desktop;

        /** Whether the window says it is a dock window */
        enum { W_NODOCK = 0,
               W_DOCK_TOP = 1,
               W_DOCK_BOTTOM = 2 } dock;

        /** When this window was marked urgent. 0 means not urgent */
        std::optional<std::chrono::time_point<std::chrono::system_clock>> urgent{};

        /** Pixels the window reserves. left/right/top/bottom */
        reservedpx reserved;

        /** Depth of the window */
        uint16_t depth;

        /* the wanted size of the window, used in combination with size
         * increments (see below). */
        int base_width;
        int base_height;

        /* minimum increment size specified for the window (in pixels) */
        int width_increment;
        int height_increment;

        /* Minimum size specified for the window. */
        uint32_t min_width;
        uint32_t min_height;

        /* Maximum size specified for the window. */
        uint32_t max_width;
        uint32_t max_height;

        /* aspect ratio from WM_NORMAL_HINTS (MPlayer uses this for example) */
        double min_aspect_ratio;
        double max_aspect_ratio;

        /** Window icon, as Cairo surface */
        cairo_surface_t *icon;

        /** The window has a nonrectangular shape. */
        bool shaped;
        /** The window has a nonrectangular input shape. */
        bool input_shaped;

        /* Time when the window became managed. Used to determine whether a window
         * should be swallowed after initial management. */
        std::chrono::time_point<std::chrono::system_clock> managed_since{};

        /* The window has been swallowed. */
        bool swallowed;

        /**
         * Updates the WM_CLASS (consisting of the class and instance) for the
         * given window.
         *
         */
        void window_update_class(xcb_get_property_reply_t *prop);

        /**
         * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
         * window. Further updates using window_update_name_legacy will be ignored.
         *
         */
        void window_update_name(xcb_get_property_reply_t *prop);

        /**
         * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
         * touch what the client sends us but pass it to xcb_image_text_8. To get
         * proper unicode rendering, the application has to use _NET_WM_NAME (see
         * window_update_name()).
         *
         */
        void window_update_name_legacy(xcb_get_property_reply_t *prop);

        /**
         * Updates the CLIENT_LEADER (logical parent window).
         *
         */
        void window_update_leader(xcb_get_property_reply_t *prop);

        /**
         * Updates the TRANSIENT_FOR (logical parent window).
         *
         */
        void window_update_transient_for(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
         *
         */
        void window_update_strut_partial(xcb_get_property_reply_t *prop);

        /**
         * Updates the WM_WINDOW_ROLE
         *
         */
        void window_update_role(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_WINDOW_TYPE property.
         *
         */
        bool window_update_type(xcb_get_property_reply_t *reply);

        /**
         * Updates the WM_NORMAL_HINTS
         *
         */
        bool window_update_normal_hints(xcb_get_property_reply_t *reply, xcb_get_geometry_reply_t *geom);

        /**
         * Updates the WM_HINTS (we only care about the input focus handling part).
         *
         */
        void window_update_hints(xcb_get_property_reply_t *prop, bool *urgency_hint);

        /**
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
        bool window_update_motif_hints(xcb_get_property_reply_t *prop, border_style_t *motif_border_style);

        /**
         * Updates the WM_CLIENT_MACHINE
         *
         */
        [[deprecated("Replaced by handle_property, which has an improved interface")]]
        void window_update_machine(xcb_get_property_reply_t *prop);

        void window_update_machine(const std::string &machine);

        /**
         * Updates the _NET_WM_ICON
         *
         */
        void window_update_icon(xcb_get_property_reply_t *prop);

        ~i3Window();
    };
}