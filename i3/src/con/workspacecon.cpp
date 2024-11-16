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
import i3ipc;

/*
 * Make all parent containers urgent if con is urgent or clear the urgent flag
 * of all parent containers if there are no more urgent children left.
 *
 */
void WorkspaceCon::con_update_parents_urgency() {
    /* Urgency hints should not be set on any container higher up in the
     * hierarchy than the workspace level. Unfortunately, since the content
     * container has type == CT_CON, that’s not easy to verify in the loop
     * below, so we need another condition to catch that case: */
}

void WorkspaceCon::con_attach(Con *parent, bool ignore_focus, Con *previous_con) {
    this->parent = parent;
    auto previous = dynamic_cast<WorkspaceCon *>(previous_con);
    WorkspaceCon *current = previous;
    auto &focused = parent->focused;

    /* Workspaces are handled differently: they need to be inserted at the
     * right position. */
    DLOG(fmt::sprintf("it's a workspace. num = %d\n", this->num));
    if (this->num == -1 || parent->nodes.empty()) {
        parent->nodes.push_back(this);
    } else {
        current = dynamic_cast<WorkspaceCon *>(con::first(parent->nodes));
        if (this->num < current->num) {
            /* we need to insert the container at the beginning */
            parent->nodes.push_front(this);
        } else {
            while (current && current->num != -1 && this->num > current->num) {
                current = dynamic_cast<WorkspaceCon *>(con::next(current, parent->nodes));
                if (current == con::last(parent->nodes)) {
                    current = nullptr;
                    break;
                }
            }
            /* we need to insert con after current, if current is not NULL */
            if (current) {
                current->insert_before(this);
            } else {
                parent->nodes.push_back(this);
            }
        }
    }

    /* We insert to the TAIL because con_focus() will correct this.
     * This way, we have the option to insert Cons without having
     * to focus them. */
    focused.push_back(this);
    con_force_split_parents_redraw(this);
}

FloatingCon *WorkspaceCon::con_inside_floating() {
    return nullptr;
}

/*
 * This function changes the layout of a given container. Use it to handle
 * special cases like changing a whole workspace to stacked/tabbed (creates a
 * new split container before).
 *
 * Users can focus workspaces, but not any higher in the hierarchy.
 * Focus on the workspace is a special case, since in every other case, the
 * user means "change the layout of the parent split container".
 *
 */
void WorkspaceCon::con_set_layout(layout_t layout) {
    WorkspaceCon *con = this;
    DLOG(fmt::sprintf("con_set_layout(%p, %d), con->type = %d\n",
                      fmt::ptr(con), std::to_underlying(layout), std::to_underlying(con->type)));

    /* We fill in last_split_layout when switching to a different layout
     * since there are many places in the code that don’t use
     * con_set_layout(). */
    if (con->layout == L_SPLITH || con->layout == L_SPLITV) {
        con->last_split_layout = con->layout;
    }

    /* When the container type is CT_WORKSPACE, the user wants to change the
     * whole workspace into stacked/tabbed mode. To do this and still allow
     * intuitive operations (like level-up and then opening a new window), we
     * need to create a new split container. */
    if (con->con_num_children() == 0) {
        layout_t ws_layout = (layout == L_STACKED || layout == L_TABBED) ? layout : L_DEFAULT;
        DLOG(fmt::sprintf("Setting workspace_layout to %d\n", std::to_underlying(ws_layout)));
        con->workspace_layout = ws_layout;
        DLOG(fmt::sprintf("Setting layout to %d\n", std::to_underlying(layout)));

        if (layout == L_DEFAULT) {
            /* Special case: the layout formerly known as "default" (in combination
             * with an orientation). Since we switched to splith/splitv layouts,
             * using the "default" layout (which "only" should happen when using
             * legacy configs) is using the last split layout (either splith or
             * splitv) in order to still do the same thing. */
            con->layout = con->last_split_layout;
            /* In case last_split_layout was not initialized… */
            if (con->layout == L_DEFAULT) {
                con->layout = L_SPLITH;
            }
        } else {
            con->layout = layout;
        }
    } else if (layout == L_STACKED || layout == L_TABBED || layout == L_SPLITV || layout == L_SPLITH) {
        DLOG("Creating new split container\n");
        /* 1: create a new split container */
        Con *new_con = new ConCon();
        new_con->parent = con;

        /* 2: Set the requested layout on the split container and mark it as
         * split. */
        new_con->layout = layout;
        new_con->last_split_layout = con->last_split_layout;

        /* 3: move the existing cons of this workspace below the new con */
        auto focus_order = con->get_focus_order();

        DLOG("Moving cons\n");
        Con *child;
        while (!con->nodes.empty()) {
            child = con::first(con->nodes);
            child->con_detach();
            child->con_attach(new_con, true);
        }

        new_con->set_focus_order(focus_order);

        /* 4: attach the new split container to the workspace */
        DLOG("Attaching new split to ws\n");
        new_con->con_attach(con, false);

        tree_flatten(global.croot);
    }
    con_force_split_parents_redraw(con);
}

void WorkspaceCon::on_remove_child() {
    DLOG("on_remove_child\n");

    /* Every container 'above' (in the hierarchy) the workspace content should
     * not be closed when the last child was removed */
    if (this->parent != nullptr && this->parent->type == CT_OUTPUT) {
        DLOG(fmt::sprintf("not handling, type = %d, name = %s\n", std::to_underlying(this->type), this->name));
        return;
    }

    /* For workspaces, close them only if they're not visible anymore */
    if (this->focused.empty() && !workspace_is_visible(this)) {
        LOG(fmt::sprintf("Closing old workspace (%p / %s), it is empty\n", fmt::ptr(this), this->name));
        auto gen = ipc_marshal_workspace_event("empty", this, nullptr);
        tree_close_internal(this, kill_window_t::DONT_KILL_WINDOW, false);

        auto payload = gen.dump();
        global.ipcManager->ipc_send_event("workspace", i3ipc::EVENT_WORKSPACE, payload);
    }
}

/*
 * Returns true if this node has regular or floating children.
 *
 */
bool WorkspaceCon::con_has_children() const {
    return (!this->con_is_leaf() || !this->floating_windows.empty());
}
