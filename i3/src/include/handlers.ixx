/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * handlers.c: Small handlers for various events (keypresses, focus changes,
 *             …).
 *
 */
module;
#include <config.h>

#include <xcb/randr.h>
export module i3:handlers;

import std;

class Con;
class X;

export {
struct Ignore_Event {
    int sequence;
    int response_type;
    time_t added;
};

/* Returns false if the event could not be processed (e.g. the window could not
 * be found), true otherwise */
using cb_property_handler_t = bool (*)(Con *con, xcb_get_property_reply_t *property);

struct property_handler_t {
    xcb_atom_t atom;
    uint32_t long_len;
    cb_property_handler_t cb;

    property_handler_t(
        xcb_atom_t _atom,
        uint32_t _long_len,
        cb_property_handler_t _cb) : atom(_atom), long_len(_long_len), cb(_cb) {}
};

class PropertyHandlers {
   private:
    std::vector<property_handler_t> property_handlers{};
    std::vector<std::unique_ptr<Ignore_Event>> ignore_events{};
    void handle_enter_notify(xcb_enter_notify_event_t *event);
    static void handle_motion_notify(xcb_motion_notify_event_t *event);
    static void handle_mapping_notify(xcb_mapping_notify_event_t *event);
    void handle_map_request(xcb_map_request_event_t *event);
    static void handle_configure_request(xcb_configure_request_event_t *event);
    static void handle_screen_change(xcb_generic_event_t *e);
    void handle_unmap_notify_event(xcb_unmap_notify_event_t *event);
    void handle_destroy_notify_event(xcb_destroy_notify_event_t *event);
    static void handle_expose_event(xcb_expose_event_t *event);
    void handle_client_message(xcb_client_message_event_t *event);
    static void handle_focus_in(xcb_focus_in_event_t *event);
    static void handle_configure_notify(xcb_configure_notify_event_t *event);
    void property_notify(xcb_property_notify_event_t *event);
    static void handle_key_press(xcb_key_press_event_t *event);

   public:
    X *x;

    /**
     * Sets the appropriate atoms for the property handlers after the atoms were
     * received from X11
     *
     */
    explicit PropertyHandlers(X *x);

    /**
     * Adds the given sequence to the list of events which are ignored.
     * If this ignore should only affect a specific response_type, pass
     * response_type, otherwise, pass -1.
     *
     * Every ignored sequence number gets garbage collected after 5 seconds.
     *
     */
    void add_ignore_event(int sequence, int response_type);

    /**
     * Checks if the given sequence is ignored and returns true if so.
     *
     */
    bool event_is_ignored(int sequence, int response_type);

    /**
     * Takes an xcb_generic_event_t and calls the appropriate handler, based on the
     * event type.
     *
     */
    void handle_event(int type, xcb_generic_event_t *event);

};
}