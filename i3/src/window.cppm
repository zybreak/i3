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
#include <xcb/xcb_icccm.h>
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
        uint32_t left{0};
        uint32_t right{0};
        uint32_t top{0};
        uint32_t bottom{0};
    };

    class i3WindowIcon {
        /** Window icon, as Cairo surface */
        cairo_surface_t *icon;
        uint32_t *icon_data;

      public:
        cairo_surface_t *data() {
            return icon;
        }

        i3WindowIcon() = delete;
        i3WindowIcon(i3WindowIcon const &other) = delete;
        i3WindowIcon &operator=(i3WindowIcon const &other) = delete;
        i3WindowIcon(i3WindowIcon &&other) {
            std::swap(icon, other.icon);
            std::swap(icon_data, other.icon_data);
        }
        i3WindowIcon(uint32_t width, uint32_t height, uint32_t *data, uint64_t len) {
            this->icon_data = new uint32_t[len * 4];

            for (uint64_t i = 0; i < len; i++) {
                uint8_t r, g, b, a;
                uint32_t const pixel = data[2 + i];
                a = (pixel >> 24) & 0xff;
                r = (pixel >> 16) & 0xff;
                g = (pixel >> 8) & 0xff;
                b = (pixel >> 0) & 0xff;

                /* Cairo uses premultiplied alpha */
                r = (r * a) / 0xff;
                g = (g * a) / 0xff;
                b = (b * a) / 0xff;

                icon_data[i] = (static_cast<uint32_t>(a) << 24) | (r << 16) | (g << 8) | b;
            }

            this->icon = cairo_image_surface_create_for_data(
                reinterpret_cast<unsigned char *>(icon_data),
                CAIRO_FORMAT_ARGB32,
                width,
                height,
                width * 4);
        }

        ~i3WindowIcon() {
            cairo_surface_destroy(this->icon);
            delete[] icon_data;
        }
    };

    enum window_dock_t {
        W_NODOCK = 0,
        W_DOCK_TOP = 1,
        W_DOCK_BOTTOM = 2
    };

    /**
     * A 'Window' is a type which contains an xcb_window_t and all the related
     * information (hints like _NET_WM_NAME for that window).
     *
     */
    class i3Window {
      public:
        xcb_window_t id{XCB_WINDOW_NONE};

        /** Holds the xcb_window_t (just an ID) for the leader window (logical
         * parent for toolwindows and similar floating windows) */
        xcb_window_t leader{XCB_WINDOW_NONE};
        xcb_window_t transient_for{XCB_WINDOW_NONE};

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
        bool name_x_changed{false};

        /** Whether the application used _NET_WM_NAME */
        bool uses_net_wm_name{false};

        /** Whether the application needs to receive WM_TAKE_FOCUS */
        bool needs_take_focus{false};

        /** Whether this window accepts focus. We store this inverted so that the
         * default will be 'accepts focus'. */
        bool doesnt_accept_focus{false};

        /** The _NET_WM_WINDOW_TYPE for this window. */
        xcb_atom_t window_type{XCB_ATOM_NONE};

        /** The _NET_WM_DESKTOP for this window. */
        uint32_t wm_desktop{0};

        /** Whether the window says it is a dock window */
        window_dock_t dock{W_NODOCK};

        /** When this window was marked urgent. 0 means not urgent */
        std::optional<std::chrono::time_point<std::chrono::system_clock>> urgent{};

        /** Pixels the window reserves. left/right/top/bottom */
        reservedpx reserved{};

        /** Depth of the window */
        uint16_t depth{0};

        /* the wanted size of the window, used in combination with size
         * increments (see below). */
        int base_width{0};
        int base_height{0};

        /* minimum increment size specified for the window (in pixels) */
        int width_increment{0};
        int height_increment{0};

        /* Minimum size specified for the window. */
        uint32_t min_width{0};
        uint32_t min_height{0};

        /* Maximum size specified for the window. */
        uint32_t max_width{0};
        uint32_t max_height{0};

        /* aspect ratio from WM_NORMAL_HINTS (MPlayer uses this for example) */
        double min_aspect_ratio{0};
        double max_aspect_ratio{0};

        /** Window icon, as Cairo surface */
        std::unique_ptr<i3WindowIcon> icon{};

        /** The window has a nonrectangular shape. */
        bool shaped{false};
        /** The window has a nonrectangular input shape. */
        bool input_shaped{false};

        /* Time when the window became managed. Used to determine whether a window
         * should be swallowed after initial management. */
        std::chrono::time_point<std::chrono::system_clock> managed_since{};

        /* The window has been swallowed. */
        bool swallowed{false};

        explicit i3Window(xcb_window_t id)
            : id(id) {};
        i3Window() = delete;
        i3Window(i3Window const &) = delete;
        i3Window &operator=(i3Window const &) = delete;

        /**
         * Updates the WM_CLASS (consisting of the class and instance) for the
         * given window.
         *
         */
        void window_update_class(std::string &window_class, std::string &window_instance);

        /**
         * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
         * window. Further updates using window_update_name_legacy will be ignored.
         *
         */
        void window_update_name(std::string &name);

        /**
         * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
         * touch what the client sends us but pass it to xcb_image_text_8. To get
         * proper unicode rendering, the application has to use _NET_WM_NAME (see
         * window_update_name()).
         *
         */
        void window_update_name_legacy(std::string &name);

        /**
         * Updates the CLIENT_LEADER (logical parent window).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_leader(xcb_get_property_reply_t *prop);

        /**
         * Updates the TRANSIENT_FOR (logical parent window).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_transient_for(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_strut_partial(xcb_get_property_reply_t *prop);

        /**
         * Updates the WM_WINDOW_ROLE
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_role(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_WINDOW_TYPE property.
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_type(xcb_get_property_reply_t *reply);

        /**
         * Updates the WM_NORMAL_HINTS
         *
         */
        bool window_update_normal_hints(xcb_size_hints_t &size_hints, xcb_get_geometry_reply_t *geom);
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_normal_hints(xcb_get_property_reply_t *reply, xcb_get_geometry_reply_t *geom);

        /**
         * Updates the WM_HINTS (we only care about the input focus handling part).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
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
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_motif_hints(xcb_get_property_reply_t *prop, border_style_t *motif_border_style);

        /**
         * Updates the WM_CLIENT_MACHINE
         *
         */
        void window_update_machine(std::string const &machine);

        /**
         * Updates the _NET_WM_ICON
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_icon(xcb_get_property_reply_t *prop);
    };
}
