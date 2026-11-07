/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * resize.c: Interactive resizing.
 *
 */
module;
#include <cassert>

#include <fmt/printf.h>
module i3;

import std;
import log;
import rect;

/*
 * This is an ugly data structure which we need because there is no standard
 * way of having nested functions (only available as a gcc extension at the
 * moment, clang doesn’t support it) or blocks (only available as a clang
 * extension and only on Mac OS X systems at the moment).
 *
 */
struct callback_params {
    orientation_t orientation;
    Con *output;
    xcb::xproto::window_t helpwin;
    uint32_t *new_position;
    bool *threshold_exceeded;
};

static void resize_callback(Con *con, Rect const &, uint32_t new_x, uint32_t new_y,
                            xcb::xproto::button_press_event_t const *event, callback_params const *params) {
    Con *output = params->output;
    DLOG(fmt::sprintf("new x = %d, y = %d\n", new_x, new_y));

    if (!*params->threshold_exceeded) {
        xcb::xproto::map_window(&global.x->connection(), params->helpwin);
        /* Warp pointer in the same way as resize_graphical_handler() would do
         * if threshold wasn't enabled, but also take into account travelled
         * distance. */
        if (params->orientation == HORIZ) {
            xcb::xproto::warp_pointer(&global.x->connection(), xcb::none, event->root, 0, 0, 0, 0,
                                      *params->new_position + new_x - event->root_x,
                                      new_y);
        } else {
            xcb::xproto::warp_pointer(&global.x->connection(), xcb::none, event->root, 0, 0, 0, 0,
                                      new_x,
                                      *params->new_position + new_y - event->root_y);
        }
        *params->threshold_exceeded = true;
        return;
    }

    if (params->orientation == HORIZ) {
        /* Check if the new coordinates are within screen boundaries */
        if (new_x > (output->rect.x + output->rect.width - 25) ||
            new_x < (output->rect.x + 25)) {
            return;
        }

        *(params->new_position) = new_x;
        xcb::xproto::configure_window(&global.x->connection(), params->helpwin,
                                      xcb::xproto::ConfigureWindowValueList{}.with_x(static_cast<int32_t>(*params->new_position)));
    } else {
        if (new_y > (output->rect.y + output->rect.height - 25) ||
            new_y < (output->rect.y + 25)) {
            return;
        }

        *(params->new_position) = new_y;
        xcb::xproto::configure_window(&global.x->connection(), params->helpwin,
                                      xcb::xproto::ConfigureWindowValueList{}.with_y(static_cast<int32_t>(*params->new_position)));
    }

    global.x->connection().flush();
}

