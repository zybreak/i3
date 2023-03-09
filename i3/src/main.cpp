/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * main.c: Initialization, main loop
 *
 */
#include <config.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <clocale>
#include <csignal>
#include <queue>

#include <err.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_keysyms.h>

#include "libi3.h"

#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "xcb.h"
#include "manage.h"
#include "i3.h"
#include "x.h"
#include "configuration.h"
#include "handlers.h"
#include "randr.h"
#include "con.h"
#include "xcursor.h"
#include "sighandler.h"
#include "output.h"
#include "ewmh.h"
#include "startup.h"
#include "scratchpad.h"
#include "bindings.h"
#include "config_parser.h"
#include "restore_layout.h"
#include "nagbar.h"

#include <ev.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <program_arguments.h>
#include <autostarts.h>
#include <fmt/core.h>
#include <filesystem>
#include <xpp/xpp.hpp>
#include <xpp/proto/randr.hpp>
#include <xpp/proto/xkb.hpp>
#include <xpp/proto/shape.hpp>
#include <xpp/proto/bigreq.hpp>
#include "main.h"
#include "global.h"

#ifdef I3_ASAN_ENABLED
#include <sanitizer/lsan_interface.h>
#endif

#include "atoms.h"

/* The number of file descriptors passed via socket activation. */
int listen_fds;

/* We keep the xcb_prepare watcher around to be able to enable and disable it
 * temporarily for drag_pointer(). */
static struct ev_prepare *xcb_prepare;

char **start_argv;

/* Display handle for libstartup-notification */
SnDisplay *sndisplay;

xcb_window_t root;

/* Color depth, visual id and colormap to use when creating windows and
 * pixmaps. Will use 32 bit depth and an appropriate visual, if available,
 * otherwise the root window’s default (usually 24 bit TrueColor). */
uint8_t root_depth;
xcb_visualtype_t *visual_type;
xcb_colormap_t colormap;

struct ev_loop *main_loop;

xcb_key_symbols_t *keysyms;

/* We hope that those are supported and set them to true */
bool xkb_supported = true;
bool shape_supported = true;

bool is_background_set(xcb_connection_t *conn, xcb_screen_t *screen);

/*
 * This callback is only a dummy, see xcb_prepare_cb.
 * See also man libev(3): "ev_prepare" and "ev_check" - customise your event loop
 *
 */
static void xcb_got_event(EV_P_ struct ev_io *w, int revents) {
    /* empty, because xcb_prepare_cb are used */
}

/*
 * Called just before the event loop sleeps. Ensures xcb’s incoming and outgoing
 * queues are empty so that any activity will trigger another event loop
 * iteration, and hence another xcb_prepare_cb invocation.
 *
 */
static void xcb_prepare_cb(EV_P_ ev_prepare *w, int revents) {
    /* Process all queued (and possibly new) events before the event loop
       sleeps. */
    xcb_generic_event_t *event;

    while ((event = xcb_poll_for_event(global.conn)) != nullptr) {
        if (event->response_type == 0) {
            if (event_is_ignored(event->sequence, 0))
                DLOG(fmt::sprintf("Expected X11 Error received for sequence %x\n",  event->sequence));
            else {
                auto *error = (xcb_generic_error_t *)event;
                DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                     error->sequence, error->error_code));
            }
            free(event);
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        handle_event(type, event);

        free(event);
    }

    /* Flush all queued events to X11. */
    xcb_flush(global.conn);
}

/*
 * Enable or disable the main X11 event handling function.
 * This is used by drag_pointer() which has its own, modal event handler, which
 * takes precedence over the normal event handler.
 *
 */
void main_set_x11_cb(bool enable) {
    DLOG(fmt::sprintf("Setting main X11 callback to enabled=%d\n",  enable));
    if (enable) {
        ev_prepare_start(main_loop, xcb_prepare);
        /* Trigger the watcher explicitly to handle all remaining X11 events.
         * drag_pointer()’s event handler exits in the middle of the loop. */
        ev_feed_event(main_loop, xcb_prepare, 0);
    } else {
        ev_prepare_stop(main_loop, xcb_prepare);
    }
}

/*
 * Exit handler which destroys the main_loop. Will trigger cleanup handlers.
 *
 */
