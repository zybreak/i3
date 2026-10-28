/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * restore_layout.c: Everything for restored containers that is not pure state
 *                   parsing (which can be found in load_layout.c).
 *
 *
 */
module;
#include <err.h>

#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

#include <ev.h>

#include <config.h>

#include <sys/time.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#ifdef I3_ASAN_ENABLED
#include <sanitizer/lsan_interface.h>
#endif

#include <fmt/printf.h>
module i3;

import utils;
import log;
import rect;
import xcb.icccm;

struct placeholder_state {
    /** The X11 placeholder window. */
    xcb_window_t window{};
    /** The container to which this placeholder window belongs. */
    Con *con{};

    /** Current size of the placeholder window (to detect size changes). */
    Rect rect;

    /** The drawable surface */
    std::unique_ptr<surface_t> surface{};
};

static std::deque<std::unique_ptr<placeholder_state>> states{};

/* The placeholders' connection: its libxcb handle, which owns it, for the drawing code, and the xcbpp connection
 * behind it. */
static xcb_connection_t *restore_conn;
static xcb::Connection *restore_xconn;
/* The drawing state of restore_conn, for the placeholders' surfaces. */
static std::optional<DrawContext> restore_drawing;

static ev_io *xcb_watcher;
static ev_prepare *xcb_prepare;

static void restore_handle_event(int type, xcb::generic_event_t *event);

/* Documentation for these functions can be found in src/main.c, starting at xcb_got_event */
static void restore_xcb_got_event(EV_P_ ev_io *w, int revents) {
}

static void restore_xcb_prepare_cb(EV_P_ ev_prepare *w, int revents) {
    if (restore_xconn->connection_has_error()) {
        DLOG("restore X11 connection has an error, reconnecting\n");
        restore_connect();
        return;
    }

    while (auto const event = restore_xconn->poll_for_event()) {
        if (event->response_type == 0) {
            auto const *error = reinterpret_cast<xcb::generic_error_t const *>(event.get());
            DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                              error->sequence, error->error_code));
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        restore_handle_event(type, event.get());
    }

    restore_xconn->flush();
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
        ev_io_stop(global.main_loop, xcb_watcher);
        ev_prepare_stop(global.main_loop, xcb_prepare);

        states.clear();
        restore_drawing.reset();

        /* xcb_disconnect leaks memory in libxcb versions earlier than 1.11,
         * but it’s the right function to call. See
         * https://cgit.freedesktop.org/xcb/libxcb/commit/src/xcb_conn.c?id=4dcbfd77b
         */
        xcb_disconnect(restore_conn);
        delete xcb_watcher;
        delete xcb_prepare;
    }

    int screen;
    restore_conn = x_connect(&screen);
    /* Null for libxcb's handle of a connection that couldn't be made. */
    restore_xconn = xcb::xcbcompat::connection(restore_conn);
    if (restore_xconn == nullptr || restore_xconn->connection_has_error()) {
        if (restore_conn != nullptr) {
            xcb_disconnect(restore_conn);
        }
#ifdef I3_ASAN_ENABLED
        __lsan_do_leak_check();
#endif
        errx(EXIT_FAILURE, "Cannot open display");
    }
    restore_drawing.emplace(restore_conn, xcb_aux_get_screen(restore_conn, screen));

    xcb_watcher = new ev_io{};
    xcb_prepare = new ev_prepare{};

    ev_io_init(xcb_watcher, restore_xcb_got_event, restore_xconn->get_file_descriptor(), EV_READ);
    ev_io_start(global.main_loop, xcb_watcher);

    ev_prepare_init(xcb_prepare, restore_xcb_prepare_cb);
    ev_prepare_start(global.main_loop, xcb_prepare);
}

