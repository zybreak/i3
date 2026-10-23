/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * drag.c: click and drag.
 *
 */
module;
#include <fmt/printf.h>

#include <xcb/xcb.h>

#include <ev.h>

module i3;

import log;
import rect;

/* Custom data structure used to track dragging-related events. */
struct drag_x11_cb {
    InputManager *inputManager;
    PropertyHandlers *propertyHandlers;
    ev_prepare prepare;

    /* Whether this modal event loop should be exited and with which result. */
    drag_result_t result;

    /* The container that is being dragged or resized, or NULL if this is a
     * drag of the resize handle. */
    Con *con;

    /* The original event that initiated the drag. */
    xcb_button_press_event_t const *event;

    /* The dimensions of con when the loop was started. */
    Rect old_rect;

    /* The callback to invoke after every pointer movement. */
    callback_t callback;

    /* Drag distance threshold exceeded. If use_threshold is not set, then
     * threshold_exceeded is always true. */
    bool threshold_exceeded;

    /* Cursor to set after the threshold is exceeded. */
    xcb_cursor_t xcursor;
};

static bool threshold_exceeded(uint32_t x1, uint32_t y1,
                               uint32_t x2, uint32_t y2) {
    /* The threshold is about the height of one window decoration. */
    uint32_t const threshold = logical_px(global.x->root_screen, 15);
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) > threshold * threshold;
}

bool InputManager::drain_drag_events(EV_P_ drag_x11_cb *dragloop) {
    xcb::unique_C_ptr<xcb::generic_event_t> last_motion_notify{};
    PropertyHandlers *handlers = dragloop->propertyHandlers;

    while (auto event = x.connection().poll_for_event()) {
        if (event->response_type == 0) {
            auto const *error = reinterpret_cast<xcb::generic_error_t const *>(event.get());
            DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                              error->sequence, error->error_code));
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);
        /* The handlers take libxcb's event type, whose layout is xcbpp's. */
        auto *generic = reinterpret_cast<xcb_generic_event_t *>(event.get());

        switch (type) {
            case XCB_BUTTON_RELEASE:
                dragloop->result = DRAG_SUCCESS;
                break;

            case XCB_KEY_PRESS:
                DLOG("A key was pressed during drag, reverting changes.\n");
                dragloop->result = DRAG_REVERT;
                handlers->handle_event(type, generic);
                break;

            case XCB_UNMAP_NOTIFY: {
                auto const *unmap_event = reinterpret_cast<xcb::xproto::unmap_notify_event_t const *>(event.get());
                ConCon *con = con_by_window_id(unmap_event->window);

                if (con != nullptr) {
                    DLOG(fmt::sprintf("UnmapNotify for window 0x%08x (container %p)\n", unmap_event->window, fmt::ptr(con)));

                    if (con->con_get_workspace() == global.focused->con_get_workspace()) {
                        DLOG("UnmapNotify for a managed window on the current workspace, aborting\n");
                        dragloop->result = DRAG_ABORT;
                    }
                }

                handlers->handle_event(type, generic);
                break;
            }

            case XCB_MOTION_NOTIFY:
                /* motion_notify events are saved for later */
                last_motion_notify = std::move(event);
                break;

            default:
                DLOG("Passing to original handler\n");
                handlers->handle_event(type, generic);
                break;
        }

        if (dragloop->result != DRAGGING) {
            ev_break(EV_A_ EVBREAK_ONE);
            if (dragloop->result == DRAG_SUCCESS) {
                /* Ensure motion notify events are handled. */
                break;
            } else {
                return true;
            }
        }
    }

    if (!last_motion_notify) {
        return true;
    }
    auto const *motion = reinterpret_cast<xcb::xproto::motion_notify_event_t const *>(last_motion_notify.get());

    if (!dragloop->threshold_exceeded &&
        threshold_exceeded(motion->root_x, motion->root_y,
                           dragloop->event->root_x, dragloop->event->root_y)) {
        if (dragloop->xcursor != XCB_NONE) {
            xcb::xproto::change_active_pointer_grab(
                &x.connection(),
                dragloop->xcursor,
                XCB_CURRENT_TIME,
                xcb::xproto::event_mask_t::EVENT_MASK_BUTTON_RELEASE | xcb::xproto::event_mask_t::EVENT_MASK_POINTER_MOTION);
        }
        dragloop->threshold_exceeded = true;
    }

    /* Ensure that we are either dragging the resize handle (con is NULL) or that the
     * container still exists. The latter might not be true, e.g., if the window closed
     * for any reason while the user was dragging it. */
    if (dragloop->threshold_exceeded && (!dragloop->con || dragloop->con->exists())) {
        dragloop->callback(
            dragloop->con,
            dragloop->old_rect,
            motion->root_x,
            motion->root_y,
            dragloop->event);
    }

    x.connection().flush();
    return dragloop->result != DRAGGING;
}

