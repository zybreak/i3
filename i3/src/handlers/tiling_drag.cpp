/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * tiling_drag.c: Reposition tiled windows by dragging.
 *
 */
module;
#include <xcb/xproto.h>
#include <fmt/printf.h>
module i3;

import rect;
import log;

static xcb_window_t create_drop_indicator(Rect rect);

/*
 * Converts direction to a string representation.
 *
 */
char const *direction_to_string(direction_t direction) {
    switch (direction) {
        case D_LEFT:
            return "left";
        case D_RIGHT:
            return "right";
        case D_UP:
            return "up";
        case D_DOWN:
            return "down";
    }
    return "invalid";
}

/*
 * Converts position to a string representation.
 *
 */
static char const *position_to_string(position_t position) {
    switch (position) {
        case BEFORE:
            return "before";
        case AFTER:
            return "after";
    }
    return "invalid";
}

static bool is_tiling_drop_target(Con *con) {
    if (!con->con_has_managed_window() ||
        con->con_is_floating() ||
        con->con_is_hidden()) {
        return false;
    }
    WorkspaceCon *ws = con->con_get_workspace();
    if (!workspace_is_visible(ws)) {
        return false;
    }
    Con *fs = ws->con_get_fullscreen_covering_ws();
    if (fs != nullptr && fs != con) {
        /* Workspace is visible, but con is not visible because some other
           container is in fullscreen. */
        return false;
    }
    return true;
}

/*
 * Returns whether there currently are any drop targets.
 * Used to only initiate a drag when there is something to drop onto.
 *
 */
bool PropertyHandlers::has_drop_targets() {
    int drop_targets = 0;
    for (auto &con : global.all_cons) {
        if (!is_tiling_drop_target(con)) {
            continue;
        }
        drop_targets++;
    }

    /* In addition to tiling containers themselves, an visible but empty
     * workspace (in a multi-monitor scenario) also is a drop target. */
    for (auto &output : global.croot->focused) {
        auto visible_ws = std::ranges::find_if(dynamic_cast<OutputCon *>(output)->output_get_content()->nodes, [](auto &child) { return workspace_is_visible(child); });
        if (visible_ws != dynamic_cast<OutputCon *>(output)->output_get_content()->nodes.end() && (*visible_ws)->con_num_children() == 0) {
            drop_targets++;
        }
    }

    return drop_targets > 1;
}

/*
 * Return an appropriate target at given coordinates.
 *
 */
static Con *find_drop_target(uint32_t x, uint32_t y) {
    for (auto const &con : global.all_cons) {
        Rect rect = con->rect;
        if (!rect.rect_contains(x, y) ||
            !is_tiling_drop_target(con)) {
            continue;
        }
        WorkspaceCon *ws = con->con_get_workspace();
        Con *fs = ws->con_get_fullscreen_covering_ws();
        return fs ? fs : con;
    }

    /* Couldn't find leaf container, get a workspace. */
    Output *output = global.randr->get_output_containing(x, y);
    if (!output) {
        return nullptr;
    }
    Con *content = output->con->output_get_content();
    /* Still descend because you can drag to the bar on an non-empty workspace. */
    return con_descend_tiling_focused(content);
}

enum drop_type_t { DT_SIBLING,
                   DT_CENTER,
                   DT_PARENT
};

struct callback_params {
    xcb_window_t *indicator;
    Con **target;
    direction_t *direction;
    drop_type_t *drop_type;
};

static Rect adjust_rect(Rect rect, direction_t direction, uint32_t threshold) {
    switch (direction) {
        case direction_t::D_LEFT:
            rect.width = threshold;
            break;
        case direction_t::D_UP:
            rect.height = threshold;
            break;
        case direction_t::D_RIGHT:
            rect.x += (rect.width - threshold);
            rect.width = threshold;
            break;
        case direction_t::D_DOWN:
            rect.y += (rect.height - threshold);
            rect.height = threshold;
            break;
    }
    return rect;
}

