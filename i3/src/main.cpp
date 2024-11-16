/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * main.c: Initialization, main loop
 *
 */
struct criteria_state;
#include <spdlog/spdlog.h>
#include <config.h>

#include <err.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_keysyms.h>

#include <ev.h>

#include <config.h>

#include <sys/time.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>
#undef SN_API_NOT_YET_FROZEN

#include <fcntl.h>
#include <libgen.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <xpp/xpp.hpp>
#include <xpp/proto/randr.hpp>
#include <xpp/proto/xkb.hpp>
#include <xpp/proto/shape.hpp>
#include <xpp/proto/bigreq.hpp>
#include <xcb/xproto.h>
#include <xcb/xcb_atom.h>
#include <boost/di.hpp>

#ifdef I3_ASAN_ENABLED
#include <sanitizer/lsan_interface.h>
#endif

import std;

import i3ipc;
import i3;
import utils;
import log;
import program_arguments;
import i3_config_base;
import i3ipc;

/* The number of file descriptors passed via socket activation. */
int listen_fds;

/*
 * Returns true if this version of i3 is a debug build (anything which is not a
 * release version), based on the git version number.
 *
 */
static bool is_debug_build() {
    /* i3_version contains either something like this:
     *     "4.0.2 (2011-11-11)" (release version)
     * or: "4.0.2-123-gC0FFEE"  (debug version)
     *
     * So we check for the offset of the first opening round bracket to
     * determine whether this is a git version or a release version. */
    if (strchr(I3_VERSION, '(') == nullptr) {
        return true;  // e.g. 4.0.2-123-gC0FFEE
    }
    /* In practice, debug versions do not contain parentheses at all,
     * but leave the logic as it was before so that we can re-add
     * parentheses if we chose to. */
    return ((strchr(I3_VERSION, '(') - I3_VERSION) > 10);
}

/*
 * Exit handler which destroys the main_loop. Will trigger cleanup handlers.
 *
 */
static void i3_exit() {
    global.ipcManager->ipc_shutdown(shutdown_reason_t::SHUTDOWN_REASON_EXIT);
    if (global.configManager->config->ipc_socket_path) {
        unlink(global.configManager->config->ipc_socket_path->c_str());
    }
    xcb_disconnect(**global.x);

    /* If a nagbar is active, kill it */
    kill_nagbar(global.config_error_nagbar_pid, false);
    kill_nagbar(global.command_error_nagbar_pid, false);

#ifdef I3_ASAN_ENABLED
    __lsan_do_leak_check();
#endif
}

static int parse_restart_fd() {
    char const *restart_fd = getenv("_I3_RESTART_FD");
    if (restart_fd == nullptr) {
        return -1;
    }

    long int fd = -1;
    if (!utils::parse_long(restart_fd, &fd, 10)) {
        ELOG(fmt::sprintf("Malformed _I3_RESTART_FD \"%s\"\n", restart_fd));
        return -1;
    }
    return static_cast<int>(fd);
}

void handle_extra_args(int argc, char *argv[]) {
    /* We enable verbose mode so that the user knows what’s going on.
     * This should make it easier to find mistakes when the user passes
     * arguments by mistake. */
    set_verbosity(true);

    DLOG("Additional arguments passed. Sending them as a command to i3.\n");
    std::string payload;
    while (optind < argc) {
        payload.append(argv[optind]).append(" ");
        optind++;
    }
    DLOG(fmt::sprintf("Command is: %s (%zd bytes)\n", payload, payload.length()));
    std::optional<std::string> socket_path = root_atom_contents("I3_SOCKET_PATH", nullptr, 0);
    if (!socket_path) {
        ELOG("Could not get i3 IPC socket path\n");
        exit(1);
    }

    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd == -1) {
        err(EXIT_FAILURE, "Could not create socket");
    }

    struct sockaddr_un addr{};
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, socket_path->c_str(), sizeof(addr.sun_path) - 1);
    if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
        err(EXIT_FAILURE, "Could not connect to i3");
    }

    if (i3ipc::ipc_send_message(sockfd, strlen(payload.c_str()), i3ipc::MESSAGE_TYPE::RUN_COMMAND,
                                (uint8_t *)payload.c_str()) == -1) {
        err(EXIT_FAILURE, "IPC: write()");
    }

    uint32_t reply_length;
    uint32_t reply_type;
    uint8_t *reply;
    int ret;
    if ((ret = i3ipc::ipc_recv_message(sockfd, &reply_type, &reply_length, &reply)) != 0) {
        if (ret == -1) {
            err(EXIT_FAILURE, "IPC: read()");
        }
        exit(1);
    }
    if (reply_type != std::to_underlying(i3ipc::REPLY_TYPE::COMMAND)) {
        errx(EXIT_FAILURE, "IPC: received reply of type %d but expected %d (COMMAND)", reply_type, std::to_underlying(i3ipc::REPLY_TYPE::COMMAND));
    }

    printf("%.*s\n", reply_length, reply);
    if (reply != nullptr) {
        free(reply);
    }
}