/*
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
drag_result_t InputManager::drag_pointer(Con *con, xcb_button_press_event_t const *event,
                                         xcb_window_t confine_to, int cursor,
                                         bool use_threshold, callback_t callback) {
    xcb_cursor_t xcursor = cursor ? x.xcursor_get_cursor(static_cast<xcursor_cursor_t>(cursor)) : XCB_NONE;

    xcb::Connection &conn = x.connection();

    /* Grab the pointer (its status isn't checked, only that the request succeeded) */
    auto const reply = xcb::xproto::grab_pointer(&conn,
                                                 false,                                                                                                        /* get all pointer events specified by the following mask */
                                                 x.root,                                                                                                       /* grab the root window */
                                                 xcb::xproto::event_mask_t::EVENT_MASK_BUTTON_RELEASE | xcb::xproto::event_mask_t::EVENT_MASK_POINTER_MOTION, /* which events to let through */
                                                 xcb::xproto::grab_mode_t::GRAB_MODE_ASYNC,                                                                    /* pointer events should continue as normal */
                                                 xcb::xproto::grab_mode_t::GRAB_MODE_ASYNC,                                                                    /* keyboard mode */
                                                 confine_to,                                                                                                   /* confine_to = in which window should the cursor stay */
                                                 use_threshold ? XCB_NONE : xcursor,                                                                           /* possibly display a special cursor */
                                                 XCB_CURRENT_TIME)
                           .get();
    if (!reply) {
        ELOG(fmt::sprintf("Could not grab pointer (error_code = %d)\n", reply.error().error_code));
        return DRAG_ABORT;
    }

    /* Grab the keyboard */
    auto const keyb_reply = xcb::xproto::grab_keyboard(&conn,
                                                       false,  /* get all keyboard events */
                                                       x.root, /* grab the root window */
                                                       XCB_CURRENT_TIME,
                                                       xcb::xproto::grab_mode_t::GRAB_MODE_ASYNC, /* continue processing pointer events as normal */
                                                       xcb::xproto::grab_mode_t::GRAB_MODE_ASYNC  /* keyboard mode */
                                                       )
                                .get();
    if (!keyb_reply) {
        ELOG(fmt::sprintf("Could not grab keyboard (error_code = %d)\n", keyb_reply.error().error_code));
        xcb::xproto::ungrab_pointer(&conn, XCB_CURRENT_TIME);
        return DRAG_ABORT;
    }

    /* Go into our own event loop */
    drag_x11_cb loop = {
        .inputManager = this,
        .propertyHandlers = global.handlers,
        .result = DRAGGING,
        .con = con,
        .event = event,
        .callback = callback,
        .threshold_exceeded = !use_threshold,
        .xcursor = xcursor};
    ev_prepare *prepare = &loop.prepare;
    if (con) {
        loop.old_rect = con->rect;
    }
    ev_prepare_init(prepare, [](EV_P_ ev_prepare *w, int revents) {
        auto *dragloop = (struct drag_x11_cb *)w->data;
        while (!dragloop->inputManager->drain_drag_events(EV_A_ dragloop)) {
            /* repeatedly drain events: draining might produce additional ones */
        }
    });
    prepare->data = &loop;
    global.eventHandler->main_set_x11_cb(false);
    ev_prepare_start(global.main_loop, prepare);

    ev_run(global.main_loop, 0);

    ev_prepare_stop(global.main_loop, prepare);
    global.eventHandler->main_set_x11_cb(true);

    xcb::xproto::ungrab_keyboard(&conn, XCB_CURRENT_TIME);
    xcb::xproto::ungrab_pointer(&conn, XCB_CURRENT_TIME);
    conn.flush();

    return loop.result;
}
