/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * tree.c: Everything that primarily modifies the layout tree data structure.
 *
 */
module;
#include <cassert>
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#include <ev.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <fmt/printf.h>
module i3;

import std;
import :output;
import log;
import utils;
import rect;

/*
 * Loads tree from 'path' (used for in-place restarts).
 *
 */
bool tree_restore(const std::string_view path, const xcb_get_geometry_reply_t *geometry) {
    bool result = false;
    auto globbed = utils::resolve_tilde(path);
    std::string buf;

    if (!std::filesystem::exists(globbed)) {
        LOG(fmt::sprintf("%s does not exist, not restoring tree\n",  globbed));
        return result;
    }

    try {
        auto slurped = utils::slurp(globbed);
        buf = slurped;
    } catch (std::exception &e) {
        return result;
    }

    /* TODO: refactor the following */
    global.croot = new RootCon();
    global.croot->rect = {
        static_cast<uint32_t>(geometry->x),
        static_cast<uint32_t>(geometry->y),
        geometry->width,
        geometry->height
    };
    global.focused = global.croot;

    tree_append_json(global.focused, buf, nullptr);

    DLOG("appended tree, using new root\n");
    global.croot = dynamic_cast<RootCon*>(con::first(global.croot->nodes));
    if (!global.croot) {
        /* tree_append_json failed. Continuing here would segfault. */
        return result;
    }
    DLOG(fmt::sprintf("new root = %p\n", fmt::ptr(global.croot)));
    Con *out = con::first(global.croot->nodes);
    DLOG(fmt::sprintf("out = %p\n", fmt::ptr(out)));
    Con *ws = con::first(out->nodes);
    DLOG(fmt::sprintf("ws = %p\n", fmt::ptr(ws)));

    restore_open_placeholder_windows(global.croot);
    result = true;

    return result;
}

/*
 * Initializes the tree by creating the root node. The CT_OUTPUT Cons below the
 * root node are created in randr.c for each Output.
 *
 */
void tree_init(const xcb_get_geometry_reply_t *geometry) {
    global.croot = new RootCon();
    global.croot->layout = L_SPLITH;
    global.croot->rect = {
        static_cast<uint32_t>(geometry->x),
        static_cast<uint32_t>(geometry->y),
        geometry->width,
        geometry->height
    };
}

/*
 * Opens an empty container in the current container
 *
 */
Con *tree_open_con(Con *con, i3Window *window) {
    if (con == nullptr) {
        /* every focusable Con has a parent (outputs have parent root) */
        con = global.focused->parent;
        /* If the parent is an output, we are on a workspace. In this case,
         * the new container needs to be opened as a leaf of the workspace. */
        if (con->parent->type == CT_OUTPUT && con->type != CT_DOCKAREA) {
            con = global.focused;
        }

        /* If the currently focused container is a floating container, we
         * attach the new container to the currently focused spot in its
         * workspace. */
        if (con->type == CT_FLOATING_CON) {
            con = con_descend_tiling_focused(con->parent);
            if (con->type != CT_WORKSPACE)
                con = con->parent;
        }
        DLOG(fmt::sprintf("con = %p\n", fmt::ptr(con)));
    }

    assert(con != nullptr);

    /* 3. create the container and attach it to its parent */
    Con *new_con = new ConCon(window);
    new_con->con_attach(con, false);

    new_con->layout = L_SPLITH;

    /* 4: re-calculate child->percent for each child */
    con->con_fix_percent();

    return new_con;
}

/*
 * Closes the given container including all children.
 * Returns true if the container was killed or false if just WM_DELETE was sent
 * and the window is expected to kill itself.
 *
 * The dont_kill_parent flag is specified when the function calls itself
 * recursively while deleting a containers children.
 *
 */
