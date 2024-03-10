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

#include <vector>
#include <memory>
#include <mutex>

#include <xcb/randr.h>
export module i3:handlers;

class Con;
class X;

export {
    struct Ignore_Event {
        int sequence;
        int response_type;
        time_t added;
    };

    // TODO: zybreak Rename to EventHandlers
    class PropertyHandlers {
       private:
        X *x;

        /* After mapping/unmapping windows, a notify event is generated. However, we don’t want it,
           since it’d trigger an infinite loop of switching between the different windows when
           changing workspaces */
        std::mutex mtx{};

        std::vector<std::unique_ptr<Ignore_Event>> ignore_events{};

        /**
         * The button press X callback. This function determines whether the floating
         * modifier is pressed and where the user clicked (decoration, border, inside
         * the window).
         *
         * Then, route_click is called on the appropriate con.
         *
         */
        void handle_button_press(xcb_button_press_event_t *event);

        /**
         * When the user moves the mouse pointer onto a window, this callback gets called.
         */
        void handle_enter_notify(xcb_enter_notify_event_t *event);

        /**
         * When the user moves the mouse but does not change the active window
         * (e.g. when having no windows opened but moving mouse on the root screen
         * and crossing virtual screen boundaries), this callback gets called.
         */
        void handle_motion_notify(xcb_motion_notify_event_t *event);

        /**
         * When the user moves the mouse but does not change the active window
         * (e.g. when having no windows opened but moving mouse on the root screen
         * and crossing virtual screen boundaries), this callback gets called.
         */
        void handle_mapping_notify(xcb_mapping_notify_event_t *event);

        /**
         * A new window appeared on the screen (=was mapped), so let’s manage it.
         */
        void handle_map_request(xcb_map_request_event_t *event);

        /**
         * Gets triggered upon a RandR screen change event, that is when the user
         * changes the screen configuration in any way (mode, position, …)
         */
        void handle_screen_change(xcb_generic_event_t *e);

        /**
         * Our window decorations were unmapped. That means, the window will be killed
         * now, so we better clean up before.
         */
        void handle_unmap_notify_event(xcb_unmap_notify_event_t *event);

        /**
         * A destroy notify event is sent when the window is not unmapped, but
         * immediately destroyed (for example when starting a window and immediately
         * killing the program which started it).
         *
         * We just pass on the event to the unmap notify handler (by copying the
         * important fields in the event data structure).
         */
        void handle_destroy_notify_event(xcb_destroy_notify_event_t *event);

        /**
         * Expose event means we should redraw our windows (= title bar)
         */
        void handle_expose_event(xcb_expose_event_t *event);

        /**
         * Handle client messages (EWMH)
         */
        void handle_client_message(xcb_client_message_event_t *event);

        /**
         * Handles FocusIn events which are generated by clients (i3’s focus changes
         * don’t generate FocusIn events due to a different EventMask) and updates the
         * decorations accordingly.
         */
        void handle_focus_in(xcb_focus_in_event_t *event);

        /**
         * Log FocusOut events.
         */
        void handle_focus_out(xcb_focus_in_event_t *event);

        /**
         * Handles ConfigureNotify events for the root window, which are generated when
         * the monitor configuration changed.
         *
         */
        void handle_configure_notify(xcb_configure_notify_event_t *event);

        void property_notify(xcb_property_notify_event_t *event);

        /**
         * There was a KeyPress or KeyRelease (both events have the same fields). We
         * compare this key code with our bindings table and pass the bound action to
         * parse_command().
         */
        void handle_key_press(xcb_key_press_event_t *event);

       public:
        PropertyHandlers() = delete;

        /**
         * Sets the appropriate atoms for the property handlers after the atoms were
         * received from X11
         *
         */
        explicit PropertyHandlers(X *x);

        /**
         * Configure requests are received when the application wants to resize windows
         * on their own.
         *
         * We generate a synthetic configure notify event to signalize the client its
         * "new" position.
         */
        void handle_configure_request(xcb_configure_request_event_t *event);

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