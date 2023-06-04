/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * restore_layout.c: Everything for restored containers that is not pure state
 *                   parsing (which can be found in load_layout.c).
 *
 *
 */
#include <err.h>
#include <ev.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>
#include "global.h"
#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "font.h"
#include "wrapper.h"
#include "dpi.h"
#include "get_visualtype.h"

#include "xcb.h"
#include "i3.h"
#include "configuration.h"
#include "con.h"
#include "match.h"
#include "restore_layout.h"

#ifdef I3_ASAN_ENABLED
#include <sanitizer/lsan_interface.h>
#endif

typedef struct placeholder_state {
    /** The X11 placeholder window. */
    xcb_window_t window{};
    /** The container to which this placeholder window belongs. */
    Con *con{};

    /** Current size of the placeholder window (to detect size changes). */
    Rect rect;

    /** The drawable surface */
    surface_t surface{};
} placeholder_state;

static std::deque<std::unique_ptr<placeholder_state>> states{};

static xcb_connection_t *restore_conn;

static ev_io *xcb_watcher;
static ev_prepare *xcb_prepare;

static void restore_handle_event(int type, xcb_generic_event_t *event);

/* Documentation for these functions can be found in src/main.c, starting at xcb_got_event */
static void restore_xcb_got_event(EV_P_ struct ev_io *w, int revents) {
}

static void restore_xcb_prepare_cb(EV_P_ ev_prepare *w, int revents) {
    xcb_generic_event_t *event;

    if (xcb_connection_has_error(restore_conn)) {
        DLOG("restore X11 connection has an error, reconnecting\n");
        restore_connect();
        return;
    }

    while ((event = xcb_poll_for_event(restore_conn)) != nullptr) {
        if (event->response_type == 0) {
            auto *error = (xcb_generic_error_t *)event;
            DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                 error->sequence, error->error_code));
            free(event);
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        restore_handle_event(type, event);

        free(event);
    }

    xcb_flush(restore_conn);
}

/*
 * Opens a separate connection to X11 for placeholder windows when restoring
 * layouts. This is done as a safety measure (users can xkill a placeholder
 * window without killing their window manager) and for better isolation, both
 * on the wire to X11 and thus also in the code.
 *
 */
void restore_connect() {
    if (restore_conn != nullptr) {
        /* This is not the initial connect, but a reconnect, most likely
         * because our X11 connection was killed (e.g. by a user with xkill. */
        ev_io_stop(main_loop, xcb_watcher);
        ev_prepare_stop(main_loop, xcb_prepare);

        states.clear();

        /* xcb_disconnect leaks memory in libxcb versions earlier than 1.11,
         * but it’s the right function to call. See
         * https://cgit.freedesktop.org/xcb/libxcb/commit/src/xcb_conn.c?id=4dcbfd77b
         */
        xcb_disconnect(restore_conn);
        delete xcb_watcher;
        delete xcb_prepare;
    }

    int screen;
    restore_conn = xcb_connect(nullptr, &screen);
    if (restore_conn == nullptr || xcb_connection_has_error(restore_conn)) {
        if (restore_conn != nullptr) {
            xcb_disconnect(restore_conn);
        }
#ifdef I3_ASAN_ENABLED
        __lsan_do_leak_check();
#endif
        errx(EXIT_FAILURE, "Cannot open display");
    }

    xcb_watcher = new ev_io{};
    xcb_prepare = new ev_prepare{};

    ev_io_init(xcb_watcher, restore_xcb_got_event, xcb_get_file_descriptor(restore_conn), EV_READ);
    ev_io_start(main_loop, xcb_watcher);

    ev_prepare_init(xcb_prepare, restore_xcb_prepare_cb);
    ev_prepare_start(main_loop, xcb_prepare);
}

