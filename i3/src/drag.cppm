/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * drag.c: click and drag.
 *
 */
module;
#include <xcb/xcb.h>
#include <ev.h>
export module i3:drag;

import std;
import rect;

class Con;
class X;

export {
    /** Callback for dragging */
    using callback_t = std::function<void(Con *, Rect const &, uint32_t, uint32_t, xcb_button_press_event_t const *)>;

    /**
     * This is the return value of a drag operation like drag_pointer.
     *
     * DRAGGING will indicate the drag action is still in progress and can be
     * continued or resolved.
     *
     * DRAG_SUCCESS will indicate the intention of the drag action should be
     * carried out.
     *
     * DRAG_REVERT will indicate an attempt should be made to restore the state of
     * the involved windows to their condition before the drag.
     *
     * DRAG_ABORT will indicate that the intention of the drag action cannot be
     * carried out (e.g. because the window has been unmapped).
     *
     */
    enum drag_result_t {
        DRAGGING = 0,
        DRAG_SUCCESS,
        DRAG_REVERT,
        DRAG_ABORT
    };

    class InputManager {
      private:
        X &x;
        bool drain_drag_events(EV_P_ struct drag_x11_cb *dragloop);

      public:
        InputManager(X &x)
            : x(x) {};
        /**
         * This function grabs your pointer and keyboard and lets you drag stuff around
         * (borders). Every time you move your mouse, an XCB_MOTION_NOTIFY event will
         * be received and the given callback will be called with the parameters
         * specified (client, the original event), the original rect of the client,
         * and the new coordinates (x, y).
         *
         * If use_threshold is set, dragging only starts after the user moves the
         * pointer past a certain threshold. That is, the cursor will not be set and the
         * callback will not be called until then.
         *
         */
        drag_result_t drag_pointer(Con *con, xcb_button_press_event_t const *event,
                                   xcb_window_t confine_to, int cursor,
                                   bool use_threshold, callback_t callback);
    };
}