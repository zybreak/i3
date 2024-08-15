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
#include <cassert>

#include <ev.h>

#include <config.h>

#include <sys/time.h>

#include <xcb/xcb.h>
#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <fmt/printf.h>
#include <glib.h>
module i3;

import std;
import :output;
import format_placeholders;
import utils;
import log;
import rect;
import i3ipc;

/*
 * Create a new container (and attach it to the given parent, if not NULL).
 * This function only initializes the data structures.
 *
 */
Con::Con(bool skeleton) {
    this->fullscreen_mode = CF_NONE;
    this->layout = L_DEFAULT;
    this->last_split_layout = L_DEFAULT;
    this->floating = FLOATING_AUTO_OFF;

    this->type = CT_CON;
    this->border_style = this->max_user_border_style = global.configManager->config->default_border;
    this->current_border_width = -1;
    this->depth = global.x->root_depth;

    global.all_cons.push_back(this);

    DLOG("opening window\n");

    if (!skeleton) {
        global.x->con_init(this);
    }
}

/*
 * Frees the specified container.
 *
 */
Con::~Con() {
    this->stop_urgency_timer();
    
    auto it = std::ranges::find(global.all_cons, this);

    if (it != global.all_cons.end()) {
        global.all_cons.erase(it);
    }

    DLOG(fmt::sprintf("con %p freed\n", fmt::ptr(this)));
}

/*
 * Like strcasecmp but considers the case where either string is NULL.
 *
 */
static int strcasecmp_nullable(const char *a, const char *b) {
    if (a == b) {
        return 0;
    }
    if (a == nullptr) {
        return -1;
    }
    if (b == nullptr) {
        return 1;
    }
    return strcasecmp(a, b);
}

/*
 * Attaches the given container to the given parent. This happens when moving
 * a container or when inserting a new container at a specific place in the
 * tree.
 *
 * ignore_focus is to just insert the Con at the end (useful when creating a
 * new split container *around* some containers, that is, detaching and
 * attaching them in order without wanting to mess with the focus in between).
 *
 */
void Con::con_attach(Con *parent, bool ignore_focus, Con *previous) {
    this->parent = parent;
    Con *current = previous;
    auto &nodes = parent->nodes;
    auto &focused = parent->focused;

    if (parent->type == CT_DOCKAREA) {
        /* Insert dock client, sorting alphanumerically by class and then
         * instance name. This makes dock client order deterministic. As a side
         * effect, bars without a custom bar id will be sorted according to
         * their declaration order in the config file. See #3491. */
        current = nullptr;
        for (auto &loop : nodes) {
            int result = strcasecmp_nullable(dynamic_cast<ConCon*>(this)->get_window()->class_class.c_str(), dynamic_cast<ConCon*>(loop)->get_window()->class_class.c_str());
            if (result == 0) {
                result = strcasecmp_nullable(dynamic_cast<ConCon*>(this)->get_window()->class_instance.c_str(), dynamic_cast<ConCon*>(loop)->get_window()->class_instance.c_str());
            }
            if (result < 0) {
                current = loop;
                break;
            }
        }
        if (current) {
            current->insert_before(this);
        } else {
            nodes.push_back(this);
        }
        goto add_to_focused;
    }

    if (!ignore_focus) {
        /* Get the first tiling container in focus stack */
        for (auto &loop : parent->focused) {
            if (loop->type == CT_FLOATING_CON) {
                continue;
            }
            current = loop;
            break;
        }
    }

    /* When the container is not a split container (but contains a window)
     * and is attached to a workspace, we check if the user configured a
     * workspace_layout. This is done in workspace_attach_to, which will
     * provide us with the container to which we should attach (either the
     * workspace or a new split container with the configured
     * workspace_layout).
     */
    if (this->get_window() != nullptr &&
        parent->type == CT_WORKSPACE &&
        dynamic_cast<WorkspaceCon*>(parent)->workspace_layout != L_DEFAULT) {
        DLOG("Parent is a workspace. Applying default layout...\n");
        Con *target = workspace_attach_to(dynamic_cast<WorkspaceCon*>(parent));

        /* Attach the original con to this new split con instead */
        nodes = target->nodes;
        focused = target->focused;
        this->parent = target;
        current = nullptr;

        DLOG("done\n");
    }

    /* Insert the container after the tiling container, if found.
     * When adding to a CT_OUTPUT, just append one after another. */
    if (current != nullptr && parent->type != CT_OUTPUT) {
        DLOG(fmt::sprintf("Inserting con = %p after con %p\n", fmt::ptr(this), fmt::ptr(current)));
        current->insert_after(this);
    } else {
        nodes.push_back(this);
    }

    add_to_focused:
    /* We insert to the TAIL because con_focus() will correct this.
     * This way, we have the option to insert Cons without having
     * to focus them. */
    focused.push_back(this);
    con_force_split_parents_redraw(this);
}