bool tree_close_internal(Con *con, kill_window_t kill_window, bool dont_kill_parent) {
    Con *parent = con->parent;

    /* remove the urgency hint of the workspace (if set) */
    if (con->urgent) {
        con->con_set_urgency(false);
        workspace_update_urgent_flag(con->con_get_workspace());
    }

    DLOG(fmt::sprintf("closing %p, kill_window = %d\n", fmt::ptr(con), std::to_underlying(kill_window)));
    bool abort_kill = false;
    /* We cannot use TAILQ_FOREACH because the children get deleted
     * in their parent’s nodes */
    for (auto &child : con->nodes) {
        DLOG(fmt::sprintf("killing child=%p\n", fmt::ptr(child)));
        if (!tree_close_internal(child, kill_window, true)) {
            abort_kill = true;
        }
    }

    if (abort_kill) {
        DLOG("One of the children could not be killed immediately (WM_DELETE sent), aborting.\n");
        return false;
    }

    if (con->window != nullptr) {
        if (kill_window != kill_window_t::DONT_KILL_WINDOW) {
            x_window_kill(**global.x, con->window->id, kill_window);
            return false;
        } else {
            xcb_void_cookie_t cookie;
            /* Ignore any further events by clearing the event mask,
             * unmap the window,
             * then reparent it to the root window. */
            uint32_t value_list[]{XCB_NONE};
            xcb_change_window_attributes(**global.x, con->window->id,
                                         XCB_CW_EVENT_MASK, value_list);
            xcb_unmap_window(**global.x, con->window->id);
            cookie = xcb_reparent_window(**global.x, con->window->id, global.x->root, con->rect.x, con->rect.y);

            /* Ignore X11 errors for the ReparentWindow request.
             * X11 Errors are returned when the window was already destroyed */
            global.handlers->add_ignore_event(cookie.sequence, 0);

            /* We are no longer handling this window, thus set WM_STATE to
             * WM_STATE_WITHDRAWN (see ICCCM 4.1.3.1) */
            long data[] = {XCB_ICCCM_WM_STATE_WITHDRAWN, XCB_NONE};
            cookie = xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE,
                                         con->window->id, i3::atoms[i3::Atom::WM_STATE], i3::atoms[i3::Atom::WM_STATE], 32, 2, data);

            /* Remove the window from the save set. All windows in the save set
             * will be mapped when i3 closes its connection (e.g. when
             * restarting). This is not what we want, since some apps keep
             * unmapped windows around and don’t expect them to suddenly be
             * mapped. See https://bugs.i3wm.org/1617 */
            xcb_change_save_set(**global.x, XCB_SET_MODE_DELETE, con->window->id);

            /* Stop receiving ShapeNotify events. */
            if (global.shape->shape_supported) {
                xcb_shape_select_input(**global.x, con->window->id, false);
            }

            /* Ignore X11 errors for the ReparentWindow request.
             * X11 Errors are returned when the window was already destroyed */
            global.handlers->add_ignore_event(cookie.sequence, 0);
        }
        ipc_send_window_event("close", con);
        delete con->window;
        con->window = nullptr;
    }

    Con *ws = con->con_get_workspace();

    /* Figure out which container to focus next before detaching 'con'. */
    Con *next = (con == global.focused) ? con_next_focused(con) : nullptr;
    DLOG(fmt::sprintf("next = %p, focused = %p\n", fmt::ptr(next), fmt::ptr(global.focused)));

    /* Detach the container so that it will not be rendered anymore. */
    con->con_detach();

    /* disable urgency timer, if needed */
    if (con->urgency_timer != nullptr) {
        DLOG(fmt::sprintf("Removing urgency timer of con %p\n", fmt::ptr(con)));
        workspace_update_urgent_flag(ws);
        ev_timer_stop(global.eventHandler->main_loop, con->urgency_timer);
        delete con->urgency_timer;
        con->urgency_timer = nullptr;
    }

    if (con->type != CT_FLOATING_CON) {
        /* If the container is *not* floating, we might need to re-distribute
         * percentage values for the resized containers. */
        parent->con_fix_percent();
    }

    /* Render the tree so that the surrounding containers take up the space
     * which 'con' does no longer occupy. If we don’t render here, there will
     * be a gap in our containers and that could trigger an EnterNotify for an
     * underlying container, see ticket #660.
     *
     * Rendering has to be avoided when dont_kill_parent is set (when
     * tree_close_internal calls itself recursively) because the tree is in a
     * non-renderable state during that time. */
    if (!dont_kill_parent)
        tree_render();

    /* kill the X11 part of this container */
    x_con_kill(con);

    if (ws == con) {
        DLOG(fmt::sprintf("Closing workspace container %s, updating EWMH atoms\n",  ws->name));
        ewmh_update_desktop_properties();
    }

    delete con;

    if (next) {
        next->con_activate();
    } else {
        DLOG("not changing focus, the container was not focused before\n");
    }

    /* check if the parent container is empty now and close it */
    if (!dont_kill_parent) {
        parent->on_remove_child();
    }

    return true;
}