void enable_coredumps() {
    LOG("CORE DUMPS: You are running a development version of i3, so coredumps were automatically enabled (ulimit -c unlimited).\n");
    struct rlimit limit = {RLIM_INFINITY, RLIM_INFINITY};
    setrlimit(RLIMIT_CORE, &limit);
}

void do_tree_init(program_arguments const &args, xcb_get_geometry_reply_t const *greply) {
    bool needs_tree_init = true;
    if (args.layout_path) {
        LOG(fmt::format("Trying to restore the layout from \"{}\".", args.layout_path->native()));
        needs_tree_init = !tree_restore(*args.layout_path, greply);
        if (args.delete_layout_path) {
            std::filesystem::remove(*args.layout_path);
            auto dir = args.layout_path->parent_path();
            /* possibly fails with ENOTEMPTY if there are files (or
             * sockets) left. */
            std::error_code ec{};
            std::filesystem::remove(dir, ec);
        }
    }
    if (needs_tree_init) {
        tree_init(greply);
    }
}

static void set_screenshot_as_wallpaper(x_connection *conn, xcb_screen_t *screen) {
    uint16_t width = screen->width_in_pixels;
    uint16_t height = screen->height_in_pixels;
    xcb_pixmap_t pixmap = conn->generate_id();
    xcb_gcontext_t gc = conn->generate_id();

    conn->create_pixmap(screen->root_depth, pixmap, screen->root, width, height);

    uint32_t list[]{XCB_GX_COPY, (uint32_t)~0, XCB_FILL_STYLE_SOLID, XCB_SUBWINDOW_MODE_INCLUDE_INFERIORS};
    conn->create_gc(gc, screen->root,
                    XCB_GC_FUNCTION | XCB_GC_PLANE_MASK | XCB_GC_FILL_STYLE | XCB_GC_SUBWINDOW_MODE,
                    list);

    conn->copy_area(screen->root, pixmap, gc, 0, 0, 0, 0, width, height);
    uint32_t value_list[]{pixmap};
    conn->change_window_attributes(screen->root, XCB_CW_BACK_PIXMAP, value_list);
    conn->free_gc(gc);
    conn->free_pixmap(pixmap);
    conn->flush();
}

static void force_disable_output(RandR &randr, program_arguments const &args) {
    if (args.layout_path && randr.randr_base > -1) {
        for (auto &con : global.croot->nodes) {
            for (Output *output : randr.outputs) {
                if (output->active || con->name != output->output_primary_name()) {
                    continue;
                }

                /* This will correctly correlate the output with its content
                 * container. We need to make the connection to properly
                 * disable the output. */
                if (output->con == nullptr) {
                    output_init_con(output);
                    output->changed = false;
                }

                output->to_be_disabled = true;
                randr.randr_disable_output(output);
            }
        }
    }
}

static Output *get_focused_output() {
    Output *output{nullptr};

    auto pointerreply = xpp::x::query_pointer(**global.x, global.x->root);
    if (pointerreply.get() == nullptr) {
        ELOG("Could not query pointer position, using first screen\n");
    } else {
        DLOG(fmt::sprintf("Pointer at %d, %d\n", pointerreply->root_x, pointerreply->root_y));
        output = global.randr->get_output_containing(pointerreply->root_x, pointerreply->root_y);
        if (!output) {
            ELOG(fmt::sprintf("ERROR: No screen at (%d, %d), starting on the first screen\n",
                              pointerreply->root_x, pointerreply->root_y));
        }
    }
    if (!output) {
        output = global.randr->get_first_output();
    }
    return output;
}

static void confirm_restart(IPCManager &ipcManager) {
    int const restart_fd = parse_restart_fd();
    if (restart_fd != -1) {
        DLOG(fmt::sprintf("serving restart fd %d", restart_fd));
        ipc_client *client = ipcManager.ipc_new_client_on_fd(restart_fd);
        ipc_confirm_restart(client);
        unsetenv("_I3_RESTART_FD");
    }
}

