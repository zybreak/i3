/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * click.c: Button press (mouse click) events.
 *
 */
module;
#include <cassert>

#include <cstdint>
#include <ranges>

#include <xcb/xcb.h>

#include "i3.h"
#include <fmt/printf.h>
#include <utility>
module i3;

import :output;
import log;
import rect;

enum click_destination_t {
    CLICK_BORDER = 0,
    CLICK_DECORATION = 1,
    CLICK_INSIDE = 2
};

/*
 * Finds the correct pair of first/second cons between the resize will take
 * place according to the passed border position (top, left, right, bottom),
 * then calls resize_graphical_handler().
 *
 */
static bool tiling_resize_for_border(Con *con, border_t border, xcb_button_press_event_t *event, bool use_threshold) {
    DLOG(fmt::sprintf("border = %d, con = %p\n", std::to_underlying(border), fmt::ptr(con)));
    Con *second = nullptr;
    Con *first = con;
    direction_t search_direction;
    switch (border) {
        case BORDER_LEFT:
            search_direction = D_LEFT;
            break;
        case BORDER_RIGHT:
            search_direction = D_RIGHT;
            break;
        case BORDER_TOP:
            search_direction = D_UP;
            break;
        case BORDER_BOTTOM:
            search_direction = D_DOWN;
            break;
        default:
            ELOG(fmt::sprintf("BUG: invalid border value %d\n", std::to_underlying(border)));
            return false;
    }

    bool res = resize_find_tiling_participants(&first, &second, search_direction, false);
    if (!res) {
        DLOG("No second container in this direction found.\n");
        return false;
    }
    if (first->fullscreen_mode != second->fullscreen_mode) {
        DLOG(fmt::sprintf("Avoiding resize between containers with different fullscreen modes, %d != %d\n", std::to_underlying(first->fullscreen_mode), std::to_underlying(second->fullscreen_mode)));
        return false;
    }

    assert(first != second);
    assert(first->parent == second->parent);

    /* The first container should always be in front of the second container */
    if (search_direction == D_UP || search_direction == D_LEFT) {
        Con *tmp = first;
        first = second;
        second = tmp;
    }

    const orientation_t orientation = ((border == BORDER_LEFT || border == BORDER_RIGHT) ? HORIZ : VERT);

    resize_graphical_handler(first, second, orientation, event, use_threshold);

    DLOG("After resize handler, rendering\n");
    tree_render();
    return true;
}

/*
 * Called when the user clicks using the floating_modifier, but the client is in
 * tiling layout.
 *
 * Returns false if it does not do anything (that is, the click should be sent
 * to the client).
 *
 */
static bool floating_mod_on_tiled_client(Con *con, xcb_button_press_event_t *event) {
    /* The client is in tiling layout. We can still initiate a resize with the
     * right mouse button, by choosing the border which is the most near one to
     * the position of the mouse pointer */
    uint32_t to_right = con->rect.width - event->event_x,
             to_left = event->event_x,
             to_top = event->event_y,
             to_bottom = con->rect.height - event->event_y;

    DLOG(fmt::sprintf("click was %d px to the right, %d px to the left, %d px to top, %d px to bottom\n",
                      to_right, to_left, to_top, to_bottom));

    if (to_right < to_left &&
        to_right < to_top &&
        to_right < to_bottom)
        return tiling_resize_for_border(con, BORDER_RIGHT, event, false);

    if (to_left < to_right &&
        to_left < to_top &&
        to_left < to_bottom)
        return tiling_resize_for_border(con, BORDER_LEFT, event, false);

    if (to_top < to_right &&
        to_top < to_left &&
        to_top < to_bottom)
        return tiling_resize_for_border(con, BORDER_TOP, event, false);

    if (to_bottom < to_right &&
        to_bottom < to_left &&
        to_bottom < to_top)
        return tiling_resize_for_border(con, BORDER_BOTTOM, event, false);

    return false;
}

/*
 * Finds out which border was clicked on and calls tiling_resize_for_border().
 *
 */