/*
 * Splits (horizontally or vertically) the given container by creating a new
 * container which contains the old one and the future ones.
 *
 */
void tree_split(Con *con, orientation_t orientation) {
    if (con->con_is_floating()) {
        DLOG("Floating containers can't be split.\n");
        return;
    }

    if (con->type == CT_WORKSPACE) {
        if (con->con_num_children() < 2) {
            if (con->con_num_children() == 0) {
                DLOG("Changing workspace_layout to L_DEFAULT\n");
                dynamic_cast<WorkspaceCon*>(con)->workspace_layout = L_DEFAULT;
            }
            DLOG("Changing orientation of workspace\n");
            con->layout = (orientation == HORIZ) ? L_SPLITH : L_SPLITV;
            return;
        } else {
            /* if there is more than one container on the workspace
             * move them into a new container and handle this instead */
            con = workspace_encapsulate(con);
        }
    }

    Con *parent = con->parent;

    /* Force re-rendering to make the indicator border visible. */
    con_force_split_parents_redraw(con);

    /* if we are in a container whose parent contains only one
     * child (its split functionality is unused so far), we just change the
     * orientation (more intuitive than splitting again) */
    if (parent->con_num_children() == 1 &&
        (parent->layout == L_SPLITH ||
         parent->layout == L_SPLITV)) {
        parent->layout = (orientation == HORIZ) ? L_SPLITH : L_SPLITV;
        DLOG("Just changing orientation of existing container\n");
        return;
    }

    DLOG(fmt::sprintf("Splitting in orientation %d\n", std::to_underlying(orientation)));

    /* 2: replace it with a new Con */
    Con *new_con = new ConCon();
    auto nodes_it = std::ranges::find(parent->nodes, con);
    *nodes_it = new_con;
    auto con_it = std::ranges::find(parent->focused, con);
    *con_it = new_con;
    new_con->parent = parent;
    new_con->layout = (orientation == HORIZ) ? L_SPLITH : L_SPLITV;

    /* 3: swap 'percent' (resize factor) */
    new_con->percent = con->percent;
    con->percent = 0.0;

    /* 4: add it as a child to the new Con */
    con->con_attach(new_con, false);
}

/*
 * Moves focus one level up. Returns true if focus changed.
 *
 */
bool level_up() {
    /* Skip over floating containers and go directly to the grandparent
     * (which should always be a workspace) */
    if (global.focused->parent->type == CT_FLOATING_CON) {
        global.focused->parent->parent->con_activate();
        return true;
    }

    /* We can focus up to the workspace, but not any higher in the tree */
    if ((global.focused->parent->type != CT_CON &&
         global.focused->parent->type != CT_WORKSPACE) ||
        global.focused->type == CT_WORKSPACE) {
        ELOG("'focus parent': Focus is already on the workspace, cannot go higher than that.\n");
        return false;
    }
    global.focused->parent->con_activate();
    return true;
}

/*
 * Moves focus one level down. Returns true if focus changed.
 *
 */