void Con::insert_before(Con *con) {
    auto c_itr = std::ranges::find(this->parent->nodes, this);

    if (c_itr != this->parent->nodes.end()) {
        this->parent->nodes.insert(c_itr, con);
    }
}

void Con::insert_after(Con *con) {
    auto c_itr = std::ranges::find(this->parent->nodes, this);

    if (c_itr != this->parent->nodes.end()) {
        this->parent->nodes.insert(std::next(c_itr), con);
    }
}

/*
 * Detaches the given container from its current parent
 *
 */
void Con::con_detach() {
    con_force_split_parents_redraw(this);
    std::erase(this->parent->nodes, this);
    std::erase(this->parent->focused, this);
}

/*
 * Sets input focus to the given container. Will be updated in X11 in the next
 * run of x_push_changes().
 *
 */
void Con::con_focus() {
    DLOG(fmt::sprintf("con_focus = %p\n", fmt::ptr(this)));

    /* 1: set focused-pointer to the new con */
    /* 2: exchange the position of the container in focus stack of the parent all the way up */
    std::erase(this->parent->focused, this);
    this->parent->focused.push_front(this);
    if (this->parent->parent != nullptr) {
        this->parent->con_focus();
    }

    global.focused = this;
    /* We can't blindly reset non-leaf containers since they might have
     * other urgent children. Therefore we only reset leafs and propagate
     * the changes upwards via con_update_parents_urgency() which does proper
     * checks before resetting the urgency.
     */
    if (this->urgent && this->con_is_leaf()) {
        this->con_set_urgency(false);
        workspace_update_urgent_flag(this->con_get_workspace());
        ipc_send_window_event("urgent", this);
    }
}

/*
 *
 * Checks if the given container has an urgent child.
 *
 */
static bool con_has_urgent_child(Con *con) {
    if (con->con_is_leaf()) {
        return con->urgent;
    }

    /* We are not interested in floating windows since they can only be
     * attached to a workspace → nodes instead of focused */
    return std::ranges::any_of(con->nodes, [](Con *child) { return con_has_urgent_child(child); });
}

/*
 * Raise container to the top if it is floating or inside some floating
 * container.
 *
 */
static void con_raise(Con *con) {
    FloatingCon *floating = con->con_inside_floating();
    if (floating) {
        floating_raise_con(floating);
    }
}

/*
 * Sets input focus to the given container and raises it to the top.
 *
 */
void Con::con_activate() {
    this->con_focus();
    con_raise(this);
}

/*
 * Activates the container like in con_activate but removes fullscreen
 * restrictions and properly warps the pointer if needed.
 *
 */
void Con::con_activate_unblock() {
    WorkspaceCon *ws = this->con_get_workspace();
    Con *previous_focus = global.focused;
    Con *fullscreen_on_ws = (ws) ? ws->con_get_fullscreen_covering_ws() : nullptr;

    if (fullscreen_on_ws && fullscreen_on_ws != this && !this->con_has_parent(fullscreen_on_ws)) {
        con_disable_fullscreen(fullscreen_on_ws);
    }

    this->con_activate();

    /* If the container is not on the current workspace, workspace_show() will
     * switch to a different workspace and (if enabled) trigger a mouse pointer
     * warp to the currently focused container (!) on the target workspace.
     *
     * Therefore, before calling workspace_show(), we make sure that 'con' will
     * be focused on the workspace. However, we cannot just con_focus(con)
     * because then the pointer will not be warped at all (the code thinks we
     * are already there).
     *
     * So we focus 'con' to make it the currently focused window of the target
     * workspace, then revert focus. */
    if (ws != previous_focus->con_get_workspace()) {
        previous_focus->con_activate();
        /* Now switch to the workspace, then focus */
        workspace_show(ws);
        this->con_activate();
    }
}