/* What follows is a fugly consequence of X11 protocol race conditions like
 * the following: In an i3 in-place restart, i3 will reparent all windows
 * to the root window, then exec() itself. In the new process, it calls
 * manage_existing_windows. However, in case any application sent a
 * generated UnmapNotify message to the WM (as GIMP does), this message
 * will be handled by i3 *after* managing the window, thus i3 thinks the
 * window just closed itself. In reality, the message was sent in the time
 * period where i3 wasn’t running yet.
 *
 * To prevent this, we grab the server (disables processing of any other
 * connections), then discard all pending events (since we didn’t do
 * anything, there cannot be any meaningful responses), then ungrab the
 * server. */
static void ignore_restart_events(x_connection *conn) {
    conn->grab_server();
    {
        xcb_aux_sync(*conn);
        std::shared_ptr<xcb_generic_event_t> event;
        while ((event = conn->poll_for_event()) != nullptr) {
            if (event->response_type == 0) {
                continue;
            }

            /* Strip off the highest bit (set if the event is generated) */
            int type = (event->response_type & 0x7F);

            /* We still need to handle MapRequests which are sent in the
             * timespan starting from when we register as a window manager and
             * this piece of code which drops events. */
            if (type == XCB_MAP_REQUEST) {
                global.handlers->handle_event(type, event.get());
            }
        }
        manage_existing_windows(global.x->root);
    }
    conn->ungrab_server();
}

static void fix_empty_background(x_connection *conn) {
    /* When the root's window background is set to NONE, that might mean
     * that old content stays visible when a window is closed. That has
     * unpleasant effect of "my terminal (does not seem to) close!".
     *
     * There does not seem to be an easy way to query for this problem, so
     * we test for it: Open & close a window and check if the background is
     * redrawn or the window contents stay visible.
     */
    LOG("This is not an in-place restart, checking if a wallpaper is set.\n");

    xcb_screen_t *s = xcb_aux_get_screen(*conn, global.x->conn->default_screen());
    if (is_background_set(conn, s)) {
        LOG("A wallpaper is set, so no screenshot is necessary.\n");
    } else {
        LOG("No wallpaper set, copying root window contents to a pixmap\n");
        set_screenshot_as_wallpaper(conn, s);
    }
}

/* Acquire the WM_Sn selection. */
static void aquire_selection(X &x, xcb_timestamp_t last_timestamp) {
    /* Get the WM_Sn atom */
    char *atom_name = xcb_atom_name_by_screen("WM", x.conn_screen);
    x.wm_sn_selection_owner = xcb_generate_id(*x);

    if (atom_name == nullptr) {
        throw std::runtime_error(fmt::sprintf("xcb_atom_name_by_screen(\"WM\", %d) failed, exiting\n", x.conn_screen));
    }

    xcb_intern_atom_reply_t *atom_reply;
    atom_reply = xcb_intern_atom_reply(*x,
                                       xcb_intern_atom_unchecked(*x,
                                                                 0,
                                                                 strlen(atom_name),
                                                                 atom_name),
                                       nullptr);
    free(atom_name);
    if (atom_reply == nullptr) {
        throw std::runtime_error("Failed to intern the WM_Sn atom, exiting\n");
    }
    x.wm_sn = atom_reply->atom;
    free(atom_reply);

    /* Check if the selection is already owned */
    xcb_get_selection_owner_reply_t *selection_reply =
        xcb_get_selection_owner_reply(*x,
                                      xcb_get_selection_owner(*x, x.wm_sn),
                                      nullptr);
    if (selection_reply && selection_reply->owner != XCB_NONE && !global.args->replace_wm) {
        throw std::runtime_error("Another window manager is already running (WM_Sn is owned)");
    }

    /* Become the selection owner */
    xcb_create_window(*x,
                      x.root_screen->root_depth,
                      x.wm_sn_selection_owner, /* window id */
                      x.root_screen->root,     /* parent */
                      -1, -1, 1, 1,            /* geometry */
                      0,                       /* border width */
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      x.root_screen->root_visual,
                      0, nullptr);
    xcb_change_property(*x,
                        XCB_PROP_MODE_REPLACE,
                        x.wm_sn_selection_owner,
                        XCB_ATOM_WM_CLASS,
                        XCB_ATOM_STRING,
                        8,
                        (strlen("i3-WM_Sn") + 1) * 2,
                        "i3-WM_Sn\0i3-WM_Sn\0");

    xcb_set_selection_owner(*x, x.wm_sn_selection_owner, x.wm_sn, last_timestamp);

    if (selection_reply && selection_reply->owner != XCB_NONE) {
        unsigned int usleep_time = 100000; /* 0.1 seconds */
        int check_rounds = 150;            /* Wait for a maximum of 15 seconds */
        xcb_get_geometry_reply_t *geom_reply = nullptr;

        DLOG("waiting for old WM_Sn selection owner to exit");
        do {
            free(geom_reply);
            usleep(usleep_time);
            if (check_rounds-- == 0) {
                throw std::runtime_error("The old window manager is not exiting");
            }
            geom_reply = xcb_get_geometry_reply(*x,
                                                xcb_get_geometry(*x, selection_reply->owner),
                                                nullptr);
        } while (geom_reply != nullptr);
    }
    free(selection_reply);

    /* Announce that we are the new owner */
    /* Every X11 event is 32 bytes long. Therefore, XCB will copy 32 bytes.
     * In order to properly initialize these bytes, we allocate 32 bytes even
     * though we only need less for an xcb_client_message_event_t */
    union {
        xcb_client_message_event_t message;
        char storage[32];
    } event;
    memset(&event, 0, sizeof(event));
    event.message.response_type = XCB_CLIENT_MESSAGE;
    event.message.window = x.root_screen->root;
    event.message.format = 32;
    event.message.type = i3::atoms[i3::Atom::MANAGER];
    event.message.data.data32[0] = last_timestamp;
    event.message.data.data32[1] = x.wm_sn;
    event.message.data.data32[2] = x.wm_sn_selection_owner;

    xcb_send_event(*x, 0, x.root_screen->root, XCB_EVENT_MASK_STRUCTURE_NOTIFY, event.storage);
}