static bool tiling_resize(Con *con, xcb_button_press_event_t *event, const click_destination_t dest, bool use_threshold) {
    /* check if this was a click on the window border (and on which one) */
    Rect bsr = con_border_style_rect(con);
    DLOG(fmt::sprintf("BORDER x = %d, y = %d for con %p, window 0x%08x\n",
                      event->event_x, event->event_y, fmt::ptr(con), event->event));
    DLOG(fmt::sprintf("checks for right >= %d\n", con->window_rect.x + con->window_rect.width));
    if (dest == CLICK_DECORATION) {
        return tiling_resize_for_border(con, BORDER_TOP, event, use_threshold);
    } else if (dest == CLICK_BORDER) {
        if (event->event_y >= 0 && event->event_y <= static_cast<int32_t>(bsr.y) &&
            event->event_x >= static_cast<int32_t>(bsr.x) && event->event_x <= static_cast<int32_t>(con->rect.width + bsr.width))
            return tiling_resize_for_border(con, BORDER_TOP, event, false);
    }
    if (event->event_x >= 0 && event->event_x <= static_cast<int32_t>(bsr.x) &&
        event->event_y >= static_cast<int32_t>(bsr.y) && event->event_y <= static_cast<int32_t>(con->rect.height + bsr.height))
        return tiling_resize_for_border(con, BORDER_LEFT, event, false);

    if (event->event_x >= static_cast<int32_t>(con->window_rect.x + con->window_rect.width) &&
        event->event_y >= static_cast<int32_t>(bsr.y) && event->event_y <= static_cast<int32_t>(con->rect.height + bsr.height))
        return tiling_resize_for_border(con, BORDER_RIGHT, event, false);

    if (event->event_y >= static_cast<int32_t>(con->window_rect.y + con->window_rect.height))
        return tiling_resize_for_border(con, BORDER_BOTTOM, event, false);

    return false;
}

static void allow_replay_pointer(xcb_timestamp_t time) {
    global.x->conn->allow_events(XCB_ALLOW_REPLAY_POINTER, time);
    global.x->conn->flush();
    tree_render();
}

/*
 * Being called by handle_button_press, this function calls the appropriate
 * functions for resizing/dragging.
 *
 */