static void update_placeholder_contents(X &conn, placeholder_state *state) {
    color_t const foreground = global.configManager->config->client.placeholder.text;
    color_t const background = global.configManager->config->client.placeholder.background;

    state->surface->draw_util_clear_surface(background);

    // TODO: make i3font functions per-connection, at least these two for now…?
    /* A round trip, as xcb_aux_sync() makes. */
    std::ignore = xcb::xproto::get_input_focus(restore_xconn).get();

    int n = 0;
    for (auto &swallows : state->con->swallow) {
        char *serialized = nullptr;

#define APPEND_REGEX(re_name)                                                                                                                                \
    do {                                                                                                                                                     \
        if (swallows->re_name.get() != nullptr) {                                                                                                            \
            sasprintf(&serialized, "%s%s" #re_name "=\"%s\"", (serialized ? serialized : "["), (serialized ? " " : ""), swallows->re_name->pattern.c_str()); \
        }                                                                                                                                                    \
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
        DLOG(fmt::sprintf("con %p (placeholder 0x%08x) line %d: %s\n", fmt::ptr(state->con), state->window, n, serialized));

        std::string str{serialized};
        state->surface->draw_util_text(*global.configManager->config->font, str, foreground, background,
                                       logical_px(global.x->root_screen, 2),
                                       (n * (global.configManager->config->font->height + logical_px(global.x->root_screen, 2))) + logical_px(global.x->root_screen, 2),
                                       state->rect.width - 2 * logical_px(global.x->root_screen, 2));
        n++;
        free(serialized);
    }

    // TODO: render the watch symbol in a bigger font
    std::string line{"⌚"};
    int text_width = global.configManager->config->font->predict_text_width(line);
    int x = (state->rect.width / 2) - (text_width / 2);
    int y = (state->rect.height / 2) - (global.configManager->config->font->height / 2);
    state->surface->draw_util_text(*global.configManager->config->font, line, foreground, background, x, y, text_width);
    std::ignore = xcb::xproto::get_input_focus(restore_xconn).get();
}

static void open_placeholder_window(X &conn, Con *con) {
    if (con->con_is_leaf() &&
        (con->get_window() == nullptr || con->get_window()->id == XCB_NONE) &&
        !con->swallow.empty() &&
        con->type == CT_CON) {
        auto const values = xcb::xproto::CreateWindowValueList{}
                                .with_background_pixel(global.configManager->config->client.placeholder.background.colorpixel)
                                .with_event_mask(xcb::xproto::event_mask_t::EVENT_MASK_EXPOSURE | xcb::xproto::event_mask_t::EVENT_MASK_STRUCTURE_NOTIFY);
        xcb_window_t placeholder = create_window(
            *restore_xconn,
            con->rect,
            XCB_COPY_FROM_PARENT,
            XCB_COPY_FROM_PARENT,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            XCURSOR_CURSOR_POINTER,
            true,
            values);
        /* Make i3 not focus this window. */
        xcb::icccm::wm_hints_t hints{};
        hints.set_none();
        hints.set_input(false);
        xcb::icccm::set_wm_hints(restore_xconn, placeholder, hints);
        /* Set the same name as was stored in the layout file. While perhaps
         * slightly confusing in the first instant, this brings additional
         * clarity to which placeholder is waiting for which actual window. */
        if (!con->name.empty()) {
            x_change_property(*restore_xconn, xcb::xproto::prop_mode_t::PROP_MODE_REPLACE, placeholder,
                              i3::atoms[i3::Atom::_NET_WM_NAME], i3::atoms[i3::Atom::UTF8_STRING], std::string_view{con->name});
        }
        DLOG(fmt::sprintf("Created placeholder window 0x%08x for leaf container %p / %s\n",
                          placeholder, fmt::ptr(con), con->name));

        auto state = std::make_unique<placeholder_state>();
        state->window = placeholder;
        state->con = con;
        state->rect = con->rect;

        state->surface = std::make_unique<surface_t>(*restore_drawing, placeholder, get_visualtype(restore_drawing->screen()), state->rect.width, state->rect.height);
        update_placeholder_contents(conn, state.get());
        states.push_back(std::move(state));

        /* create temporary id swallow to match the placeholder */
        auto temp_id = std::make_unique<Match>();
        temp_id->dock = M_DONTCHECK;
        temp_id->id = placeholder;
        con->swallow.push_front(std::move(temp_id));
    }

    for (auto &child : con->nodes) {
        open_placeholder_window(conn, child);
    }
    if (dynamic_cast<WorkspaceCon *>(con)) {
        for (auto &child : dynamic_cast<WorkspaceCon *>(con)->floating_windows) {
            open_placeholder_window(conn, child);
        }
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
    for (auto &child : parent->nodes) {
        open_placeholder_window(*global.x, child);
    }
    if (dynamic_cast<WorkspaceCon *>(parent)) {
        for (auto &child : dynamic_cast<WorkspaceCon *>(parent)->floating_windows) {
            open_placeholder_window(*global.x, child);
        }
    }

    restore_xconn->flush();
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
        if (state->window != placeholder) {
            continue;
        }

        xcb::xproto::destroy_window(restore_xconn, state->window);
        state->surface.reset();
        state_ptr = states.erase(state_ptr);
        DLOG(fmt::sprintf("placeholder window 0x%08x destroyed.\n", placeholder));
        return true;
    }

    DLOG(fmt::sprintf("0x%08x is not a placeholder window, ignoring.\n", placeholder));
    return false;
}

static void expose_event(xcb::xproto::expose_event_t *event) {
    for (auto &state : states) {
        if (state->window != event->window) {
            continue;
        }

        DLOG(fmt::sprintf("refreshing window 0x%08x contents (con %p)\n", state->window, fmt::ptr(state->con)));

        update_placeholder_contents(*global.x, state.get());

        return;
    }

    ELOG(fmt::sprintf("Received ExposeEvent for unknown window 0x%08x\n", event->window));
}

/*
 * Window size has changed. Update the width/height, then recreate the back
 * buffer pixmap and the accompanying graphics context and force an immediate
 * re-rendering.
 *
 */
static void configure_notify(xcb::xproto::configure_notify_event_t *event) {
    for (auto &state : states) {
        if (state->window != event->window) {
            continue;
        }

        DLOG(fmt::sprintf("ConfigureNotify: window 0x%08x has now width=%d, height=%d (con %p)\n",
                          state->window, event->width, event->height, fmt::ptr(state->con)));

        state->rect.width = event->width;
        state->rect.height = event->height;

        state->surface->draw_util_surface_set_size(state->rect.width, state->rect.height);

        update_placeholder_contents(*global.x, state.get());

        return;
    }

    ELOG(fmt::sprintf("Received ConfigureNotify for unknown window 0x%08x\n", event->window));
}

static void restore_handle_event(int type, xcb::generic_event_t *event) {
    switch (type) {
        case XCB_EXPOSE:
            if (auto *expose = reinterpret_cast<xcb::xproto::expose_event_t *>(event); expose->count == 0) {
                expose_event(expose);
            }

            break;
        case XCB_CONFIGURE_NOTIFY:
            configure_notify(reinterpret_cast<xcb::xproto::configure_notify_event_t *>(event));
            break;
        default:
            DLOG(fmt::sprintf("Received unhandled X11 event of type %d\n", type));
            break;
    }
}
