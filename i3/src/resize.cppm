/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * resize.c: Interactive resizing.
 *
 */
module;
#include <xcb/xcb.h>
export module i3:resize;

import :internal;

class Con;

export {
    bool resize_find_tiling_participants(Con * *current, Con * *other, direction_t direction, bool both_sides);

    /**
     * Resize the two given containers using the given amount of pixels or
     * percentage points. One of the two needs to be 0. A positive amount means
     * growing the first container while a negative means shrinking it.
     * Returns false when the resize would result in one of the two containers
     * having less than 1 pixel of size.
     *
     */
    bool resize_neighboring_cons(Con * first, Con * second, int px, int ppt);

    /**
     * Calculate the minimum percent needed for the given container to be at least 1
     * pixel.
     *
     */
    double percent_for_1px(Con * con);
}
