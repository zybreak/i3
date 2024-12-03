/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * x.c: Interface to X11, transfers our in-memory state to X11 (see also
 *      render.c). Basically a big state machine.
 *
 */
module;
#include <config.h>
#include <xcb/xcb.h>
#include <xcb/shape.h>

#include <xpp/xpp.hpp>
#include <xpp/proto/randr.hpp>
#define explicit dont_use_cxx_explicit
#include <xpp/proto/xkb.hpp>
#undef explicit
#include <xpp/proto/shape.hpp>
#include <xpp/proto/bigreq.hpp>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_aux.h>
export module i3:x;

import std;
import rect;
import :internal;

class Con;
class ConCon;

export {
    using x_connection = xpp::connection<xpp::xkb::extension, xpp::shape::extension, xpp::big_requests::extension, xpp::randr::extension>;

    /*
     * Describes the X11 state we may modify (map state, position, window stack).
     * There is one entry per container. The state represents the current situation
     * as X11 sees it (with the exception of the order in the state_head CIRCLEQ,
     * which represents the order that will be pushed to X11, while old_state_head
     * represents the current order). It will be updated in x_push_changes().
     *
     */
    struct con_state {
        xcb_window_t id{};
        bool mapped{};
        bool unmap_now{};
        bool child_mapped{};
        bool is_hidden{};
        bool is_maximized_vert;
        bool is_maximized_horz;

        /* The con for which this state is. */
        Con *con{};

        /* For reparenting, we have a flag (need_reparent) and the X ID of the old
         * frame this window was in. The latter is necessary because we need to
         * ignore UnmapNotify events (by changing the window event mask). */
        bool need_reparent{};
        xcb_window_t old_frame{};

        /* The container was child of floating container during the previous call of
         * x_push_node(). This is used to remove the shape when the container is no
         * longer floating. */
        bool was_floating{};

        Rect rect{};
        Rect window_rect{};

        bool initial{};

        std::string name{};
    };

    class X {
      protected:
        xcb_cursor_context_t *ctx;
        std::map<xcursor_cursor_t, xcb_cursor_t> cursors{};

        xcb_visualtype_t *get_visual_type_for_root();

        explicit X(x_connection *conn)
            : conn(conn) {
        }

      public:
        /**
         * The EWMH support window that is used to indicate that an EWMH-compliant
         * window manager is present. This window is created when i3 starts and
         * kept alive until i3 exits.
         * We also use this window as the focused window if no other window is
         * available to be focused on the active workspace in order to prevent
         * keyboard focus issues (see #1378).
         */
        xcb_window_t ewmh_window;

        /* Color depth, visual id and colormap to use when creating windows and
         * pixmaps. Will use 32 bit depth and an appropriate visual, if available,
         * otherwise the root window’s default (usually 24 bit TrueColor). */
        uint8_t root_depth;
        /* The default visual_type to use if none is specified when creating the surface. Must be defined globally. */
        xcb_visualtype_t *visual_type;
        xcb_colormap_t colormap;

        unsigned int xcb_numlock_mask;
        /* Stores coordinates to warp mouse pointer to if set */
        std::optional<Rect> warp_to{};

        std::deque<con_state *> state_head{};
        std::deque<con_state *> old_state_head{};
        std::deque<con_state *> initial_mapping_head{};

        /* Because 'focused_id' might be reset to force input focus, we separately keep
         * track of the X11 window ID to be able to always tell whether the focused
         * window actually changed. */
        xcb_window_t last_focused{XCB_NONE};
        /** Stores the X11 window ID of the currently focused window */
        xcb_window_t focused_id{XCB_NONE};
        x_connection *conn;
        xcb_screen_t *root_screen;
        /* The screen (0 when you are using DISPLAY=:0) of the connection 'conn' */
        int conn_screen;
        xcb_atom_t wm_sn;
        xcb_window_t wm_sn_selection_owner;
        xcb_window_t root;

        int shape_base{-1};
        bool shape_supported{true};

        explicit X();
        virtual ~X() = default;

        operator x_connection *() {
            return conn;
        }

        void xcursor_load_cursors();
        xcb_cursor_t xcursor_get_cursor(xcursor_cursor_t c);

        /**
         * Sets the cursor of the root window to the 'pointer' cursor.
         *
         * This function is called when i3 is initialized, because with some login
         * managers, the root window will not have a cursor otherwise.
         *
         * We have a separate xcursor function to use the same X11 connection as the
         * xcursor_load_cursors() function. If we mix the Xlib and the XCB connection,
         * races might occur (even though we flush the Xlib connection).
         *
         */
        void xcursor_set_root_cursor(xcursor_cursor_t cursor_id);

        /**
         * Initializes the X11 part for the given container. Called exactly once for
         * every container from con_new().
         *
         */
        virtual void con_init(Con *con, std::optional<xcb_drawable_t> id = std::nullopt);
    };

    /*
     * Moves a child window from Container src to Container dest.
     *
     */
    void x_move_win(Con * src, Con * dest);

    /*
     * Re-initializes the associated X window state for this container. You have
     * to call this when you assign a client to an empty container to ensure that
     * its state gets updated correctly.
     *
     */
    void x_reinit(Con * con);

    /**
     * Reparents the child window of the given container (necessary for sticky
     * containers). The reparenting happens in the next call of x_push_changes().
     *
     */
    void x_reparent_child(Con * con, Con * old);

    /**
     * Kills the window decoration associated with the given container.
     *
     */
    void x_con_kill(Con * con);

    /*
     * Completely reinitializes the container's frame, without destroying the old window.
     *
     */
    void x_con_reframe(Con * con);

    /**
     * Returns true if the client supports the given protocol atom (like WM_DELETE_WINDOW)
     *
     */
    bool window_supports_protocol(xcb_window_t window, xcb_atom_t atom);

    /**
     * Kills the given X11 window using WM_DELETE_WINDOW (if supported).
     *
     */
    void x_window_kill(xcb_connection_t * c, xcb_window_t window, kill_window_t kill_window);

    /**
     * Draws the decoration of the given container onto its parent.
     *
     */
    void x_draw_decoration(Con * con);

    /**
     * Recursively calls x_draw_decoration. This cannot be done in x_push_node
     * because x_push_node uses focus order to recurse (see the comment above)
     * while drawing the decoration needs to happen in the actual order.
     *
     */
    void x_deco_recurse(Con * con);

    /**
     * This function pushes the properties of each node of the layout tree to
     * X11 if they have changed (like the map state, position of the window, …).
     * It recursively traverses all children of the given node.
     *
     */
    void x_push_node(Con * con);

    /**
     * Pushes all changes (state of each node, see x_push_node() and the window
     * stack) to X11.
     *
     */
    void x_push_changes(Con * con);

    /**
     * Raises the specified container in the internal stack of X windows. The
     * next call to x_push_changes() will make the change visible in X11.
     *
     */
    void x_raise_con(Con * con);

    /**
     * Sets the WM_NAME property (so, no UTF8, but used only for debugging anyways)
     * of the given name. Used for properly tagging the windows for easily spotting
     * i3 windows in xwininfo -root -all.
     *
     */
    void x_set_name(Con * con, std::string const &name);

    /**
     * Sets up i3 specific atoms (I3_SOCKET_PATH and I3_CONFIG_PATH)
     *
     */
    void x_set_i3_atoms();

    /**
     * Set warp_to coordinates.  This will trigger on the next call to
     * x_push_changes().
     *
     */
    void x_set_warp_to(std::optional<Rect> rect);

    /**
     * Applies the given mask to the event mask of every i3 window decoration X11
     * window. This is useful to disable EnterNotify while resizing so that focus
     * is untouched.
     *
     */
    void x_mask_event_mask(uint32_t mask);

    /**
     * Enables or disables nonrectangular shape of the container frame.
     */
    void x_set_shape(ConCon * con, xcb_shape_sk_t kind, bool enable);
}
