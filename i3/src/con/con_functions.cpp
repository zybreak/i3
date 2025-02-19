/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * con.c: Functions which deal with containers directly (creating containers,
 *        searching containers, getting specific properties from containers,
 *        …).
 *
 */
module;
#include <cassert>
#include <xcb/xcb.h>

#include <config.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <fmt/printf.h>
#include <glib.h>
module i3;

import std;
import :output;
import format_placeholders;
import utils;
import log;
import rect;
import i3ipc;

/*
 * Escapes the given string if a pango font is currently used.
 * If the string has to be escaped, the input string will be free'd.
 *
 */
static std::string pango_escape_markup(std::string input) {
    char *escaped = g_markup_escape_text(input.c_str(), -1);

    return escaped;
}

/*
 * force parent split containers to be redrawn
 *
 */
void con_force_split_parents_redraw(Con *con) {
    Con *parent = con;

    while (parent != nullptr && parent->type != CT_WORKSPACE && parent->type != CT_DOCKAREA) {
        if (!parent->con_is_leaf()) {
            parent->deco_render_params.reset();
        }

        parent = parent->parent;
    }
}

namespace con {
    Con *first(std::deque<Con *> &queue) {
        if (queue.empty()) {
            return nullptr;
        }

        return queue.front();
    }

    Con *next(Con *con, std::deque<Con *> &queue) {
        auto c_itr = std::ranges::find(queue, con);
        if (c_itr == queue.end()) {
            return nullptr;
        }

        return (std::next(c_itr) == queue.end()) ? nullptr : *(++c_itr);
    }

    Con *previous(Con *con, std::deque<Con *> &queue) {
        auto c_itr = std::ranges::find(queue, con);
        if (c_itr == queue.end()) {
            return nullptr;
        }

        return (c_itr == queue.begin()) ? nullptr : *(--c_itr);
    }

    Con *last(std::deque<Con *> &queue) {
        if (queue.empty()) {
            return nullptr;
        }

        return queue.back();
    }
}  // namespace con

/*
 *
 * Checks if the given container has an urgent child.
 *
 */
static bool con_has_urgent_child(Con *con) {
    if (con->con_is_leaf()) {
        return con->urgent;
    }

    /* We are not interested in floating windows since they can only be
     * attached to a workspace → nodes instead of focused */
    return std::ranges::any_of(con->nodes, [](Con *child) { return con_has_urgent_child(child); });
}

/*
 * Returns the container with the given client window ID or NULL if no such
 * container exists.
 *
 */
ConCon *con_by_window_id(xcb_window_t window) {
    for (auto const &c : global.all_cons) {
        auto con = dynamic_cast<ConCon *>(c);
        if (con && con->get_window() != nullptr && con->get_window()->id == window) {
            return con;
        }
    }
    return nullptr;
}

/*
 * Returns the container with the given frame ID or NULL if no such container
 * exists.
 *
 */
Con *con_by_frame_id(xcb_window_t frame) {
    for (auto const &con : global.all_cons) {
        if (con->frame->id == frame) {
            return con;
        }
    }
    return nullptr;
}

/*
 * Start from a container and traverse the transient_for linked list. Returns
 * true if target window is found in the list. Protects againsts potential
 * cycles.
 *
 */
bool con_find_transient_for_window(Con *start, xcb_window_t target) {
    Con *transient_con = start;
    int count = global.croot->con_num_windows();
    while (transient_con != nullptr &&
           transient_con->get_window() != nullptr &&
           transient_con->get_window()->transient_for != XCB_NONE) {
        DLOG(fmt::sprintf("transient_con = 0x%08x, transient_con->window->transient_for = 0x%08x, target = 0x%08x\n",
                          transient_con->get_window()->id, transient_con->get_window()->transient_for, target));
        if (transient_con->get_window()->transient_for == target) {
            return true;
        }
        auto next_transient = con_by_window_id(transient_con->get_window()->transient_for);
        if (next_transient == nullptr) {
            break;
        }
        /* Some clients (e.g. x11-ssh-askpass) actually set WM_TRANSIENT_FOR to
         * their own window id, so break instead of looping endlessly. */
        if (transient_con == next_transient) {
            break;
        }
        transient_con = next_transient;

        if (count-- <= 0) { /* Avoid cycles, see #4404 */
            break;
        }
    }
    return false;
}

/*
 * Returns the first container below 'con' which wants to swallow this window
 * TODO: priority
 *
 */
