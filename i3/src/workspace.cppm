/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * workspace.c: Modifying workspaces, accessing them, moving containers to
 *              workspaces.
 *
 */
module;

#include <config.h>

export module i3:workspace;

import std;
import :internal;

using std::uint32_t;

class Output;
class OutputCon;
class WorkspaceCon;
class Con;
class ConfigurationManager;

export {
    /* We use NET_WM_DESKTOP_NONE for cases where we cannot determine the EWMH
     * desktop index for a window. We cannot use a negative value like -1 since we
     * need to use uint32_t as we actually need the full range of it. This is
     * technically dangerous, but it's safe to assume that we will never have more
     * than 4294967279 workspaces open at a time. */
    const uint32_t NET_WM_DESKTOP_NONE = 0xFFFFFFF0;
    const uint32_t NET_WM_DESKTOP_ALL = 0xFFFFFFFF;

    /**
     * Stores which workspace (by name or number) goes to which output and its gaps config.
     *
     */
    class WorkspaceConfig {
       public:
        std::string name{};
        std::string output{};
        gaps_t gaps;
        gaps_mask_t gaps_mask;
        
        auto operator<=>(WorkspaceConfig const&) const = default;
        WorkspaceConfig& operator=(WorkspaceConfig const&) = default;
        
        explicit WorkspaceConfig(std::string name) : name(std::move(name)) {
        }
        
        WorkspaceConfig(std::string name, std::string output) : name(std::move(name)), output(std::move(output)) {
        }
        
        WorkspaceConfig(WorkspaceConfig const &) = default;
        WorkspaceConfig(WorkspaceConfig &&) = default;
    };

   
    // TODO: I feel this should reside with the configuration
    class WorkspaceManager {
    private:

      ConfigurationManager &configManager;
        /* The list of workspace assignments (which workspace should end up on which
         * output) */
        std::map<std::string, WorkspaceConfig> ws_assignments{};

    public:
        /**
         * Returns the first output that is assigned to a workspace specified by the
         * given name or number. Returns NULL if no such output exists.
         *
         * If an assignment matches by number but there is an assignment later that
         * matches by name, the second one is preferred.
         * The order of the 'ws_assignments' queue is respected: if multiple
         * assignments match the criteria, the first one is returned.
         * 'name' is ignored when NULL, 'parsed_num' is ignored when it is -1.
         *
         */
        Output* get_assigned_output(std::string const name, long const parsed_num);
        Output* get_assigned_output(long const parsed_num);
        Output* get_assigned_output(std::string const name);

        /**
         * Stores a copy of the name of the last used workspace for the workspace
         * back-and-forth switching.
         *
         */
        std::string previous_workspace_name{};
        
        std::optional<WorkspaceConfig> get_workspace_config(const std::string &name);

        std::optional<WorkspaceConfig> get_workspace_config(WorkspaceCon const *ws);
        
        void add_workspace_config(WorkspaceConfig &&config) {
            auto name = config.name;
            ws_assignments.insert_or_assign(name, std::forward<WorkspaceConfig>(config));
        }
        
        void clear() {
            ws_assignments.clear();
        }
        
        std::vector<WorkspaceConfig> all_workspace_configs() {
            std::vector<WorkspaceConfig> config{};
            config.reserve(ws_assignments.size());
            for (auto &c : ws_assignments) {
                config.push_back(c.second);
            }
            return config;
        }

        /**
         * Returns workspace assignments which would be triggered for the output.
         */
        std::vector<WorkspaceConfig> configs_for_output(Output * output);

        WorkspaceCon *workspace_get_or_create(const std::string &num);
        WorkspaceCon *create_workspace_on_output(Output *output, Con *content);
        
        /**
         * Move the given workspace to the specified output.
         *
         */
        void workspace_move_to_output(WorkspaceCon * ws, Output * output);
        
        /**
         * Returns the previously focused workspace con, or NULL if unavailable.
         *
         */
        WorkspaceCon *workspace_back_and_forth_get();
        
        /**
         * Focuses the previously focused workspace.
         *
         */
        void workspace_back_and_forth();
        
        /**
         * Looks up the workspace by name and switches to it.
         *
         */
        void workspace_show_by_name(const std::string &num);
        
        /**
         * Switches to the given workspace
         *
         */
        void workspace_show(WorkspaceCon * ws);

        WorkspaceManager(ConfigurationManager &configManager) : configManager(configManager) {}
    };

    /**
     * Returns the workspace with the given name or NULL if such a workspace does
     * not exist.
     *
     */
    WorkspaceCon *get_existing_workspace_by_name(const std::string &name);

    /**
     * Returns the workspace with the given number or NULL if such a workspace does
     * not exist.
     *
     */
    WorkspaceCon *get_existing_workspace_by_num(int num);

    /**
     * Returns a pointer to the workspace with the given number (starting at 0),
     * creating the workspace if necessary (by allocating the necessary amount of
     * memory and initializing the data structures correctly).
     *
     */
    WorkspaceCon *workspace_get_or_create(const std::string &num);

    /**
     * Returns a pointer to a new workspace in the given output. The workspace
     * is created attached to the tree hierarchy through the given content
     * container.
     *
     */
    WorkspaceCon *create_workspace_on_output(Output * output, Con * content);

    /**
     * Returns true if the workspace is currently visible. Especially important for
     * multi-monitor environments, as they can have multiple currently active
     * workspaces.
     *
     */
    bool workspace_is_visible(Con * ws);

    /**
     * Returns the next workspace.
     *
     */
    WorkspaceCon *workspace_next();

    /**
     * Returns the previous workspace.
     *
     */
    WorkspaceCon *workspace_prev();

    /**
     * Returns the next workspace on the same output
     *
     */
    WorkspaceCon *workspace_next_on_output();

    /**
     * Returns the previous workspace on the same output
     *
     */
    WorkspaceCon *workspace_prev_on_output();

    /**
     * Goes through all clients on the given workspace and updates the workspace’s
     * urgent flag accordingly.
     *
     */
    void workspace_update_urgent_flag(WorkspaceCon * ws);

    /**
     * 'Forces' workspace orientation by moving all cons into a new split-con with
     * the same orientation as the workspace and then changing the workspace
     * orientation.
     *
     */
    void ws_force_orientation(Con * ws, orientation_t orientation);

    /**
     * Called when a new con (with a window, not an empty or split con) should be
     * attached to the workspace (for example when managing a new window or when
     * moving an existing window to the workspace level).
     *
     * Depending on the workspace_layout setting, this function either returns the
     * workspace itself (default layout) or creates a new stacked/tabbed con and
     * returns that.
     *
     */
    Con *workspace_attach_to(WorkspaceCon * ws);

    /**
     * Creates a new container and re-parents all of children from the given
     * workspace into it.
     *
     * The container inherits the layout from the workspace.
     */
    Con *workspace_encapsulate(Con * ws);

}