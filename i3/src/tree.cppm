/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * tree.c: Everything that primarily modifies the layout tree data structure.
 *
 */
module;
#include <xcb/xcb.h>
export module i3:tree;

import std;
import :internal;

class Con;
class ConCon;
class i3Window;
class X;

export {
    
    class TreeManager {
        X &x;
      public:
        TreeManager(X &x);
    };

    /**
     * Initializes the Tree by creating the root node, adding all RandR outputs
     * to the Tree (that means randr_init() has to be called before) and
     * assigning a workspace to each RandR output.
     *
     */
    void tree_init(const xcb_get_geometry_reply_t *geometry);

    /**
     * Opens an empty container in the current container
     *
     */
    ConCon *tree_open_con(Con * con, i3Window * window = nullptr);

    /**
     * Splits (horizontally or vertically) the given container by creating a new
     * container which contains the old one and the future ones.
     *
     */
    void tree_split(Con * con, orientation_t orientation);

    /**
     * Moves focus one level up. Returns true if focus changed.
     *
     */
    bool level_up();

    /**
     * Moves focus one level down. Returns true if focus changed.
     *
     */
    bool level_down();

    /**
     * Renders the Tree, that is rendering all outputs using render_con() and
     * pushing the changes to X11 using x_push_changes().
     *
     */
    void tree_render();

    /**
     * Changes focus in the given direction
     *
     */
    void tree_next(Con * con, direction_t direction);

    /**
     * Get the previous / next sibling
     *
     */
    Con *get_tree_next_sibling(Con * con, position_t direction);

    /**
     * Closes the given container including all children.
     * Returns true if the container was killed or false if just WM_DELETE was sent
     * and the window is expected to kill itself.
     *
     * The dont_kill_parent flag is specified when the function calls itself
     * recursively while deleting a containers children.
     *
     */
    bool tree_close_internal(Con * con, kill_window_t kill_window, bool dont_kill_parent);

    /**
     * Loads Tree from ~/.i3/_restart.json (used for in-place restarts).
     *
     */
    bool tree_restore(const std::filesystem::path path, const xcb_get_geometry_reply_t *geometry);

    /**
     * tree_flatten() removes pairs of redundant split containers, e.g.:
     *       [workspace, horizontal]
     *   [v-split]           [child3]
     *   [h-split]
     * [child1] [child2]
     * In this example, the v-split and h-split container are redundant.
     * Such a situation can be created by moving containers in a direction which is
     * not the orientation of their parent container. i3 needs to create a new
     * split container then and if you move containers this way multiple times,
     * redundant chains of split-containers can be the result.
     *
     */
    void tree_flatten(Con * child);
}