template <typename List>
static Con *_con_for_window(List &list, i3Window const *window, Match **store_match) {
    for (auto &child : list) {
        for (auto &match : child->swallow) {
            if (!match->match_matches_window(window)) {
                continue;
            }
            if (store_match != nullptr) {
                *store_match = match.get();
            }
            return child;
        }
        Con *result = con_for_window(child, window, store_match);
        if (result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

/*
 * Returns the first container below 'con' which wants to swallow this window
 * TODO: priority
 *
 */
Con *con_for_window(Con const *con, i3Window const *window, Match **store_match) {
    if (Con *result = _con_for_window(con->nodes, window, store_match); result != nullptr) {
        return result;
    }

    if (auto ws = dynamic_cast<WorkspaceCon const *>(con); ws) {
        if (Con *result = _con_for_window(ws->floating_windows, window, store_match); result != nullptr) {
            return result;
        }
    }

    return nullptr;
}

/*
 * Toggles fullscreen mode for the given container. If there already is a
 * fullscreen container on this workspace, fullscreen will be disabled and then
 * enabled for the container the user wants to have in fullscreen mode.
 *
 */
void con_toggle_fullscreen(Con *con, int fullscreen_mode) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    DLOG(fmt::sprintf("toggling fullscreen for %p / %s\n", fmt::ptr(con), con->name));

    if (con->fullscreen_mode == CF_NONE) {
        con_enable_fullscreen(con, static_cast<fullscreen_mode_t>(fullscreen_mode));
    } else {
        con_disable_fullscreen(con);
    }
}

/*
 * Sets the specified fullscreen mode for the given container, sends the
 * “fullscreen_mode” event and changes the XCB fullscreen property of the
 * container’s window, if any.
 *
 */
static void con_set_fullscreen_mode(xcb_connection_t *conn, Con *con, fullscreen_mode_t fullscreen_mode) {
    con->fullscreen_mode = fullscreen_mode;

    DLOG(fmt::sprintf("mode now: %d\n", std::to_underlying(con->fullscreen_mode)));

    /* Send an ipc window "fullscreen_mode" event */
    global.ipcManager->ipc_send_window_event("fullscreen_mode", con);

    /* update _NET_WM_STATE if this container has a window */
    /* TODO: when a window is assigned to a container which is already
     * fullscreened, this state needs to be pushed to the client, too */
    if (con->get_window() == nullptr) {
        return;
    }

    if (con->fullscreen_mode != CF_NONE) {
        DLOG(fmt::sprintf("Setting _NET_WM_STATE_FULLSCREEN for con = %p / window = %d.\n", fmt::ptr(con), con->get_window()->id));
        xcb_add_property_atom(conn, con->get_window()->id, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_FULLSCREEN]);
    } else {
        DLOG(fmt::sprintf("Removing _NET_WM_STATE_FULLSCREEN for con = %p / window = %d.\n", fmt::ptr(con), con->get_window()->id));
        xcb_remove_property_atom(conn, con->get_window()->id, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_FULLSCREEN]);
    }
}

/*
 * Enables fullscreen mode for the given container, if necessary.
 *
 * If the container’s mode is already CF_OUTPUT or CF_GLOBAL, the container is
 * kept fullscreen but its mode is set to CF_GLOBAL and CF_OUTPUT,
 * respectively.
 *
 * Other fullscreen containers will be disabled first, if they hide the new
 * one.
 *
 */
void con_enable_fullscreen(Con *con, fullscreen_mode_t fullscreen_mode) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    assert(fullscreen_mode == CF_GLOBAL || fullscreen_mode == CF_OUTPUT);

    if (fullscreen_mode == CF_GLOBAL) {
        DLOG(fmt::sprintf("enabling global fullscreen for %p / %s\n", fmt::ptr(con), con->name));
    } else {
        DLOG(fmt::sprintf("enabling fullscreen for %p / %s\n", fmt::ptr(con), con->name));
    }

    if (con->fullscreen_mode == fullscreen_mode) {
        DLOG(fmt::sprintf("fullscreen already enabled for %p / %s\n", fmt::ptr(con), con->name));
        return;
    }

    WorkspaceCon *con_ws = con->con_get_workspace();

    /* Disable any fullscreen container that would conflict the new one. */
    Con *fullscreen = global.croot->con_get_fullscreen_con(CF_GLOBAL);
    if (fullscreen == nullptr) {
        fullscreen = con_ws->con_get_fullscreen_con(CF_OUTPUT);
    }
    if (fullscreen != nullptr) {
        con_disable_fullscreen(fullscreen);
    }

    /* Set focus to new fullscreen container. Unless in global fullscreen mode
     * and on another workspace restore focus afterwards.
     * Switch to the container’s workspace if mode is global. */
    WorkspaceCon *cur_ws = global.focused->con_get_workspace();
    Con *old_focused = global.focused;
    if (fullscreen_mode == CF_GLOBAL && cur_ws != con_ws) {
        global.workspaceManager->workspace_show(con_ws);
    }
    con->con_activate();
    if (fullscreen_mode != CF_GLOBAL && cur_ws != con_ws) {
        old_focused->con_activate();
    }

    con_set_fullscreen_mode(**global.x, con, fullscreen_mode);
}

/*
 * Disables fullscreen mode for the given container regardless of the mode, if
 * necessary.
 *
 */
