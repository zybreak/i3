/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * con.c: Functions which deal with containers directly (creating containers,
 *        searching containers, getting specific properties from containers,
 *        …).
 *
 */
module;
#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import std;
import log;

bool ConCon::con_has_managed_window() {
    return (this->window != nullptr && this->window->id != XCB_WINDOW_NONE && this->con_get_workspace() != nullptr);
}

bool ConCon::con_accepts_window() {
    if (this->con_is_split()) {
        DLOG(fmt::sprintf("container %p does not accept windows, it is a split container.\n", fmt::ptr(this)));
        return false;
    }

    /* TODO: if this is a swallowing container, we need to check its max_clients */
    return (this->window == nullptr);
}