/*
 * Closes the given container.
 *
 * TODO: Only one usage in commands.cpp, move code?
 *
 */
void Con::con_close(kill_window_t kill_window) {
    DLOG(fmt::sprintf("Closing con = %p.\n", fmt::ptr(this)));

    /* We never close output or root containers. */
    if (this->type == CT_OUTPUT || this->type == CT_ROOT) {
        DLOG(fmt::sprintf("con = %p is of type %d, not closing anything.\n", fmt::ptr(this), std::to_underlying(this->type)));
        return;
    }

    if (this->type == CT_WORKSPACE) {
        DLOG(fmt::sprintf("con = %p is a workspace, closing all children instead.\n", fmt::ptr(this)));
        for (auto &child : this->focused) {
            DLOG(fmt::sprintf("killing child = %p.\n", fmt::ptr(child)));
            tree_close_internal(child, kill_window, false);
        }

        return;
    }

    tree_close_internal(this, kill_window, false);
}

/*
 * Returns true when this node is a leaf node (has no children)
 *
 */
bool Con::con_is_leaf() const {
    return this->nodes.empty();
}

/*
 * Returns true when this con is a leaf node with a managed X11 window (e.g.,
 * excluding dock containers)
 */
bool Con::con_has_managed_window() {
    return false;
}

/*
 * Returns true if this node has regular or floating children.
 *
 */
bool Con::con_has_children() const {
    return !this->con_is_leaf();
}

/*
 * Returns true if a container should be considered split.
 *
 */
bool Con::con_is_split() const {
    if (this->con_is_leaf()) {
        return false;
    }

    switch (this->layout) {
        case layout_t::L_DOCKAREA:
        case layout_t::L_OUTPUT:
            return false;

        default:
            return true;
    }
}

/*
 * This will only return true for containers which have some parent with
 * a tabbed / stacked parent of which they are not the currently focused child.
 *
 */
bool Con::con_is_hidden() {
    Con *current = this;

    /* ascend to the workspace level and memorize the highest-up container
     * which is stacked or tabbed. */
    while (current != nullptr && current->type != CT_WORKSPACE) {
        Con *parent = current->parent;
        if (parent != nullptr && (parent->layout == L_TABBED || parent->layout == L_STACKED)) {
            if (con::first(parent->focused) != current) {
                return true;
            }
        }

        current = parent;
    }

    return false;
}

/*
 * Returns whether the container or any of its children is sticky.
 *
 */
bool Con::con_is_sticky() {
    if (this->sticky) {
        return true;
    }

    for (auto &child : this->nodes) {
        if (child->con_is_sticky()) {
            return true;
        }
    }

    return false;
}

/*
 * Returns true if this node accepts a window (if the node swallows windows,
 * it might already have swallowed enough and cannot hold any more).
 *
 */
bool Con::con_accepts_window() {
    return false;
}

/*
 * Gets the output container (first container with CT_OUTPUT in hierarchy) this
 * node is on.
 *
 */
OutputCon* Con::con_get_output() const {
    Con const *result = this;
    while (result != nullptr && result->type != con_type_t::CT_OUTPUT) {
        result = result->parent;
    }
    /* We must be able to get an output because focus can never be set higher
     * in the tree (root node cannot be focused). */
    if (result == nullptr) {
        throw std::runtime_error("Output not found");
    }
    return dynamic_cast<OutputCon*>(const_cast<Con*>(result));
}

/*
 * Gets the workspace container this node is on.
 *
 */
WorkspaceCon* Con::con_get_workspace() const {
    Con const *result = this;
    while (result != nullptr) {
        if (result->type == con_type_t::CT_WORKSPACE) {
            return dynamic_cast<WorkspaceCon*>(const_cast<Con*>(result));
        } else {
            result = result->parent;
        }
    }
    return nullptr;
}

