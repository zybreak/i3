module;
#include <fmt/core.h>
#include <fmt/printf.h>

#include <xcb/xcb.h>
#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>
module i3;

import std;
import utils;
import log;
import rect;

static void drag_window_callback(Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y,
                                 xcb_button_press_event_t const *event) {
    /* Reposition the client correctly while moving */
    con->rect.x = old_rect.x + (new_x - event->root_x);
    con->rect.y = old_rect.y + (new_y - event->root_y);

    render_con(con);
    x_push_node(con);
    xcb_flush(**global.x);

    /* Check if we cross workspace boundaries while moving */
    if (!floating_maybe_reassign_ws(con)) {
        return;
    }
    /* Ensure not to warp the pointer while dragging */
    x_set_warp_to(std::nullopt);
    tree_render();
}

/*
 * Called when the user clicked on the titlebar of a floating window.
 * Calls the drag_pointer function with the drag_window callback
 *
 */
void PropertyHandlers::floating_drag_window(Con *con, xcb_button_press_event_t const *event, bool use_threshold) {
    DLOG("floating_drag_window\n");

    /* Push changes before dragging, so that the window gets raised now and not
     * after the user releases the mouse button */
    tree_render();

    /* Store the initial rect in case of user revert/cancel */
    Rect initial_rect = con->rect;

    /* Drag the window */
    drag_result_t drag_result = inputManager.drag_pointer(con, event, XCB_NONE, XCURSOR_CURSOR_MOVE, use_threshold, drag_window_callback);

    if (!con->exists()) {
        DLOG("The container has been closed in the meantime.\n");
        return;
    }

    /* If the user cancelled, undo the changes. */
    if (drag_result == DRAG_REVERT) {
        floating_reposition(con, initial_rect);
        return;
    }

    tree_render();
}

/*
 * This is an ugly data structure which we need because there is no standard
 * way of having nested functions (only available as a gcc extension at the
 * moment, clang doesn’t support it) or blocks (only available as a clang
 * extension and only on Mac OS X systems at the moment).
 *
 */
struct resize_window_callback_params {
    border_t const corner;
    bool const proportional;
};

static void resize_window_callback(Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y,
                                   xcb_button_press_event_t const *event, resize_window_callback_params const *params) {
    border_t corner = params->corner;

    int32_t dest_x = con->rect.x;
    int32_t dest_y = con->rect.y;
    uint32_t dest_width;
    uint32_t dest_height;

    double ratio = static_cast<double>(old_rect.width) / old_rect.height;

    /* First guess: We resize by exactly the amount the mouse moved,
     * taking into account in which corner the client was grabbed */
    if (corner & BORDER_LEFT) {
        dest_width = old_rect.width - (new_x - event->root_x);
    } else {
        dest_width = old_rect.width + (new_x - event->root_x);
    }

    if (corner & BORDER_TOP) {
        dest_height = old_rect.height - (new_y - event->root_y);
    } else {
        dest_height = old_rect.height + (new_y - event->root_y);
    }

    /* User wants to keep proportions, so we may have to adjust our values */
    if (params->proportional) {
        dest_width = std::max(dest_width, static_cast<uint32_t>(dest_height * ratio));
        dest_height = std::max(dest_height, static_cast<uint32_t>(dest_width / ratio));
    }

    con->rect = (Rect){static_cast<uint32_t>(dest_x), static_cast<uint32_t>(dest_y), dest_width, dest_height};

    /* Obey window size */
    floating_check_size(con, false);

    /* If not the lower right corner is grabbed, we must also reposition
     * the client by exactly the amount we resized it */
    if (corner & BORDER_LEFT) {
        dest_x = old_rect.x + (old_rect.width - con->rect.width);
    }

    if (corner & BORDER_TOP) {
        dest_y = old_rect.y + (old_rect.height - con->rect.height);
    }

    con->rect.x = dest_x;
    con->rect.y = dest_y;

    render_con(con);
    x_push_changes(global.croot);
}

/*
 * Called when the user clicked on a floating window while holding the
 * floating_modifier and the right mouse button.
 * Calls the drag_pointer function with the resize_window callback
 *
 */
void PropertyHandlers::floating_resize_window(Con *con, bool const proportional,
                                              xcb_button_press_event_t const *event) {
    DLOG("floating_resize_window\n");

    /* Push changes before resizing, so that the window gets raised now and not
     * after the user releases the mouse button */
    tree_render();

    /* corner saves the nearest corner to the original click. It contains
     * a bitmask of the nearest borders (BORDER_LEFT, BORDER_RIGHT, …) */
    auto corner = static_cast<border_t>(0);

    if (event->event_x <= static_cast<int16_t>(con->rect.width / 2)) {
        corner = static_cast<border_t>(corner | BORDER_LEFT);
    } else {
        corner = static_cast<border_t>(corner | BORDER_RIGHT);
    }

    int cursor = 0;
    if (event->event_y <= static_cast<int16_t>(con->rect.height / 2)) {
        corner = static_cast<border_t>(corner | BORDER_TOP);
        cursor = (corner & BORDER_LEFT) ? XCURSOR_CURSOR_TOP_LEFT_CORNER : XCURSOR_CURSOR_TOP_RIGHT_CORNER;
    } else {
        corner = static_cast<border_t>(corner | BORDER_BOTTOM);
        cursor = (corner & BORDER_LEFT) ? XCURSOR_CURSOR_BOTTOM_LEFT_CORNER : XCURSOR_CURSOR_BOTTOM_RIGHT_CORNER;
    }

    resize_window_callback_params params = {corner, proportional};

    /* get the initial rect in case of revert/cancel */
    Rect initial_rect = con->rect;

    auto cb = [&params](Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y,
                        xcb_button_press_event_t const *event) {
        resize_window_callback(con, old_rect, new_x, new_y, event, &params);
    };
    drag_result_t drag_result = inputManager.drag_pointer(con, event, XCB_NONE, cursor, false, cb);

    if (!con->exists()) {
        DLOG("The container has been closed in the meantime.\n");
        return;
    }

    /* If the user cancels, undo the resize */
    if (drag_result == DRAG_REVERT) {
        floating_reposition(con, initial_rect);
    }
}