static void i3_exit() {
    if (!global.run_atexit) {
        return;
    }

    ipc_shutdown(SHUTDOWN_REASON_EXIT, -1);
    unlink(config.ipc_socket_path);
    xcb_disconnect(global.conn);

    /* If a nagbar is active, kill it */
    kill_nagbar(global.config_error_nagbar_pid, false);
    kill_nagbar(global.command_error_nagbar_pid, false);

/* We need ev >= 4 for the following code. Since it is not *that* important (it
 * only makes sure that there are no i3-nagbar instances left behind) we still
 * support old systems with libev 3. */
#if EV_VERSION_MAJOR >= 4
    ev_loop_destroy(main_loop);
#endif

#ifdef I3_ASAN_ENABLED
    __lsan_do_leak_check();
#endif
}

/*
 * (One-shot) Handler for all signals with default action "Term", see signal(7)
 *
 * Exits the program gracefully.
 *
 */
static void handle_term_signal(struct ev_loop *loop, ev_signal *signal, int revents) {
    /* We exit gracefully here in the sense that cleanup handlers
     * installed via atexit are invoked. */
    exit(128 + signal->signum);
}

/*
 * Set up handlers for all signals with default action "Term", see signal(7)
 *
 */
static void setup_term_handlers() {
    static struct ev_signal signal_watchers[6];

    /* We have to rely on libev functionality here and should not use
     * sigaction handlers because we need to invoke the exit handlers
     * and cannot do so from an asynchronous signal handling context as
     * not all code triggered during exit is signal safe (and exiting
     * the main loop from said handler is not easily possible). libev's
     * signal handlers does not impose such a constraint on us. */
    ev_signal_init(&signal_watchers[0], handle_term_signal, SIGHUP);
    ev_signal_init(&signal_watchers[1], handle_term_signal, SIGINT);
    ev_signal_init(&signal_watchers[2], handle_term_signal, SIGALRM);
    ev_signal_init(&signal_watchers[3], handle_term_signal, SIGTERM);
    ev_signal_init(&signal_watchers[4], handle_term_signal, SIGUSR1);
    ev_signal_init(&signal_watchers[5], handle_term_signal, SIGUSR1);
    for (auto &signal_watcher : signal_watchers) {
        ev_signal_start(main_loop, &signal_watcher);
        /* The signal handlers should not block ev_run from returning
         * and so none of the signal handlers should hold a reference to
         * the main loop. */
        ev_unref(main_loop);
    }
}

/*
 * Creates the UNIX domain socket at the given path, sets it to non-blocking
 * mode, bind()s and listen()s on it.
 *
 * The full path to the socket is stored in the char* that out_socketpath points
 * to.
 *
 */
static std::tuple<std::string, int> create_socket(const char *filename) {
    auto resolved = resolve_tilde(filename);
    DLOG(fmt::sprintf("Creating UNIX socket at %s\n",  resolved));
    const std::filesystem::path p(resolved);
    const std::filesystem::path &parent = p.parent_path();

    if (!std::filesystem::exists(parent)) {
        try {
            std::filesystem::create_directories(parent);
        } catch (std::exception &e) {
            ELOG(fmt::sprintf("mkdir(%s) failed: %s\n",  parent.string(), e.what()));
        }
    }

    /* Unlink the unix domain socket before */
    std::filesystem::remove(resolved);

    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket()");
        throw std::exception();
    }

    (void)fcntl(sockfd, F_SETFD, FD_CLOEXEC);

    struct sockaddr_un addr{};
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, resolved.c_str(), sizeof(addr.sun_path) - 1);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind()");
        throw std::exception();
    }

    set_nonblock(sockfd);

    if (listen(sockfd, 5) < 0) {
        perror("listen()");
        throw std::exception();
    }

    return std::make_tuple(resolved, sockfd);
}

static int parse_restart_fd() {
    const char *restart_fd = getenv("_I3_RESTART_FD");
    if (restart_fd == nullptr) {
        return -1;
    }

    long int fd = -1;
    if (!parse_long(restart_fd, &fd, 10)) {
         ELOG(fmt::sprintf("Malformed _I3_RESTART_FD \"%s\"\n", restart_fd));
        return -1;
    }
    return (int)fd;
}