void PropertyHandlers::resize_graphical_handler(Con *first, Con *second, orientation_t orientation,
                                                xcb::xproto::button_press_event_t const *event,
                                                bool use_threshold) {
    Con *output = first->con_get_output();
    DLOG(fmt::sprintf("x = %d, width = %d\n", output->rect.x, output->rect.width));
    DLOG(fmt::sprintf("first = %p / %s\n", fmt::ptr(first), first->name));
    DLOG(fmt::sprintf("second = %p / %s\n", fmt::ptr(second), second->name));

    x_mask_event_mask(~xcb::xproto::event_mask_t::enter_window);
    global.x->connection().flush();

    /* Open a new window, the resizebar. Grab the pointer and move the window
     * around as the user moves the pointer. */
    xcb::xproto::window_t grabwin = create_window(global.x->connection(), output->rect, xcb::copy_from_parent, xcb::copy_from_parent,
                                                  std::to_underlying(xcb::xproto::window_class_t::input_only), XCURSOR_CURSOR_POINTER, true,
                                                  xcb::xproto::CreateWindowValueList{}.with_override_redirect(1));

    /* Keep track of the coordinate orthogonal to motion so we can determine the
     * length of the resize afterward. */
    uint32_t initial_position, new_position;

    /* Configure the resizebar and snap the pointer. The resizebar runs along
     * the rect of the second con and follows the motion of the pointer. */
    Rect helprect{};
    helprect.x = second->rect.x;
    helprect.y = second->rect.y;
    /* Resizes might happen between a split container and a leaf
     * container. Because gaps happen *within* a split container, we need to
     * work with (any) leaf window inside the split, so descend focused. */
    Con *ffirst = con_descend_focused(first);
    Con *fsecond = con_descend_focused(second);
    if (orientation == HORIZ) {
        helprect.width = logical_px(global.x->root_screen, 2);
        helprect.height = second->rect.height;
        uint32_t const ffirst_right = ffirst->rect.x + ffirst->rect.width;
        uint32_t const gap = (fsecond->rect.x - ffirst_right);
        uint32_t const middle = fsecond->rect.x - (gap / 2);
        DLOG(fmt::sprintf("ffirst->rect = {.x = %u, .width = %u}\n", ffirst->rect.x, ffirst->rect.width));
        DLOG(fmt::sprintf("fsecond->rect = {.x = %u, .width = %u}\n", fsecond->rect.x, fsecond->rect.width));
        DLOG(fmt::sprintf("gap = %u, middle = %u\n", gap, middle));
        initial_position = middle;
    } else {
        helprect.width = second->rect.width;
        helprect.height = logical_px(global.x->root_screen, 2);
        uint32_t const ffirst_bottom = ffirst->rect.y + ffirst->rect.height;
        uint32_t const gap = (fsecond->rect.y - ffirst_bottom);
        uint32_t const middle = fsecond->rect.y - (gap / 2);
        DLOG(fmt::sprintf("ffirst->rect = {.y = %u, .height = %u}\n", ffirst->rect.y, ffirst->rect.height));
        DLOG(fmt::sprintf("fsecond->rect = {.y = %u, .height = %u}\n", fsecond->rect.y, fsecond->rect.height));
        DLOG(fmt::sprintf("gap = %u, middle = %u\n", gap, middle));
        initial_position = middle;
    }

    auto const values = xcb::xproto::CreateWindowValueList{}
                            .with_background_pixel(global.configManager->config->client.focused.border.colorpixel)
                            .with_override_redirect(1);

    xcb::xproto::window_t helpwin = create_window(global.x->connection(), helprect, xcb::copy_from_parent, xcb::copy_from_parent,
                                                  std::to_underlying(xcb::xproto::window_class_t::input_output), (orientation == HORIZ ? XCURSOR_CURSOR_RESIZE_HORIZONTAL : XCURSOR_CURSOR_RESIZE_VERTICAL), false, values);

    if (!use_threshold) {
        xcb::xproto::map_window(&global.x->connection(), helpwin);
        if (orientation == HORIZ) {
            xcb::xproto::warp_pointer(&global.x->connection(), xcb::none, event->root, 0, 0, 0, 0,
                                      initial_position, event->root_y);
        } else {
            xcb::xproto::warp_pointer(&global.x->connection(), xcb::none, event->root, 0, 0, 0, 0,
                                      event->root_x, initial_position);
        }
    }

    xcb::xproto::circulate_window(&global.x->connection(), xcb::xproto::circulate_t::raise_lowest, helpwin);

    global.x->connection().flush();

    /* `new_position' will be updated by the `resize_callback'. */
    new_position = initial_position;

    bool threshold_exceeded = !use_threshold;

    const struct callback_params params = {orientation, output, helpwin, &new_position, &threshold_exceeded};

    /* Re-render the tree before returning to the event loop (drag_pointer()
     * runs its own event-loop) in case if there are unrendered updates. */
    tree_render();

    auto cb = [&params](Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y,
                        xcb::xproto::button_press_event_t const *event) {
        resize_callback(con, old_rect, new_x, new_y, event, &params);
    };

    /* `drag_pointer' blocks until the drag is completed. */
    drag_result_t drag_result = inputManager.drag_pointer(nullptr, event, grabwin, 0, use_threshold, cb);

    xcb::xproto::destroy_window(&global.x->connection(), helpwin);
    xcb::xproto::destroy_window(&global.x->connection(), grabwin);
    global.x->connection().flush();

    /* User cancelled the drag so no action should be taken. */
    if (drag_result == DRAG_REVERT) {
        return;
    }

    int pixels = (new_position - initial_position);
    DLOG(fmt::sprintf("Done, pixels = %d\n", pixels));

    /* No change; no action needed. */
    if (pixels == 0) {
        return;
    }

    /* if we got thus far, the containers must have valid percentages. */
    assert(first->percent > 0.0);
    assert(second->percent > 0.0);
    bool const result = resize_neighboring_cons(first, second, pixels, 0);
    DLOG(fmt::sprintf("Graphical resize %s: first->percent = %f, second->percent = %f.\n",
                      result ? "successful" : "failed", first->percent, second->percent));
}
