/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * move.c: Moving containers into some direction.
 *
 */
module;
#include <cassert>
#include <fmt/printf.h>
module i3;

import std;
import log;
import i3ipc;
/*
 * Returns the lowest container in the tree that has both a and b as descendants.
 *
 */
static Con *lowest_common_ancestor(Con *a, Con *b) {
    Con *parent_a = a;
    while (parent_a) {
        Con *parent_b = b;
        while (parent_b) {
            if (parent_a == parent_b) {
                return parent_a;
            }
            parent_b = parent_b->parent;
        }
        parent_a = parent_a->parent;
    }
    std::terminate();
}

/*
 * Returns the direct child of ancestor that contains con.
 *
 */
static Con *child_containing_con_recursively(Con *ancestor, Con *con) {
    Con *child = con;
    while (child && child->parent != ancestor) {
        child = child->parent;
        assert(child->parent);
    }
    return child;
}

/*
 * Returns true if the given container is the focused descendant of ancestor, recursively.
 *
 */
static bool is_focused_descendant(Con *con, Con *ancestor) {
    Con *current = con;
    while (current != ancestor) {
        if (con::first(current->parent->focused) != current) {
            return false;
        }
        current = current->parent;
        assert(current->parent);
    }
    return true;
}

/*
 * This function detaches 'con' from its parent and inserts it either before or
 * after 'target'.
 *
 */
void insert_con_into(Con *con, Con *target, position_t position) {
    Con *parent = target->parent;
    /* We need to preserve the old con->parent. While it might still be used to
     * insert the entry before/after it, we call the on_remove_child callback
     * afterwards which might then close the con if it is empty. */
    Con *old_parent = con->parent;

    /* We compare the focus order of the children of the lowest common ancestor. If con or
     * its ancestor is before target's ancestor then con should be placed before the target
     * in the focus stack. */
    Con *lca = lowest_common_ancestor(con, parent);
    if (lca == con) {
        ELOG("Container is being inserted into one of its descendants.\n");
        return;
    }

    Con *con_ancestor = child_containing_con_recursively(lca, con);
    Con *target_ancestor = child_containing_con_recursively(lca, target);
    bool moves_focus_from_ancestor = is_focused_descendant(con, con_ancestor);
    bool focus_before;

    /* Determine if con is going to be placed before or after target in the parent's focus stack. */
    if (con_ancestor == target_ancestor) {
        /* Happens when the target is con's old parent. Eg with layout V [ A H [ B C ] ],
         * if we move C up. Target will be H. */
        focus_before = moves_focus_from_ancestor;
    } else {
        /* Look at the focus stack order of the children of the lowest common ancestor. */
        auto current_it = std::ranges::find_if(lca->focused, [&con_ancestor, &target_ancestor](auto &current) {
            return (current == con_ancestor || current == target_ancestor);
        });
        focus_before = (current_it != lca->focused.end() && *current_it == con_ancestor);
    }

    /* If con is the focused container in our old ancestor we place the new ancestor
     * before the old ancestor in the focus stack. Example:
     * Consider the layout [ H [ V1 [ A* B ] V2 [ C ] ] ] where A is focused. We move to
     * a second workspace and from there we move A to the right and switch back to the
     * original workspace. Without the change focus would move to B instead of staying
     * with A. */
    if (moves_focus_from_ancestor && focus_before) {
        std::erase(lca->focused, target_ancestor);
        auto place_it = std::ranges::find(lca->focused, con_ancestor);
        if (place_it != lca->focused.end()) {
            //place_it = (place_it == lca->focused.begin()) ? place_it : std::prev(place_it);
            lca->focused.insert(place_it, target_ancestor);
        } else {
            lca->focused.push_front(target_ancestor);
        }
    }

    con->con_detach();
    con->parent->con_fix_percent();

    /* When moving to a workspace, we respect the user’s configured
     * workspace_layout */
    if (parent->type == CT_WORKSPACE) {
        Con *split = workspace_attach_to(dynamic_cast<WorkspaceCon *>(parent));
        if (split != parent) {
            DLOG("Got a new split con, using that one instead\n");
            con->parent = split;
            con->con_attach(split, false);
            DLOG("attached\n");
            con->percent = 0.0;
            split->con_fix_percent();
            con = split;
            DLOG(fmt::sprintf("ok, continuing with con %p instead\n", fmt::ptr(con)));
            con->con_detach();
        }
    }

    con->parent = parent;

    if (parent == lca) {
        if (focus_before) {
            /* Example layout: H [ A B* ], we move A up/down. 'target' will be H. */
            auto it = std::ranges::find(target->parent->focused, target);
            target->parent->focused.insert(it, con);
        } else {
            /* Example layout: H [ A B* ], we move A up/down. 'target' will be H. */
            auto it = std::ranges::find(parent->focused, target);
            if (std::next(it) != parent->focused.end()) {
                it++;
            }
            parent->focused.insert(it, con);
        }
    } else {
        if (focus_before) {
            /* Example layout: V [ H [ A B ] C* ], we move C up. 'target' will be A. */
            parent->focused.push_front(con);
        } else {
            /* Example layout: V [ H [ A* B ] C ], we move C up. 'target' will be A. */
            parent->focused.push_back(con);
        }
    }

    if (position == BEFORE) {
        target->insert_before(con);
    } else if (position == AFTER) {
        target->insert_after(con);
    }

    /* Pretend the con was just opened with regards to size percent values.
     * Since the con is moved to a completely different con, the old value
     * does not make sense anyways. */
    con->percent = 0.0;
    parent->con_fix_percent();

    old_parent->on_remove_child();
}

