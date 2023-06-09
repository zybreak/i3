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
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <ranges>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"

#include <xcb/xcb.h>
#include <vector>

#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "xcb.h"
#include "workspace.h"
#include "i3.h"
#include "x.h"
#include "floating.h"
#include "configuration.h"
#include "con.h"
#include "render.h"
#include "match.h"
#include "output.h"
#include "ewmh.h"
#include "startup.h"
#include "global.h"
#include "format_placeholders.h"

/*
 * Returns the content container below the given output container.
 *
 */
Con* Con::output_get_content() {
    for (auto &child : this->nodes_head) {
        if (child->type == CT_CON) {
            return child;
        }
    }

    return nullptr;
}

/*
 * force parent split containers to be redrawn
 *
 */
void con_force_split_parents_redraw(Con *con) {
    Con *parent = con;

    while (parent != nullptr && parent->type != CT_WORKSPACE && parent->type != CT_DOCKAREA) {
        if (!parent->con_is_leaf()) {
            delete parent->deco_render_params;
            parent->deco_render_params = nullptr;
        }

        parent = parent->parent;
    }
}

/*
 * Create a new container (and attach it to the given parent, if not NULL).
 * This function only initializes the data structures.
 *
 */
Con::Con(Con *parent, i3Window *window, bool skeleton) {
    this->fullscreen_mode = CF_NONE;
    this->layout = L_DEFAULT;
    this->last_split_layout = L_DEFAULT;
    this->workspace_layout = L_DEFAULT;
    this->floating = FLOATING_AUTO_OFF;
    this->scratchpad_state = SCRATCHPAD_NONE;

    this->type = CT_CON;
    this->window = window;
    this->border_style = config.default_border;
    this->current_border_width = -1;
    this->window_icon_padding = -1;
    if (window) {
        this->depth = window->depth;
    } else {
        this->depth = global.x->root_depth;
    }

    global.all_cons.push_back(this);

    DLOG("opening window\n");

    if (parent != nullptr)
        this->con_attach(parent, false);

    if (!skeleton) {
        x_con_init(this);
    }
}

/*
 * Frees the specified container.
 *
 */
