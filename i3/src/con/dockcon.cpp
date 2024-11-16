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
#include <fmt/printf.h>
module i3;

import std;
import log;

void DockCon::on_remove_child() {
    DLOG("on_remove_child\n");
    DLOG(fmt::sprintf("not handling, type = %d, name = %s\n", std::to_underlying(this->type), this->name));
}

FloatingCon *DockCon::con_inside_floating() {
    return nullptr;
}