/*
 * This function detaches 'con' from its parent and puts it in the given
 * workspace. Position is determined by the direction of movement into the
 * workspace container.
 *
 */
static void attach_to_workspace(Con *con, Con *ws, direction_t direction) {
    con->con_detach();
    Con *old_parent = con->parent;
    con->parent = ws;

    if (direction == D_RIGHT || direction == D_DOWN) {
        ws->nodes.push_front(con);
    } else {
        ws->nodes.push_back(con);
    }
    ws->focused.push_back(con);

    /* Pretend the con was just opened with regards to size percent values.
     * Since the con is moved to a completely different con, the old value
     * does not make sense anyways. */
    con->percent = 0.0;
    ws->con_fix_percent();

    old_parent->con_fix_percent();
    old_parent->on_remove_child();
}

/*
 * Moves the given container to the closest output in the given direction if
 * such an output exists.
 *
 */
static void move_to_output_directed(Con *con, direction_t direction) {
    Output *current_output = get_output_for_con(con);
    Output *output = global.randr->get_output_next(direction, current_output, CLOSEST_OUTPUT);

    if (!output) {
        DLOG("No output in this direction found. Not moving.\n");
        return;
    }

    auto ws = std::ranges::find_if(output->con->output_get_content()->nodes, [](auto &child) { return workspace_is_visible(child); });

    if (ws != output->con->output_get_content()->nodes.end()) {
        DLOG("No workspace on output in this direction found. Not moving.\n");
        return;
    }

    WorkspaceCon *old_ws = con->con_get_workspace();
    const bool moves_focus = (global.focused == con);
    attach_to_workspace(con, *ws, direction);
    if (moves_focus) {
        /* workspace_show will not correctly update the active workspace because
         * the focused container, con, is now a child of ws. To work around this
         * and still produce the correct workspace focus events (see
         * 517-regress-move-direction-ipc.t) we need to temporarily set focused
         * to the old workspace.
         *
         * The following happen:
         * 1. Focus con to push it on the top of the focus stack in its new
         * workspace
         * 2. Set focused to the old workspace to force workspace_show to
         * execute
         * 3. workspace_show will descend focus and target our con for
         * focusing. This also ensures that the mouse warps correctly.
         * See: #3518. */
        con->con_focus();
        global.focused = old_ws;
        global.workspaceManager->workspace_show(dynamic_cast<WorkspaceCon*>(*ws));
        con->con_focus();
    }

    /* force re-painting the indicators */
    con->deco_render_params.reset();

    global.ipcManager->ipc_send_window_event("move", con);
    tree_flatten(global.croot);
    ewmh_update_wm_desktop();
}

/*
 * Moves the given container in the given direction
 *
 */