static bool con_on_side_of_parent(Con *con, direction_t direction) {
    orientation_t const orientation = orientation_from_direction(direction);
    direction_t reverse_direction;
    switch (direction) {
        case direction_t::D_LEFT:
            reverse_direction = D_RIGHT;
            break;
        case direction_t::D_RIGHT:
            reverse_direction = D_LEFT;
            break;
        case direction_t::D_UP:
            reverse_direction = D_DOWN;
            break;
        case direction_t::D_DOWN:
            reverse_direction = D_UP;
            break;
    }
    return (con_orientation(con->parent) != orientation ||
            con->parent->layout == L_STACKED || con->parent->layout == L_TABBED ||
            con_descend_direction(con->parent, reverse_direction) == con);
}

static void create_indicator(drop_type_t drop_type, direction_t direction, Con *target, Rect rect, bool draw_window, Con *con, Rect const &, uint32_t new_x, uint32_t new_y,
                             xcb_button_press_event_t const *event, callback_params const *params) {
    if (draw_window) {
        if (*(params->indicator) == 0) {
            *(params->indicator) = create_drop_indicator(rect);
        } else {
            uint32_t const values[4] = {rect.x, rect.y, rect.width, rect.height};
            uint32_t const mask = XCB_CONFIG_WINDOW_X |
                                  XCB_CONFIG_WINDOW_Y |
                                  XCB_CONFIG_WINDOW_WIDTH |
                                  XCB_CONFIG_WINDOW_HEIGHT;
            xcb_configure_window(**global.x, *(params->indicator), mask, values);
        }
    }
    x_mask_event_mask(~XCB_EVENT_MASK_ENTER_WINDOW);
    xcb_flush(**global.x);

    *(params->target) = target;
    *(params->direction) = direction;
    *(params->drop_type) = drop_type;
}

/*
 * Returns a new drop indicator window with the given initial coordinates.
 *
 */
static xcb_window_t create_drop_indicator(Rect rect) {
    uint32_t mask = 0;
    uint32_t values[2];

    mask = XCB_CW_BACK_PIXEL;
    values[0] = global.configManager->config->client.focused.indicator.colorpixel;

    mask |= XCB_CW_OVERRIDE_REDIRECT;
    values[1] = 1;

    xcb_window_t indicator = create_window(**global.x, rect, XCB_COPY_FROM_PARENT, XCB_COPY_FROM_PARENT,
                                           XCB_WINDOW_CLASS_INPUT_OUTPUT, XCURSOR_CURSOR_MOVE, false, mask, values);
    /* Change the window class to "i3-drag", so that it can be matched in a
     * compositor configuration. Note that the class needs to be changed before
     * mapping the window. */
    xcb_change_property(**global.x,
                        XCB_PROP_MODE_REPLACE,
                        indicator,
                        XCB_ATOM_WM_CLASS,
                        XCB_ATOM_STRING,
                        8,
                        (strlen("i3-drag") + 1) * 2,
                        "i3-drag\0i3-drag\0");
    xcb_map_window(**global.x, indicator);
    xcb_circulate_window(**global.x, XCB_CIRCULATE_RAISE_LOWEST, indicator);

    return indicator;
}

/*
 * The callback that is executed on every mouse move while dragging. On each
 * invocation we determine the drop target and the direction in which to insert
 * the dragged container. The indicator window is updated to show the new
 * position of the dragged container. The target container and direction are
 * passed out using the callback params.
 *
 */