/*
 * Searches parents of the given 'con' until it reaches one with the specified
 * 'orientation'. Aborts when it comes across a floating_con.
 *
 */
Con* Con::con_parent_with_orientation(orientation_t orientation) {
    DLOG(fmt::sprintf("Searching for parent of Con %p with orientation %d\n", fmt::ptr(this), std::to_underlying(orientation)));
    Con *parent = this->parent;
    if (parent->type == CT_FLOATING_CON) {
        return nullptr;
    }
    while (con_orientation(parent) != orientation) {
        DLOG("Need to go one level further up\n");
        parent = parent->parent;
        /* Abort when we reach a floating con, or an output con */
        if (parent &&
            (parent->type == CT_FLOATING_CON ||
             parent->type == CT_OUTPUT ||
             (parent->parent && parent->parent->type == CT_OUTPUT))) {
            parent = nullptr;
        }
        if (parent == nullptr) {
            break;
        }
    }
    DLOG(fmt::sprintf("Result: %p\n", fmt::ptr(parent)));
    return parent;
}

/*
 * Returns the first fullscreen node below this node.
 *
 */
Con* Con::con_get_fullscreen_con(fullscreen_mode_t fullscreen_mode) {
    std::deque<Con*> bfs{};
    bfs.push_back(this);

    while (!bfs.empty()) {
        Con *current = bfs.front();
        if (current != this && current->fullscreen_mode == fullscreen_mode) {
            /* empty the queue */
            bfs.clear();
            return current;
        }

        bfs.pop_front();

        for (auto child : current->nodes) {
            bfs.push_back(child);
        }

        if (dynamic_cast<WorkspaceCon*>(current) != nullptr) {
            for (auto child : dynamic_cast<WorkspaceCon*>(current)->floating_windows) {
                bfs.push_back(child);
            }
        }
    }

    return nullptr;
}

/*
 * Returns the fullscreen node that covers the given workspace if it exists.
 * This is either a CF_GLOBAL fullscreen container anywhere or a CF_OUTPUT
 * fullscreen container in the workspace.
 *
 */
Con* Con::con_get_fullscreen_covering_ws() {
    Con *fs = global.croot->con_get_fullscreen_con(CF_GLOBAL);
    if (!fs) {
        return this->con_get_fullscreen_con(CF_OUTPUT);
    }
    return fs;
}

/*
 * Returns true if the node is floating.
 *
 */
bool Con::con_is_floating() const {
    return (this->floating >= FLOATING_AUTO_ON);
}

/*
 * Returns true if the container is a docked container.
 *
 */
bool Con::con_is_docked() const {
    if (this->parent == nullptr) {
        return false;
    }

    if (this->parent->type == CT_DOCKAREA) {
        return true;
    }

    return this->parent->con_is_docked();
}

/*
 * Checks if the given container is either floating or inside some floating
 * container. It returns the FLOATING_CON container.
 *
 */
FloatingCon* Con::con_inside_floating() {
    if (this->floating >= FLOATING_AUTO_ON) {
        return dynamic_cast<FloatingCon *>(this->parent);
    }

    return this->parent == nullptr ? nullptr : this->parent->con_inside_floating();
}

/*
 * Checks if the given container is inside a focused container.
 *
 */
bool Con::con_inside_focused() const {
    if (this == global.focused) {
        return true;
    }
    if (!this->parent) {
        return false;
    }
    return this->parent->con_inside_focused();
}

/*
 * Checks if the container has the given parent as an actual parent.
 *
 */
bool Con::con_has_parent(Con *parent) const {
    Con *current = this->parent;
    if (current == nullptr) {
        return false;
    }

    if (current == parent) {
        return true;
    }

    return current != nullptr && current->con_has_parent(parent);
}

/*
 * Returns true if the given container (still) exists.
 * This can be used, e.g., to make sure a container hasn't been closed in the meantime.
 *
 */
bool Con::exists() {
    return std::ranges::find(global.all_cons, this) != global.all_cons.end();
}

/*
 * Iterate over the container's focus stack and return an array with the
 * containers inside it, ordered from higher focus order to lowest.
 *
 */