Con::~Con() {

    delete this->deco_render_params;
    auto it = std::ranges::find(global.all_cons, this);

    if (it != global.all_cons.end()) {
        global.all_cons.erase(it);
    }

    this->swallow.clear();

    DLOG(fmt::sprintf("con %p freed\n",  (void*)this));
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
    auto &nodes_head = parent->nodes_head;
    auto &focus_head = parent->focus_head;

    /* Workspaces are handled differently: they need to be inserted at the
     * right position. */
    if (this->type == CT_WORKSPACE) {
        DLOG(fmt::sprintf("it's a workspace. num = %d\n",  this->num));
        if (this->num == -1 || nodes_head.empty()) {
            nodes_head.push_back(this);
        } else {
            current = con::first(nodes_head);
            if (this->num < current->num) {
                /* we need to insert the container at the beginning */
                nodes_head.push_front(this);
            } else {
                while (current && current->num != -1 && this->num > current->num) {
                    current = con::next(current, nodes_head);
                    if (current == con::last(nodes_head)) {
                        current = nullptr;
                        break;
                    }
                }
                /* we need to insert con after current, if current is not NULL */
                if (current)
                    current->insert_before(this);
                else
                    nodes_head.push_back(this);
            }
        }
        goto add_to_focus_head;
    }

    if (parent->type == CT_DOCKAREA) {
        /* Insert dock client, sorting alphanumerically by class and then
         * instance name. This makes dock client order deterministic. As a side
         * effect, bars without a custom bar id will be sorted according to
         * their declaration order in the config file. See #3491. */
        current = nullptr;
        for (auto &loop : nodes_head) {
            int result = strcasecmp_nullable(this->window->class_class, loop->window->class_class);
            if (result == 0) {
                result = strcasecmp_nullable(this->window->class_instance, loop->window->class_instance);
            }
            if (result < 0) {
                current = loop;
                break;
            }
        }
        if (current) {
            current->insert_before(this);
        } else {
            nodes_head.push_back(this);
        }
        goto add_to_focus_head;
    }

    if (this->type == CT_FLOATING_CON) {
        DLOG("Inserting into floating containers\n");
        parent->floating_windows.push_back(this);
    } else {
        if (!ignore_focus) {
            /* Get the first tiling container in focus stack */
            for (auto &loop : parent->focus_head) {
                if (loop->type == CT_FLOATING_CON)
                    continue;
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
        if (this->window != nullptr &&
            parent->type == CT_WORKSPACE &&
            parent->workspace_layout != L_DEFAULT) {
            DLOG("Parent is a workspace. Applying default layout...\n");
            Con *target = workspace_attach_to(parent);

            /* Attach the original con to this new split con instead */
            nodes_head = target->nodes_head;
            focus_head = target->focus_head;
            this->parent = target;
            current = nullptr;

            DLOG("done\n");
        }

        /* Insert the container after the tiling container, if found.
         * When adding to a CT_OUTPUT, just append one after another. */
        if (current != nullptr && parent->type != CT_OUTPUT) {
            DLOG(fmt::sprintf("Inserting con = %p after con %p\n",  (void*)this, (void*)current));
            current->insert_after(this);
        } else
            nodes_head.push_back(this);
    }

    add_to_focus_head:
    /* We insert to the TAIL because con_focus() will correct this.
     * This way, we have the option to insert Cons without having
     * to focus them. */
    focus_head.push_back(this);
    con_force_split_parents_redraw(this);
}

namespace con {
    Con *first(std::deque<Con*> &queue) {
        if (queue.empty()) {
            return nullptr;
        }

        return queue.front();
    }

    Con *next(Con *con, std::deque<Con*> &queue) {
        auto c_itr = std::ranges::find(queue, con);
        if (c_itr == queue.end()) {
            return nullptr;
        }

        return (std::next(c_itr) == queue.end()) ? nullptr : *(++c_itr);
    }

    Con *previous(Con *con, std::deque<Con*> &queue) {
        auto c_itr = std::ranges::find(queue, con);
        if (c_itr == queue.end()) {
            return nullptr;
        }

        return (c_itr == queue.begin()) ? nullptr : *(--c_itr);
    }

    Con *last(std::deque<Con*> &queue) {
        if (queue.empty()) {
            return nullptr;
        }

        return queue.back();
    }
}

void Con::insert_before(Con *con) {
    auto c_itr = std::ranges::find(this->parent->nodes_head, this);

    if (c_itr != this->parent->nodes_head.end()) {
        this->parent->nodes_head.insert(c_itr, con);
    }
}

void Con::insert_after(Con *con) {
    auto c_itr = std::ranges::find(this->parent->nodes_head, this);

    if (c_itr != this->parent->nodes_head.end()) {
        this->parent->nodes_head.insert(std::next(c_itr), con);
    }
}

/*
 * Detaches the given container from its current parent
 *
 */
void Con::con_detach() {
    con_force_split_parents_redraw(this);
    if (this->type == CT_FLOATING_CON) {
        std::erase(this->parent->floating_windows, this);
        std::erase(this->parent->focus_head, this);
    } else {
        std::erase(this->parent->nodes_head, this);
        std::erase(this->parent->focus_head, this);
    }
}

/*
 * Sets input focus to the given container. Will be updated in X11 in the next
 * run of x_push_changes().
 *
 */
void Con::con_focus() {
    DLOG(fmt::sprintf("con_focus = %p\n",  (void*)this));

    /* 1: set focused-pointer to the new con */
    /* 2: exchange the position of the container in focus stack of the parent all the way up */
    std::erase(this->parent->focus_head, this);
    this->parent->focus_head.push_front(this);
    if (this->parent->parent != nullptr)
        this->parent->con_focus();

    global.focused = this;
    /* We can't blindly reset non-leaf containers since they might have
     * other urgent children. Therefore we only reset leafs and propagate
     * the changes upwards via con_update_parents_urgency() which does proper
     * checks before resetting the urgency.
     */
    if (this->urgent && this->con_is_leaf()) {
        con_set_urgency(this, false);
        con_update_parents_urgency(this);
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
    if (con->con_is_leaf())
        return con->urgent;

    /* We are not interested in floating windows since they can only be
     * attached to a workspace → nodes_head instead of focus_head */
    return std::ranges::any_of(con->nodes_head, [](Con *child) { return con_has_urgent_child(child); });
}

/*
 * Raise container to the top if it is floating or inside some floating
 * container.
 *
 */
static void con_raise(Con *con) {
    Con *floating = con->con_inside_floating();
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
    Con *ws = this->con_get_workspace();
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
    assert(this != nullptr);
    DLOG(fmt::sprintf("Closing con = %p.\n",  (void*)this));

    /* We never close output or root containers. */
    if (this->type == CT_OUTPUT || this->type == CT_ROOT) {
        DLOG(fmt::sprintf("con = %p is of type %d, not closing anything.\n",  (void*)this, this->type));
        return;
    }

    if (this->type == CT_WORKSPACE) {
        DLOG(fmt::sprintf("con = %p is a workspace, closing all children instead.\n",  (void*)this));
        for (auto &child : this->focus_head) {
            DLOG(fmt::sprintf("killing child = %p.\n",  (void*)child));
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
    return this->nodes_head.empty();
}

/*
 * Returns true when this con is a leaf node with a managed X11 window (e.g.,
 * excluding dock containers)
 */
bool Con::con_has_managed_window() {
    return (this->window != nullptr && this->window->id != XCB_WINDOW_NONE && this->con_get_workspace() != nullptr);
}

/*
 * Returns true if this node has regular or floating children.
 *
 */
bool Con::con_has_children() {
    return (!this->con_is_leaf() || !this->floating_windows.empty());
}

/*
 * Returns true if a container should be considered split.
 *
 */
bool Con::con_is_split() {
    if (this->con_is_leaf())
        return false;

    switch (this->layout) {
        case L_DOCKAREA:
        case L_OUTPUT:
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
            if (con::first(parent->focus_head) != current)
                return true;
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
    if (this->sticky)
        return true;

    for (auto &child : this->nodes_head) {
        if (child->con_is_sticky())
            return true;
    }

    return false;
}

/*
 * Returns true if this node accepts a window (if the node swallows windows,
 * it might already have swallowed enough and cannot hold any more).
 *
 */
bool Con::con_accepts_window() {
    /* 1: workspaces never accept direct windows */
    if (this->type == CT_WORKSPACE)
        return false;

    if (this->con_is_split()) {
        DLOG(fmt::sprintf("container %p does not accept windows, it is a split container.\n",  (void*)this));
        return false;
    }

    /* TODO: if this is a swallowing container, we need to check its max_clients */
    return (this->window == nullptr);
}

/*
 * Gets the output container (first container with CT_OUTPUT in hierarchy) this
 * node is on.
 *
 */
Con* Con::con_get_output() {
    Con *result = this;
    while (result != nullptr && result->type != CT_OUTPUT)
        result = result->parent;
    /* We must be able to get an output because focus can never be set higher
     * in the tree (root node cannot be focused). */
    assert(result != nullptr);
    return result;
}

/*
 * Gets the workspace container this node is on.
 *
 */
Con* Con::con_get_workspace() {
    Con *result = this;
    while (result != nullptr && result->type != CT_WORKSPACE)
        result = result->parent;
    return result;
}

/*
 * Searches parents of the given 'con' until it reaches one with the specified
 * 'orientation'. Aborts when it comes across a floating_con.
 *
 */
Con* Con::con_parent_with_orientation(orientation_t orientation) {
    DLOG(fmt::sprintf("Searching for parent of Con %p with orientation %d\n",  (void*)this, orientation));
    Con *parent = this->parent;
    if (parent->type == CT_FLOATING_CON)
        return nullptr;
    while (con_orientation(parent) != orientation) {
        DLOG("Need to go one level further up\n");
        parent = parent->parent;
        /* Abort when we reach a floating con, or an output con */
        if (parent &&
            (parent->type == CT_FLOATING_CON ||
             parent->type == CT_OUTPUT ||
             (parent->parent && parent->parent->type == CT_OUTPUT)))
            parent = nullptr;
        if (parent == nullptr)
            break;
    }
    DLOG(fmt::sprintf("Result: %p\n",  (void*)parent));
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
        auto &current = bfs.front();
        if (current != this && current->fullscreen_mode == fullscreen_mode) {
            /* empty the queue */
            bfs.clear();
            return current;
        }

        bfs.pop_front();

        for (auto &child : current->nodes_head) {
            bfs.push_back(child);
        }

        for (auto &child : current->floating_windows) {
            bfs.push_back(child);
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
 * Returns true if the container is internal, such as __i3_scratch
 *
 */
bool Con::con_is_internal() const {
    return (this->name[0] == '_' && this->name[1] == '_');
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
    if (this->parent == nullptr)
        return false;

    if (this->parent->type == CT_DOCKAREA)
        return true;

    return this->parent->con_is_docked();
}

/*
 * Checks if the given container is either floating or inside some floating
 * container. It returns the FLOATING_CON container.
 *
 */
Con* Con::con_inside_floating() {
    if (this->type == CT_FLOATING_CON)
        return this;

    if (this->floating >= FLOATING_AUTO_ON)
        return this->parent;

    if (this->type == CT_WORKSPACE || this->type == CT_OUTPUT)
        return nullptr;

    return this->parent->con_inside_floating();
}

/*
 * Checks if the given container is inside a focused container.
 *
 */
bool Con::con_inside_focused() {
    if (this == global.focused)
        return true;
    if (!this->parent)
        return false;
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
 * Returns the container with the given client window ID or NULL if no such
 * container exists.
 *
 */
Con *con_by_window_id(xcb_window_t window) {
    for (const auto &con : global.all_cons) {
        if (con->window != nullptr && con->window->id == window) {
            return con;
        }
    }
    return nullptr;
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
 * Returns the container with the given frame ID or NULL if no such container
 * exists.
 *
 */
Con *con_by_frame_id(xcb_window_t frame) {
    for (const auto &con : global.all_cons) {
        if (con->frame.id == frame) {
            return con;
        }
    }
    return nullptr;
}

/*
 * Returns the first container below 'con' which wants to swallow this window
 * TODO: priority
 *
 */
Con *con_for_window(Con *con, i3Window *window, Match **store_match) {
    // LOG(fmt::sprintf("searching con for window %p starting at con %p\n",  window, con));
    // LOG(fmt::sprintf("class == %s\n",  window->class_class));

    for (auto &child : con->nodes_head) {
        for (auto &match : child->swallow) {
            if (!match_matches_window(*match, window))
                continue;
            if (store_match != nullptr)
                *store_match = match.get();
            return child;
        }
        Con *result = con_for_window(child, window, store_match);
        if (result != nullptr)
            return result;
    }

    for (auto &child : con->floating_windows) {
        for (auto &match : child->swallow) {
            if (!match_matches_window(*match, window))
                continue;
            if (store_match != nullptr)
                *store_match = match.get();
            return child;
        }
        Con *result = con_for_window(child, window, store_match);
        if (result != nullptr)
            return result;
    }

    return nullptr;
}

/*
 * Iterate over the container's focus stack and return an array with the
 * containers inside it, ordered from higher focus order to lowest.
 *
 */
std::vector<Con*> Con::get_focus_order() {
    std::vector<Con*> focus_order;
    focus_order.reserve(focus_head.size());
    for (auto &current : focus_head) {
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
    auto focus_heads = this->focus_head.size();
    this->focus_head.clear();

    for (int idx = 0; idx < focus_heads; idx++) {
        /* Useful when encapsulating a workspace. */
        if (this->type != CT_WORKSPACE && focus_order[idx]->con_inside_floating()) {
            focus_heads++;
            continue;
        }

        this->focus_head.push_back(focus_order[idx]);
    }
}

/*
 * Returns the number of children of this container.
 *
 */
unsigned long Con::con_num_children() const {
    return this->nodes_head.size();
}

/*
 * Returns the number of visible non-floating children of this container.
 * For example, if the container contains a hsplit which has two children,
 * this will return 2 instead of 1.
 */
int Con::con_num_visible_children() {
    int children = 0;
    for (auto &current : this->nodes_head) {
        /* Visible leaf nodes are a child. */
        if (!current->con_is_hidden() && current->con_is_leaf())
            children++;
        /* All other containers need to be recursed. */
        else
            children += current->con_num_visible_children();
    }

    return children;
}

/*
 * Count the number of windows (i.e., leaf containers).
 *
 */
int Con::con_num_windows() {
    if (this->con_has_managed_window())
        return 1;

    int num = 0;
    for (auto &current : this->nodes_head) {
        num += current->con_num_windows();
    }

    for (auto &current : this->floating_windows) {
        num += current->con_num_windows();
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

    // calculate how much we have distributed and how many containers
    // with a percentage set we have
    double total = 0.0;
    int children_with_percent = 0;
    for (auto &child : this->nodes_head) {
        if (child->percent > 0.0) {
            total += child->percent;
            ++children_with_percent;
        }
    }

    // if there were children without a percentage set, set to a value that
    // will make those children proportional to all others
    if (children_with_percent != children) {
        for (auto &child : this->nodes_head) {
            if (child->percent <= 0.0) {
                if (children_with_percent == 0) {
                    total += (child->percent = 1.0);
                } else {
                    total += (child->percent = total / children_with_percent);
                }
            }
        }
    }

    // if we got a zero, just distribute the space equally, otherwise
    // distribute according to the proportions we got
    if (total == 0.0) {
        for (auto &child : this->nodes_head) {
            child->percent = 1.0 / children;
        }
    } else if (total != 1.0) {
        for (auto &child : this->nodes_head) {
            child->percent /= total;
        }
    }
}

/*
 * Toggles fullscreen mode for the given container. If there already is a
 * fullscreen container on this workspace, fullscreen will be disabled and then
 * enabled for the container the user wants to have in fullscreen mode.
 *
 */
void con_toggle_fullscreen(Con *con, int fullscreen_mode) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    DLOG(fmt::sprintf("toggling fullscreen for %p / %s\n",  (void*)con, con->name));

    if (con->fullscreen_mode == CF_NONE)
        con_enable_fullscreen(con, (fullscreen_mode_t)fullscreen_mode);
    else
        con_disable_fullscreen(con);
}

/*
 * Sets the specified fullscreen mode for the given container, sends the
 * “fullscreen_mode” event and changes the XCB fullscreen property of the
 * container’s window, if any.
 *
 */
static void con_set_fullscreen_mode(xcb_connection_t *conn, Con *con, fullscreen_mode_t fullscreen_mode) {
    con->fullscreen_mode = fullscreen_mode;

    DLOG(fmt::sprintf("mode now: %d\n",  con->fullscreen_mode));

    /* Send an ipc window "fullscreen_mode" event */
    ipc_send_window_event("fullscreen_mode", con);

    /* update _NET_WM_STATE if this container has a window */
    /* TODO: when a window is assigned to a container which is already
     * fullscreened, this state needs to be pushed to the client, too */
    if (con->window == nullptr)
        return;

    if (con->fullscreen_mode != CF_NONE) {
        DLOG(fmt::sprintf("Setting _NET_WM_STATE_FULLSCREEN for con = %p / window = %d.\n",  (void*)con, con->window->id));
        xcb_add_property_atom(conn, con->window->id, A__NET_WM_STATE, A__NET_WM_STATE_FULLSCREEN);
    } else {
        DLOG(fmt::sprintf("Removing _NET_WM_STATE_FULLSCREEN for con = %p / window = %d.\n",  (void*)con, con->window->id));
        xcb_remove_property_atom(conn, con->window->id, A__NET_WM_STATE, A__NET_WM_STATE_FULLSCREEN);
    }
}

/*
 * Enables fullscreen mode for the given container, if necessary.
 *
 * If the container’s mode is already CF_OUTPUT or CF_GLOBAL, the container is
 * kept fullscreen but its mode is set to CF_GLOBAL and CF_OUTPUT,
 * respectively.
 *
 * Other fullscreen containers will be disabled first, if they hide the new
 * one.
 *
 */
void con_enable_fullscreen(Con *con, fullscreen_mode_t fullscreen_mode) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    assert(fullscreen_mode == CF_GLOBAL || fullscreen_mode == CF_OUTPUT);

    if (fullscreen_mode == CF_GLOBAL)
        DLOG(fmt::sprintf("enabling global fullscreen for %p / %s\n",  (void*)con, con->name));
    else
        DLOG(fmt::sprintf("enabling fullscreen for %p / %s\n",  (void*)con, con->name));

    if (con->fullscreen_mode == fullscreen_mode) {
        DLOG(fmt::sprintf("fullscreen already enabled for %p / %s\n",  (void*)con, con->name));
        return;
    }

    Con *con_ws = con->con_get_workspace();

    /* Disable any fullscreen container that would conflict the new one. */
    Con *fullscreen = global.croot->con_get_fullscreen_con(CF_GLOBAL);
    if (fullscreen == nullptr)
        fullscreen = con_ws->con_get_fullscreen_con(CF_OUTPUT);
    if (fullscreen != nullptr)
        con_disable_fullscreen(fullscreen);

    /* Set focus to new fullscreen container. Unless in global fullscreen mode
     * and on another workspace restore focus afterwards.
     * Switch to the container’s workspace if mode is global. */
    Con *cur_ws = global.focused->con_get_workspace();
    Con *old_focused = global.focused;
    if (fullscreen_mode == CF_GLOBAL && cur_ws != con_ws)
        workspace_show(con_ws);
    con->con_activate();
    if (fullscreen_mode != CF_GLOBAL && cur_ws != con_ws)
        old_focused->con_activate();

    con_set_fullscreen_mode(**global.x, con, fullscreen_mode);
}

/*
 * Disables fullscreen mode for the given container regardless of the mode, if
 * necessary.
 *
 */
void con_disable_fullscreen(Con *con) {
    if (con->type == CT_WORKSPACE) {
        DLOG("You cannot make a workspace fullscreen.\n");
        return;
    }

    DLOG(fmt::sprintf("disabling fullscreen for %p / %s\n",  (void*)con, con->name));

    if (con->fullscreen_mode == CF_NONE) {
        DLOG(fmt::sprintf("fullscreen already disabled for %p / %s\n",  (void*)con, con->name));
        return;
    }

    con_set_fullscreen_mode(**global.x, con, CF_NONE);
}

static bool _con_move_to_con(Con *con, Con *target, bool behind_focused, bool fix_coordinates, bool dont_warp, bool ignore_focus, bool fix_percentage) {
    Con *orig_target = target;

    /* Prevent moving if this would violate the fullscreen focus restrictions. */
    Con *target_ws = target->con_get_workspace();
    if (!ignore_focus && !con_fullscreen_permits_focusing(target_ws)) {
        LOG("Cannot move out of a fullscreen container.\n");
        return false;
    }

    if (con->con_is_floating()) {
        DLOG("Container is floating, using parent instead.\n");
        con = con->parent;
    }

    Con *source_ws = con->con_get_workspace();

    if (con->type == CT_WORKSPACE) {
        /* Re-parent all of the old workspace's floating windows. */
        Con *child;
        while (!source_ws->floating_windows.empty()) {
            child = source_ws->floating_windows.front();
            con_move_to_workspace(child, target_ws, true, true, false);
        }

        /* If there are no non-floating children, ignore the workspace. */
        if (con->con_is_leaf())
            return false;

        con = workspace_encapsulate(con);
        if (con == nullptr) {
            ELOG("Workspace failed to move its contents into a container!\n");
            return false;
        }
    }

    /* Save the urgency state so that we can restore it. */
    bool urgent = con->urgent;

    /* Save the current workspace. So we can call workspace_show() by the end
     * of this function. */
    Con *current_ws = global.focused->con_get_workspace();

    Con *source_output = con->con_get_output(),
        *dest_output = target_ws->con_get_output();

    /* 1: save the container which is going to be focused after the current
     * container is moved away */
    Con *focus_next = nullptr;
    if (!ignore_focus && source_ws == current_ws && target_ws != source_ws) {
        focus_next = con_descend_focused(source_ws);
        if (focus_next == con || focus_next->con_has_parent(con)) {
            focus_next = con_next_focused(con);
        }
    }

    /* 2: we go up one level, but only when target is a normal container */
    if (target->type != CT_WORKSPACE) {
        DLOG(fmt::sprintf("target originally = %p / %s / type %d\n",  (void*)target, target->name, target->type));
        target = target->parent;
    }

    /* 3: if the original target is the direct child of a floating container, we
     * can't move con next to it - floating containers have only one child - so
     * we get the workspace instead. */
    if (target->type == CT_FLOATING_CON) {
        DLOG("floatingcon, going up even further\n");
        orig_target = target;
        target = target->parent;
    }

    if (con->type == CT_FLOATING_CON) {
        Con *ws = target->con_get_workspace();
        DLOG(fmt::sprintf("This is a floating window, using workspace %p / %s\n",  (void*)ws, ws->name));
        target = ws;
    }

    if (source_output != dest_output) {
        /* Take the relative coordinates of the current output, then add them
         * to the coordinate space of the correct output */
        if (fix_coordinates && con->type == CT_FLOATING_CON) {
            floating_fix_coordinates(con, &(source_output->rect), &(dest_output->rect));
        } else
            DLOG(fmt::sprintf("Not fixing coordinates, fix_coordinates flag = %d\n",  fix_coordinates));
    }

    /* If moving a fullscreen container and the destination already has a
     * fullscreen window on it, un-fullscreen the target's fullscreen con.
     * con->fullscreen_mode is not enough in some edge cases:
     * 1. con is CT_FLOATING_CON, child is fullscreen.
     * 2. con is the parent of a fullscreen container, can be triggered by
     * moving the parent with command criteria.
     */
    Con *fullscreen = target_ws->con_get_fullscreen_con(CF_OUTPUT);
    const bool con_has_fullscreen = con->fullscreen_mode != CF_NONE ||
                                    con->con_get_fullscreen_con(CF_GLOBAL) ||
                                    con->con_get_fullscreen_con(CF_OUTPUT);
    if (con_has_fullscreen && fullscreen != nullptr) {
        con_toggle_fullscreen(fullscreen, CF_OUTPUT);
        fullscreen = nullptr;
    }

    DLOG(fmt::sprintf("Re-attaching container to %p / %s\n",  (void*)target, target->name));
    /* 4: re-attach the con to the parent of this focused container */
    Con *parent = con->parent;
    con->con_detach();
    con->con_attach(target, !behind_focused, behind_focused ? nullptr : orig_target);

    /* 5: fix the percentages */
    if (fix_percentage) {
        parent->con_fix_percent();
        con->percent = 0.0;
        target->con_fix_percent();
    }

    /* 6: focus the con on the target workspace, but only within that
     * workspace, that is, don’t move focus away if the target workspace is
     * invisible.
     * We don’t focus the con for i3 pseudo workspaces like __i3_scratch and
     * we don’t focus when there is a fullscreen con on that workspace. We
     * also don't do it if the caller requested to ignore focus. */
    if (!ignore_focus && !target_ws->con_is_internal() && !fullscreen) {
        /* We need to save the focused workspace on the output in case the
         * new workspace is hidden and it's necessary to immediately switch
         * back to the originally-focused workspace. */
        Con *old_focus_ws = con::first(dest_output->output_get_content()->focus_head);
        Con *old_focus = global.focused;
        con_descend_focused(con)->con_activate();

        if (old_focus_ws == current_ws && old_focus->type != CT_WORKSPACE) {
            /* Restore focus to the currently focused container. */
            old_focus->con_activate();
        } else if (global.focused->con_get_workspace() != old_focus_ws) {
            /* Restore focus if the output's focused workspace has changed. */
            con_descend_focused(old_focus_ws)->con_focus();
        }
    }

    /* 7: when moving to another workspace, we leave the focus on the current
     * workspace. (see also #809) */
    if (!ignore_focus) {
        workspace_show(current_ws);
        if (dont_warp) {
            DLOG("x_set_warp_to(NULL) because dont_warp is set\n");
            x_set_warp_to(nullptr);
        }
    }

    /* Set focus only if con was on current workspace before moving.
     * Otherwise we would give focus to some window on different workspace. */
    if (focus_next)
        con_descend_focused(focus_next)->con_activate();

    /* 8. If anything within the container is associated with a startup sequence,
     * delete it so child windows won't be created on the old workspace. */
    if (!con->con_is_leaf()) {
        for (auto &child : con->nodes_head) {
            if (!child->window)
                continue;
            startup_sequence_delete_by_window(child->window);
        }
    }

    if (con->window) {
        startup_sequence_delete_by_window(con->window);
    }

    /* 9. If the container was marked urgent, move the urgency hint. */
    if (urgent) {
        workspace_update_urgent_flag(source_ws);
        con_set_urgency(con, true);
    }

    /* Ensure the container will be redrawn. */
    delete con->deco_render_params;
    con->deco_render_params = nullptr;

    parent->on_remove_child();

    ipc_send_window_event("move", con);
    ewmh_update_wm_desktop();
    return true;
}

/*
 * Moves the given container to the currently focused container on the given
 * workspace.
 *
 * The fix_coordinates flag will translate the current coordinates (offset from
 * the monitor position basically) to appropriate coordinates on the
 * destination workspace.
 * Not enabling this behaviour comes in handy when this function gets called by
 * floating_maybe_reassign_ws, which will only "move" a floating window when it
 * *already* changed its coordinates to a different output.
 *
 * The dont_warp flag disables pointer warping and will be set when this
 * function is called while dragging a floating window.
 *
 * If ignore_focus is set, the container will be moved without modifying focus
 * at all.
 *
 * TODO: is there a better place for this function?
 *
 */
void con_move_to_workspace(Con *con, Con *workspace, bool fix_coordinates, bool dont_warp, bool ignore_focus) {
    assert(workspace->type == CT_WORKSPACE);

    Con *source_ws = con->con_get_workspace();
    if (workspace == source_ws) {
        DLOG("Not moving, already there\n");
        return;
    }

    Con *target = con_descend_focused(workspace);
    _con_move_to_con(con, target, true, fix_coordinates, dont_warp, ignore_focus, true);
}

/*
 * Moves the given container to the currently focused container on the
 * visible workspace on the given output.
 *
 */
void con_move_to_output(Con *con, Output *output, bool fix_coordinates) {
    auto ws = std::ranges::find_if(output->con->output_get_content()->nodes_head, [](auto &child) { return workspace_is_visible(child); });
    assert(ws != output->con->output_get_content()->nodes_head.end());
    DLOG(fmt::sprintf("Moving con %p to output %s\n",  (void*)con, output->output_primary_name()));
    con_move_to_workspace(con, *ws, fix_coordinates, false, false);
}

/*
 * Returns the orientation of the given container (for stacked containers,
 * vertical orientation is used regardless of the actual orientation of the
 * container).
 *
 */
orientation_t con_orientation(Con *con) {
    switch (con->layout) {
        case L_SPLITV:
        /* stacking containers behave like they are in vertical orientation */
        case L_STACKED:
            return VERT;

        case L_SPLITH:
        /* tabbed containers behave like they are in vertical orientation */
        case L_TABBED:
            return HORIZ;

        case L_DEFAULT:
            ELOG("Someone called con_orientation() on a con with L_DEFAULT, this is a bug in the code.\n");
            assert(false);

        case L_DOCKAREA:
        case L_OUTPUT:
            ELOG(fmt::sprintf("con_orientation() called on dockarea/output (%d) container %p\n",  con->layout, (void*)con));
            assert(false);
    }
    /* should not be reached */
    assert(false);
}

/*
 * Returns the container which will be focused next when the given container
 * is not available anymore. Called in tree_close_internal and con_move_to_workspace
 * to properly restore focus.
 *
 */
Con *con_next_focused(Con *con) {
    /* dock clients cannot be focused, so we focus the workspace instead */
    if (con->parent->type == CT_DOCKAREA) {
        DLOG("selecting workspace for dock client\n");
        return con_descend_focused(con->parent->parent->output_get_content());
    }
    if (con->con_is_floating()) {
        con = con->parent;
    }

    /* if 'con' is not the first entry in the focus stack, use the first one as
     * it’s currently focused already */
    Con *next = con::first(con->parent->focus_head);
    if (next != con) {
        DLOG(fmt::sprintf("Using first entry %p\n",  (void*)next));
    } else {
        /* try to focus the next container on the same level as this one or fall
         * back to its parent */
        if (!(next = con::next(con, con->parent->focus_head))) {
            next = con->parent;
        }
    }

    /* now go down the focus stack as far as
     * possible, excluding the current container */
    while (!next->focus_head.empty() && con::first(next->focus_head) != con) {
        next = con::first(next->focus_head);
    }

    if (con->type == CT_FLOATING_CON && next != con->parent) {
        next = con_descend_focused(next);
    }

    return next;
}

/*
 * Returns the focused con inside this client, descending the tree as far as
 * possible. This comes in handy when attaching a con to a workspace at the
 * currently focused position, for example.
 *
 */
Con *con_descend_focused(Con *con) {
    Con *next = con;
    while (next != global.focused && !next->focus_head.empty())
        next = con::first(next->focus_head);
    return next;
}

/*
 * Returns the focused con inside this client, descending the tree as far as
 * possible. This comes in handy when attaching a con to a workspace at the
 * currently focused position, for example.
 *
 * Works like con_descend_focused but considers only tiling cons.
 *
 */
Con *con_descend_tiling_focused(Con *con) {
    Con *next = con;
    Con *before;
    if (next == global.focused)
        return next;
    do {
        before = next;
        for (auto &child : next->focus_head) {
            if (child->type == CT_FLOATING_CON)
                continue;

            next = child;
            break;
        }
    } while (before != next && next != global.focused);
    return next;
}

/*
 * Returns the leftmost, rightmost, etc. container in sub-tree. For example, if
 * direction is D_LEFT, then we return the rightmost container and if direction
 * is D_RIGHT, we return the leftmost container.  This is because if we are
 * moving D_LEFT, and thus want the rightmost container.
 *
 */
Con *con_descend_direction(Con *con, direction_t direction) {
    Con *most = nullptr;
    int orientation = con_orientation(con);
    DLOG(fmt::sprintf("con_descend_direction(%p, orientation %d, direction %d)\n",  (void*)con, orientation, direction));
    if (direction == D_LEFT || direction == D_RIGHT) {
        if (orientation == HORIZ) {
            /* If the direction is horizontal, we can use either the first
             * (D_RIGHT) or the last con (D_LEFT) */
            if (direction == D_RIGHT)
                most = con::first(con->nodes_head);
            else
                most = con::last(con->nodes_head);
        } else if (orientation == VERT) {
            /* Wrong orientation. We use the last focused con. Within that con,
             * we recurse to chose the left/right con or at least the last
             * focused one. */
            for (auto &current : con->focus_head) {
                if (current->type != CT_FLOATING_CON) {
                    most = current;
                    break;
                }
            }
        } else {
            /* If the con has no orientation set, it’s not a split container
             * but a container with a client window, so stop recursing */
            return con;
        }
    }

    if (direction == D_UP || direction == D_DOWN) {
        if (orientation == VERT) {
            /* If the direction is vertical, we can use either the first
             * (D_DOWN) or the last con (D_UP) */
            if (direction == D_UP)
                most = con::last(con->nodes_head);
            else
                most = con::first(con->nodes_head);
        } else if (orientation == HORIZ) {
            /* Wrong orientation. We use the last focused con. Within that con,
             * we recurse to chose the top/bottom con or at least the last
             * focused one. */
            for (auto &current : con->focus_head) {
                if (current->type != CT_FLOATING_CON) {
                    most = current;
                    break;
                }
            }
        } else {
            /* If the con has no orientation set, it’s not a split container
             * but a container with a client window, so stop recursing */
            return con;
        }
    }

    if (!most)
        return con;
    return con_descend_direction(most, direction);
}

/*
 * Returns a "relative" Rect which contains the amount of pixels that need to
 * be added to the original Rect to get the final position (obviously the
 * amount of pixels for normal, 1pixel and borderless are different).
 *
 */
Rect con_border_style_rect(Con *con) {
    if (config.hide_edge_borders == HEBM_SMART && con->con_get_workspace()->con_num_visible_children() <= 1) {
        if (!con->con_is_floating()) {
            return (Rect){0, 0, 0, 0};
        }
    }

    adjacent_t borders_to_hide = ADJ_NONE;
    int border_width = con->current_border_width;
    DLOG(fmt::sprintf("The border width for con is set to: %d\n",  con->current_border_width));
    Rect result;
    if (con->current_border_width < 0) {
        if (con->con_is_floating()) {
            border_width = config.default_floating_border_width;
        } else {
            border_width = config.default_border_width;
        }
    }
    DLOG(fmt::sprintf("Effective border width is set to: %d\n",  border_width));
    /* Shortcut to avoid calling con_adjacent_borders() on dock containers. */
    int border_style = con_border_style(con);
    if (border_style == BS_NONE)
        return (Rect){0, 0, 0, 0};
    if (border_style == BS_NORMAL) {
        result = (Rect){(uint32_t)border_width, 0, (uint32_t)-(2 * border_width), (uint32_t)-(border_width)};
    } else {
        result = (Rect){(uint32_t)border_width, (uint32_t)border_width, (uint32_t)-(2 * border_width), (uint32_t)-(2 * border_width)};
    }

    borders_to_hide = (adjacent_t)(con_adjacent_borders(con) & config.hide_edge_borders);
    if (borders_to_hide & ADJ_LEFT_SCREEN_EDGE) {
        result.x -= border_width;
        result.width += border_width;
    }
    if (borders_to_hide & ADJ_RIGHT_SCREEN_EDGE) {
        result.width += border_width;
    }
    if (borders_to_hide & ADJ_UPPER_SCREEN_EDGE && (border_style != BS_NORMAL)) {
        result.y -= border_width;
        result.height += border_width;
    }
    if (borders_to_hide & ADJ_LOWER_SCREEN_EDGE) {
        result.height += border_width;
    }
    return result;
}

/*
 * Returns adjacent borders of the window. We need this if hide_edge_borders is
 * enabled.
 */
adjacent_t con_adjacent_borders(Con *con) {
    adjacent_t result = ADJ_NONE;
    /* Floating windows are never adjacent to any other window, so
       don’t hide their border(s). This prevents bug #998. */
    if (con->con_is_floating())
        return result;

    Con *workspace = con->con_get_workspace();
    if (con->rect.x == workspace->rect.x)
        result = (adjacent_t)(result | ADJ_LEFT_SCREEN_EDGE);
    if (con->rect.x + con->rect.width == workspace->rect.x + workspace->rect.width)
        result = (adjacent_t)(result | ADJ_RIGHT_SCREEN_EDGE);
    if (con->rect.y == workspace->rect.y)
        result = (adjacent_t)(result | ADJ_UPPER_SCREEN_EDGE);
    if (con->rect.y + con->rect.height == workspace->rect.y + workspace->rect.height)
        result = (adjacent_t)(result | ADJ_LOWER_SCREEN_EDGE);
    return result;
}

/*
 * Use this function to get a container’s border style. This is important
 * because when inside a stack, the border style is always BS_NORMAL.
 * For tabbed mode, the same applies, with one exception: when the container is
 * borderless and the only element in the tabbed container, the border is not
 * rendered.
 *
 * For children of a CT_DOCKAREA, the border style is always none.
 *
 */
int con_border_style(Con *con) {
    if (con->fullscreen_mode == CF_OUTPUT || con->fullscreen_mode == CF_GLOBAL) {
        DLOG("this one is fullscreen! overriding BS_NONE\n");
        return BS_NONE;
    }

    if (con->parent->layout == L_STACKED)
        return (con->parent->con_num_children() == 1 ? con->border_style : BS_NORMAL);

    if (con->parent->layout == L_TABBED && con->border_style != BS_NORMAL)
        return (con->parent->con_num_children() == 1 ? con->border_style : BS_NORMAL);

    if (con->parent->type == CT_DOCKAREA)
        return BS_NONE;

    return con->border_style;
}

/*
 * Sets the given border style on con, correctly keeping the position/size of a
 * floating window.
 *
 */
void con_set_border_style(Con *con, int border_style, int border_width) {
    /* Handle the simple case: non-floating containerns */
    if (!con->con_is_floating()) {
        con->border_style = (border_style_t)border_style;
        con->current_border_width = border_width;
        return;
    }

    /* For floating containers, we want to keep the position/size of the
     * *window* itself. We first add the border pixels to con->rect to make
     * con->rect represent the absolute position of the window (same for
     * parent). Then, we change the border style and subtract the new border
     * pixels. For the parent, we do the same also for the decoration. */
    DLOG("This is a floating container\n");

    Con *parent = con->parent;
    Rect bsr = con_border_style_rect(con);
    int deco_height = (con->border_style == BS_NORMAL ? render_deco_height() : 0);

    con->rect += bsr;
    parent->rect += bsr;
    parent->rect.y += deco_height;
    parent->rect.height -= deco_height;

    /* Change the border style, get new border/decoration values. */
    con->border_style = (border_style_t)border_style;
    con->current_border_width = border_width;
    bsr = con_border_style_rect(con);
    deco_height = (con->border_style == BS_NORMAL ? render_deco_height() : 0);

    con->rect -= bsr;
    parent->rect -= bsr;
    parent->rect.y -= deco_height;
    parent->rect.height += deco_height;
}

/*
 * This function changes the layout of a given container. Use it to handle
 * special cases like changing a whole workspace to stacked/tabbed (creates a
 * new split container before).
 *
 */
void con_set_layout(Con *con, layout_t layout) {
    DLOG(fmt::sprintf("con_set_layout(%p, %d), con->type = %d\n",
         (void*)con, layout, con->type));

    /* Users can focus workspaces, but not any higher in the hierarchy.
     * Focus on the workspace is a special case, since in every other case, the
     * user means "change the layout of the parent split container". */
    if (con->type != CT_WORKSPACE)
        con = con->parent;

    /* We fill in last_split_layout when switching to a different layout
     * since there are many places in the code that don’t use
     * con_set_layout(). */
    if (con->layout == L_SPLITH || con->layout == L_SPLITV)
        con->last_split_layout = con->layout;

    /* When the container type is CT_WORKSPACE, the user wants to change the
     * whole workspace into stacked/tabbed mode. To do this and still allow
     * intuitive operations (like level-up and then opening a new window), we
     * need to create a new split container. */
    if (con->type == CT_WORKSPACE) {
        if (con->con_num_children() == 0) {
            layout_t ws_layout = (layout == L_STACKED || layout == L_TABBED) ? layout : L_DEFAULT;
            DLOG(fmt::sprintf("Setting workspace_layout to %d\n",  ws_layout));
            con->workspace_layout = ws_layout;
            DLOG(fmt::sprintf("Setting layout to %d\n",  layout));
            con->layout = layout;
        } else if (layout == L_STACKED || layout == L_TABBED || layout == L_SPLITV || layout == L_SPLITH) {
            DLOG("Creating new split container\n");
            /* 1: create a new split container */
            Con *new_con = new Con();
            new_con->parent = con;

            /* 2: Set the requested layout on the split container and mark it as
             * split. */
            new_con->layout = layout;
            new_con->last_split_layout = con->last_split_layout;

            /* 3: move the existing cons of this workspace below the new con */
            auto focus_order = con->get_focus_order();

            DLOG("Moving cons\n");
            Con *child;
            while (!con->nodes_head.empty()) {
                child = con::first(con->nodes_head);
                child->con_detach();
                child->con_attach(new_con, true);
            }

            new_con->set_focus_order(focus_order);

            /* 4: attach the new split container to the workspace */
            DLOG("Attaching new split to ws\n");
            new_con->con_attach(con, false);

            tree_flatten(global.croot);
            con_force_split_parents_redraw(con);
            return;
        }
    }

    if (layout == L_DEFAULT) {
        /* Special case: the layout formerly known as "default" (in combination
         * with an orientation). Since we switched to splith/splitv layouts,
         * using the "default" layout (which "only" should happen when using
         * legacy configs) is using the last split layout (either splith or
         * splitv) in order to still do the same thing. */
        con->layout = con->last_split_layout;
        /* In case last_split_layout was not initialized… */
        if (con->layout == L_DEFAULT)
            con->layout = L_SPLITH;
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
    if (this->type == CT_OUTPUT ||
        this->type == CT_ROOT ||
        this->type == CT_DOCKAREA ||
        (this->parent != nullptr && this->parent->type == CT_OUTPUT)) {
        DLOG(fmt::sprintf("not handling, type = %d, name = %s\n",  this->type, this->name));
        return;
    }

    /* For workspaces, close them only if they're not visible anymore */
    if (this->type == CT_WORKSPACE) {
        if (this->focus_head.empty() && !workspace_is_visible(this)) {
            LOG(fmt::sprintf("Closing old workspace (%p / %s), it is empty\n",  (void*)this, this->name));
            auto gen = ipc_marshal_workspace_event("empty", this, nullptr);
            tree_close_internal(this, DONT_KILL_WINDOW, false);

            auto payload = gen.dump();
            ipc_send_event("workspace", I3_IPC_EVENT_WORKSPACE, payload);
        }
        return;
    }

    con_force_split_parents_redraw(this);
    this->urgent = con_has_urgent_child(this);
    con_update_parents_urgency(this);

    /* TODO: check if this container would swallow any other client and
     * don’t close it automatically. */
    auto children = this->con_num_children();
    if (children == 0) {
        DLOG("Container empty, closing\n");
        tree_close_internal(this, DONT_KILL_WINDOW, false);
        return;
    }
}

/*
 * Returns true if changing the focus to con would be allowed considering
 * the fullscreen focus constraints. Specifically, if a fullscreen container or
 * any of its descendants is focused, this function returns true if and only if
 * focusing con would mean that focus would still be visible on screen, i.e.,
 * the newly focused container would not be obscured by a fullscreen container.
 *
 * In the simplest case, if a fullscreen container or any of its descendants is
 * fullscreen, this functions returns true if con is the fullscreen container
 * itself or any of its descendants, as this means focus wouldn't escape the
 * boundaries of the fullscreen container.
 *
 * In case the fullscreen container is of type CF_OUTPUT, this function returns
 * true if con is on a different workspace, as focus wouldn't be obscured by
 * the fullscreen container that is constrained to a different workspace.
 *
 * Note that this same logic can be applied to moving containers. If a
 * container can be focused under the fullscreen focus constraints, it can also
 * become a parent or sibling to the currently focused container.
 *
 */
bool con_fullscreen_permits_focusing(Con *con) {
    /* No focus, no problem. */
    if (!global.focused)
        return true;

    /* Find the first fullscreen ascendent. */
    Con *fs = global.focused;
    while (fs && fs->fullscreen_mode == CF_NONE)
        fs = fs->parent;

    /* fs must be non-NULL since the workspace con doesn’t have CF_NONE and
     * there always has to be a workspace con in the hierarchy. */
    assert(fs != nullptr);
    /* The most common case is we hit the workspace level. In this
     * situation, changing focus is also harmless. */
    assert(fs->fullscreen_mode != CF_NONE);
    if (fs->type == CT_WORKSPACE)
        return true;

    /* Allow it if the container itself is the fullscreen container. */
    if (con == fs)
        return true;

    /* If fullscreen is per-output, the focus being in a different workspace is
     * sufficient to guarantee that change won't leave fullscreen in bad shape. */
    if (fs->fullscreen_mode == CF_OUTPUT &&
            con->con_get_workspace() != fs->con_get_workspace()) {
        return true;
    }

    /* Allow it only if the container to be focused is contained within the
     * current fullscreen container. */
    return con->con_has_parent(fs);
}

/*
 * Make all parent containers urgent if con is urgent or clear the urgent flag
 * of all parent containers if there are no more urgent children left.
 *
 */
void con_update_parents_urgency(Con *con) {
    Con *parent = con->parent;

    /* Urgency hints should not be set on any container higher up in the
     * hierarchy than the workspace level. Unfortunately, since the content
     * container has type == CT_CON, that’s not easy to verify in the loop
     * below, so we need another condition to catch that case: */
    if (con->type == CT_WORKSPACE)
        return;

    bool new_urgency_value = con->urgent;
    while (parent && parent->type != CT_WORKSPACE && parent->type != CT_DOCKAREA) {
        if (new_urgency_value) {
            parent->urgent = true;
        } else {
            /* We can only reset the urgency when the parent
             * has no other urgent children */
            if (!con_has_urgent_child(parent))
                parent->urgent = false;
        }
        parent = parent->parent;
    }
}

/*
 * Set urgency flag to the container, all the parent containers and the workspace.
 *
 */
void con_set_urgency(Con *con, bool urgent) {
    if (urgent && global.focused == con) {
        DLOG("Ignoring urgency flag for current client\n");
        return;
    }

    const bool old_urgent = con->urgent;

    if (con->urgency_timer == nullptr) {
        con->urgent = urgent;
    } else
        DLOG("Discarding urgency WM_HINT because timer is running\n");

    //CLIENT_LOG(con);
    if (con->window) {
        if (con->urgent) {
            gettimeofday(&con->window->urgent, nullptr);
        } else {
            con->window->urgent.tv_sec = 0;
            con->window->urgent.tv_usec = 0;
        }
    }

    con_update_parents_urgency(con);

    Con *ws;
    /* Set the urgency flag on the workspace, if a workspace could be found
     * (for dock clients, that is not the case). */
    if ((ws = con->con_get_workspace()) != nullptr)
        workspace_update_urgent_flag(ws);

    if (con->urgent != old_urgent) {
        LOG(fmt::sprintf("Urgency flag changed to %d\n",  con->urgent));
        ipc_send_window_event("urgent", con);
    }
}

/*
 * Create a string representing the subtree under con.
 *
 */
std::string con_get_tree_representation(Con *con) {
    /* this code works as follows:
     *  1) create a string with the layout type (D/V/H/T/S) and an opening bracket
     *  2) append the tree representation of the children to the string
     *  3) add closing bracket
     *
     * The recursion ends when we hit a leaf, in which case we return the
     * class_instance of the contained window.
     */

    /* end of recursion */
    if (con->con_is_leaf()) {
        if (!con->window)
            return sstrdup("nowin");

        if (!con->window->class_instance)
            return sstrdup("noinstance");

        return sstrdup(con->window->class_instance);
    }

    std::string buf{};
    /* 1) add the Layout type to buf */
    if (con->layout == L_DEFAULT)
        buf.push_back('D');
    else if (con->layout == L_SPLITV)
        buf.push_back('V');
    else if (con->layout == L_SPLITH)
        buf.push_back('H');
    else if (con->layout == L_TABBED)
        buf.push_back('T');
    else if (con->layout == L_STACKED)
        buf.push_back('S');
    else {
        ELOG("BUG: Code not updated to account for new layout type\n");
        assert(false);
    }
    buf.push_back('[');

    /* 2) append representation of children */
    for (auto &child : con->nodes_head) {
        buf.append((con::first(con->nodes_head) == child ? "" : " "));
        buf.append(con_get_tree_representation(child));
    }

    /* 3) close the brackets */
    buf.push_back(']');
    return buf;
}

/*
 * Returns the container's title considering the current title format.
 *
 */
i3String *con_parse_title_format(Con *con) {
    assert(!con->title_format.empty());

    i3Window *win = con->window;

    std::string title;
    std::string window_class;
    std::string instance;
    std::string machine;
    if (win == nullptr) {
        title = pango_escape_markup(con_get_tree_representation(con));
        window_class = "i3-frame";
        instance = "i3-frame";
        machine = "";
    } else {
        title = pango_escape_markup((win->name == nullptr) ? "" : win->name->get_utf8());
        window_class = pango_escape_markup((win->class_class == nullptr) ? "" : win->class_class);
        instance = pango_escape_markup((win->class_instance == nullptr) ? "" : win->class_instance);
        machine = pango_escape_markup((win->machine == nullptr) ? "" : win->machine);
    }

    std::vector<placeholder_t> placeholders = {
        {.name = "%title", .value = title},
        {.name = "%class", .value = window_class},
        {.name = "%instance", .value = instance},
        {.name = "%machine", .value = machine},
    };

    std::string formatted_str = format_placeholders(con->title_format, placeholders);
    i3String *formatted = new i3String{formatted_str};
    formatted->set_markup(true);

    return formatted;
}

/*
 * Returns container's rect size depending on its orientation.
 * i.e. its width when horizontal, its height when vertical.
 *
 */
uint32_t con_rect_size_in_orientation(Con *con) {
    return (con_orientation(con) == HORIZ ? con->rect.width : con->rect.height);
}