int main(int argc, char *argv[]) {
    /* Keep a symbol pointing to the I3_VERSION string constant so that we have
     * it in gdb backtraces. */
    static char const *_i3_version __attribute__((used)) = I3_VERSION;

    setlocale(LC_ALL, "");

    /* Get the RLIMIT_CORE limit at startup time to restore this before
     * starting processes. */
    getrlimit(RLIMIT_CORE, &global.original_rlimit_core);

    /* Disable output buffering to make redirects in .xsession actually useful for debugging */
    if (!isatty(fileno(stdout))) {
        setbuf(stdout, nullptr);
    }

    set_formatting();

    auto args = parse_args(argc, argv);
    global.args = &args;

    for (int i = 0; i < argc; i++) {
        global.start_argv.emplace_back(argv[i]);
    }
    global.new_parser = args.new_parser;

    auto const injector = boost::di::make_injector();
    auto &configManager = injector.create<ConfigurationManager &>();

    if (args.only_check_config) {
        try {
            configManager.load_configuration(args.override_configpath);
            exit(EXIT_SUCCESS);
        } catch (std::exception &e) {
            exit(EXIT_FAILURE);
        }
    }

    /* If the user passes more arguments, we act like old i3-msg would: Just send
     * the arguments as an IPC message to i3. This allows for nice semantic
     * commands such as 'i3 border none'. */
    if (optind < argc) {
        handle_extra_args(argc, argv);
        return 0;
    }

    /* Try to enable core dumps by default when running a debug build */
    if (is_debug_build()) {
        enable_coredumps();
    }

    LOG(fmt::sprintf("i3 %s starting\n", I3_VERSION));

    global.assignmentManager = &injector.create<AssignmentManager &>();
    global.workspaceManager = &injector.create<WorkspaceManager &>();
    global.configManager = &configManager;

    /* Prefetch X11 extensions that we are interested in. */
    auto &x = injector.create<X &>();
    global.x = &x;
    if (x.conn->connection_has_error()) {
        errx(EXIT_FAILURE, "Cannot open display");
    }

    xcb_change_window_attributes(*x, x.root, XCB_CW_EVENT_MASK, (uint32_t[]){XCB_EVENT_MASK_PROPERTY_CHANGE});
    xcb_change_property(*x, XCB_PROP_MODE_APPEND, x.root, XCB_ATOM_SUPERSCRIPT_X, XCB_ATOM_CARDINAL, 32, 0, "");

    /* Place requests for the atoms we need as soon as possible */
    setup_atoms(x);
    xcb_timestamp_t last_timestamp = XCB_CURRENT_TIME;

    /* Get the PropertyNotify event we caused above */
    xcb_flush(*x);
    {
        xcb_generic_event_t *event;
        DLOG("waiting for PropertyNotify event\n");
        while ((event = xcb_wait_for_event(*x)) != nullptr) {
            if (event->response_type == XCB_PROPERTY_NOTIFY) {
                last_timestamp = ((xcb_property_notify_event_t *)event)->time;
                free(event);
                break;
            }
            free(event);
        }
        DLOG(fmt::sprintf("got timestamp %d\n", last_timestamp));
    }

    x.conn->prefetch_maximum_request_length();

    init_dpi(*x, x.root_screen);

    try {
        configManager.set_config(configManager.load_configuration(args.override_configpath));
    } catch (std::domain_error &e) {
        using namespace std::literals;
        std::vector<button_t> buttons{};
        std::string prompt = e.what();
        std::string font = "fixed"s;
        start_nagbar(&global.config_error_nagbar_pid, buttons, prompt, font, bar_type_t::TYPE_ERROR, false);
    }

    if (!configManager.config->ipc_socket_path) {
        /* Fall back to a file name in /tmp/ based on the PID */
        if (char *i3sock = getenv("I3SOCK")) {
            configManager.config->ipc_socket_path = i3sock;
        } else {
            auto process_filename = get_process_filename("ipc-socket");
            if (process_filename) {
                configManager.config->ipc_socket_path = *process_filename;
            } else {
                configManager.config->ipc_socket_path = std::nullopt;
            }
        }
    }

    auto &ipcManager = injector.create<IPCManager &>();

    global.ipcManager = &ipcManager;

    /* Create the UNIX domain socket for IPC */
    int ipc_socket = ipcManager.create_socket();

    aquire_selection(x, last_timestamp);

    try {
        uint32_t valueList[]{ROOT_EVENT_MASK};
        x.conn->change_window_attributes_checked(x.root, XCB_CW_EVENT_MASK, valueList);
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("Another window manager seems to be running (X error %s)\n", e.what()));
        exit(EXIT_FAILURE);
    }

    x.xcursor_load_cursors();

    /* Set a cursor for the root window (otherwise the root window will show no
       cursor until the first client is launched). */
    x.xcursor_set_root_cursor(XCURSOR_CURSOR_POINTER);

    auto &xkb = injector.create<Xkb &>();
    global.xkb = &xkb;

    auto &propertyHandlers = injector.create<PropertyHandlers &>();

    EventHandler &eventHandler = injector.create<EventHandler &>();
    global.eventHandler = &eventHandler;

    restore_connect();

    global.handlers = &propertyHandlers;

    ewmh_setup_hints();

    x.xcb_numlock_mask = xkb.get_numlock_mask();

    xkb.load_keymap();
    translate_keysyms(xkb.keymap());
    grab_all_keys(&*x);

    RandR randr = injector.create<RandR &>();
    global.randr = &randr;

    ewmh_update_desktop_properties();
    tree_render();  // TODO: is this call needed?

    /* We need to force disabling outputs which have been loaded from the
     * layout file but are no longer active. This can happen if the output has
     * been disabled in the short time between writing the restart layout file
     * and restarting i3. See #2326. */
    force_disable_output(randr, args);

    Output *output = get_focused_output();
    con_descend_focused(output->con->output_get_content())->con_activate();

    tree_render();

    /* Listen to the IPC socket for clients */
    ipcManager.listen();

    confirm_restart(ipcManager);

    /* Set up i3 specific atoms like I3_SOCKET_PATH and I3_CONFIG_PATH */
    x_set_i3_atoms();
    ewmh_update_workarea();

    /* Set the ewmh desktop properties. */
    ewmh_update_desktop_properties();

    x.conn->flush();

    ignore_restart_events(&*x);

    if (args.autostart) {
        fix_empty_background(&*x);
    }

#if defined(__OpenBSD__)
    if (pledge("stdio rpath wpath cpath proc exec unix", NULL) == -1) {
        err(EXIT_FAILURE, "pledge");
    }
#endif

    if (!args.disable_signalhandler) {
        setup_signal_handler();
    }

    eventHandler.setup_term_handlers();
    /* Ignore SIGPIPE to survive errors when an IPC client disconnects
     * while we are sending them a message */
    signal(SIGPIPE, SIG_IGN);

    ApplicationLauncher &applicationLauncher = injector.create<ApplicationLauncher &>();
    global.applicationLauncher = &applicationLauncher;

    /* Autostarting exec-lines */
    if (args.autostart) {
        autostart(applicationLauncher);
    }

    /* Autostarting exec_always-lines */
    autostart_always(applicationLauncher);

    /* Make sure to destroy the event loop to invoke the cleanup callbacks
     * when calling exit() */
    atexit(i3_exit);

    eventHandler.loop();
}