static void update_placeholder_contents(x_connection *conn, placeholder_state *state) {
    const color_t foreground = config.client.placeholder.text;
    const color_t background = config.client.placeholder.background;

    draw_util_clear_surface(&(state->surface), background);

    // TODO: make i3font functions per-connection, at least these two for now…?
    xcb_aux_sync(restore_conn);

    int n = 0;
    for (auto &swallows : state->con->swallow) {
        char *serialized = nullptr;

#define APPEND_REGEX(re_name)                                                                                                                        \
    do {                                                                                                                                             \
        if (swallows->re_name != NULL) {                                                                                                             \
            sasprintf(&serialized, "%s%s" #re_name "=\"%s\"", (serialized ? serialized : "["), (serialized ? " " : ""), swallows->re_name->pattern); \
        }                                                                                                                                            \
    } while (0)

        APPEND_REGEX(window_class);
        APPEND_REGEX(instance);
        APPEND_REGEX(window_role);
        APPEND_REGEX(title);
        APPEND_REGEX(machine);

        if (serialized == nullptr) {
            DLOG("This swallows specification is not serializable?!\n");
            continue;
        }

        sasprintf(&serialized, "%s]", serialized);
        DLOG(fmt::sprintf("con %p (placeholder 0x%08x) line %d: %s\n",  (void*)state->con, state->window, n, serialized));

        i3String *str = new i3String{serialized};
        draw_util_text(*conn, str, &(state->surface), foreground, background,
                       logical_px(global.x->root_screen, 2),
                       (n * (config.font->height + logical_px(global.x->root_screen, 2))) + logical_px(global.x->root_screen, 2),
                       state->rect.width - 2 * logical_px(global.x->root_screen, 2));
        delete str;
        n++;
        free(serialized);
    }

    // TODO: render the watch symbol in a bigger font
    i3String *line = new i3String{"⌚"};
    int text_width = predict_text_width(*conn, global.x->root_screen, line);
    int x = (state->rect.width / 2) - (text_width / 2);
    int y = (state->rect.height / 2) - (config.font->height / 2);
    draw_util_text(*conn, line, &(state->surface), foreground, background, x, y, text_width);
    delete line;
    xcb_aux_sync(restore_conn);
}

static void open_placeholder_window(x_connection *conn, Con *con) {
    if (con->con_is_leaf() &&
        (con->window == nullptr || con->window->id == XCB_NONE) &&
        !con->swallow.empty() &&
        con->type == CT_CON) {

        uint32_t values[]{
                config.client.placeholder.background.colorpixel,
                XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY,
        };
        xcb_window_t placeholder = create_window(
            restore_conn,
            con->rect,
            XCB_COPY_FROM_PARENT,
            XCB_COPY_FROM_PARENT,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            XCURSOR_CURSOR_POINTER,
            true,
            XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
            values);
        /* Make i3 not focus this window. */
        xcb_icccm_wm_hints_t hints;
        xcb_icccm_wm_hints_set_none(&hints);
        xcb_icccm_wm_hints_set_input(&hints, 0);
        xcb_icccm_set_wm_hints(restore_conn, placeholder, &hints);
        /* Set the same name as was stored in the layout file. While perhaps
         * slightly confusing in the first instant, this brings additional
         * clarity to which placeholder is waiting for which actual window. */
        if (!con->name.empty())
            xcb_change_property(restore_conn, XCB_PROP_MODE_REPLACE, placeholder,
                                A__NET_WM_NAME, A_UTF8_STRING, 8, con->name.length(), con->name.c_str());
        DLOG(fmt::sprintf("Created placeholder window 0x%08x for leaf container %p / %s\n",
             placeholder, (void*)con, con->name));

        auto state = std::make_unique<placeholder_state>();
        state->window = placeholder;
        state->con = con;
        state->rect = con->rect;

        draw_util_surface_init(restore_conn, &(state->surface), placeholder, get_visualtype(global.x->root_screen), state->rect.width, state->rect.height);
        update_placeholder_contents(conn, state.get());
        states.push_back(std::move(state));

        /* create temporary id swallow to match the placeholder */
        auto temp_id = std::make_unique<Match>();
        temp_id->dock = M_DONTCHECK;
        temp_id->id = placeholder;
        con->swallow.push_front(std::move(temp_id));
    }

    for (auto &child : con->nodes_head) {
        open_placeholder_window(conn, child);
    }
    for (auto &child : con->floating_windows) {
        open_placeholder_window(conn, child);
    }
}

/*
 * Open placeholder windows for all children of parent. The placeholder window
 * will vanish as soon as a real window is swallowed by the container. Until
 * then, it exposes the criteria that must be fulfilled for a window to be
 * swallowed by this container.
 *
 */
void restore_open_placeholder_windows(Con *parent) {
    for (auto &child : parent->nodes_head) {
        open_placeholder_window(*global.x, child);
    }
    for (auto &child : parent->floating_windows) {
        open_placeholder_window(*global.x, child);
    }

    xcb_flush(restore_conn);
}

/*
 * Kill the placeholder window, if placeholder refers to a placeholder window.
 * This function is called when manage.c puts a window into an existing
 * container. In order not to leak resources, we need to destroy the window and
 * all associated X11 objects (pixmap/gc).
 *
 */
bool restore_kill_placeholder(xcb_window_t placeholder) {
    for (auto state_ptr = states.begin(); state_ptr != states.end(); ++state_ptr) {
        auto &state = *state_ptr;
        if (state->window != placeholder)
            continue;

        xcb_destroy_window(restore_conn, state->window);
        draw_util_surface_free(restore_conn, &(state->surface));
        state_ptr = states.erase(state_ptr);
        DLOG(fmt::sprintf("placeholder window 0x%08x destroyed.\n",  placeholder));
        return true;
    }

    DLOG(fmt::sprintf("0x%08x is not a placeholder window, ignoring.\n",  placeholder));
    return false;
}

static void expose_event(xcb_expose_event_t *event) {
    for (auto &state : states) {
        if (state->window != event->window)
            continue;

        DLOG(fmt::sprintf("refreshing window 0x%08x contents (con %p)\n",  state->window, (void*)state->con));

        update_placeholder_contents(*global.x, state.get());

        return;
    }

    ELOG(fmt::sprintf("Received ExposeEvent for unknown window 0x%08x\n",  event->window));
}

/*
 * Window size has changed. Update the width/height, then recreate the back
 * buffer pixmap and the accompanying graphics context and force an immediate
 * re-rendering.
 *
 */
static void configure_notify(xcb_configure_notify_event_t *event) {
    for (auto &state : states) {
        if (state->window != event->window)
            continue;

        DLOG(fmt::sprintf("ConfigureNotify: window 0x%08x has now width=%d, height=%d (con %p)\n",
             state->window, event->width, event->height, (void*)state->con));

        state->rect.width = event->width;
        state->rect.height = event->height;

        draw_util_surface_set_size(&(state->surface), state->rect.width, state->rect.height);

        update_placeholder_contents(*global.x, state.get());

        return;
    }

    ELOG(fmt::sprintf("Received ConfigureNotify for unknown window 0x%08x\n",  event->window));
}

static void restore_handle_event(int type, xcb_generic_event_t *event) {
    switch (type) {
        case XCB_EXPOSE:
            if (((xcb_expose_event_t *)event)->count == 0) {
                expose_event((xcb_expose_event_t *)event);
            }

            break;
        case XCB_CONFIGURE_NOTIFY:
            configure_notify((xcb_configure_notify_event_t *)event);
            break;
        default:
            DLOG(fmt::sprintf("Received unhandled X11 event of type %d\n",  type));
            break;
    }
}