bool level_down() {
    /* Go down the focus stack of the current node */
    Con *next = con::first(global.focused->focused);
    if (next == con::last(global.focused->focused)) {
        DLOG("cannot go down\n");
        return false;
    } else if (next->type == CT_FLOATING_CON) {
        /* Floating cons shouldn't be directly focused; try immediately
         * going to the grandchild of the focused con. */
        Con *child = con::first(next->focused);
        if (child == con::last(next->focused)) {
            DLOG("cannot go down\n");
            return false;
        } else
            next = con::first(next->focused);
    }

    next->con_activate();
    return true;
}

static void mark_unmapped(Con *con) {

    con->mapped = false;
    for (auto &current : con->nodes) {
        mark_unmapped(current);
    }
    if (con->type == CT_WORKSPACE) {
        /* We need to call mark_unmapped on floating nodes as well since we can
         * make containers floating. */
        for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
            mark_unmapped(current);
        }
    }
}

/*
 * Renders the tree, that is rendering all outputs using render_con() and
 * pushing the changes to X11 using x_push_changes().
 *
 */
void tree_render() {
    if (global.croot == nullptr)
        return;

    DLOG("-- BEGIN RENDERING --\n");
    /* Reset map state for all nodes in tree */
    /* TODO: a nicer method to walk all nodes would be good, maybe? */
    mark_unmapped(global.croot);
    global.croot->mapped = true;

    render_con(global.croot);

    x_push_changes(global.croot);
    DLOG("-- END RENDERING --\n");
}

static Con *get_tree_next_workspace(Con *con, direction_t direction) {
    if (con->con_get_fullscreen_con(CF_GLOBAL)) {
        DLOG("Cannot change workspace while in global fullscreen mode.\n");
        return nullptr;
    }

    // Use the center of the container instead of the left/top edges, to make
    // this work with negative gaps. See https://github.com/i3/i3/issues/5293
    const uint32_t x = con->rect.x + (con->rect.width / 2);
    const uint32_t y = con->rect.y + (con->rect.height / 2);
    Output *current_output = global.randr->get_output_containing(x, y);
    if (!current_output) {
        return nullptr;
    }
    DLOG(fmt::sprintf("Current output is %s\n",  current_output->output_primary_name()));

    Output *next_output = global.randr->get_output_next(direction, current_output, CLOSEST_OUTPUT);
    if (!next_output) {
        return nullptr;
    }
    DLOG(fmt::sprintf("Next output is %s\n",  next_output->output_primary_name()));

    /* Find visible workspace on next output */
    auto workspace = std::ranges::find_if(next_output->con->output_get_content()->nodes, [](auto &child) { return workspace_is_visible(child); });
    return workspace == next_output->con->output_get_content()->nodes.end() ? nullptr : *workspace;
}

/*
 * Returns the next / previous container to focus in the given direction. Does
 * not modify focus and ensures focus restrictions for fullscreen containers
 * are respected.
 *
 */
