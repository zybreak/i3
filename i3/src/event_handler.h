#ifndef I3_EVENT_HANDLER_H
#define I3_EVENT_HANDLER_H

#include "ev.h"
#include "x.h"

class EventHandler {
   public:
    explicit EventHandler(X *x);
    ~EventHandler();

    void mainLoop();
};


/**
 * Enable or disable the main X11 event handling function.
 * This is used by drag_pointer() which has its own, modal event handler, which
 * takes precedence over the normal event handler.
 *
 */
void main_set_x11_cb(bool enable);

/*
 * (One-shot) Handler for all signals with default action "Term", see signal(7)
 *
 * Exits the program gracefully.
 *
 */
void handle_term_signal(struct ev_loop *loop, ev_signal *signal, int revents);

/*
 * Set up handlers for all signals with default action "Term", see signal(7)
 *
 */
void setup_term_handlers();

#endif  // I3_EVENT_HANDLER_H