static void drag_callback(Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y, xcb_button_press_event_t const *event, callback_params const *params) {
    /* 30% of the container (minus the parent indicator) is used to drop the
     * dragged container as a sibling to the target */
    double const sibling_indicator_percent_of_rect = 0.3;
    /* Use the base decoration height and add a few pixels. This makes the
     * outer indicator generally thin but at least thick enough to cover
     * container titles */
    double const parent_indicator_max_size = render_deco_height() + logical_px(global.x->root_screen, 5);

    Con *target = find_drop_target(new_x, new_y);
    if (target == nullptr) {
        return;
    }

    Rect rect = target->rect;

    direction_t direction = direction_t::D_LEFT;
    drop_type_t drop_type = DT_CENTER;
    bool draw_window = true;

    if (target->type == CT_WORKSPACE) {
        return create_indicator(drop_type, direction, target, rect, draw_window, con, old_rect, new_x, new_y, event, params);
    }

    /* Define the thresholds in pixels. The drop type depends on the cursor
     * position. */
    uint32_t const min_rect_dimension = std::min(rect.width, rect.height);
    uint32_t const sibling_indicator_size = std::max(logical_px(global.x->root_screen, 2), static_cast<long>(sibling_indicator_percent_of_rect * min_rect_dimension));
    uint32_t const parent_indicator_size = std::min(
        static_cast<uint32_t>(parent_indicator_max_size),
        /* For small containers, start where the sibling indicator finishes.
         * This is always at least 1 pixel. We use min() to not override the
         * sibling indicator: */
        sibling_indicator_size - 1);

    /* Find which edge the cursor is closer to. */
    uint32_t const d_left = new_x - rect.x;
    uint32_t const d_top = new_y - rect.y;
    uint32_t const d_right = rect.x + rect.width - new_x;
    uint32_t const d_bottom = rect.y + rect.height - new_y;
    uint32_t const d_min = std::min(std::min(d_left, d_right), std::min(d_top, d_bottom));
    /* And move the container towards that direction. */
    if (d_left == d_min) {
        direction = D_LEFT;
    } else if (d_top == d_min) {
        direction = D_UP;
    } else if (d_right == d_min) {
        direction = D_RIGHT;
    } else if (d_bottom == d_min) {
        direction = D_DOWN;
    } else {
        /* Keep the compiler happy */
        ELOG("min() is broken\n");
        std::terminate();
    }
    bool const target_parent = (d_min < parent_indicator_size &&
                                con_on_side_of_parent(target, direction));
    bool const target_sibling = (d_min < sibling_indicator_size);
    drop_type = target_parent ? DT_PARENT : (target_sibling ? DT_SIBLING : DT_CENTER);

    /* target == con makes sense only when we are moving away from target's parent. */
    if (drop_type != DT_PARENT && target == con) {
        draw_window = false;
        xcb_destroy_window(**global.x, *(params->indicator));
        *(params->indicator) = 0;
        return create_indicator(drop_type, direction, target, rect, draw_window, con, old_rect, new_x, new_y, event, params);
    }

    switch (drop_type) {
        case DT_PARENT:
            while (target->parent->type != CT_WORKSPACE && con_on_side_of_parent(target->parent, direction)) {
                target = target->parent;
            }
            rect = adjust_rect(target->parent->rect, direction, parent_indicator_size);
            break;
        case DT_CENTER:
            rect = target->rect;
            rect.x += sibling_indicator_size;
            rect.y += sibling_indicator_size;
            rect.width -= sibling_indicator_size * 2;
            rect.height -= sibling_indicator_size * 2;
            break;
        case DT_SIBLING:
            rect = adjust_rect(target->rect, direction, sibling_indicator_size);
            break;
    }
    return create_indicator(drop_type, direction, target, rect, draw_window, con, old_rect, new_x, new_y, event, params);
}

static bool con_swap(Con *con, Con *con2) {
    // TODO: I removed con_swap before, this stub is just here since new tiling_drag function relies on it
    return false;
}

/*
 * Initiates a mouse drag operation on a tiled window.
 *
 */