static Con *get_tree_next(Con *con, direction_t direction) {
    const bool previous = position_from_direction(direction) == BEFORE;
    const orientation_t orientation = orientation_from_direction(direction);

    Con *first_wrap = nullptr;

    if (con->type == CT_WORKSPACE) {
        /* Special case for FOCUS_WRAPPING_WORKSPACE: allow the focus to leave
         * the workspace only when a workspace is selected. */
        goto handle_workspace;
    }

    while (con->type != CT_WORKSPACE) {
        if (con->fullscreen_mode == CF_OUTPUT) {
            /* We've reached a fullscreen container. Directional focus should
             * now operate on the workspace level. */
            con = con->con_get_workspace();
            break;
        } else if (con->fullscreen_mode == CF_GLOBAL) {
            /* Focus changes should happen only inside the children of a global
             * fullscreen container. */
            return first_wrap;
        }

        Con *const parent = con->parent;
        if (con->type == CT_FLOATING_CON) {
            auto floating_parent = dynamic_cast<WorkspaceCon*>(parent);
            std::deque<Con*> floating_windows{};
            if (floating_parent) {
                floating_windows = floating_parent->floating_windows;
            }
            if (orientation != HORIZ) {
                /* up/down does not change floating containers */
                return nullptr;
            }

            /* left/right focuses the previous/next floating container */
            auto con_it = std::ranges::find(floating_windows, con);

            /* Our parent does not list us in floating heads? */
            assert(con_it != floating_windows.end());

            Con *next;

            if (con_it == floating_windows.begin()) {
                if (previous) {
                    /* If there is no next/previous container, wrap */
                    next = floating_windows.back();
                } else {
                    next = (std::next(con_it) == floating_windows.end()) ? nullptr : *std::next(con_it);
                }
            } else if ((con_it+1) == floating_windows.end()) {
                if (previous) {
                    next = (con_it == floating_windows.begin()) ? nullptr : *std::prev(con_it);
                } else {
                    /* If there is no next/previous container, wrap */
                    next = floating_windows.front();
                }
            } else {
                next = previous ? *std::prev(con_it) : *std::next(con_it);
            }

            return next;
        }

        if (parent->con_num_children() > 1 && con_orientation(parent) == orientation) {
            Con *const next = previous ? con::previous(con, con->parent->nodes) : con::next(con, con->parent->nodes);
            if (next && con_fullscreen_permits_focusing(next)) {
                return next;
            }

            Con *const wrap = previous ? con::last(parent->nodes)
                                       : con::first(parent->nodes);
            switch (global.configManager->config->focus_wrapping) {
                case FOCUS_WRAPPING_OFF:
                    break;
                case FOCUS_WRAPPING_WORKSPACE:
                case FOCUS_WRAPPING_ON:
                    if (!first_wrap && con_fullscreen_permits_focusing(wrap)) {
                        first_wrap = wrap;
                    }
                    break;
                case FOCUS_WRAPPING_FORCE:
                    /* 'force' should always return to ensure focus doesn't
                     * leave the parent. */
                    if (next) {
                        return nullptr; /* blocked by fullscreen */
                    }
                    return con_fullscreen_permits_focusing(wrap) ? wrap : nullptr;
            }
        }

        con = parent;
    }

    assert(con->type == CT_WORKSPACE);
    if (global.configManager->config->focus_wrapping == FOCUS_WRAPPING_WORKSPACE) {
        return first_wrap;
    }

handle_workspace:;
    Con *workspace = get_tree_next_workspace(con, direction);
    return workspace ? workspace : first_wrap;
}

/*
 * Changes focus in the given direction
 *
 */
void tree_next(Con *con, direction_t direction) {
    Con *next = get_tree_next(con, direction);
    if (!next) {
        return;
    }
    if (next->type == CT_WORKSPACE) {
        /* Show next workspace and focus appropriate container if possible. */
        /* Use descend_focused first to give higher priority to floating or
         * tiling fullscreen containers. */
        Con *focus = con_descend_focused(next);
        if (focus->fullscreen_mode == CF_NONE) {
            Con *focus_tiling = con_descend_tiling_focused(next);
            /* If descend_tiling returned a workspace then focus is either a
             * floating container or the same workspace. */
            if (focus_tiling != next) {
                focus = focus_tiling;
            }
        }

        workspace_show(next);
        focus->con_activate();
        x_set_warp_to(focus->rect);
        return;
    } else if (next->type == CT_FLOATING_CON) {
        /* Raise the floating window on top of other windows preserving relative
         * stack order */
        WorkspaceCon *parent = dynamic_cast<WorkspaceCon*>(next->parent);
        while (parent->floating_windows.back() != next) {
            Con *last = parent->floating_windows.back();
            parent->floating_windows.pop_back();
            parent->floating_windows.push_front(last);
        }
    }

    workspace_show(next->con_get_workspace());
    con_descend_focused(next)->con_activate();
}

/*
 * Get the previous / next sibling
 *
 */
Con *get_tree_next_sibling(Con *con, position_t direction) {
    Con *to_focus = (direction == BEFORE) ? con::previous(con, con->parent->nodes) : con::next(con, con->parent->nodes);
    if (to_focus && con_fullscreen_permits_focusing(to_focus)) {
        return to_focus;
    }
    return nullptr;
}