static void route_click(x_connection *conn, Con *con, xcb_button_press_event_t *event, const bool mod_pressed, const click_destination_t dest) {
    DLOG(fmt::sprintf("--> click properties: mod = %d, destination = %d\n", mod_pressed ? 1 : 0, std::to_underlying(dest)));
    DLOG(fmt::sprintf("--> OUTCOME = %p\n", fmt::ptr(con)));
    DLOG(fmt::sprintf("type = %d, name = %s\n", std::to_underlying(con->type), con->name));

    /* don’t handle dockarea cons, they must not be focused */
    if (con->parent->type == CT_DOCKAREA) {
        allow_replay_pointer(event->time);
        return;
    }

    /* if the user has bound an action to this click, it should override the
     * default behavior. */
    Binding *bind = get_binding_from_xcb_event((xcb_generic_event_t *)event);
    if (bind && ((dest == CLICK_DECORATION && !bind->exclude_titlebar) ||
                 (dest == CLICK_INSIDE && bind->whole_window) ||
                 (dest == CLICK_BORDER && bind->border))) {
        run_binding(bind, con);

        /* ASYNC_POINTER eats the event */
        conn->allow_events(XCB_ALLOW_ASYNC_POINTER, event->time);
        conn->flush();

        return;
    }

    /* There is no default behavior for button release events so we are done. */
    if (event->response_type == XCB_BUTTON_RELEASE) {
        allow_replay_pointer(event->time);
        return;
    }

    /* Any click in a workspace should focus that workspace. If the
     * workspace is on another output we need to do a workspace_show in
     * order for i3bar (and others) to notice the change in workspace. */
    Con *ws = con->con_get_workspace();
    Con *focused_workspace = global.focused->con_get_workspace();

    if (!ws) {
        ws = con::first(con->con_get_output()->output_get_content()->focused);
        if (!ws) {
            allow_replay_pointer(event->time);
            return;
        }
    }

    /* get the floating con */
    Con *floatingcon = con->con_inside_floating();
    const bool proportional = (event->state & XCB_KEY_BUT_MASK_SHIFT) == XCB_KEY_BUT_MASK_SHIFT;
    const bool in_stacked = (con->parent->layout == L_STACKED || con->parent->layout == L_TABBED);
    const bool was_focused = global.focused == con;
    const bool is_left_click = (event->detail == XCB_BUTTON_CLICK_LEFT);
    const bool is_right_click = (event->detail == XCB_BUTTON_CLICK_RIGHT);
    const bool is_left_or_right_click = (is_left_click || is_right_click);
    const bool is_scroll = (event->detail == XCB_BUTTON_SCROLL_UP ||
                            event->detail == XCB_BUTTON_SCROLL_DOWN ||
                            event->detail == XCB_BUTTON_SCROLL_LEFT ||
                            event->detail == XCB_BUTTON_SCROLL_RIGHT);

    /* 1: see if the user scrolled on the decoration of a stacked/tabbed con */
    if (in_stacked && dest == CLICK_DECORATION && is_scroll) {
        DLOG("Scrolling on a window decoration\n");
        /* Correctly move workspace focus first, see: #5472 */
        workspace_show(ws);

        /* Use the focused child of the tabbed / stacked container, not the
         * container the user scrolled on. */
        Con *current = con::first(con->parent->focused);
        const position_t direction =
            (event->detail == XCB_BUTTON_SCROLL_UP || event->detail == XCB_BUTTON_SCROLL_LEFT) ? BEFORE : AFTER;
        Con *next = get_tree_next_sibling(current, direction);
        auto p = con_descend_focused(next ? next : current);
        p->con_activate();

        allow_replay_pointer(event->time);
        return;
    }

    /* 2: floating modifier pressed, initiate a drag */
    if (mod_pressed && is_left_click && !floatingcon &&
        (config.tiling_drag == TILING_DRAG_MODIFIER ||
         config.tiling_drag == TILING_DRAG_MODIFIER_OR_TITLEBAR) &&
        has_drop_targets()) {
        const bool use_threshold = !mod_pressed;
        tiling_drag(con, event, use_threshold);
        allow_replay_pointer(event->time);
        return;
    }

    /* 3: focus this con or one of its children. */
    Con *con_to_focus = con;
    if (in_stacked && dest == CLICK_DECORATION) {
        /* If the container is a tab/stacked container and the click happened
         * on a tab, switch to the tab. If the tab contents were already
         * focused, focus the tab container itself. If the tab container was
         * already focused, cycle back to focusing the tab contents. */
        if (was_focused || !global.focused->con_has_parent(con)) {
            while (!con_to_focus->focused.empty()) {
                con_to_focus = con::first(con_to_focus->focused);
            }
        }
    }
    if (ws != focused_workspace) {
        workspace_show(ws);
    }
    con_to_focus->con_activate();

    /* 4: For floating containers, we also want to raise them on click.
     * We will skip handling events on floating cons in fullscreen mode */
    Con *fs = ws->con_get_fullscreen_covering_ws();
    if (floatingcon != nullptr && fs != con) {
        /* 5: floating_modifier plus left mouse button drags */
        if (mod_pressed && is_left_click) {
            floating_drag_window(floatingcon, event, false);
            return;
        }

        /*  6: resize (floating) if this was a (left or right) click on the
         * left/right/bottom border, or a right click on the decoration.
         * also try resizing (tiling) if possible */
        if (mod_pressed && is_right_click) {
            DLOG("floating resize due to floatingmodifier\n");
            floating_resize_window(floatingcon, proportional, event);
            return;
        }

        if ((dest == CLICK_BORDER || dest == CLICK_DECORATION) &&
            is_left_or_right_click) {
            /* try tiling resize, but continue if it doesn’t work */
            DLOG("tiling resize with fallback\n");
            if (tiling_resize(con, event, dest, dest == CLICK_DECORATION && !was_focused)) {
                allow_replay_pointer(event->time);
                return;
            }
        }

        if (dest == CLICK_DECORATION && is_right_click) {
            DLOG("floating resize due to decoration right click\n");
            floating_resize_window(floatingcon, proportional, event);
            return;
        }

        if (dest == CLICK_BORDER && is_left_or_right_click) {
            DLOG("floating resize due to border click\n");
            floating_resize_window(floatingcon, proportional, event);
            return;
        }

        /* 7: dragging, if this was a click on a decoration (which did not lead
         * to a resize) */
        if (dest == CLICK_DECORATION && is_left_click) {
            floating_drag_window(floatingcon, event, !was_focused);
            return;
        }

        allow_replay_pointer(event->time);
        return;
    }

    /* 8: floating modifier pressed, or click in titlebar, initiate a drag */
    if (is_left_click &&
        ((config.tiling_drag == TILING_DRAG_TITLEBAR && dest == CLICK_DECORATION) ||
         (config.tiling_drag == TILING_DRAG_MODIFIER_OR_TITLEBAR &&
          (mod_pressed || dest == CLICK_DECORATION))) &&
        has_drop_targets()) {
        allow_replay_pointer(event->time);
        const bool use_threshold = !mod_pressed;
        tiling_drag(con, event, use_threshold);
        return;
    }

    /* 9: floating modifier pressed, initiate a resize */
    if (dest == CLICK_INSIDE && mod_pressed && is_right_click) {
        if (floating_mod_on_tiled_client(con, event)) {
            return;
        }
        /* Avoid propagating events to clients, since the user expects
         * $mod+click to be handled by i3. */
        conn->allow_events(XCB_ALLOW_ASYNC_POINTER, event->time);
        conn->flush();
        return;
    }
    /* 10: otherwise, check for border/decoration clicks and resize */
    if ((dest == CLICK_BORDER || dest == CLICK_DECORATION) &&
        is_left_or_right_click) {
        DLOG("Trying to resize (tiling)\n");
        tiling_resize(con, event, dest, dest == CLICK_DECORATION && !was_focused);
    }

    allow_replay_pointer(event->time);
}