void PropertyHandlers::tiling_drag(Con *con, xcb_button_press_event_t *event, bool use_threshold) {
    DLOG(fmt::sprintf("Start dragging tiled container: con = %p\n", fmt::ptr(con)));
    bool set_focus = (con == global.focused);
    bool set_fs = con->fullscreen_mode != CF_NONE;

    /* Don't change focus while dragging. */
    x_mask_event_mask(~XCB_EVENT_MASK_ENTER_WINDOW);
    xcb_flush(*x);

    /* Indicate drop location while dragging. This blocks until the drag is completed. */
    Con *target = nullptr;
    direction_t direction;
    drop_type_t drop_type;
    xcb_window_t indicator = 0;
    callback_params const params{.indicator = &indicator, .target = &target, .direction = &direction, .drop_type = &drop_type};
    auto cb = [&params](Con *con, Rect const &old_rect, uint32_t new_x, uint32_t new_y, xcb_button_press_event_t const *event) {
        drag_callback(con, old_rect, new_x, new_y, event, &params);
    };

    drag_result_t drag_result = inputManager.drag_pointer(con, event, XCB_NONE, XCURSOR_CURSOR_MOVE, use_threshold, cb);

    /* Dragging is done. We don't need the indicator window any more. */
    xcb_destroy_window(*x, indicator);

    if (drag_result == DRAG_REVERT ||
        target == nullptr ||
        (target == con && drop_type != DT_PARENT) ||
        !target->exists()) {
        DLOG("drop aborted\n");
        return;
    }

    orientation_t const orientation = orientation_from_direction(direction);
    position_t const position = position_from_direction(direction);
    layout_t const layout = orientation == VERT ? L_SPLITV : L_SPLITH;
    con_disable_fullscreen(con);
    switch (drop_type) {
        case DT_CENTER: {
            /* Also handles workspaces.*/
            DLOG(fmt::sprintf("drop to center of %p\n", fmt::ptr(target)));
            const uint32_t mod = (global.configManager->config->swap_modifier & 0xFFFF);
            const bool swap_pressed = (mod != 0 && (event->state & mod) == mod);
            if (swap_pressed) {
                if (!con_swap(con, target)) {
                    return;
                }
            } else {
                con_move_to_target(con, target);
            }
            break;
        }
        case DT_SIBLING: {
            DLOG(fmt::sprintf("drop %s %p\n", position_to_string(position), fmt::ptr(target)));
            if (con_orientation(target->parent) != orientation) {
                /* If con and target are the only children of the same parent, we can just change
                 * the parent's layout manually and then move con to the correct position.
                 * tree_split checks for a parent with only one child so it would create a new
                 * parent with the new layout. */
                if (con->parent == target->parent && target->parent->con_num_children() == 2) {
                    target->parent->layout = layout;
                } else {
                    tree_split(target, orientation);
                }
            }

            insert_con_into(con, target, position);

            global.ipcManager->ipc_send_window_event("move", con);
            break;
        }
        case DT_PARENT: {
            bool const parent_tabbed_or_stacked = (target->parent->layout == L_TABBED || target->parent->layout == L_STACKED);
            DLOG(fmt::sprintf("drop %s (%s) of %s%p\n",
                              direction_to_string(direction),
                              position_to_string(position),
                              parent_tabbed_or_stacked ? "tabbed/stacked " : "",
                              fmt::ptr(target)));
            if (parent_tabbed_or_stacked) {
                /* When dealing with tabbed/stacked the target can be in the
                 * middle of the container. Thus, after a directional move, con
                 * will still be bound to the tabbed/stacked parent. */
                if (position == BEFORE) {
                    target = con::first(target->parent->nodes);
                } else {
                    target = con::last(target->parent->nodes);
                }
            }
            if (con != target) {
                insert_con_into(con, target, position);
            }
            /* tree_move can change the focus */
            Con *old_focus = global.focused;
            tree_move(con, direction);
            if (global.focused != old_focus) {
                old_focus->con_activate();
            }
            break;
        }
    }
    /* Warning: target might not exist anymore */
    target = nullptr;

    /* Manage fullscreen status. */
    if (set_focus || set_fs) {
        Con *fs = con->con_get_workspace()->con_get_fullscreen_covering_ws();
        if (fs == con) {
            ELOG("dragged container somehow got fullscreen again.\n");
            std::terminate();
        } else if (fs && set_focus && set_fs) {
            /* con was focused & fullscreen, disable other fullscreen container. */
            con_disable_fullscreen(fs);
        } else if (fs) {
            /* con was not focused, prefer other fullscreen container. */
            set_fs = set_focus = false;
        } else if (!set_focus) {
            /* con was not focused. If it was fullscreen and we are moving it to the focused
             * workspace we must focus it. */
            set_focus = (set_fs && global.focused->con_get_workspace() == con->con_get_workspace());
        }
    }
    if (set_fs) {
        con_enable_fullscreen(con, CF_OUTPUT);
    }
    if (set_focus) {
        workspaceManager.workspace_show(con->con_get_workspace());
        con->con_focus();
    }
    tree_render();
}
