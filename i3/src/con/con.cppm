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
#include <xcb/xproto.h>
export module i3:con;

import std;
import rect;
import utils;
import :draw;
import :internal;
import :window;

class Output;
class Match;
struct ev_timer;

export {
    /**
     * Stores a width/height pair, used as part of deco_render_params to check
     * whether the rects width/height have changed.
     *
     */
    struct width_height {
        uint32_t w;
        uint32_t h;

        auto operator<=>(const width_height &r) const = default;
    };

    /**
     * Stores the parameters for rendering a window decoration. This structure is
     * cached in every Con and no re-rendering will be done if the parameters have
     * not changed (only the pixmaps will be copied).
     *
     */
    struct deco_render_params {
        Colortriple *color;
        border_style_t border_style;
        width_height con_rect;
        width_height con_window_rect;
        Rect con_deco_rect;
        color_t background;
        layout_t parent_layout;
        bool con_is_leaf;

        auto operator<=>(const deco_render_params &r) const = default;
    };

    class WorkspaceCon;
    class FloatingCon;
    class OutputCon;
    class ConCon;

    /**
     * A 'Con' represents everything from the X11 root window down to a single X11 window.
     *
     * The following things are all stored in a 'Con', from very high level (the
     * biggest Cons) to very small (a single window):
     *
     * 1) X11 root window (as big as all your outputs combined)
     * 2) output (like LVDS1)
     * 3) content container, dockarea containers
     * 4) workspaces
     * 5) split containers
     * ... (you can arbitrarily nest split containers)
     * 6) X11 window containers
     *
     */
    class Con {
      private:

        /**
         * Make all parent containers urgent if con is urgent or clear the urgent flag
         * of all parent containers if there are no more urgent children left.
         *
         */
        virtual void con_update_parents_urgency();

        /* timer used for disabling urgency */
        ev_timer *urgency_timer{};
        std::function<void(Con *)> urgency_timer_cb{};

    public:
        bool mapped{};

        /* Should this container be marked urgent? This gets set when the window
         * inside this container (if any) sets the urgency hint, for example. */
        bool urgent{};

        /** This counter contains the number of UnmapNotify events for this
         * container (or, more precisely, for its ->frame) which should be ignored.
         * UnmapNotify events need to be ignored when they are caused by i3 itself,
         * for example when reparenting or when unmapping the window on a workspace
         * change. */
        uint8_t ignore_unmap{};

        /* The surface used for the frame window. */
        std::unique_ptr<surface_t> frame{};
        std::unique_ptr<surface_t> frame_buffer{};
        bool pixmap_recreated{};

        con_type_t type;

        Con *parent{};

        /* The position and size for this con. These coordinates are absolute. Note
         * that the rect of a container does not include the decoration. */
        Rect rect{};
        /* The position and size of the container's decoration. These coordinates
         * are relative to the container's parent's rect. */
        Rect deco_rect{};

        std::string name{};

        /** The format with which the window's name should be displayed. */
        std::string title_format{};

        /* a sticky-group is an identifier which bundles several containers to a
         * group. The contents are shared between all of them, that is they are
         * displayed on whichever of the containers is currently visible */
        std::string sticky_group{};

        double percent{};

        /* the x11 border pixel attribute */
        int border_width{};
        int current_border_width;

        /** Cache for the decoration rendering */
        std::unique_ptr<deco_render_params> deco_render_params{};

        std::deque<Con *> nodes{};
        std::deque<Con *> focused{};

        std::deque<std::unique_ptr<Match>> swallow{};

        fullscreen_mode_t fullscreen_mode;

        /* Whether this window should stick to the glass. This corresponds to
         * the _NET_WM_STATE_STICKY atom and will only be respected if the
         * window is floating. */
        bool sticky{};

        /* layout is the layout of this container: one of split[v|h], stacked or
         * tabbed. Special containers in the tree (above workspaces) have special
         * layouts like dockarea or output.
         *
         * last_split_layout is one of splitv or splith to support the old "layout
         * default" command which by now should be "layout splitv" or "layout
         * splith" explicitly.
         */
        layout_t layout, last_split_layout;

        border_style_t border_style;
        /* When the border style of a con changes because of motif hints, we don't
         * want to set more decoration that the user wants. The user's preference is determined by these:
         * 1. For new tiling windows, as set by `default_border`
         * 2. For new floating windows, as set by `default_floating_border`
         * 3. For all windows that the user runs the `border` command, whatever is
         * the result of that command for that window. */
        border_style_t max_user_border_style;

        /** floating? (= not in tiling layout) This cannot be simply a bool
         * because we want to keep track of whether the status was set by the
         * application (by setting _NET_WM_WINDOW_TYPE appropriately) or by the
         * user. The user’s choice overwrites automatic mode, of course. The
         * order of the values is important because we check with >=
         * FLOATING_AUTO_ON if a client is floating. */
        con_floating_t floating;

        /* The ID of this container before restarting. Necessary to correctly
         * interpret back-references in the JSON (such as the focus stack). */
        int old_id{};

        /* Depth of the container window */
        uint16_t depth;

        /* The colormap for this con if a custom one is used. */
        xcb_colormap_t colormap{};
        
        void stop_urgency_timer();
        void start_urgency_timer(float after, float repeat, std::function<void(Con*)> cb);

        /** callbacks */
        virtual void on_remove_child();

        /**
         * Sets input focus to the given container. Will be updated in X11 in the next
         * run of x_push_changes().
         *
         */
        void con_focus();

        /**
         * Sets input focus to the given container and raises it to the top.
         *
         */
        void con_activate();

        /**
         * Activates the container like in con_activate but removes fullscreen
         * restrictions and properly warps the pointer if needed.
         *
         */
        void con_activate_unblock();

        /**
         * Returns true when this node is a leaf node (has no children)
         *
         */
        bool con_is_leaf() const;

        /**
         * Returns true when this con is a leaf node with a managed X11 window (e.g.,
         * excluding dock containers)
         */
        virtual bool con_has_managed_window();

        /**
         * Returns true if a container should be considered split.
         *
         */
        bool con_is_split() const;

        /**
         * This will only return true for containers which have some parent with
         * a tabbed / stacked parent of which they are not the currently focused child.
         *
         */
        bool con_is_hidden();

        /**
         * Returns whether the container or any of its children is sticky.
         *
         */
        bool con_is_sticky();

        /**
         * Returns true if this node has regular or floating children.
         *
         */
        virtual bool con_has_children() const;

        /**
         * Returns true if this node accepts a window (if the node swallows windows,
         * it might already have swallowed enough and cannot hold any more).
         *
         */
        virtual bool con_accepts_window();

        /**
         * Gets the output container (first container with CT_OUTPUT in hierarchy) this
         * node is on.
         *
         */
        OutputCon *con_get_output() const;

        /**
         * Gets the workspace container this node is on.
         *
         */
        WorkspaceCon *con_get_workspace() const;

        /**
         * Returns the fullscreen node that covers the given workspace if it exists.
         * This is either a CF_GLOBAL fullscreen container anywhere or a CF_OUTPUT
         * fullscreen container in the workspace.
         *
         */
        Con *con_get_fullscreen_covering_ws();

        /**
         * Returns true if the node is floating.
         *
         */
        bool con_is_floating() const;

        /**
         * Returns true if the container is a docked container.
         *
         */
        bool con_is_docked() const;

        /**
         * Checks if the given container is either floating or inside some floating
         * container. It returns the FLOATING_CON container.
         *
         */
        virtual FloatingCon *con_inside_floating();

        /**
         * Checks if the given container is inside a focused container.
         *
         */
        bool con_inside_focused() const;

        /**
         * Returns the number of children of this container.
         *
         */
        unsigned long con_num_children() const;

        /**
         * Returns the number of visible non-floating children of this container.
         * For example, if the container contains a hsplit which has two children,
         * this will return 2 instead of 1.
         */
        int con_num_visible_children();

        /**
         * Count the number of windows (i.e., leaf containers).
         *
         */
        int con_num_windows();

        /**
         * Iterate over the container's focus stack and return an array with the
         * containers inside it, ordered from higher focus order to lowest.
         *
         */
        std::vector<Con *> get_focus_order();

        /**
         * Clear the container's focus stack and re-add it using the provided container
         * array. The function doesn't check if the provided array contains the same
         * containers with the previous focus stack but will not add floating containers
         * in the new focus stack if container is not a workspace.
         *
         */
        void set_focus_order(std::vector<Con *> focus_order);

        /**
         * Attaches the given container to the given parent. This happens when moving
         * a container or when inserting a new container at a specific place in the
         * tree.
         *
         * ignore_focus is to just insert the Con at the end (useful when creating a
         * new split container *around* some containers, that is, detaching and
         * attaching them in order without wanting to mess with the focus in between).
         *
         */
        virtual void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr);

        /**
         * Detaches the given container from its current parent
         *
         */
        virtual void con_detach();

        /**
         * Closes the given container.
         *
         */
        void con_close(kill_window_t kill_window);

        /**
         * Searches parents of the given 'con' until it reaches one with the specified
         * 'orientation'. Aborts when it comes across a floating_con.
         *
         */
        Con *con_parent_with_orientation(orientation_t orientation);

        /**
         * Returns the first fullscreen node below this node.
         *
         */
        Con *con_get_fullscreen_con(fullscreen_mode_t fullscreen_mode);

        /**
         * Checks if the container has the given parent as an actual parent.
         *
         */
        bool con_has_parent(Con *parent) const;

        void insert_before(Con *con);

        void insert_after(Con *con);

        /**
         * Returns true if the given container (still) exists.
         * This can be used, e.g., to make sure a container hasn't been closed in the meantime.
         *
         */
        bool exists();

        /**
         * Updates the percent attribute of the children of the given container. This
         * function needs to be called when a window is added or removed from a
         * container.
         *
         */
        void con_fix_percent();

        /**
         * This function changes the layout of a given container. Use it to handle
         * special cases like changing a whole workspace to stacked/tabbed (creates a
         * new split container before).
         *
         */
        virtual void con_set_layout(layout_t layout);

        /**
         * Set urgency flag to the container, all the parent containers and the workspace.
         *
         */
        void con_set_urgency(bool urgent);
        
        virtual i3Window* get_window() {
            return nullptr;
        }
        
        virtual Rect& get_geometry() {
            static Rect r{};
            return r;
        }
        
        virtual Rect& get_window_rect() {
            static Rect r{};
            return r;
        }
        
        virtual int get_window_icon_padding() {
            return -1;
        }
        
        Con() = delete;

        /**
         * Frees the specified container.
         *
         */
        virtual ~Con();

    protected:
        /**
         * Create a new container (and attach it to the given parent, if not NULL).
         * This function only initializes the data structures.
         *
         */
        explicit Con(bool skeleton);

    };

    class RootCon : public Con {
       public:
        RootCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_ROOT;
            this->name = "root";
        }

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of root");
        }
        
        void on_remove_child() override;
    };

    class OutputCon : public Con {
       public:
        OutputCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_OUTPUT;
            this->layout = L_OUTPUT;
        }

        /**
         * Returns the output container below the given output container.
         *
         */
        ConCon *output_get_content();

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of output");
        }

        void on_remove_child() override;
    };

    class ConCon : public Con {
    private:
        /** The position and size of the actual client window. These coordinates are
         * relative to the container's rect. */
        Rect window_rect{};
        /** the geometry this window requested when getting mapped */
        Rect geometry{};
        /** Whether the window icon should be displayed, and with what padding. -1
         * means display no window icon (default behavior), 0 means display without
         * any padding, 1 means display with 1 pixel of padding and so on. */
        int window_icon_padding{-1};
        std::unique_ptr<i3Window> window{};

    public:

        bool con_has_managed_window() override;
        bool con_accepts_window() override;
        int get_window_icon_padding() override;
        void set_window_icon_padding(int padding);
        i3Window* get_window() override;
        i3Window* release_window();
        void set_window(i3Window* _window);
        void set_geometry(Rect _geometry);
        Rect& get_geometry() override;
        void set_window_rect(Rect _window_rect);
        Rect& get_window_rect() override;

        ConCon();
        explicit ConCon(i3Window *window);
        ConCon(i3Window *window, bool skeleton);
    };

    /* Wrap a floating ConCon in a FloatingCon. */
    class FloatingCon : public Con {
       public:
        FloatingCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_FLOATING_CON;
        }
        void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr) override;
        void con_detach() override;
        FloatingCon *con_inside_floating() override;
    };

    class DockCon : public Con {
       public:
        DockCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_DOCKAREA;
            this->layout = L_DOCKAREA;
        }
        
        FloatingCon *con_inside_floating() override;

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of dockarea");
        }

        void on_remove_child() override;
    };

    class WorkspaceCon : public Con {
       private:
        void con_update_parents_urgency() override;
       public:
        /* Only workspace-containers can have floating clients */
        std::deque<FloatingCon *> floating_windows{};
        /* workspace_layout is only for type == CT_WORKSPACE cons. When you change
         * the layout of a workspace without any children, i3 cannot just set the
         * layout (because workspaces need to be splitv/splith to allow focus
         * parent and opening new containers). Instead, it stores the requested
         * layout in workspace_layout and creates a new split container with that
         * layout whenever a new container is attached to the workspace. */
        layout_t workspace_layout;

        /** Only applicable for containers of type CT_WORKSPACE. */
        gaps_t gaps;

        /** the workspace number, if this Con is of type CT_WORKSPACE and the
         * workspace is not a named workspace (for named workspaces, num == -1) */
        int num{};

        WorkspaceCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_WORKSPACE;
            this->workspace_layout = L_DEFAULT;
        }
        
        void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr) override;
        
        FloatingCon *con_inside_floating() override;

        void con_set_layout(layout_t layout) override;
        
        void on_remove_child() override;

        bool con_has_children() const override;
    };

    namespace con {
        Con *first(std::deque<Con *> &queue);

        Con *previous(Con *con, std::deque<Con *> &queue);

        Con *next(Con *con, std::deque<Con *> &queue);

        Con *last(std::deque<Con *> &queue);
    }  // namespace con

    /**
     * Returns the container with the given client window ID or NULL if no such
     * container exists.
     *
     */
    ConCon *con_by_window_id(xcb_window_t window);

    /**
     * Returns the container with the given frame ID or NULL if no such container
     * exists.
     *
     */
    Con *con_by_frame_id(xcb_window_t frame);


    /**
     * Start from a container and traverse the transient_for linked list. Returns
     * true if target window is found in the list. Protects againsts potential
     * cycles.
     *
     */
    bool con_find_transient_for_window(Con *start, xcb_window_t target);


    /**
     * Returns the first container below 'con' which wants to swallow this window
     * TODO: priority
     *
     */
    Con *con_for_window(Con const * con, i3Window const * window, Match * *store_match);

    /**
     * Toggles fullscreen mode for the given container. Fullscreen mode will not be
     * entered when there already is a fullscreen container on this workspace.
     *
     */
    void con_toggle_fullscreen(Con * con, int fullscreen_mode);

    /**
     * Enables fullscreen mode for the given container, if necessary.
     *
     */
    void con_enable_fullscreen(Con * con, fullscreen_mode_t fullscreen_mode);

    /**
     * Disables fullscreen mode for the given container, if necessary.
     *
     */
    void con_disable_fullscreen(Con * con);

    /**
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
    void con_move_to_workspace(Con * con, WorkspaceCon * workspace, bool fix_coordinates,
                               bool dont_warp, bool ignore_focus);

    /**
     * Moves the given container to the currently focused container on the
     * visible workspace on the given output.
     *
     */
    void con_move_to_output(Con * con, Output * output, bool fix_coordinates);

    bool con_move_to_target(Con *con, Con *target);

    /**
     * Returns the orientation of the given container (for stacked containers,
     * vertical orientation is used regardless of the actual orientation of the
     * container).
     *
     */
    orientation_t con_orientation(Con const * const con);

    /**
     * Returns the container which will be focused next when the given container
     * is not available anymore. Called in tree_close_internal and con_move_to_workspace
     * to properly restore focus.
     *
     */
    Con *con_next_focused(Con * con);

    /**
     * Returns the focused con inside this client, descending the tree as far as
     * possible. This comes in handy when attaching a con to a workspace at the
     * currently focused position, for example.
     *
     */
    Con *con_descend_focused(Con * con);

    /**
     * Returns the focused con inside this client, descending the tree as far as
     * possible. This comes in handy when attaching a con to a workspace at the
     * currently focused position, for example.
     *
     * Works like con_descend_focused but considers only tiling cons.
     *
     */
    Con *con_descend_tiling_focused(Con * con);

    /**
     * Returns the leftmost, rightmost, etc. container in sub-tree. For example, if
     * direction is D_LEFT, then we return the rightmost container and if direction
     * is D_RIGHT, we return the leftmost container.  This is because if we are
     * moving D_LEFT, and thus want the rightmost container.
     */
    Con *con_descend_direction(Con * con, direction_t direction);

    /**
     * Returns whether the window decoration (title bar) should be drawn into the
     * X11 frame window of this container (default) or into the X11 frame window of
     * the parent container (for stacked/tabbed containers).
     *
     */
    bool con_draw_decoration_into_frame(Con const * const con);

    /**
     * Returns a "relative" Rect which contains the amount of pixels that need to
     * be added to the original Rect to get the final position (obviously the
     * amount of pixels for normal, 1pixel and borderless are different).
     *
     */
    Rect con_border_style_rect(Con const * const con);

    /**
     * Returns adjacent borders of the window. We need this if hide_edge_borders is
     * enabled.
     */
    adjacent_t con_adjacent_borders(Con const * const con);

    /**
     * Use this function to get a container’s border style. This is important
     * because when inside a stack, the border style is always BS_NORMAL.
     * For tabbed mode, the same applies, with one exception: when the container is
     * borderless and the only element in the tabbed container, the border is not
     * rendered.
     *
     * For children of a CT_DOCKAREA, the border style is always none.
     *
     */
    border_style_t con_border_style(Con const * const con);

    /**
     * Sets the given border style on con, correctly keeping the position/size of a
     * floating window.
     *
     */
    void con_set_border_style(Con *con, border_style_t border_style, int border_width);

    /**
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
    bool con_fullscreen_permits_focusing(Con * con);

    /**
     * Create a string representing the subtree under con.
     *
     */
    std::string con_get_tree_representation(Con * con);

    /**
     * force parent split containers to be redrawn
     *
     */
    void con_force_split_parents_redraw(Con * con);

    /**
     * Returns the window title considering the current title format.
     *
     */
    std::string con_parse_title_format(Con * con);

    /**
     * Returns given container's rect size depending on its orientation.
     * i.e. its width when horizontal, its height when vertical.
     *
     */
    uint32_t con_rect_size_in_orientation(Con * con);

    /**
     * Returns true if the container is within any stacked/tabbed split container.
     *
     */
    bool con_inside_stacked_or_tabbed(Con *con);
}