void con_disable_fullscreen(Con *con) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    DLOG(fmt::sprintf("disabling fullscreen for %p / %s\n", fmt::ptr(con), con->name));

    if (con->fullscreen_mode == CF_NONE) {
        DLOG(fmt::sprintf("fullscreen already disabled for %p / %s\n", fmt::ptr(con), con->name));
        return;
    }

    con_set_fullscreen_mode(**global.x, con, CF_NONE);
}

static bool _con_move_to_con(Con *con, Con *target, bool behind_focused, bool fix_coordinates, bool dont_warp, bool ignore_focus, bool fix_percentage) {
    Con *orig_target = target;

    /* Prevent moving if this would violate the fullscreen focus restrictions. */
    WorkspaceCon *target_ws = target->con_get_workspace();
    if (!ignore_focus && !con_fullscreen_permits_focusing(target_ws)) {
        LOG("Cannot move out of a fullscreen container.\n");
        return false;
    }

    if (con->con_is_floating()) {
        DLOG("Container is floating, using parent instead.\n");
        con = con->parent;
    }

    WorkspaceCon *source_ws = con->con_get_workspace();

    if (con->type == CT_WORKSPACE) {
        /* Re-parent all of the old workspace's floating windows. */
        while (!source_ws->floating_windows.empty()) {
            auto *child = source_ws->floating_windows.front();
            con_move_to_workspace(child, target_ws, true, true, false);
        }

        /* If there are no non-floating children, ignore the workspace. */
        if (con->con_is_leaf()) {
            return false;
        }

        con = workspace_encapsulate(con);
        if (con == nullptr) {
            ELOG("Workspace failed to move its contents into a container!\n");
            return false;
        }
    }

    /* Save the urgency state so that we can restore it. */
    bool urgent = con->urgent;

    /* Save the current workspace. So we can call workspace_show() by the end
     * of this function. */
    WorkspaceCon *current_ws = global.focused->con_get_workspace();

    OutputCon *source_output = con->con_get_output();
    OutputCon *dest_output = target_ws->con_get_output();

    /* 1: save the container which is going to be focused after the current
     * container is moved away */
    Con *focus_next = nullptr;
    if (!ignore_focus && source_ws == current_ws && target_ws != source_ws) {
        focus_next = con_descend_focused(source_ws);
        if (focus_next == con || focus_next->con_has_parent(con)) {
            focus_next = con_next_focused(con);
        }
    }

    /* 2: we go up one level, but only when target is a normal container */
    if (target->type != CT_WORKSPACE) {
        DLOG(fmt::sprintf("target originally = %p / %s / type %d\n", fmt::ptr(target), target->name, std::to_underlying(target->type)));
        target = target->parent;
    }

    /* 3: if the original target is the direct child of a floating container, we
     * can't move con next to it - floating containers have only one child - so
     * we get the workspace instead. */
    if (target->type == CT_FLOATING_CON) {
        DLOG("floatingcon, going up even further\n");
        orig_target = target;
        target = target->parent;
    }

    if (con->type == CT_FLOATING_CON) {
        WorkspaceCon *ws = target->con_get_workspace();
        DLOG(fmt::sprintf("This is a floating window, using workspace %p / %s\n", fmt::ptr(ws), ws->name));
        target = ws;
    }

    if (source_output != dest_output) {
        /* Take the relative coordinates of the current output, then add them
         * to the coordinate space of the correct output */
        if (fix_coordinates && con->type == CT_FLOATING_CON) {
            floating_fix_coordinates(dynamic_cast<FloatingCon *>(con), source_output->rect, dest_output->rect);
        } else {
            DLOG(fmt::sprintf("Not fixing coordinates, fix_coordinates flag = %d\n", fix_coordinates));
        }
    }

    /* If moving a fullscreen container and the destination already has a
     * fullscreen window on it, un-fullscreen the target's fullscreen con.
     * con->fullscreen_mode is not enough in some edge cases:
     * 1. con is CT_FLOATING_CON, child is fullscreen.
     * 2. con is the parent of a fullscreen container, can be triggered by
     * moving the parent with command criteria.
     */
    Con *fullscreen = target_ws->con_get_fullscreen_con(CF_OUTPUT);
    bool const con_has_fullscreen = con->fullscreen_mode != CF_NONE ||
                                    con->con_get_fullscreen_con(CF_GLOBAL) ||
                                    con->con_get_fullscreen_con(CF_OUTPUT);
    if (con_has_fullscreen && fullscreen != nullptr) {
        con_toggle_fullscreen(fullscreen, CF_OUTPUT);
        fullscreen = nullptr;
    }

    DLOG(fmt::sprintf("Re-attaching container to %p / %s\n", fmt::ptr(target), target->name));
    /* 4: re-attach the con to the parent of this focused container */
    Con *parent = con->parent;
    con->con_detach();
    con->con_attach(target, !behind_focused, behind_focused ? nullptr : orig_target);

    /* 5: fix the percentages */
    if (fix_percentage) {
        parent->con_fix_percent();
        con->percent = 0.0;
        target->con_fix_percent();
    }

    /* 6: focus the con on the target workspace, but only within that
     * workspace, that is, don’t move focus away if the target workspace is
     * invisible.
     * We don’t focus the con for i3 pseudo workspaces like __i3_scratch and
     * we don’t focus when there is a fullscreen con on that workspace. We
     * also don't do it if the caller requested to ignore focus. */
    if (!ignore_focus && !fullscreen) {
        /* We need to save the focused workspace on the output in case the
         * new workspace is hidden and it's necessary to immediately switch
         * back to the originally-focused workspace. */
        Con *old_focus_ws = con::first(dest_output->output_get_content()->focused);
        Con *old_focus = global.focused;
        con_descend_focused(con)->con_activate();

        if (old_focus_ws == current_ws && old_focus->type != CT_WORKSPACE) {
            /* Restore focus to the currently focused container. */
            old_focus->con_activate();
        } else if (global.focused->con_get_workspace() != old_focus_ws) {
            /* Restore focus if the output's focused workspace has changed. */
            con_descend_focused(old_focus_ws)->con_focus();
        }
    }

    /* 7: when moving to another workspace, we leave the focus on the current
     * workspace. (see also #809) */
    if (!ignore_focus) {
        global.workspaceManager->workspace_show(current_ws);
        if (dont_warp) {
            DLOG("x_set_warp_to(NULL) because dont_warp is set\n");
            x_set_warp_to(std::nullopt);
        }
    }

    /* Set focus only if con was on current workspace before moving.
     * Otherwise we would give focus to some window on different workspace. */
    if (focus_next) {
        con_descend_focused(focus_next)->con_activate();
    }

    /* 8. If anything within the container is associated with a startup sequence,
     * delete it so child windows won't be created on the old workspace. */
    if (!con->con_is_leaf()) {
        for (auto &child : con->nodes) {
            if (child->get_window() == nullptr) {
                continue;
            }
            global.applicationLauncher->startup_sequence_delete_by_window(child->get_window());
        }
    }

    if (con->get_window()) {
        global.applicationLauncher->startup_sequence_delete_by_window(con->get_window());
    }

    /* 9. If the container was marked urgent, move the urgency hint. */
    if (urgent) {
        workspace_update_urgent_flag(source_ws);
        con->con_set_urgency(true);
    }

    /* Ensure the container will be redrawn. */
    con->deco_render_params.reset();

    parent->on_remove_child();

    global.ipcManager->ipc_send_window_event("move", con);
    ewmh_update_wm_desktop();
    return true;
}