void handle_extra_args(int argc, char *argv[])  {
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
    DLOG(fmt::sprintf("Command is: %s (%zd bytes)\n",  payload, payload.length()));
    char *socket_path = root_atom_contents("I3_SOCKET_PATH", nullptr, 0);
    if (!socket_path) {
        ELOG("Could not get i3 IPC socket path\n");
        exit(1);
    }

    int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd == -1)
        err(EXIT_FAILURE, "Could not create socket");

    struct sockaddr_un addr{};
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    FREE(socket_path);
    if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0)
        err(EXIT_FAILURE, "Could not connect to i3");

    if (ipc_send_message(sockfd, strlen(payload.c_str()), I3_IPC_MESSAGE_TYPE_RUN_COMMAND,
                         (uint8_t *)payload.c_str()) == -1)
        err(EXIT_FAILURE, "IPC: write()");

    uint32_t reply_length;
    uint32_t reply_type;
    uint8_t *reply;
    int ret;
    if ((ret = ipc_recv_message(sockfd, &reply_type, &reply_length, &reply)) != 0) {
        if (ret == -1)
            err(EXIT_FAILURE, "IPC: read()");
        exit(1);
    }
    if (reply_type != I3_IPC_REPLY_TYPE_COMMAND)
        errx(EXIT_FAILURE, "IPC: received reply of type %d but expected %d (COMMAND)", reply_type, I3_IPC_REPLY_TYPE_COMMAND);
    printf("%.*s\n", reply_length, reply);
    FREE(reply);
}

void enable_coredumps() {
    LOG("CORE DUMPS: You are running a development version of i3, so coredumps were automatically enabled (ulimit -c unlimited).\n");
    struct rlimit limit = {RLIM_INFINITY, RLIM_INFINITY};
    setrlimit(RLIMIT_CORE, &limit);
}

void do_tree_init(const program_arguments &args, const xcb_get_geometry_reply_t *greply) {
    bool needs_tree_init = true;
    if (!args.layout_path.empty()) {
         LOG(fmt::sprintf("Trying to restore the layout from \"%s\".\n", args.layout_path));
        needs_tree_init = !tree_restore(args.layout_path, greply);
        if (args.delete_layout_path) {
            unlink(args.layout_path.c_str());
            std::string copy = args.layout_path;
            const char *dir = dirname(copy.data());
            /* possibly fails with ENOTEMPTY if there are files (or
             * sockets) left. */
            rmdir(dir);
        }
    }
    if (needs_tree_init)
        tree_init(greply);
}

static void set_screenshot_as_wallpaper(xcb_connection_t *conn, xcb_screen_t *screen) {
    uint16_t width = screen->width_in_pixels;
    uint16_t height = screen->height_in_pixels;
    xcb_pixmap_t pixmap = xcb_generate_id(conn);
    xcb_gcontext_t gc = xcb_generate_id(conn);

    xcb_create_pixmap(conn, screen->root_depth, pixmap, screen->root, width, height);

    uint32_t list[]{XCB_GX_COPY, (uint32_t)~0, XCB_FILL_STYLE_SOLID, XCB_SUBWINDOW_MODE_INCLUDE_INFERIORS};
    xcb_create_gc(conn, gc, screen->root,
            XCB_GC_FUNCTION | XCB_GC_PLANE_MASK | XCB_GC_FILL_STYLE | XCB_GC_SUBWINDOW_MODE,
            list);

    xcb_copy_area(conn, screen->root, pixmap, gc, 0, 0, 0, 0, width, height);
    uint32_t value_list[]{pixmap};
    xcb_change_window_attributes(conn, screen->root, XCB_CW_BACK_PIXMAP, value_list);
    xcb_free_gc(conn, gc);
    xcb_free_pixmap(conn, pixmap);
    xcb_flush(conn);
}

static xcb_visualtype_t* get_visualtype_for_root() {
    auto visual_type = xcb_aux_find_visual_by_attrs(global.root_screen, -1, 32);
    if (visual_type != nullptr) {
        root_depth = xcb_aux_get_depth_of_visual(global.root_screen, visual_type->visual_id);
        colormap = global.a->generate_id();

        try {
            xpp::x::create_colormap_checked(*global.a,
                    XCB_COLORMAP_ALLOC_NONE,
                    colormap,
                    root,
                    visual_type->visual_id);

        } catch (std::exception &e) {
            ELOG(fmt::sprintf("Could not create colormap. Error: %s\n",  e.what()));
            exit(EXIT_FAILURE);
        }
    } else {
        visual_type = get_visualtype(global.root_screen);
    }

    return visual_type;
}

