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
    xcb_motion_notify_event_t *last_motion_notify = nullptr;
    xcb_generic_event_t *event;
    PropertyHandlers *handlers = dragloop->propertyHandlers;

    while ((event = xcb_poll_for_event(*x)) != nullptr) {
        if (event->response_type == 0) {
            auto *error = (xcb_generic_error_t *)event;
            DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                              error->sequence, error->error_code));
            free(event);
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        switch (type) {
            case XCB_BUTTON_RELEASE:
                dragloop->result = DRAG_SUCCESS;
                break;

            case XCB_KEY_PRESS:
                DLOG("A key was pressed during drag, reverting changes.\n");
                dragloop->result = DRAG_REVERT;
                handlers->handle_event(type, event);
                break;

            case XCB_UNMAP_NOTIFY: {
                auto *unmap_event = (xcb_unmap_notify_event_t *)event;
                ConCon *con = con_by_window_id(unmap_event->window);

                if (con != nullptr) {
                    DLOG(fmt::sprintf("UnmapNotify for window 0x%08x (container %p)\n", unmap_event->window, fmt::ptr(con)));

                    if (con->con_get_workspace() == global.focused->con_get_workspace()) {
                        DLOG("UnmapNotify for a managed window on the current workspace, aborting\n");
                        dragloop->result = DRAG_ABORT;
                    }
                }

                handlers->handle_event(type, event);
                break;
            }

            case XCB_MOTION_NOTIFY:
                /* motion_notify events are saved for later */
                free(last_motion_notify);
                last_motion_notify = nullptr;
                last_motion_notify = (xcb_motion_notify_event_t *)event;
                break;

            default:
                DLOG("Passing to original handler\n");
                handlers->handle_event(type, event);
                break;
        }

        if (last_motion_notify != (xcb_motion_notify_event_t *)event) {
            free(event);
        }

        if (dragloop->result != DRAGGING) {
            ev_break(EV_A_ EVBREAK_ONE);
            if (dragloop->result == DRAG_SUCCESS) {
                /* Ensure motion notify events are handled. */
                break;
            } else {
                free(last_motion_notify);
                return true;
            }
        }
    }

    if (last_motion_notify == nullptr) {
        return true;
    }

    if (!dragloop->threshold_exceeded &&
        threshold_exceeded(last_motion_notify->root_x, last_motion_notify->root_y,
                           dragloop->event->root_x, dragloop->event->root_y)) {
        if (dragloop->xcursor != XCB_NONE) {
            xcb_change_active_pointer_grab(
                *x,
                dragloop->xcursor,
                XCB_CURRENT_TIME,
                XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION);
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
            last_motion_notify->root_x,
            last_motion_notify->root_y,
            dragloop->event);
    }
    free(last_motion_notify);
    last_motion_notify = nullptr;

    xcb_flush(*x);
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

    /* Grab the pointer */
    xcb_grab_pointer_cookie_t cookie;
    xcb_grab_pointer_reply_t *reply;
    xcb_generic_error_t *error;

    cookie = xcb_grab_pointer(*x,
                              false,                                                         /* get all pointer events specified by the following mask */
                              x.root,                                                        /* grab the root window */
                              XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION, /* which events to let through */
                              XCB_GRAB_MODE_ASYNC,                                           /* pointer events should continue as normal */
                              XCB_GRAB_MODE_ASYNC,                                           /* keyboard mode */
                              confine_to,                                                    /* confine_to = in which window should the cursor stay */
                              use_threshold ? XCB_NONE : xcursor,                            /* possibly display a special cursor */
                              XCB_CURRENT_TIME);

    if ((reply = xcb_grab_pointer_reply(*x, cookie, &error)) == nullptr) {
        ELOG(fmt::sprintf("Could not grab pointer (error_code = %d)\n", error->error_code));
        free(error);
        return DRAG_ABORT;
    }

    free(reply);

    /* Grab the keyboard */
    xcb_grab_keyboard_cookie_t keyb_cookie;
    xcb_grab_keyboard_reply_t *keyb_reply;

    keyb_cookie = xcb_grab_keyboard(*x,
                                    false,               /* get all keyboard events */
                                    x.root,              /* grab the root window */
                                    XCB_CURRENT_TIME,
                                    XCB_GRAB_MODE_ASYNC, /* continue processing pointer events as normal */
                                    XCB_GRAB_MODE_ASYNC  /* keyboard mode */
    );

    if ((keyb_reply = xcb_grab_keyboard_reply(*x, keyb_cookie, &error)) == nullptr) {
        ELOG(fmt::sprintf("Could not grab keyboard (error_code = %d)\n", error->error_code));
        free(error);
        xcb_ungrab_pointer(*x, XCB_CURRENT_TIME);
        return DRAG_ABORT;
    }

    free(keyb_reply);

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

    xcb_ungrab_keyboard(*x, XCB_CURRENT_TIME);
    xcb_ungrab_pointer(*x, XCB_CURRENT_TIME);
    xcb_flush(*x);

    return loop.result;
}