bool con_move_to_target(Con *con, Con *target) {
    /* For floating target containers, we just send the window to the same workspace. */
    if (target->con_is_floating()) {
        DLOG("target container is floating, moving container to target's workspace.\n");
        con_move_to_workspace(con, target->con_get_workspace(), true, false, false);
        return true;
    }

    if (target->type == CT_WORKSPACE && target->con_is_leaf()) {
        DLOG("target container is an empty workspace, simply moving the container there.\n");
        con_move_to_workspace(con, dynamic_cast<WorkspaceCon *>(target), true, false, false);
        return true;
    }

    /* For split containers, we use the currently focused container within it.
     * This allows setting marks on, e.g., tabbed containers which will move
     * con to a new tab behind the focused tab. */
    if (target->con_is_split()) {
        DLOG("target is a split container, descending to the currently focused child.\n");
        target = con::first(target->focused);
    }

    if (con == target || target->con_has_parent(con)) {
        DLOG("cannot move the container to or inside itself, aborting.\n");
        return false;
    }

    return _con_move_to_con(con, target, false, true, false, false, true);
}

/*
 * Moves the given container to the currently focused container on the given
 * workspace.
 *
 * The fix_coordinates flag will translate the current coordinates (offset from
 * the monitor position basically) to appropriate coordinates on the
 * destination workspace.
 * Not enabling this behaviour comes in handy when this function gets called by
 * floating_maybe_reassign_ws, which will only "move" a floating window when it
 * *already* changed its coordinates to a different output.
 *
 * The dont_warp flag disables pointer warping and will be set when this
 * function is called while dragging a floating window.
 *
 * If ignore_focus is set, the container will be moved without modifying focus
 * at all.
 *
 * TODO: is there a better place for this function?
 *
 */
void con_move_to_workspace(Con *con, WorkspaceCon *workspace, bool fix_coordinates, bool dont_warp, bool ignore_focus) {
    WorkspaceCon *source_ws = con->con_get_workspace();
    if (workspace == source_ws) {
        DLOG("Not moving, already there\n");
        return;
    }

    Con *target = con_descend_focused(workspace);
    _con_move_to_con(con, target, true, fix_coordinates, dont_warp, ignore_focus, true);
}