std::vector<Con*> Con::get_focus_order() {
    std::vector<Con*> focus_order;
    focus_order.reserve(focused.size());
    for (auto &current : focused) {
        focus_order.push_back(current);
    }

    return focus_order;
}

/*
 * Clear the container's focus stack and re-add it using the provided container
 * array. The function doesn't check if the provided array contains the same
 * containers with the previous focus stack but will not add floating containers
 * in the new focus stack if container is not a workspace.
 *
 */
void Con::set_focus_order(std::vector<Con*> focus_order) {
    auto focus_heads = this->focused.size();
    this->focused.clear();

    for (int idx = 0; idx < focus_heads; idx++) {
        /* Useful when encapsulating a workspace. */
        if (this->type != CT_WORKSPACE && focus_order[idx]->con_inside_floating()) {
            focus_heads++;
            continue;
        }

        this->focused.push_back(focus_order[idx]);
    }
}

/*
 * Returns the number of children of this container.
 *
 */
unsigned long Con::con_num_children() const {
    return this->nodes.size();
}

/*
 * Returns the number of visible non-floating children of this container.
 * For example, if the container contains a hsplit which has two children,
 * this will return 2 instead of 1.
 */
int Con::con_num_visible_children() {
    int children = 0;
    for (auto &current : this->nodes) {
        /* Visible leaf nodes are a child. */
        if (!current->con_is_hidden() && current->con_is_leaf()) {
            children++;
        /* All other containers need to be recursed. */
        } else {
            children += current->con_num_visible_children();
        }
    }

    return children;
}

/*
 * Count the number of windows (i.e., leaf containers).
 *
 */
int Con::con_num_windows() {
    if (this->con_has_managed_window()) {
        return 1;
    }

    int num = 0;
    for (auto &current : this->nodes) {
        num += current->con_num_windows();
    }

    if (dynamic_cast<WorkspaceCon*>(this) != nullptr) {
        for (auto &current : dynamic_cast<WorkspaceCon*>(this)->floating_windows) {
            num += current->con_num_windows();
        }
    }

    return num;
}

/*
 * Updates the percent attribute of the children of the given container. This
 * function needs to be called when a window is added or removed from a
 * container.
 *
 */
void Con::con_fix_percent() {
    auto children = this->con_num_children();

    /* calculate how much we have distributed and how many containers
     * with a percentage set we have */
    double total = 0.0;
    int children_with_percent = 0;
    for (auto &child : this->nodes) {
        if (child->percent > 0.0) {
            total += child->percent;
            ++children_with_percent;
        }
    }

    /* if there were children without a percentage set, set to a value that
     * will make those children proportional to all others */
    if (children_with_percent != children) {
        for (auto &child : this->nodes) {
            if (child->percent <= 0.0) {
                if (children_with_percent == 0) {
                    total += (child->percent = 1.0);
                } else {
                    total += (child->percent = total / children_with_percent);
                }
            }
        }
    }

    /* if we got a zero, just distribute the space equally, otherwise
     * distribute according to the proportions we got */
    if (total == 0.0) {
        for (auto &child : this->nodes) {
            child->percent = 1.0 / children;
        }
    } else if (total != 1.0) {
        for (auto &child : this->nodes) {
            child->percent /= total;
        }
    }
}

/*
 * This function changes the layout of a given container. Use it to handle
 * special cases like changing a whole workspace to stacked/tabbed (creates a
 * new split container before).
 *
 */
void Con::con_set_layout(layout_t layout) {
    DLOG(fmt::sprintf("con_set_layout(%p, %d), con->type = %d\n",
         fmt::ptr(this), std::to_underlying(layout), std::to_underlying(this->type)));

    Con *con = this->parent;

    /* We fill in last_split_layout when switching to a different layout
     * since there are many places in the code that don’t use
     * con_set_layout(). */
    if (con->layout == L_SPLITH || con->layout == L_SPLITV) {
        con->last_split_layout = con->layout;
    }

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
    con_force_split_parents_redraw(con);
}

/*
 * Callback which will be called when removing a child from the given con.
 * Kills the container if it is empty and replaces it with the child if there
 * is exactly one child.
 *
 */
