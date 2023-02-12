/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * scratchpad.c: Moving windows to the scratchpad and making them visible again.
 *
 */


#include "libi3.h"

#include "util.h"
#include "tree.h"
#include "workspace.h"
#include "floating.h"
#include "configuration.h"
#include "con.h"
#include "scratchpad.h"

/*
 * Moves the specified window to the __i3_scratch workspace, making it floating
 * and setting the appropriate scratchpad_state.
 *
 * Gets called upon the command 'move scratchpad'.
 *
 */
void scratchpad_move(Con *con) {
    if (con->type == CT_WORKSPACE) {
         LOG(fmt::sprintf("'move scratchpad' used on a workspace \"%s\". Calling it "
            "recursively on all windows on this workspace.\n",
            con->name));
        for (auto &current : con->focus_head) {
            scratchpad_move(current);
        }
        return;
    }
    DLOG(fmt::sprintf("should move con %p to __i3_scratch\n",  (void*)con));

    Con *__i3_scratch = workspace_get("__i3_scratch");
    if (con->con_get_workspace() == __i3_scratch) {
        DLOG("This window is already on __i3_scratch.\n");
        return;
    }

    /* If the current con is in fullscreen mode, we need to disable that,
     *  as a scratchpad window should never be in fullscreen mode */
    con_disable_fullscreen(con);

    /* 1: Ensure the window or any parent is floating. From now on, we deal
     * with the CT_FLOATING_CON. We use automatic == false because the user
     * made the choice that this window should be a scratchpad (and floating).
     */
    Con *maybe_floating_con = con->con_inside_floating();
    if (maybe_floating_con == nullptr) {
        floating_enable(con, false);
        con = con->parent;
    } else {
        con = maybe_floating_con;
    }

    /* 2: Send the window to the __i3_scratch workspace, mainting its
     * coordinates and not warping the pointer. */
    con_move_to_workspace(con, __i3_scratch, true, true, false);

    /* 3: If this is the first time this window is used as a scratchpad, we set
     * the scratchpad_state to SCRATCHPAD_FRESH. The window will then be
     * adjusted in size according to what the user specifies. */
    if (con->scratchpad_state == SCRATCHPAD_NONE) {
        DLOG("This window was never used as a scratchpad before.\n");
        if (con == maybe_floating_con) {
            DLOG("It was in floating mode before, set scratchpad state to changed.\n");
            con->scratchpad_state = SCRATCHPAD_CHANGED;
        } else {
            DLOG("It was in tiling mode before, set scratchpad state to fresh.\n");
            con->scratchpad_state = SCRATCHPAD_FRESH;
        }
    }
}

/*
 * Either shows the top-most scratchpad window (con == NULL) or shows the
 * specified con (if it is scratchpad window).
 *
 * When called with con == NULL and the currently focused window is a
 * scratchpad window, this serves as a shortcut to hide it again (so the user
 * can press the same key to quickly look something up).
 *
 */
