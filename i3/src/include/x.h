/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * x.c: Interface to X11, transfers our in-memory state to X11 (see also
 *      render.c). Basically a big state machine.
 *
 */
#pragma once

#include <config.h>
#include <xcb/shape.h>

/** Stores the X11 window ID of the currently focused window */
extern xcb_window_t focused_id;

/*
 * Describes the X11 state we may modify (map state, position, window stack).
 * There is one entry per container. The state represents the current situation
 * as X11 sees it (with the exception of the order in the state_head CIRCLEQ,
 * which represents the order that will be pushed to X11, while old_state_head
 * represents the current order). It will be updated in x_push_changes().
 *
 */
struct con_state {
    xcb_window_t id{};
    bool mapped{};
    bool unmap_now{};
    bool child_mapped{};
    bool is_hidden{};

    /* The con for which this state is. */
    Con *con{};

    /* For reparenting, we have a flag (need_reparent) and the X ID of the old
     * frame this window was in. The latter is necessary because we need to
     * ignore UnmapNotify events (by changing the window event mask). */
    bool need_reparent{};
    xcb_window_t old_frame{};

    /* The container was child of floating container during the previous call of
     * x_push_node(). This is used to remove the shape when the container is no
     * longer floating. */
    bool was_floating{};

    Rect rect;
    Rect window_rect;

    bool initial{};

    std::string name{};
};

/*
 * Returns the container state for the given frame. This function always
 * returns a container state (otherwise, there is a bug in the code and the
 * container state of a container for which x_con_init() was not called was
 * requested).
 *
 */
con_state *state_for_frame(xcb_window_t window);

/**
 * Initializes the X11 part for the given container. Called exactly once for
 * every container from con_new().
 *
 */
void x_con_init(Con *con);

/**
 * Reparents the child window of the given container (necessary for sticky
 * containers). The reparenting happens in the next call of x_push_changes().
 *
 */
void x_reparent_child(Con *con, Con *old);

/**
 * Kills the window decoration associated with the given container.
 *
 */
void x_con_kill(Con *con);

/*
 * Completely reinitializes the container's frame, without destroying the old window.
 *
 */
void x_con_reframe(Con *con);

/**
 * Returns true if the client supports the given protocol atom (like WM_DELETE_WINDOW)
 *
 */
bool window_supports_protocol(xcb_window_t window, xcb_atom_t atom);

/**
 * Kills the given X11 window using WM_DELETE_WINDOW (if supported).
 *
 */
void x_window_kill(xcb_window_t window, kill_window_t kill_window);

/**
 * Draws the decoration of the given container onto its parent.
 *
 */
void x_draw_decoration(Con *con);

/**
 * Recursively calls x_draw_decoration. This cannot be done in x_push_node
 * because x_push_node uses focus order to recurse (see the comment above)
 * while drawing the decoration needs to happen in the actual order.
 *
 */
void x_deco_recurse(Con *con);

/**
 * This function pushes the properties of each node of the layout tree to
 * X11 if they have changed (like the map state, position of the window, …).
 * It recursively traverses all children of the given node.
 *
 */
void x_push_node(Con *con);

/**
 * Pushes all changes (state of each node, see x_push_node() and the window
 * stack) to X11.
 *
 */
void x_push_changes(Con *con);

/**
 * Raises the specified container in the internal stack of X windows. The
 * next call to x_push_changes() will make the change visible in X11.
 *
 */
void x_raise_con(Con *con);

/**
 * Sets the WM_NAME property (so, no UTF8, but used only for debugging anyways)
 * of the given name. Used for properly tagging the windows for easily spotting
 * i3 windows in xwininfo -root -all.
 *
 */
void x_set_name(Con *con, const std::string &name);

/**
 * Sets up i3 specific atoms (I3_SOCKET_PATH and I3_CONFIG_PATH)
 *
 */
void x_set_i3_atoms();

/**
 * Set warp_to coordinates.  This will trigger on the next call to
 * x_push_changes().
 *
 */
void x_set_warp_to(Rect *rect);

/**
 * Applies the given mask to the event mask of every i3 window decoration X11
 * window. This is useful to disable EnterNotify while resizing so that focus
 * is untouched.
 *
 */
void x_mask_event_mask(uint32_t mask);

/**
 * Enables or disables nonrectangular shape of the container frame.
 */
void x_set_shape(Con *con, xcb_shape_sk_t kind, bool enable);