/*
 * The button press X callback. This function determines whether the floating
 * modifier is pressed and where the user clicked (decoration, border, inside
 * the window).
 *
 * Then, route_click is called on the appropriate con.
 *
 */
void PropertyHandlers::handle_button_press(xcb_button_press_event_t *event) {
    Con *con;
    DLOG(fmt::sprintf("Button %d (state %d) %s on window 0x%08x (child 0x%08x) at (%d, %d) (root %d, %d)\n",
                      event->detail, event->state, (event->response_type == XCB_BUTTON_PRESS ? "press" : "release"),
                      event->event, event->child, event->event_x, event->event_y, event->root_x,
                      event->root_y));

    global.last_timestamp = event->time;

    const uint32_t mod = (config.floating_modifier & 0xFFFF);
    const bool mod_pressed = (mod != 0 && (event->state & mod) == mod);
    DLOG(fmt::sprintf("floating_mod = %d, detail = %d\n", mod_pressed, event->detail));
    if ((con = con_by_window_id(event->event))) {
        route_click(*global.x, con, event, mod_pressed, CLICK_INSIDE);
        return;
    }

    if (!(con = con_by_frame_id(event->event))) {
        /* Run bindings on the root window as well, see #2097. We only run it
         * if --whole-window was set as that's the equivalent for a normal
         * window. */
        if (event->event == global.x->root) {
            Binding *bind = get_binding_from_xcb_event((xcb_generic_event_t *)event);
            if (bind != nullptr && bind->whole_window) {
                run_binding(bind, nullptr);
            }
        }

        /* If the root window is clicked, find the relevant output from the
         * click coordinates and focus the output's active workspace. */
        if (event->event == global.x->root && event->response_type == XCB_BUTTON_PRESS) {
            Con *output, *ws;
            for (auto &c : global.croot->nodes) {
                output = c;
                if (!output->rect.rect_contains(event->event_x, event->event_y))
                    continue;

                ws = con::first(output->output_get_content()->focused);
                if (ws != global.focused->con_get_workspace()) {
                    workspace_show(ws);
                    tree_render();
                }
                return;
            }
            return;
        }

        ELOG("Clicked into unknown window?!\n");
        xcb_allow_events(**global.x, XCB_ALLOW_REPLAY_POINTER, event->time);
        xcb_flush(**global.x);
        return;
    }

    /* Check if the click was on the decoration of a child */
    if (con->window != nullptr) {
        if (con->deco_rect.rect_contains(event->event_x, event->event_y)) {
            route_click(*global.x, con, event, mod_pressed, CLICK_DECORATION);
            return;
        }
    } else {
        for (auto &child : con->nodes | std::views::reverse) {
            if (!child->deco_rect.rect_contains(event->event_x, event->event_y))
                continue;

            route_click(*global.x, child, event, mod_pressed, CLICK_DECORATION);
            return;
        }
    }

    if (event->child != XCB_NONE) {
        DLOG("event->child not XCB_NONE, so this is an event which originated from a click into the application, but the application did not handle it.\n");
        route_click(*global.x, con, event, mod_pressed, CLICK_INSIDE);
        return;
    }

    route_click(*global.x, con, event, mod_pressed, CLICK_BORDER);
}