/*
 * Moves the given container to the currently focused container on the
 * visible workspace on the given output.
 *
 */
void con_move_to_output(Con *con, Output *output, bool fix_coordinates) {
    auto ws = std::ranges::find_if(output->con->output_get_content()->nodes, [](auto &child) { return workspace_is_visible(child); });
    assert(ws != output->con->output_get_content()->nodes.end());
    DLOG(fmt::sprintf("Moving con %p to output %s\n", fmt::ptr(con), output->output_primary_name()));
    con_move_to_workspace(con, dynamic_cast<WorkspaceCon *>(*ws), fix_coordinates, false, false);
}

/*
 * Returns the orientation of the given container (for stacked containers,
 * vertical orientation is used regardless of the actual orientation of the
 * container).
 *
 */
orientation_t con_orientation(Con const *const con) {
    switch (con->layout) {
        case layout_t::L_SPLITV:
        /* stacking containers behave like they are in vertical orientation */
        case layout_t::L_STACKED:
            return VERT;

        case layout_t::L_SPLITH:
        /* tabbed containers behave like they are in vertical orientation */
        case layout_t::L_TABBED:
            return HORIZ;

        case layout_t::L_DEFAULT:
            throw std::runtime_error("Someone called con_orientation() on a con with L_DEFAULT, this is a bug in the code.");

        case layout_t::L_DOCKAREA:
        case layout_t::L_OUTPUT:
            throw std::runtime_error(fmt::format("con_orientation() called on dockarea/output ({}) container {}", std::to_underlying(con->layout), fmt::ptr(con)));
    }
    /* should not be reached */
    std::terminate();
}

/*
 * Returns the container which will be focused next when the given container
 * is not available anymore. Called in tree_close_internal and con_move_to_workspace
 * to properly restore focus.
 *
 */
Con *con_next_focused(Con *con) {
    /* dock clients cannot be focused, so we focus the workspace instead */
    if (con->parent->type == CT_DOCKAREA) {
        DLOG("selecting workspace for dock client\n");
        return con_descend_focused(dynamic_cast<OutputCon *>(con->parent->parent)->output_get_content());
    }
    if (con->con_is_floating()) {
        con = con->parent;
    }

    /* if 'con' is not the first entry in the focus stack, use the first one as
     * it’s currently focused already */
    Con *next = con::first(con->parent->focused);
    if (next != con) {
        DLOG(fmt::sprintf("Using first entry %p\n", fmt::ptr(next)));
    } else {
        /* try to focus the next container on the same level as this one or fall
         * back to its parent */
        if (!(next = con::next(con, con->parent->focused))) {
            next = con->parent;
        }
    }

    /* now go down the focus stack as far as
     * possible, excluding the current container */
    while (!next->focused.empty() && con::first(next->focused) != con) {
        next = con::first(next->focused);
    }

    if (con->type == CT_FLOATING_CON && next != con->parent) {
        next = con_descend_focused(next);
    }

    return next;
}

/*
 * Returns the focused con inside this client, descending the tree as far as
 * possible. This comes in handy when attaching a con to a workspace at the
 * currently focused position, for example.
 *
 */
Con *con_descend_focused(Con *con) {
    Con *next = con;
    while (next != global.focused && !next->focused.empty()) {
        next = con::first(next->focused);
    }
    return next;
}

/*
 * Returns the focused con inside this client, descending the tree as far as
 * possible. This comes in handy when attaching a con to a workspace at the
 * currently focused position, for example.
 *
 * Works like con_descend_focused but considers only tiling cons.
 *
 */
Con *con_descend_tiling_focused(Con *con) {
    Con *next = con;
    Con *before;
    if (next == global.focused) {
        return next;
    }
    do {
        before = next;
        for (auto &child : next->focused) {
            if (child->type == CT_FLOATING_CON) {
                continue;
            }

            next = child;
            break;
        }
    } while (before != next && next != global.focused);
    return next;
}

/*
 * Returns the leftmost, rightmost, etc. container in sub-tree. For example, if
 * direction is D_LEFT, then we return the rightmost container and if direction
 * is D_RIGHT, we return the leftmost container.  This is because if we are
 * moving D_LEFT, and thus want the rightmost container.
 *
 */