bool scratchpad_show(Con *con) {
    DLOG(fmt::sprintf("should show scratchpad window %p\n",  (void*)con));
    Con *__i3_scratch = workspace_get("__i3_scratch");
    Con *floating;

    /* If this was 'scratchpad show' without criteria, we check if the
     * currently focused window is a scratchpad window and should be hidden
     * again. */
    if (!con &&
        (floating = focused->con_inside_floating()) &&
        floating->scratchpad_state != SCRATCHPAD_NONE) {
        DLOG("Focused window is a scratchpad window, hiding it.\n");
        scratchpad_move(focused);
        return true;
    }

    /* If the current con or any of its parents are in fullscreen mode, we
     * first need to disable it before showing the scratchpad con. */
    Con *fs = focused;
    while (fs && fs->fullscreen_mode == CF_NONE)
        fs = fs->parent;

    if (fs && fs->type != CT_WORKSPACE) {
        con_toggle_fullscreen(fs, CF_OUTPUT);
    }

    /* If this was 'scratchpad show' without criteria, we check if there is a
     * unfocused scratchpad on the current workspace and focus it */
    Con *focused_ws = focused->con_get_workspace();
    for (auto &walk_con : focused_ws->floating_windows) {
        if (!con && (floating = walk_con->con_inside_floating()) &&
            floating->scratchpad_state != SCRATCHPAD_NONE &&
            floating != focused->con_inside_floating()) {
            DLOG("Found an unfocused scratchpad window on this workspace\n");
            DLOG(fmt::sprintf("Focusing it: %p\n",  (void*)walk_con));
            /* use con_descend_tiling_focused to get the last focused
             * window inside this scratch container in order to
             * keep the focus the same within this container */
            con_descend_tiling_focused(walk_con)->con_activate();
            return true;
        }
    }

    /* If this was 'scratchpad show' without criteria, we check if there is a
     * visible scratchpad window on another workspace. In this case we move it
     * to the current workspace. */
    focused_ws = focused->con_get_workspace();
    for (const auto &walk_con : all_cons) {
        Con *walk_ws = walk_con->con_get_workspace();
        if (!con && walk_ws &&
            !walk_ws->con_is_internal() && focused_ws != walk_ws &&
            (floating = walk_con->con_inside_floating()) &&
            floating->scratchpad_state != SCRATCHPAD_NONE) {
            DLOG("Found a visible scratchpad window on another workspace,\n");
            DLOG(fmt::sprintf("moving it to this workspace: con = %p\n",  (void*)walk_con));
            con_move_to_workspace(floating, focused_ws, true, false, false);
            con_descend_focused(walk_con)->con_activate();
            return true;
        }
    }

    /* If this was 'scratchpad show' with criteria, we check if the window
     * is actually in the scratchpad */
    if (con && con->parent->scratchpad_state == SCRATCHPAD_NONE) {
        DLOG("Window is not in the scratchpad, doing nothing.\n");
        return false;
    }

    /* If this was 'scratchpad show' with criteria, we check if it matches a
     * currently visible scratchpad window and hide it. */
    Con *active = focused->con_get_workspace();
    Con *current = con->con_get_workspace();
    if (con &&
        (floating = con->con_inside_floating()) &&
        floating->scratchpad_state != SCRATCHPAD_NONE &&
        current != __i3_scratch) {
        /* If scratchpad window is on the active workspace, then we should hide
         * it, otherwise we should move it to the active workspace. */
        if (current == active) {
            DLOG("Window is a scratchpad window, hiding it.\n");
            scratchpad_move(con);
            return true;
        }
    }

    if (con == nullptr) {
        /* Use the container on __i3_scratch which is highest in the focus
         * stack. When moving windows to __i3_scratch, they get inserted at the
         * bottom of the stack. */
        con = __i3_scratch->floating_windows.front();

        if (!con) {
            LOG("You don't have any scratchpad windows yet.\n");
            LOG("Use 'move scratchpad' to move a window to the scratchpad.\n");
            return false;
        }
    } else {
        /* We used a criterion, so we need to do what follows (moving,
         * resizing) on the floating parent. */
        con = con->con_inside_floating();
    }

    /* 1: Move the window from __i3_scratch to the current workspace. */
    con_move_to_workspace(con, active, true, false, false);

    /* 2: Adjust the size if this window was not adjusted yet. */
    if (con->scratchpad_state == SCRATCHPAD_FRESH) {
        DLOG("Adjusting size of this window.\n");
        Con *output = con->con_get_output();
        con->rect.width = output->rect.width * 0.5;
        con->rect.height = output->rect.height * 0.75;
        floating_check_size(con, false);
        floating_center(con, con->con_get_workspace()->rect);
    }

    /* Activate active workspace if window is from another workspace to ensure
     * proper focus. */
    if (current != active) {
        workspace_show(active);
    }

    con_descend_focused(con)->con_activate();

    return true;
}

/*
 * Greatest common divisor, implemented only for the least common multiple
 * below.
 *
 */
static int _gcd(const int m, const int n) {
    if (n == 0)
        return m;
    return _gcd(n, (m % n));
}

/*
 * Least common multiple. We use it to determine the (ideally not too large)
 * resolution for the __i3 pseudo-output on which the scratchpad is on (see
 * below). We could just multiply the resolutions, but for some pathetic cases
 * (many outputs), using the LCM will achieve better results.
 *
 * Man, when you were learning about these two algorithms for the first time,
 * did you think you’d ever need them in a real-world software project of
 * yours? I certainly didn’t until now. :-D
 *
 */
static int _lcm(const int m, const int n) {
    const int o = _gcd(m, n);
    return ((m * n) / o);
}

/*
 * When starting i3 initially (and after each change to the connected outputs),
 * this function fixes the resolution of the __i3 pseudo-output. When that
 * resolution is not set to a function which shares a common divisor with every
 * active output’s resolution, floating point calculation errors will lead to
 * the scratchpad window moving when shown repeatedly.
 *
 */
void scratchpad_fix_resolution() {
    Con *__i3_scratch = workspace_get("__i3_scratch");
    Con *__i3_output = __i3_scratch->con_get_output();
    DLOG(fmt::sprintf("Current resolution: (%d, %d) %d x %d\n",
         __i3_output->rect.x, __i3_output->rect.y,
         __i3_output->rect.width, __i3_output->rect.height));
    int new_width = -1,
        new_height = -1;
    for (auto &output : croot->nodes_head) {
        if (output == __i3_output)
            continue;
        DLOG(fmt::sprintf("output %s's resolution: (%d, %d) %d x %d\n",
             output->name, output->rect.x, output->rect.y,
             output->rect.width, output->rect.height));
        if (new_width == -1) {
            new_width = output->rect.width;
            new_height = output->rect.height;
        } else {
            new_width = _lcm(new_width, output->rect.width);
            new_height = _lcm(new_height, output->rect.height);
        }
    }

    Rect old_rect = __i3_output->rect;

    DLOG(fmt::sprintf("new width = %d, new height = %d\n",
         new_width, new_height));
    __i3_output->rect.width = new_width;
    __i3_output->rect.height = new_height;

    Rect new_rect = __i3_output->rect;

    if (new_rect == old_rect) {
        DLOG("Scratchpad size unchanged.\n");
        return;
    }

    DLOG("Fixing coordinates of scratchpad windows\n");
    for (auto &con : __i3_scratch->floating_windows) {
        floating_fix_coordinates(con, &old_rect, &new_rect);
    }
}
