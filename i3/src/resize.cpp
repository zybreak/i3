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
#include <xcb/xcb.h>

#include <fmt/printf.h>
module i3;

import std;
import log;
import rect;

bool resize_find_tiling_participants(Con **current, Con **other, direction_t direction, bool both_sides) {
    DLOG(fmt::sprintf("Find two participants for resizing container=%p in direction=%i\n", fmt::ptr(other), std::to_underlying(direction)));
    Con *first = *current;
    Con *second = nullptr;
    if (first == nullptr) {
        DLOG("Current container is NULL, aborting.\n");
        return false;
    }

    /* Go up in the tree and search for a container to resize */
    const orientation_t search_orientation = orientation_from_direction(direction);
    const bool dir_backwards = (direction == D_UP || direction == D_LEFT);
    while (first->type != CT_WORKSPACE &&
           first->type != CT_FLOATING_CON &&
           second == nullptr) {
        /* get the appropriate first container with the matching
         * orientation (skip stacked/tabbed cons) */
        if ((con_orientation(first->parent) != search_orientation) ||
            (first->parent->layout == L_STACKED) ||
            (first->parent->layout == L_TABBED)) {
            first = first->parent;
            continue;
        }

        /* get the counterpart for this resizement */
        if (dir_backwards) {
            second = con::previous(first, first->parent->nodes);
            if (second == nullptr && both_sides) {
                second = con::next(first, first->parent->nodes);
            }
        } else {
            second = con::next(first, first->parent->nodes);
            if (second == nullptr && both_sides) {
                second = con::previous(first, first->parent->nodes);
            }
        }

        if (second == nullptr) {
            DLOG("No second container in this direction found, trying to look further up in the tree...\n");
            first = first->parent;
        }
    }

    DLOG(fmt::sprintf("Found participants: first=%p and second=%p.\n",  fmt::ptr(first), fmt::ptr(second)));
    *current = first;
    *other = second;
    if (first == nullptr || second == nullptr) {
        DLOG("Could not find two participants for this resize request.\n");
        return false;
    }

    return true;
}

/*
 * Calculate the minimum percent needed for the given container to be at least 1
 * pixel.
 *
 */
double percent_for_1px(Con *con) {
    const int parent_size = con_rect_size_in_orientation(con->parent);
    /* deco_rect.height is subtracted from each child in render_con_split */
    const int min_size = (con_orientation(con->parent) == HORIZ ? 1 : 1 + con->deco_rect.height);
    return (static_cast<double>(min_size) / static_cast<double>(parent_size));
}

/*
 * Resize the two given containers using the given amount of pixels or
 * percentage points. One of the two needs to be 0. A positive amount means
 * growing the first container while a negative means shrinking it.
 * Returns false when the resize would result in one of the two containers
 * having less than 1 pixel of size.
 *
 */
bool resize_neighboring_cons(Con *first, Con *second, int px, int ppt) {
    assert(px * ppt == 0);

    Con *parent = first->parent;
    double new_first_percent;
    double new_second_percent;
    if (ppt) {
        new_first_percent = first->percent + (static_cast<double>(ppt) / 100.0);
        new_second_percent = second->percent - (static_cast<double>(ppt) / 100.0);
    } else {
        /* Convert px change to change in percentages */
        const double pct = static_cast<double>(px) / static_cast<double>(con_rect_size_in_orientation(first->parent));
        new_first_percent = first->percent + pct;
        new_second_percent = second->percent - pct;
    }
    /* Ensure that no container will be less than 1 pixel in the resizing
     * direction. */
    if (new_first_percent < percent_for_1px(first) || new_second_percent < percent_for_1px(second)) {
        return false;
    }

    first->percent = new_first_percent;
    second->percent = new_second_percent;
    parent->con_fix_percent();
    return true;
}
