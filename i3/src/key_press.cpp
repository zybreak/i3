/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * key_press.c: key press handler
 *
 */


#include <xcb/xcb.h>

#include "i3string.h"
#include "log.h"
#include "draw.h"
#include "key_press.h"
#include "bindings.h"
#include "global.h"

/*
 * There was a KeyPress or KeyRelease (both events have the same fields). We
 * compare this key code with our bindings table and pass the bound action to
 * parse_command().
 *
 */
void handle_key_press(xcb_key_press_event_t *event) {
    const bool key_release = (event->response_type == XCB_KEY_RELEASE);

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("%s %d, state raw = 0x%x\n",  (key_release ? "KeyRelease" : "KeyPress"), event->detail, event->state));

    Binding *bind = get_binding_from_xcb_event((xcb_generic_event_t *)event);

    /* if we couldn't find a binding, we are done */
    if (bind == nullptr)
        return;

    run_binding(bind, nullptr);
}