static void init_xkb() {
    auto xkb_ext = global.a->extension<xpp::xkb::extension>();
    xkb_supported = xkb_ext->present;
    if (!xkb_supported) {
        DLOG("xkb is not present on this server\n");
    } else {
        DLOG("initializing xcb-xkb\n");
        global.a->xkb().use_extension(XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION);
        global.a->xkb().select_events(
                              XCB_XKB_ID_USE_CORE_KBD,
                              XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
                              0,
                              XCB_XKB_EVENT_TYPE_STATE_NOTIFY | XCB_XKB_EVENT_TYPE_MAP_NOTIFY | XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY,
                              0xff,
                              0xff,
                              nullptr);

        /* Setting both, XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE and
         * XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED, will lead to the
         * X server sending us the full XKB state in KeyPress and KeyRelease:
         * https://cgit.freedesktop.org/xorg/xserver/tree/xkb/xkbEvents.c?h=xorg-server-1.20.0#n927
         *
         * XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT enable detectable autorepeat:
         * https://www.x.org/releases/current/doc/kbproto/xkbproto.html#Detectable_Autorepeat
         * This affects bindings using the --release flag: instead of getting multiple KeyRelease
         * events we get only one event when the key is physically released by the user.
         */
        const uint32_t mask = XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE |
                              XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED |
                              XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT;
        /* The last three parameters are unset because they are only relevant
         * when using a feature called “automatic reset of boolean controls”:
         * https://www.x.org/releases/X11R7.7/doc/kbproto/xkbproto.html#Automatic_Reset_of_Boolean_Controls
         * */
        auto pcf_reply = xpp::xkb::per_client_flags(*global.a, XCB_XKB_ID_USE_CORE_KBD, mask, mask, 0, 0, 0);

        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE))) {
            ELOG("Could not set " "XCB_XKB_PER_CLIENT_FLAG_GRABS_USE_XKB_STATE" "\n");
        }
        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED))) {
            ELOG("Could not set " "XCB_XKB_PER_CLIENT_FLAG_LOOKUP_STATE_WHEN_GRABBED" "\n");
        }
        if (pcf_reply.get() == nullptr || !(pcf_reply->value & (XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT))) {
            ELOG("Could not set " "XCB_XKB_PER_CLIENT_FLAG_DETECTABLE_AUTO_REPEAT" "\n");
        }

        xkb_base = xkb_ext->first_event;
    }
}

static void init_shape() {
    auto shape_ext = global.a->extension<xpp::shape::extension>();
    if (shape_ext->present) {
        shape_base = shape_ext->first_event;
        auto version = global.a->shape().query_version();
        shape_supported = version && version->minor_version >= 1;
    } else {
        shape_supported = false;
    }
    if (!shape_supported) {
        DLOG("shape 1.1 is not present on this server\n");
    }
}

static void force_disable_output(const program_arguments &args) {
    if (!args.layout_path.empty() && randr_base > -1) {
        for (auto &con : croot->nodes_head) {
            for (Output *output : outputs) {
                if (output->active || strcmp(con->name.c_str(), output->output_primary_name().c_str()) != 0)
                    continue;

                /* This will correctly correlate the output with its content
                 * container. We need to make the connection to properly
                 * disable the output. */
                if (output->con == nullptr) {
                    output_init_con(output);
                    output->changed = false;
                }

                output->to_be_disabled = true;
                randr_disable_output(output);
            }
        }
    }
}

static Output *get_focused_output() {
    Output *output;

    auto pointerreply = xpp::x::query_pointer(*global.a, root);
    if (pointerreply.get() == nullptr) {
        ELOG("Could not query pointer position, using first screen\n");
    } else {
        DLOG(fmt::sprintf("Pointer at %d, %d\n",  pointerreply->root_x, pointerreply->root_y));
        output = get_output_containing(pointerreply->root_x, pointerreply->root_y);
        if (!output) {
            ELOG(fmt::sprintf("ERROR: No screen at (%d, %d), starting on the first screen\n",
                 pointerreply->root_x, pointerreply->root_y));
        }
    }
    if (!output) {
        output = get_first_output();
    }
    return output;
}

/* Create the UNIX domain socket for IPC */
static void init_ipc() {
    int ipc_socket;

    std::tie(current_socketpath, ipc_socket) = create_socket(config.ipc_socket_path);
    if (ipc_socket == -1) {
        ELOG("Could not create the IPC socket, IPC disabled\n");
    } else {
        auto *ipc_io = new ev_io();
        ev_io_init(ipc_io, ipc_new_client, ipc_socket, EV_READ);
        ev_io_start(main_loop, ipc_io);
    }
}

