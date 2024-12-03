/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
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

/*
 * Returns the content container below the given output container.
 *
 */
ConCon* OutputCon::output_get_content() {
    for (auto& child : this->nodes) {
        if (child->type == CT_CON) {
            return dynamic_cast<ConCon*>(child);
        }
    }

    return nullptr;
}

void OutputCon::on_remove_child() {
    DLOG("on_remove_child\n");
    DLOG(fmt::sprintf("not handling, type = %d, name = %s\n", std::to_underlying(this->type), this->name));
}