Con *con_descend_direction(Con *con, direction_t direction) {
    Con *most = nullptr;
    int orientation = con_orientation(con);
    DLOG(fmt::sprintf("con_descend_direction(%p, orientation %d, direction %d)\n", fmt::ptr(con), orientation, std::to_underlying(direction)));
    if (direction == D_LEFT || direction == D_RIGHT) {
        if (orientation == HORIZ) {
            /* If the direction is horizontal, we can use either the first
             * (D_RIGHT) or the last con (D_LEFT) */
            if (direction == D_RIGHT) {
                most = con::first(con->nodes);
            } else {
                most = con::last(con->nodes);
            }
        } else if (orientation == VERT) {
            /* Wrong orientation. We use the last focused con. Within that con,
             * we recurse to chose the left/right con or at least the last
             * focused one. */
            for (auto &current : con->focused) {
                if (current->type != CT_FLOATING_CON) {
                    most = current;
                    break;
                }
            }
        } else {
            /* If the con has no orientation set, it’s not a split container
             * but a container with a client window, so stop recursing */
            return con;
        }
    }

    if (direction == D_UP || direction == D_DOWN) {
        if (orientation == VERT) {
            /* If the direction is vertical, we can use either the first
             * (D_DOWN) or the last con (D_UP) */
            if (direction == D_UP) {
                most = con::last(con->nodes);
            } else {
                most = con::first(con->nodes);
            }
        } else if (orientation == HORIZ) {
            /* Wrong orientation. We use the last focused con. Within that con,
             * we recurse to chose the top/bottom con or at least the last
             * focused one. */
            for (auto &current : con->focused) {
                if (current->type != CT_FLOATING_CON) {
                    most = current;
                    break;
                }
            }
        } else {
            /* If the con has no orientation set, it’s not a split container
             * but a container with a client window, so stop recursing */
            return con;
        }
    }

    if (!most) {
        return con;
    }
    return con_descend_direction(most, direction);
}

static bool has_outer_gaps(gaps_t gaps) {
    return gaps.top > 0 ||
           gaps.right > 0 ||
           gaps.bottom > 0 ||
           gaps.left > 0;
}

/*
 * Returns whether the window decoration (title bar) should be drawn into the
 * X11 frame window of this container (default) or into the X11 frame window of
 * the parent container (for stacked/tabbed containers).
 *
 */
bool con_draw_decoration_into_frame(Con const *const con) {
    return con->con_is_leaf() &&
           con_border_style(con) == border_style_t::BS_NORMAL &&
           (con->parent == nullptr ||
            (con->parent->layout != layout_t::L_TABBED &&
             con->parent->layout != layout_t::L_STACKED));
}

static Rect con_border_style_rect_without_title(Con const *const con) {

    if ((global.configManager->config->hide_edge_borders == HEBM_SMART && con->con_get_workspace()->con_num_visible_children() <= 1) ||
        (global.configManager->config->hide_edge_borders == HEBM_SMART_NO_GAPS && con->con_get_workspace()->con_num_visible_children() <= 1 && !has_outer_gaps(calculate_effective_gaps(con)))) {
        if (!con->con_is_floating()) {
            return (Rect){0, 0, 0, 0};
        }
    }

    adjacent_t borders_to_hide = ADJ_NONE;
    int border_width = con->current_border_width;
    DLOG(fmt::sprintf("The border width for con is set to: %d\n", con->current_border_width));
    Rect result;
    if (con->current_border_width < 0) {
        if (con->con_is_floating()) {
            border_width = global.configManager->config->default_floating_border_width;
        } else {
            border_width = global.configManager->config->default_border_width;
        }
    }
    DLOG(fmt::sprintf("Effective border width is set to: %d\n", border_width));
    /* Shortcut to avoid calling con_adjacent_borders() on dock containers. */
    border_style_t border_style = con_border_style(con);
    if (border_style == border_style_t::BS_NONE) {
        return (Rect){0, 0, 0, 0};
    }
    if (border_style == border_style_t::BS_NORMAL) {
        result = (Rect){static_cast<uint32_t>(border_width), 0, static_cast<uint32_t>(-2 * border_width), static_cast<uint32_t>(-border_width)};
    } else {
        result = (Rect){static_cast<uint32_t>(border_width), static_cast<uint32_t>(border_width), static_cast<uint32_t>(-2 * border_width), static_cast<uint32_t>(-2 * border_width)};
    }

    /* If hide_edge_borders is set to no_gaps and it did not pass the no border check, show all borders */
    if (global.configManager->config->hide_edge_borders == HEBM_SMART_NO_GAPS) {
        borders_to_hide = static_cast<adjacent_t>(con_adjacent_borders(con) & static_cast<adjacent_t>(HEBM_NONE));
    } else {
        borders_to_hide = static_cast<adjacent_t>(con_adjacent_borders(con) & static_cast<adjacent_t>(global.configManager->config->hide_edge_borders));
    }

    if (borders_to_hide & ADJ_LEFT_SCREEN_EDGE) {
        result.x -= border_width;
        result.width += border_width;
    }
    if (borders_to_hide & ADJ_RIGHT_SCREEN_EDGE) {
        result.width += border_width;
    }
    if (borders_to_hide & ADJ_UPPER_SCREEN_EDGE && (border_style != border_style_t::BS_NORMAL)) {
        result.y -= border_width;
        result.height += border_width;
    }
    if (borders_to_hide & ADJ_LOWER_SCREEN_EDGE) {
        result.height += border_width;
    }
    return result;
}