static void confirm_restart() {
    const int restart_fd = parse_restart_fd();
    if (restart_fd != -1) {
        DLOG(fmt::sprintf("serving restart fd %d",  restart_fd));
        ipc_client *client = ipc_new_client_on_fd(main_loop, restart_fd);
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
static void ignore_restart_events(xcb_connection_t *conn) {
    global.a->grab_server();
    {
        xcb_aux_sync(conn);
        xcb_generic_event_t *event;
        while ((event = xcb_poll_for_event(conn)) != nullptr) {
            if (event->response_type == 0) {
                free(event);
                continue;
            }

            /* Strip off the highest bit (set if the event is generated) */
            int type = (event->response_type & 0x7F);

            /* We still need to handle MapRequests which are sent in the
             * timespan starting from when we register as a window manager and
             * this piece of code which drops events. */
            if (type == XCB_MAP_REQUEST)
                handle_event(type, event);

            free(event);
        }
        manage_existing_windows(root);
    }
    global.a->ungrab_server();
}

static void fix_empty_background(xcb_connection_t *conn) {
    /* When the root's window background is set to NONE, that might mean
     * that old content stays visible when a window is closed. That has
     * unpleasant effect of "my terminal (does not seem to) close!".
     *
     * There does not seem to be an easy way to query for this problem, so
     * we test for it: Open & close a window and check if the background is
     * redrawn or the window contents stay visible.
     */
    LOG("This is not an in-place restart, checking if a wallpaper is set.\n");

    xcb_screen_t *s = xcb_aux_get_screen(conn, global.conn_screen);
    if (is_background_set(conn, s)) {
        LOG("A wallpaper is set, so no screenshot is necessary.\n");
    } else {
        LOG("No wallpaper set, copying root window contents to a pixmap\n");
        set_screenshot_as_wallpaper(conn, s);
    }
}

/* Start i3bar processes for all configured bars */
static void start_i3bar() {
    for (auto &barconfig : barconfigs) {
        std::string command = fmt::format("{} {} --bar_id={} --socket=\"{}\"",
                  barconfig->i3bar_command ? barconfig->i3bar_command : "exec i3bar",
                  barconfig->verbose ? "-V" : "",
                  barconfig->id, current_socketpath);
        LOG(fmt::sprintf("Starting bar process: %s\n",  command));
        start_application(command, true);
    }
}

int main(int argc, char *argv[]) {
    /* Keep a symbol pointing to the I3_VERSION string constant so that we have
     * it in gdb backtraces. */
    static const char *_i3_version __attribute__((used)) = I3_VERSION;


    setlocale(LC_ALL, "");

    /* Get the RLIMIT_CORE limit at startup time to restore this before
     * starting processes. */
    getrlimit(RLIMIT_CORE, &global.original_rlimit_core);

    /* Disable output buffering to make redirects in .xsession actually useful for debugging */
    if (!isatty(fileno(stdout)))
        setbuf(stdout, nullptr);

    srand(time(nullptr));

    auto args = parse_args(argc, argv);

    global.new_parser = args.new_parser;

    if (args.only_check_config) {
        exit(load_configuration(&args.override_configpath, config_load_t::C_VALIDATE) ? EXIT_SUCCESS : EXIT_FAILURE);
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

    LOG(fmt::sprintf("i3 %s starting\n",  i3_version));

    /* Prefetch X11 extensions that we are interested in. */
    global.a = new x_connection();
    if (global.a->connection_has_error())
        errx(EXIT_FAILURE, "Cannot open display");

    global.conn_screen = global.a->default_screen();
    auto conn = *global.a;
    global.conn = conn;

    sndisplay = sn_xcb_display_new(conn, nullptr, nullptr);

    /* Initialize the libev event loop. This needs to be done before loading
     * the config file because the parser will install an ev_child watcher
     * for the nagbar when config errors are found. */
    main_loop = EV_DEFAULT;
    if (main_loop == nullptr)
        errx(EXIT_FAILURE, "Could not initialize libev. Bad LIBEV_FLAGS?\n");;

    global.root_screen = global.a->screen_of_display(global.a->default_screen());
    root = global.a->root();

    /* Place requests for the atoms we need as soon as possible */
    setup_atoms();

    root_depth = global.root_screen->root_depth;
    colormap = global.root_screen->default_colormap;
    visual_type = get_visualtype_for_root();

    global.a->prefetch_maximum_request_length();

    init_dpi(*global.a, global.root_screen);

    DLOG(fmt::sprintf("root_depth = %d, visual_id = 0x%08x.\n",  root_depth, visual_type->visual_id));
    DLOG(fmt::sprintf("root_screen->height_in_pixels = %d, root_screen->height_in_millimeters = %d\n",
         global.root_screen->height_in_pixels, global.root_screen->height_in_millimeters));
    DLOG(fmt::sprintf("One logical pixel corresponds to %ld physical pixels on this display.\n",  logical_px(global.root_screen, 1)));

    load_configuration(&args.override_configpath, config_load_t::C_LOAD);

    if (config.ipc_socket_path == nullptr) {
        /* Fall back to a file name in /tmp/ based on the PID */
        if ((config.ipc_socket_path = getenv("I3SOCK")) == nullptr)
            config.ipc_socket_path = get_process_filename("ipc-socket");
        else
            config.ipc_socket_path = sstrdup(config.ipc_socket_path);
    }

    try {
        uint32_t valueList[]{ROOT_EVENT_MASK};
        global.a->change_window_attributes_checked(root, XCB_CW_EVENT_MASK, valueList);
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("Another window manager seems to be running (X error %s)\n",  e.what()));
        exit(EXIT_FAILURE);
    }

    auto greply = global.a->get_geometry(root).get();
    if (greply == nullptr) {
        ELOG("Could not get geometry of the root window, exiting\n");
        return 1;
    }
    DLOG(fmt::sprintf("root geometry reply: (%d, %d) %d x %d\n",  greply->x, greply->y, greply->width, greply->height));

    xcursor_load_cursors();

    /* Set a cursor for the root window (otherwise the root window will show no
       cursor until the first client is launched). */
    xcursor_set_root_cursor(XCURSOR_CURSOR_POINTER);

    init_xkb();

    /* Check for Shape extension. We want to handle input shapes which is
     * introduced in 1.1. */
    init_shape();

    restore_connect();

    property_handlers_init();

    ewmh_setup_hints();

    keysyms = xcb_key_symbols_alloc(*global.a);

    xcb_numlock_mask = aio_get_mod_mask_for(XCB_NUM_LOCK, keysyms);

    if (!load_keymap())
        errx(EXIT_FAILURE, "Could not load keymap\n");;

    translate_keysyms();
    grab_all_keys(*global.a);

    do_tree_init(args, greply.get());

    DLOG("Checking for XRandR...\n");
    randr_init(&randr_base);

    /* We need to force disabling outputs which have been loaded from the
     * layout file but are no longer active. This can happen if the output has
     * been disabled in the short time between writing the restart layout file
     * and restarting i3. See #2326. */
    force_disable_output(args);

    scratchpad_fix_resolution();

    Output *output = get_focused_output();
    con_descend_focused(output->con->output_get_content())->con_activate();

    tree_render();

    /* Create the UNIX domain socket for IPC */
    init_ipc();

    confirm_restart();

    /* Set up i3 specific atoms like I3_SOCKET_PATH and I3_CONFIG_PATH */
    x_set_i3_atoms();
    ewmh_update_workarea();

    /* Set the ewmh desktop properties. */
    ewmh_update_desktop_properties();

    auto *xcb_watcher = new ev_io();
    xcb_prepare = new ev_prepare;

    ev_io_init(xcb_watcher, xcb_got_event, xcb_get_file_descriptor(*global.a), EV_READ);
    ev_io_start(main_loop, xcb_watcher);

    ev_prepare_init(xcb_prepare, xcb_prepare_cb);
    ev_prepare_start(main_loop, xcb_prepare);

    global.a->flush();

    ignore_restart_events(*global.a);

    if (args.autostart) {
        fix_empty_background(*global.a);

    }

#if defined(__OpenBSD__)
    if (pledge("stdio rpath wpath cpath proc exec unix", NULL) == -1)
        err(EXIT_FAILURE, "pledge");
#endif

    if (!args.disable_signalhandler) {
        setup_signal_handler();
    }

    setup_term_handlers();
    /* Ignore SIGPIPE to survive errors when an IPC client disconnects
     * while we are sending them a message */
    signal(SIGPIPE, SIG_IGN);

    /* Autostarting exec-lines */
    if (args.autostart) {
        autostart();
    }

    /* Autostarting exec_always-lines */
    autostart_always();

    /* Start i3bar processes for all configured bars */
    start_i3bar();

    /* Make sure to destroy the event loop to invoke the cleanup callbacks
     * when calling exit() */
    atexit(i3_exit);

    ev_loop(main_loop, 0);
}
