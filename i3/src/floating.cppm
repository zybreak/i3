/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * floating.c: Floating windows.
 *
 */
module;
#include <config.h>
#include <xcb/xcb.h>
export module i3:floating;

import rect;

class Con;
class ConCon;
class FloatingCon;

export {
    /** On which border was the dragging initiated? */
    enum border_t {
        BORDER_LEFT = (1 << 0),
        BORDER_RIGHT = (1 << 1),
        BORDER_TOP = (1 << 2),
        BORDER_BOTTOM = (1 << 3)
    };

    /**
     * Enables floating mode for the given container by detaching it from its
     * parent, creating a new container around it and storing this container in the
     * floating_windows list of the workspace.
     *
     */
    bool floating_enable(ConCon * con, bool automatic);

    /**
     * Disables floating mode for the given container by re-attaching the container
     * to its old parent.
     *
     */
    void floating_disable(ConCon * con);

    /**
     * Calls floating_enable() for tiling containers and floating_disable() for
     * floating containers.
     *
     * If the automatic flag is set to true, this was an automatic update by a
     * change of the window class from the application which can be overwritten by
     * the user.
     *
     */
    void toggle_floating_mode(ConCon * con, bool automatic);

    /**
     * Raises the given container in the list of floating containers
     *
     */
    void floating_raise_con(FloatingCon * con);

    /**
     * Checks if con’s coordinates are within its workspace and re-assigns it to
     * the actual workspace if not.
     *
     */
    bool floating_maybe_reassign_ws(Con * con);

    /**
     * Centers a floating con above the specified rect.
     *
     */
    void floating_center(Con * con, Rect rect);

    /**
     * Moves the given floating con to the current pointer position.
     *
     */
    void floating_move_to_pointer(Con * con);

    /**
     * Called when a floating window is created or resized.  This function resizes
     * the window if its size is higher or lower than the configured maximum/minimum
     * size, respectively or when adjustments are needed to conform to the
     * configured size increments or aspect ratio limits.
     *
     * When prefer_height is true and the window needs to be resized because of the
     * configured aspect ratio, the width is adjusted first, preserving the previous
     * height.
     *
     */
    void floating_check_size(Con * floating_con, bool prefer_height);

    /**
     * Repositions the CT_FLOATING_CON to have the coordinates specified by
     * newrect, but only if the coordinates are not out-of-bounds. Also reassigns
     * the floating con to a different workspace if this move was across different
     * outputs.
     *
     */
    bool floating_reposition(Con * con, Rect newrect);

    /**
     * Sets size of the CT_FLOATING_CON to specified dimensions. Might limit the
     * actual size with regard to size constraints taken from user settings.
     * Additionally, the dimensions may be upscaled until they're divisible by the
     * window's size hints.
     *
     */
    void floating_resize(Con * floating_con, uint32_t x, uint32_t y);

    /**
     * Fixes the coordinates of the floating window whenever the window gets
     * reassigned to a different output (or when the output’s rect changes).
     *
     */
    void floating_fix_coordinates(FloatingCon * con, Rect const &old_rect, Rect const &new_rect);
}