/*
 * Returns a "relative" Rect which contains the amount of pixels that need to
 * be added to the original Rect to get the final position (obviously the
 * amount of pixels for normal, 1pixel and borderless are different).
 *
 */
Rect con_border_style_rect(Con const *const con) {
    Rect result = con_border_style_rect_without_title(con);
    if (con_border_style(con) == border_style_t::BS_NORMAL &&
        con_draw_decoration_into_frame(con)) {
        int const deco_height = render_deco_height();
        result.y += deco_height;
        result.height -= deco_height;
    }
    return result;
}

/*
 * Returns adjacent borders of the window. We need this if hide_edge_borders is
 * enabled.
 */
adjacent_t con_adjacent_borders(Con const *const con) {
    adjacent_t result = adjacent_t::ADJ_NONE;
    /* Floating windows are never adjacent to any other window, so
       don’t hide their border(s). This prevents bug #998. */
    if (con->con_is_floating()) {
        return result;
    }

    WorkspaceCon *workspace = con->con_get_workspace();
    if (con->rect.x == workspace->rect.x) {
        result = static_cast<adjacent_t>(result | std::to_underlying(adjacent_t::ADJ_LEFT_SCREEN_EDGE));
    }
    if (con->rect.x + con->rect.width == workspace->rect.x + workspace->rect.width) {
        result = static_cast<adjacent_t>(result | std::to_underlying(adjacent_t::ADJ_RIGHT_SCREEN_EDGE));
    }
    if (con->rect.y == workspace->rect.y) {
        result = static_cast<adjacent_t>(result | std::to_underlying(adjacent_t::ADJ_UPPER_SCREEN_EDGE));
    }
    if (con->rect.y + con->rect.height == workspace->rect.y + workspace->rect.height) {
        result = static_cast<adjacent_t>(result | std::to_underlying(adjacent_t::ADJ_LOWER_SCREEN_EDGE));
    }
    return result;
}

/*
 * Use this function to get a container’s border style. This is important
 * because when inside a stack, the border style is always BS_NORMAL.
 * For tabbed mode, the same applies, with one exception: when the container is
 * borderless and the only element in the tabbed container, the border is not
 * rendered.
 *
 * For children of a CT_DOCKAREA, the border style is always none.
 *
 */
border_style_t con_border_style(Con const *const con) {
    if (con->fullscreen_mode == fullscreen_mode_t::CF_OUTPUT || con->fullscreen_mode == fullscreen_mode_t::CF_GLOBAL) {
        DLOG("this one is fullscreen! overriding BS_NONE\n");
        return border_style_t::BS_NONE;
    }

    if (con->parent != nullptr) {
        if (con->parent->layout == layout_t::L_STACKED) {
            return con->parent->con_num_children() == 1 ? con->border_style : border_style_t::BS_NORMAL;
        }

        if (con->parent->layout == layout_t::L_TABBED && con->border_style != border_style_t::BS_NORMAL) {
            return con->parent->con_num_children() == 1 ? con->border_style : border_style_t::BS_NORMAL;
        }

        if (con->parent->type == con_type_t::CT_DOCKAREA) {
            return border_style_t::BS_NONE;
        }
    }

    return con->border_style;
}

/*
 * Sets the given border style on con, correctly keeping the position/size of a
 * floating window.
 *
 */
void con_set_border_style(Con *con, border_style_t border_style, int border_width) {
    if (std::to_underlying(border_style) > std::to_underlying(con->max_user_border_style)) {
        border_style = con->max_user_border_style;
    }

    /* Handle the simple case: non-floating containerns */
    if (!con->con_is_floating()) {
        con->border_style = border_style;
        con->current_border_width = border_width;
        return;
    }

    /* For floating containers, we want to keep the position/size of the
     * *window* itself. We first add the border pixels to con->rect to make
     * con->rect represent the absolute position of the window (same for
     * parent). Then, we change the border style and subtract the new border
     * pixels. For the parent, we do the same also for the decoration. */
    Con *parent = con->parent;
    Rect bsr = con_border_style_rect(con);

    con->rect += bsr;
    parent->rect += bsr;

    /* Change the border style, get new border/decoration values. */
    con->border_style = border_style;
    con->current_border_width = border_width;
    bsr = con_border_style_rect(con);

    con->rect -= bsr;
    parent->rect -= bsr;
}

/*
 * Returns true if changing the focus to con would be allowed considering
 * the fullscreen focus constraints. Specifically, if a fullscreen container or
 * any of its descendants is focused, this function returns true if and only if
 * focusing con would mean that focus would still be visible on screen, i.e.,
 * the newly focused container would not be obscured by a fullscreen container.
 *
 * In the simplest case, if a fullscreen container or any of its descendants is
 * fullscreen, this functions returns true if con is the fullscreen container
 * itself or any of its descendants, as this means focus wouldn't escape the
 * boundaries of the fullscreen container.
 *
 * In case the fullscreen container is of type CF_OUTPUT, this function returns
 * true if con is on a different workspace, as focus wouldn't be obscured by
 * the fullscreen container that is constrained to a different workspace.
 *
 * Note that this same logic can be applied to moving containers. If a
 * container can be focused under the fullscreen focus constraints, it can also
 * become a parent or sibling to the currently focused container.
 *
 */