/*
 * tree_flatten() removes pairs of redundant split containers, e.g.:
 *       [workspace, horizontal]
 *   [v-split]           [child3]
 *   [h-split]
 * [child1] [child2]
 * In this example, the v-split and h-split container are redundant.
 * Such a situation can be created by moving containers in a direction which is
 * not the orientation of their parent container. i3 needs to create a new
 * split container then and if you move containers this way multiple times,
 * redundant chains of split-containers can be the result.
 *
 */
void tree_flatten(Con *con) {
    Con *child, *parent = con->parent;
    DLOG(fmt::sprintf("Checking if I can flatten con = %p / %s\n", fmt::ptr(con), con->name));

    /* We only consider normal containers without windows */
    if (con->type != CT_CON ||
        parent->layout == L_OUTPUT || /* con == "content" */
        con->window != nullptr) {
        /* We cannot use normal foreach here because tree_flatten might close the current container. */
        for (auto &current : con->nodes) {
            tree_flatten(current);
        }

        if (dynamic_cast<WorkspaceCon*>(con)) {
            for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
                tree_flatten(current);
            }
        }
        return;
    }

    /* Ensure it got only one child */
    if (con->nodes.size() != 1) {
        /* We cannot use normal foreach here because tree_flatten might close the current container. */
        for (auto &current : con->nodes) {
            tree_flatten(current);
        }

        if (dynamic_cast<WorkspaceCon*>(con)) {
            for (auto &c : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
                tree_flatten(c);
            }
        }
        return;
    }

    child = con->nodes.front();

    DLOG(fmt::sprintf("child = %p, con = %p, parent = %p\n", fmt::ptr(child), fmt::ptr(con), fmt::ptr(parent)));

    /* The child must have a different orientation than the con but the same as
     * the con’s parent to be redundant */
    if (!con->con_is_split() ||
        !child->con_is_split() ||
        (con->layout != L_SPLITH && con->layout != L_SPLITV) ||
        (child->layout != L_SPLITH && child->layout != L_SPLITV) ||
        con_orientation(con) == con_orientation(child) ||
        con_orientation(child) != con_orientation(parent)) {
        /* We cannot use normal foreach here because tree_flatten might close the current container. */
        for (auto &current : con->nodes) {
            tree_flatten(current);
        }

        if (dynamic_cast<WorkspaceCon*>(con)) {
            for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
                tree_flatten(current);
            }
        }
        return;
    }

    DLOG("Alright, I have to flatten this situation now. Stay calm.\n");
    /* 1: save focus */
    Con *focus_next = con::first(child->focused);

    DLOG("detaching...\n");
    /* 2: re-attach the children to the parent before con */
    while (!child->nodes.empty()) {
        auto current = con::first(child->nodes);
        DLOG(fmt::sprintf("detaching current=%p / %s\n", fmt::ptr(current), current->name));
        current->con_detach();
        DLOG("re-attaching\n");
        /* We don’t use con_attach() here because for a CT_CON, the special
         * case handling of con_attach() does not trigger. So all it would do
         * is calling TAILQ_INSERT_AFTER, but with the wrong container. So we
         * directly use the TAILQ macros. */
        current->parent = parent;
        con->insert_before(current);
        DLOG("attaching to focus list\n");
        parent->focused.push_back(current);
        current->percent = con->percent;
    }
    DLOG("re-attached all\n");

    /* 3: restore focus, if con was focused */
    if (focus_next != nullptr &&
        con::first(parent->focused) == con) {
        DLOG(fmt::sprintf("restoring focus to focus_next=%p\n", fmt::ptr(focus_next)));
        std::erase(parent->focused, focus_next);
        parent->focused.push_front(focus_next);
        DLOG("restored focus.\n");
    }

    /* 4: close the redundant cons */
    DLOG("closing redundant cons\n");
    tree_close_internal(con, kill_window_t::DONT_KILL_WINDOW, true);
}
