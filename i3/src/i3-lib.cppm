module;
#include <xcb/xproto.h>
#include <sys/resource.h>
export module i3;

import std;

export import :assignments;
export import :autostarts;
export import :bindings;
export import :configuration;
export import :con;
export import :util;
export import :x;
export import :workspace;
export import :keysyms;
export import :event_handler;
export import :shape;
export import :xkb;
export import :handlers;
export import :criteria_state;
export import :resize;
export import :window;
export import :match;
export import :startup;
export import :draw;
export import :gaps;
export import :sync;
export import :output;
export import :render;
export import :randr;
export import :font;
export import :xcb;
export import :drag;
export import :dpi;
export import :get_visualtype;
export import :ewmh;
export import :floating;
export import :move;
export import :sighandler;
export import :ipc;
export import :load_layout;
export import :restore_layout;
export import :root_atom_contents;
export import :nagbar;
export import :manage;
export import :tree;
export import :tiling_drag;
export import :commands;
export import :is_background_set;
export import :resource_database;
export import :config_applier;
export import :commands_applier;
export import :internal;
export import :handle_property;
export import :atoms;

export namespace i3 {

}

export class Global {

public:
    Global() = default;
    Global(Global const &) = delete;
    Global &operator=(Global const &) = delete;

    std::string current_socketpath{};

    Con *focused;
    RootCon *croot;

    std::deque<Con *> all_cons{};

    std::vector<std::string> start_argv{};

    int xkb_current_group;

    pid_t config_error_nagbar_pid = -1;
    pid_t command_error_nagbar_pid = -1;

    std::optional<Keysyms> keysyms{};
    std::optional<Keymap> keymap{};
    X *x;
    RandR *randr;
    Shape *shape;
    Xkb *xkb;
    ConfigurationManager *configManager;

    /* The original value of RLIMIT_CORE when i3 was started. We need to restore
     * this before starting any other process, since we set RLIMIT_CORE to
     * RLIM_INFINITY for i3 debugging versions. */
    rlimit original_rlimit_core;

    /* The list of assignments */
    AssignmentManager *assignmentManager;

    WorkspaceManager *workspaceManager;

    /* The last timestamp we got from X11 (timestamps are included in some events
     * and are used for some things, like determining a unique ID in startup
     * notification). */
    xcb_timestamp_t last_timestamp = XCB_CURRENT_TIME;

    bool new_parser;
    PropertyHandlers *handlers;
    EventHandler *eventHandler;

    static Global& instance() {
        static Global inst{};

        return inst;
    }
};

export Global global{};