bool con_fullscreen_permits_focusing(Con *con) {
    /* No focus, no problem. */
    if (!global.focused) {
        return true;
    }

    /* Find the first fullscreen ascendent. */
    Con *fs = global.focused;
    while (fs && fs->fullscreen_mode == CF_NONE) {
        fs = fs->parent;
    }

    /* fs must be non-NULL since the workspace con doesn’t have CF_NONE and
     * there always has to be a workspace con in the hierarchy. */
    assert(fs != nullptr);
    /* The most common case is we hit the workspace level. In this
     * situation, changing focus is also harmless. */
    assert(fs->fullscreen_mode != CF_NONE);
    if (fs->type == CT_WORKSPACE) {
        return true;
    }

    /* Allow it if the container itself is the fullscreen container. */
    if (con == fs) {
        return true;
    }

    /* If fullscreen is per-output, the focus being in a different workspace is
     * sufficient to guarantee that change won't leave fullscreen in bad shape. */
    if (fs->fullscreen_mode == CF_OUTPUT &&
        con->con_get_workspace() != fs->con_get_workspace()) {
        return true;
    }

    /* Allow it only if the container to be focused is contained within the
     * current fullscreen container. */
    return con->con_has_parent(fs);
}

/*
 * Create a string representing the subtree under con.
 *
 */
std::string con_get_tree_representation(Con *con) {
    /* this code works as follows:
     *  1) create a string with the layout type (D/V/H/T/S) and an opening bracket
     *  2) append the tree representation of the children to the string
     *  3) add closing bracket
     *
     * The recursion ends when we hit a leaf, in which case we return the
     * class_instance of the contained window.
     */

    /* end of recursion */
    if (con->con_is_leaf()) {
        if (!con->get_window()) {
            return "nowin";
        }

        if (con->get_window()->class_instance.empty()) {
            return "noinstance";
        }

        return con->get_window()->class_instance;
    }

    std::string buf{};
    /* 1) add the Layout type to buf */
    if (con->layout == L_DEFAULT) {
        buf.push_back('D');
    } else if (con->layout == L_SPLITV) {
        buf.push_back('V');
    } else if (con->layout == L_SPLITH) {
        buf.push_back('H');
    } else if (con->layout == L_TABBED) {
        buf.push_back('T');
    } else if (con->layout == L_STACKED) {
        buf.push_back('S');
    } else {
        ELOG("BUG: Code not updated to account for new layout type\n");
        std::terminate();
    }
    buf.push_back('[');

    /* 2) append representation of children */
    for (auto &child : con->nodes) {
        buf.append((con::first(con->nodes) == child ? "" : " "));
        buf.append(con_get_tree_representation(child));
    }

    /* 3) close the brackets */
    buf.push_back(']');
    return buf;
}

/*
 * Returns the container's title considering the current title format.
 *
 */
std::string con_parse_title_format(Con *con) {
    assert(!con->title_format.empty());

    i3Window *win = con->get_window();

    std::string title;
    std::string window_class;
    std::string instance;
    std::string machine;
    if (win == nullptr) {
        title = pango_escape_markup(con_get_tree_representation(con));
        window_class = "i3-frame";
        instance = "i3-frame";
        machine = "";
    } else {
        title = pango_escape_markup(win->name);
        window_class = pango_escape_markup((win->class_class.empty()) ? "" : win->class_class.c_str());
        instance = pango_escape_markup((win->class_instance.empty()) ? "" : win->class_instance.c_str());
        machine = pango_escape_markup((win->machine.empty()) ? "" : win->machine.c_str());
    }

    std::vector<placeholder_t> placeholders = {
        {.name = "%title", .value = title},
        {.name = "%class", .value = window_class},
        {.name = "%instance", .value = instance},
        {.name = "%machine", .value = machine},
    };

    std::string formatted_str = format_placeholders(con->title_format, placeholders);

    return formatted_str;
}

/*
 * Returns container's rect size depending on its orientation.
 * i.e. its width when horizontal, its height when vertical.
 *
 */
uint32_t con_rect_size_in_orientation(Con *con) {
    return (con_orientation(con) == HORIZ ? con->rect.width : con->rect.height);
}

/*
 * Returns true if the container is within any stacked/tabbed split container.
 *
 */
bool con_inside_stacked_or_tabbed(Con *con) {
    if (con->parent == nullptr) {
        return false;
    }
    if (con->parent->layout == L_STACKED ||
        con->parent->layout == L_TABBED) {
        return true;
    }
    return con_inside_stacked_or_tabbed(con->parent);
}
