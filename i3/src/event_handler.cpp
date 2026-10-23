module;
#include <ev.h>
#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import log;

/*
 * This callback is only a dummy, see xcb_prepare_cb.
 * See also man libev(3): "ev_prepare" and "ev_check" - customise your event loop
 *
 */
static void xcb_got_event(EV_P_ ev_io *w, int revents) {
    /* empty, because xcb_prepare_cb are used */
}

/*
 * Called just before the event loop sleeps. Ensures xcb’s incoming and outgoing
 * queues are empty so that any activity will trigger another event loop
 * iteration, and hence another xcb_prepare_cb invocation.
 *
 */
void EventHandler::xcb_prepare_cb(EV_P_ ev_prepare *w, int revents) {
    /* Process all queued (and possibly new) events before the event loop
       sleeps. */
    while (auto const event = x.connection().poll_for_event()) {
        if (event->response_type == 0) {
            /* The 16-bit sequence, as before (task 5.3 changes the comparison). */
            if (handlers.event_is_ignored(event->sequence, 0)) {
                DLOG(fmt::sprintf("Expected X11 Error received for sequence %x\n", event->sequence));
            } else {
                auto const *error = reinterpret_cast<xcb::generic_error_t const *>(event.get());
                DLOG(fmt::sprintf("X11 Error received (probably harmless)! sequence 0x%x, error_code = %d\n",
                                  error->sequence, error->error_code));
            }
            continue;
        }

        /* Strip off the highest bit (set if the event is generated) */
        int type = (event->response_type & 0x7F);

        /* The handlers take libxcb's event types (task 5.2), whose layouts are xcbpp's. */
        handlers.handle_event(type, reinterpret_cast<xcb_generic_event_t *>(event.get()));
    }

    /* Flush all queued events to X11. */
    x.connection().flush();
}

/**
 * Enable or disable the main X11 event handling function.
 * This is used by drag_pointer() which has its own, modal event handler, which
 * takes precedence over the normal event handler.
 *
 */
void EventHandler::main_set_x11_cb(bool enable) {
    DLOG(fmt::sprintf("Setting main X11 callback to enabled=%d\n", enable));
    if (enable) {
        ev_prepare_start(global.main_loop, xcb_prepare);
        /* Trigger the watcher explicitly to handle all remaining X11 events.
         * drag_pointer()’s event handler exits in the middle of the loop. */
        ev_feed_event(global.main_loop, xcb_prepare, 0);
    } else {
        ev_prepare_stop(global.main_loop, xcb_prepare);
    }
}

/*
 * (One-shot) Handler for all signals with default action "Term", see signal(7)
 *
 * Exits the program gracefully.
 *
 */
void handle_term_signal(EV_P_ ev_signal *signal, int revents) {
    /* We exit gracefully here in the sense that cleanup handlers
     * installed via atexit are invoked. */
    exit(128 + signal->signum);
}

/*
 * Set up handlers for all signals with default action "Term", see signal(7)
 *
 */
void EventHandler::setup_term_handlers() {
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
        ev_signal_start(global.main_loop, &signal_watcher);
        /* The signal handlers should not block ev_run from returning
         * and so none of the signal handlers should hold a reference to
         * the main loop. */
        ev_unref(global.main_loop);
    }
}

EventHandler::EventHandler(X &x, PropertyHandlers &handlers)
    : handlers(handlers), x(x) {
    /* main() initialized global.main_loop. */
    xcb_watcher = new ev_io();
    xcb_prepare = new ev_prepare();
    xcb_prepare->data = this;

    ev_io_init(xcb_watcher, xcb_got_event, x.connection().get_file_descriptor(), EV_READ);
    ev_io_start(global.main_loop, xcb_watcher);

    ev_prepare_init(xcb_prepare, [](EV_P_ ev_prepare *w, int revents) { ((EventHandler *)w->data)->xcb_prepare_cb(EV_A_ w, revents); });
    ev_prepare_start(global.main_loop, xcb_prepare);
}

void EventHandler::loop() {
    ev_run(global.main_loop, 0);
}

EventHandler::~EventHandler() {
    // ev_loop_destroy(global.main_loop);
    //  Since we cannot guarantee this is destroyed after other classes usin main_loop, lets not delete it here
    //  Solution would be to add explicit dependencies to this class from those.
}
