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
import rect;

ConCon::ConCon() : ConCon(nullptr, false) {
}

ConCon::ConCon(i3Window *window) : ConCon(window, false) {
}

ConCon::ConCon(i3Window *window, bool skeleton) : Con(skeleton) {
    this->type = CT_CON;
    if (window) {
        this->window.reset(window);
        this->depth = window->depth;
    }
}

bool ConCon::con_has_managed_window() {
    return (this->window && this->window->id != XCB_WINDOW_NONE && this->con_get_workspace() != nullptr);
}

bool ConCon::con_accepts_window() {
    if (this->con_is_split()) {
        DLOG(fmt::sprintf("container %p does not accept windows, it is a split container.\n", fmt::ptr(this)));
        return false;
    }

    /* TODO: if this is a swallowing container, we need to check its max_clients */
    return (!this->window);
}

i3Window* ConCon::get_window() {
    return this->window.get();
}

i3Window* ConCon::release_window() {
    return this->window.release();
}

void ConCon::set_window(i3Window* _window) {
    this->window.reset(_window);
}
void ConCon::set_geometry(Rect _geometry) {
    this->geometry = _geometry;
}
void ConCon::set_window_rect(Rect _window_rect) {
    this->window_rect = _window_rect;
}

Rect& ConCon::get_geometry() {
    return this->geometry;
}

Rect& ConCon::get_window_rect() {
    return this->window_rect;
}

int ConCon::get_window_icon_padding() {
    return this->window_icon_padding;
}

void ConCon::set_window_icon_padding(int padding) {
    this->window_icon_padding = padding;
};