void tree_move(Con *con, direction_t direction) {
    position_t position;
    Con *target;

    DLOG(fmt::sprintf("Moving in direction %d\n", std::to_underlying(direction)));

    /* 1: get the first parent with the same orientation */

    if (con->type == CT_WORKSPACE) {
        DLOG("Not moving workspace\n");
        return;
    }

    if (con->fullscreen_mode == CF_GLOBAL) {
        DLOG("Not moving fullscreen global container\n");
        return;
    }

    if ((con->fullscreen_mode == CF_OUTPUT) ||
        (con->parent->type == CT_WORKSPACE && con->parent->con_num_children() == 1)) {
        /* This is the only con on this workspace */
        move_to_output_directed(con, direction);
        return;
    }

    orientation_t o = orientation_from_direction(direction);

    Con *same_orientation = con->con_parent_with_orientation(o);
    /* The do {} while is used to 'restart' at this point with a different
     * same_orientation, see the very last lines before the end of this block
     * */
    do {
        /* There is no parent container with the same orientation */
        if (!same_orientation) {
            if (con->con_is_floating()) {
                /* this is a floating con, we just disable floating */
                floating_disable(dynamic_cast<ConCon*>(con));
                return;
            }
            if (con->con_inside_floating()) {
                /* 'con' should be moved out of a floating container */
                DLOG("Inside floating, moving to workspace\n");
                attach_to_workspace(con, con->con_get_workspace(), direction);
                /* force re-painting the indicators */
                con->deco_render_params.reset();

                global.ipcManager->ipc_send_window_event("move", con);
                tree_flatten(global.croot);
                ewmh_update_wm_desktop();
                return;
            }
            DLOG("Force-changing orientation\n");
            ws_force_orientation(con->con_get_workspace(), o);
            same_orientation = con->con_parent_with_orientation(o);
        }

        /* easy case: the move is within this container */
        if (same_orientation == con->parent) {
            Con *swap = (direction == D_LEFT || direction == D_UP)
                            ? con::previous(con, con->parent->nodes)
                            : con::next(con, con->parent->nodes);
            if (swap) {
                if (!swap->con_is_leaf()) {
                    DLOG("Moving into our bordering branch\n");
                    target = con_descend_direction(swap, direction);
                    position = (con_orientation(target->parent) != o ||
                                        direction == D_UP ||
                                        direction == D_LEFT
                                    ? AFTER
                                    : BEFORE);
                    insert_con_into(con, target, position);
                    /* force re-painting the indicators */
                    con->deco_render_params.reset();

                    global.ipcManager->ipc_send_window_event("move", con);
                    tree_flatten(global.croot);
                    ewmh_update_wm_desktop();
                    return;
                }

                DLOG("Swapping with sibling.\n");
                if (direction == D_LEFT || direction == D_UP) {
                    auto swap_itr = std::ranges::find(con->parent->nodes, swap);
                    auto con_itr = std::ranges::find(con->parent->nodes, con);

                    *swap_itr = con;
                    *con_itr = swap;
                } else {
                    auto swap_itr = std::ranges::find(con->parent->nodes, swap);
                    auto con_itr = std::ranges::find(con->parent->nodes, con);

                    *swap_itr = con;
                    *con_itr = swap;
                }

                /* redraw parents to ensure all parent split container titles are updated correctly */
                con_force_split_parents_redraw(con);

                global.ipcManager->ipc_send_window_event("move", con);
                return;
            }

            if (con->parent == con->con_get_workspace()) {
                /* If we couldn't find a place to move it on this workspace, try
                 * to move it to a workspace on a different output */
                move_to_output_directed(con, direction);
                return;
            }

            /* If there was no con with which we could swap the current one,
             * search again, but starting one level higher. */
            same_orientation = con->parent->con_parent_with_orientation(o);
        }
    } while (same_orientation == nullptr);

    /* this time, we have to move to another container */
    /* This is the container *above* 'con' (an ancestor of con) which is inside
     * 'same_orientation' */
    Con *above = con;
    while (above->parent != same_orientation) {
        above = above->parent;
    }

    /* Enforce the fullscreen focus restrictions. */
    if (!con_fullscreen_permits_focusing(above->parent)) {
        LOG("Cannot move out of fullscreen container\n");
        return;
    }

    DLOG(fmt::sprintf("above = %p\n", fmt::ptr(above)));

    Con *next = (direction == D_UP || direction == D_LEFT) ? con::previous(above, above->parent->nodes) : con::next(above, above->parent->nodes);

    if (next && !next->con_is_leaf()) {
        DLOG("Moving into the bordering branch of our adjacent container\n");
        target = con_descend_direction(next, direction);
        position = (con_orientation(target->parent) != o ||
                            direction == D_UP ||
                            direction == D_LEFT
                        ? AFTER
                        : BEFORE);
        insert_con_into(con, target, position);
    } else if (!next &&
               con->parent->parent->type == CT_WORKSPACE &&
               con->parent->layout != L_DEFAULT &&
               con->parent->con_num_children() == 1) {
        /* Con is the lone child of a non-default layout container at the edge
         * of the workspace. Treat it as though the workspace is its parent
         * and move it to the next output. */
        DLOG("Grandparent is workspace\n");
        move_to_output_directed(con, direction);
        return;
    } else {
        DLOG("Moving into container above\n");
        position = (direction == D_UP || direction == D_LEFT ? BEFORE : AFTER);
        insert_con_into(con, above, position);
    }

    /* force re-painting the indicators */
    con->deco_render_params.reset();

    global.ipcManager->ipc_send_window_event("move", con);
    tree_flatten(global.croot);
    ewmh_update_wm_desktop();
}
