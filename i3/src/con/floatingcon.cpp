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

void FloatingCon::con_attach(Con *parent, bool ignore_focus, Con *previous) {
    this->parent = parent;
    auto &focused = parent->focused;

    DLOG("Inserting into floating containers\n");
    dynamic_cast<WorkspaceCon *>(parent)->floating_windows.push_back(this);

    /* We insert to the TAIL because con_focus() will correct this.
     * This way, we have the option to insert Cons without having
     * to focus them. */
    focused.push_back(this);
    con_force_split_parents_redraw(this);
}

void FloatingCon::con_detach() {
    con_force_split_parents_redraw(this);
    std::erase(dynamic_cast<WorkspaceCon *>(this->parent)->floating_windows, this);
    std::erase(this->parent->focused, this);
}

FloatingCon *FloatingCon::con_inside_floating() {
    return this;
}
