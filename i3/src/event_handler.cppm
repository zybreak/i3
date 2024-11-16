module;
#include <ev.h>
#include <err.h>
#include <xcb/xcb.h>
#include <fmt/printf.h>
export module i3:event_handler;

import log;
import :handlers;
import :x;

export {
    // TODO: zybreak Rename to Event or EventLoop or something
    class EventHandler {
      private:
        PropertyHandlers &handlers;
        X &x;
        /* We keep the xcb_prepare watcher around to be able to enable and disable it
         * temporarily for drag_pointer(). */
        ev_prepare *xcb_prepare;
        ev_io *xcb_watcher;
        void xcb_prepare_cb(EV_P_ ev_prepare *w, int revents);

      public:
        EventHandler() = delete;
        EventHandler(EventHandler const &) = delete;
        EventHandler(X &x, PropertyHandlers &handlers);
        ~EventHandler();

        void loop();

        /**
         * Set up handlers for all signals with default action "Term", see signal(7)
         *
         */
        void setup_term_handlers();

        /**
         * Enable or disable the main X11 event handling function.
         * This is used by drag_pointer() which has its own, modal event handler, which
         * takes precedence over the normal event handler.
         *
         */
        void main_set_x11_cb(bool enable);
    };

    /*
     * (One-shot) Handler for all signals with default action "Term", see signal(7)
     *
     * Exits the program gracefully.
     *
     */
    void handle_term_signal(EV_P_ ev_signal * signal, int revents);
}