void Con::on_remove_child() {
    DLOG("on_remove_child\n");

    /* Every container 'above' (in the hierarchy) the workspace content should
     * not be closed when the last child was removed */
    if (this->parent != nullptr && this->parent->type == CT_OUTPUT) {
        DLOG(fmt::sprintf("not handling, type = %d, name = %s\n",  std::to_underlying(this->type), this->name));
        return;
    }

    con_force_split_parents_redraw(this);
    this->urgent = con_has_urgent_child(this);
    this->con_update_parents_urgency();

    /* TODO: check if this container would swallow any other client and
     * don’t close it automatically. */
    auto children = this->con_num_children();
    if (children == 0) {
        DLOG("Container empty, closing\n");
        tree_close_internal(this, kill_window_t::DONT_KILL_WINDOW, false);
    }
}

/*
 * Make all parent containers urgent if con is urgent or clear the urgent flag
 * of all parent containers if there are no more urgent children left.
 *
 */
void Con::con_update_parents_urgency() {
    Con *parent = this->parent;

    bool new_urgency_value = this->urgent;
    while (parent && parent->type != CT_WORKSPACE && parent->type != CT_DOCKAREA) {
        if (new_urgency_value) {
            parent->urgent = true;
        } else {
            /* We can only reset the urgency when the parent
             * has no other urgent children */
            if (!con_has_urgent_child(parent)) {
                parent->urgent = false;
            }
        }
        parent = parent->parent;
    }
}

void Con::stop_urgency_timer() {
    if (this->urgency_timer == nullptr) {
        return;
    }
    
    DLOG(fmt::sprintf("Removing urgency timer of con %p\n", fmt::ptr(this)));
    ev_timer_stop(global.eventHandler->main_loop, this->urgency_timer);
    delete this->urgency_timer;
    this->urgency_timer = nullptr;
    this->urgency_timer_cb = nullptr;
}

void Con::start_urgency_timer(float after, float repeat, std::function<void(Con *con)> cb) {
    if (this->urgency_timer == nullptr) {
        DLOG(fmt::sprintf("Deferring reset of urgency flag of con %p on newly shown workspace %p\n",
                fmt::ptr(this), fmt::ptr(con_get_workspace())));
        this->urgency_timer = new ev_timer{};
        this->urgency_timer_cb = cb;
        this->urgency_timer->data = this;
        auto _cb = [](EV_P_ ev_timer *w, int revents) {
            Con *con = static_cast<Con*>(w->data);
            con->urgency_timer_cb(con);
        };
        
        /* use a repeating timer to allow for easy resets */
        ev_timer_init(this->urgency_timer, _cb, after, repeat);
        ev_timer_start(global.eventHandler->main_loop, this->urgency_timer);
    } else {
        DLOG(fmt::sprintf("Resetting urgency timer of con %p on workspace %p\n",
                fmt::ptr(this), fmt::ptr(con_get_workspace())));
        ev_timer_again(global.eventHandler->main_loop, this->urgency_timer);
    }
}

/*
 * Set urgency flag to the container, all the parent containers and the workspace.
 *
 */
void Con::con_set_urgency(bool urgent) {
    Con *con = this;
    
    if (urgent && global.focused == con) {
        DLOG("Ignoring urgency flag for current client\n");
        return;
    }

    const bool old_urgent = con->urgent;

    if (con->urgency_timer == nullptr) {
        con->urgent = urgent;
    } else {
        DLOG("Discarding urgency WM_HINT because timer is running\n");
    }

    if (con->get_window()) {
        if (con->urgent) {
            con->get_window()->urgent = std::chrono::system_clock::now();
        } else {
            con->get_window()->urgent = std::nullopt;
        }
    }

    con->con_update_parents_urgency();

    /* Set the urgency flag on the workspace, if a workspace could be found
     * (for dock clients, that is not the case). */
    if (WorkspaceCon *ws = con->con_get_workspace(); ws != nullptr) {
        workspace_update_urgent_flag(ws);
    }

    if (con->urgent != old_urgent) {
        LOG(fmt::sprintf("Urgency flag changed to %d\n",  con->urgent));
        ipc_send_window_event("urgent", con);
    }
}
