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
#include <cassert>
#include <climits>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

#include "i3_ipc/i3-ipc.h"
#include "i3.h"
#include "atoms.h"

#include <ctime>

#include <xcb/randr.h>
#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-monitor.h>
#include <memory>
#include <mutex>
#include <algorithm>
#include <ranges>
#include <span>
#include <config.h>
#include <fmt/printf.h>
module i3;

import log;
import utils;

#define FREE(pointer)   \
    do {                \
        free(pointer);  \
        pointer = NULL; \
    } while (0)

/* After mapping/unmapping windows, a notify event is generated. However, we don’t want it,
   since it’d trigger an infinite loop of switching between the different windows when
   changing workspaces */
static std::mutex mtx;

static bool window_name_changed(i3Window *window, char *old_name) {
    if ((old_name == nullptr) && (window->name == nullptr)) {
        return false;
    }

    /* Either the old or the new one is NULL, but not both. */
    if ((old_name == nullptr) ^ (window->name == nullptr)) {
        return true;
    }

    return (strcmp(old_name, window->name->get_utf8()) != 0);
}

/*
 * Adds the given sequence to the list of events which are ignored.
 * If this ignore should only affect a specific response_type, pass
 * response_type, otherwise, pass -1.
 *
 * Every ignored sequence number gets garbage collected after 5 seconds.
 *
 */
void PropertyHandlers::add_ignore_event(const int sequence, const int response_type) {
    std::lock_guard<std::mutex> guard(mtx);
    auto event = std::make_unique<Ignore_Event>();

    event->sequence = sequence;
    event->response_type = response_type;
    event->added = time(nullptr);

    ignore_events.insert(ignore_events.begin(), std::move(event));
}

/*
 * Checks if the given sequence is ignored and returns true if so.
 *
 */
bool PropertyHandlers::event_is_ignored(const int sequence, const int response_type) {
    std::lock_guard<std::mutex> guard(mtx);

    time_t now = time(nullptr);
    ignore_events.erase(std::remove_if(ignore_events.begin(), ignore_events.end(), [now](const auto &it) { return (now - it->added) > 5; }), ignore_events.end());

    return std::any_of(
        ignore_events.begin(),
        ignore_events.end(),
        [&response_type, &sequence](const auto &event) {
            return (event->sequence == sequence && (event->response_type == -1 || event->response_type == response_type));
        });
}

/*
 * Called when a window changes its title
 *
 */
static bool handle_windowname_change(Con *con, xcb_get_property_reply_t *prop) {
    char *old_name = (con->window->name != nullptr ? sstrdup(con->window->name->get_utf8()) : nullptr);

    con->window->window_update_name(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name)) {
        ipc_send_window_event("title", con);
    }

    FREE(old_name);

    return true;
}

/*
 * Handles legacy window name updates (WM_NAME), see also src/window.c,
 * window_update_name_legacy().
 *
 */
static bool handle_windowname_change_legacy(Con *con, xcb_get_property_reply_t *prop) {
    char *old_name = (con->window->name != nullptr ? sstrdup(con->window->name->get_utf8()) : nullptr);

    con->window->window_update_name_legacy(prop);

    con = remanage_window(con);

    x_push_changes(global.croot);

    if (window_name_changed(con->window, old_name)) {
        ipc_send_window_event("title", con);
    }
    FREE(old_name);

    return true;
}

/*
 * Called when a window changes its WM_WINDOW_ROLE.
 *
 */
static bool handle_windowrole_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_role(prop);

    con = remanage_window(con);

    return true;
}

/*
 * Handles the WM_HINTS property for extracting the urgency state of the window.
 *
 */
static bool handle_hints(Con *con, xcb_get_property_reply_t *reply) {
    bool urgency_hint;
    con->window->window_update_hints(reply, &urgency_hint);
    con_set_urgency(con, urgency_hint);
    tree_render();
    return true;
}

/*
 * Handles the size hints set by a window, but currently only the part necessary for displaying
 * clients proportionally inside their frames (mplayer for example)
 *
 * See ICCCM 4.1.2.3 for more details
 *
 */
static bool handle_normal_hints(Con *con, xcb_get_property_reply_t *reply) {
    bool changed = con->window->window_update_normal_hints(reply, nullptr);

    if (changed) {
        Con *floating = con->con_inside_floating();
        if (floating) {
            floating_check_size(con, false);
            tree_render();
        }
    }

    return true;
}


static bool handle_window_type(Con *con, xcb_get_property_reply_t *reply) {
    con->window->window_update_type(reply);
    return true;
}

/*
 * Handles the transient for hints set by a window, signalizing that this window is a popup window
 * for some other window.
 *
 * See ICCCM 4.1.2.6 for more details
 *
 */
static bool handle_transient_for(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_transient_for(prop);
    return true;
}

/*
 * Handles changes of the WM_CLIENT_LEADER atom which specifies if this is a
 * toolwindow (or similar) and to which window it belongs (logical parent).
 *
 */
static bool handle_clientleader_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_leader(prop);
    return true;
}


/*
 * Handles the WM_CLASS property for assignments and criteria selection.
 *
 */
static bool handle_class_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_class(prop);
    con = remanage_window(con);
    return true;
}

/*
 * Handles the WM_CLIENT_MACHINE property for assignments and criteria selection.
 *
 */
static bool handle_machine_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_machine(prop);
    con = remanage_window(con);
    return true;
}

/*
 * Handles the _MOTIF_WM_HINTS property of specifing window deocration settings.
 *
 */
static bool handle_motif_hints_change(Con *con, xcb_get_property_reply_t *prop) {
    border_style_t motif_border_style;
    update_motif_hints(prop, &motif_border_style);

    if (motif_border_style != con->border_style && motif_border_style != BS_NORMAL) {
        DLOG(fmt::sprintf("Update border style of con %p to %d\n", (void *)con, motif_border_style));
        con_set_border_style(con, motif_border_style, con->current_border_width);

        x_push_changes(global.croot);
    }

    return true;
}

/*
 * Handles the _NET_WM_STRUT_PARTIAL property for allocating space for dock clients.
 *
 */
static bool handle_strut_partial_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_strut_partial(prop);

    /* we only handle this change for dock clients */
    if (con->parent == nullptr || con->parent->type != CT_DOCKAREA) {
        return true;
    }

    Con *search_at = global.croot;
    Con *output = con->con_get_output();
    if (output != nullptr) {
        DLOG(fmt::sprintf("Starting search at output %s\n", output->name));
        search_at = output;
    }

    /* find out the desired position of this dock window */
    if (con->window->reserved.top > 0 && con->window->reserved.bottom == 0) {
        DLOG("Top dock client\n");
        con->window->dock = i3Window::W_DOCK_TOP;
    } else if (con->window->reserved.top == 0 && con->window->reserved.bottom > 0) {
        DLOG("Bottom dock client\n");
        con->window->dock = i3Window::W_DOCK_BOTTOM;
    } else {
        DLOG("Ignoring invalid reserved edges (_NET_WM_STRUT_PARTIAL), using position as fallback:\n");
        if (con->geometry.y < (search_at->rect.height / 2)) {
            DLOG(fmt::sprintf("geom->y = %d < rect.height / 2 = %d, it is a top dock client\n",
                              con->geometry.y, (search_at->rect.height / 2)));
            con->window->dock = i3Window::W_DOCK_TOP;
        } else {
            DLOG(fmt::sprintf("geom->y = %d >= rect.height / 2 = %d, it is a bottom dock client\n",
                              con->geometry.y, (search_at->rect.height / 2)));
            con->window->dock = i3Window::W_DOCK_BOTTOM;
        }
    }

    /* find the dockarea */
    Con *dockarea = con_for_window(search_at, con->window, nullptr);
    assert(dockarea != nullptr);

    /* attach the dock to the dock area */
    con->con_detach();
    con->con_attach(dockarea, true);

    tree_render();

    return true;
}

/*
 * Handles the _I3_FLOATING_WINDOW property to properly run assignments for
 * floating window changes.
 *
 * This is needed to correctly run the assignments after changes in floating
 * windows which are triggered by user commands (floating enable | disable). In
 * that case, we can't call run_assignments because it will modify the parser
 * state when it needs to parse the user-specified action, breaking the parser
 * state for the original command.
 *
 */
static bool handle_i3_floating(Con *con, xcb_get_property_reply_t *prop) {
    DLOG(fmt::sprintf("floating change for con %p\n", (void *)con));

    remanage_window(con);

    return true;
}

static bool handle_windowicon_change(Con *con, xcb_get_property_reply_t *prop) {
    con->window->window_update_icon(prop);

    x_push_changes(global.croot);

    return true;
}


/*
 * Sets the appropriate atoms for the property handlers after the atoms were
 * received from X11
 *
 */
PropertyHandlers::PropertyHandlers(X *x) : x{x} {
    sn_monitor_context_new(sndisplay, x->conn->default_screen(), startup_monitor_event, nullptr, nullptr);

    property_handlers.emplace_back(A__NET_WM_NAME, 128, handle_windowname_change);
    property_handlers.emplace_back(XCB_ATOM_WM_HINTS, UINT_MAX, handle_hints);
    property_handlers.emplace_back(XCB_ATOM_WM_NAME, 128, handle_windowname_change_legacy);
    property_handlers.emplace_back(XCB_ATOM_WM_NORMAL_HINTS, UINT_MAX, handle_normal_hints);
    property_handlers.emplace_back(A_WM_CLIENT_LEADER, UINT_MAX, handle_clientleader_change);
    property_handlers.emplace_back(XCB_ATOM_WM_TRANSIENT_FOR, UINT_MAX, handle_transient_for);
    property_handlers.emplace_back(A_WM_WINDOW_ROLE, 128, handle_windowrole_change);
    property_handlers.emplace_back(XCB_ATOM_WM_CLASS, 128, handle_class_change);
    property_handlers.emplace_back(A__NET_WM_STRUT_PARTIAL, UINT_MAX, handle_strut_partial_change);
    property_handlers.emplace_back(A__NET_WM_WINDOW_TYPE, UINT_MAX, handle_window_type);
    property_handlers.emplace_back(A_I3_FLOATING_WINDOW, UINT_MAX, handle_i3_floating);
    property_handlers.emplace_back(XCB_ATOM_WM_CLIENT_MACHINE, 128, handle_machine_change);
    property_handlers.emplace_back(A__MOTIF_WM_HINTS, 5 * sizeof(uint64_t), handle_motif_hints_change);
    property_handlers.emplace_back(A__NET_WM_ICON, UINT_MAX, handle_windowicon_change);
}

/*
 * Called with coordinates of an enter_notify event or motion_notify event
 * to check if the user crossed virtual screen boundaries and adjust the
 * current workspace, if so.
 *
 */
void PropertyHandlers::check_crossing_screen_boundary(uint32_t x, uint32_t y) {
    Output *output;

    /* If the user disable focus follows mouse, we have nothing to do here */
    if (config.disable_focus_follows_mouse) {
        return;
    }

    if ((output = global.randr->get_output_containing(x, y)) == nullptr) {
        ELOG("ERROR: No such screen\n");
        return;
    }

    if (output->con == nullptr) {
        ELOG("ERROR: The screen is not recognized by i3 (no container associated)\n");
        return;
    }

    /* Focus the output on which the user moved their cursor */
    Con *old_focused = global.focused;
    Con *next = con_descend_focused(output->con->output_get_content());
    /* Since we are switching outputs, this *must* be a different workspace, so
     * call workspace_show() */
    workspace_show(next->con_get_workspace());
    next->con_focus();

    /* If the focus changed, we re-render to get updated decorations */
    if (old_focused != global.focused) {
        tree_render();
    }
}


/*
 * Takes an xcb_generic_event_t and calls the appropriate handler, based on the
 * event type.
 *
 */
void PropertyHandlers::handle_event(int type, xcb_generic_event_t *event) {
    x_connection *conn = *x;
    if (type != XCB_MOTION_NOTIFY)
        DLOG(fmt::sprintf("event type %d, xkb_base %d\n", type, global.xkb->xkb_base));

    if (global.randr->randr_base > -1 &&
        type == global.randr->randr_base + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
        handle_screen_change(event);
        return;
    }

    if (global.xkb->xkb_base > -1 && type == global.xkb->xkb_base) {
        DLOG("xkb event, need to handle it.\n");

        auto *state = (xcb_xkb_state_notify_event_t *)event;
        if (state->xkbType == XCB_XKB_NEW_KEYBOARD_NOTIFY) {
            DLOG(fmt::sprintf("xkb new keyboard notify, sequence %d, time %d\n", state->sequence, state->time));
            global.keysyms->key_symbols_alloc();
            if (((xcb_xkb_new_keyboard_notify_event_t *)event)->changed & XCB_XKB_NKN_DETAIL_KEYCODES) {
                load_keymap();
            }
            ungrab_all_keys(conn);
            translate_keysyms();
            grab_all_keys(conn);
        } else if (state->xkbType == XCB_XKB_MAP_NOTIFY) {
            if (event_is_ignored(event->sequence, type)) {
                DLOG(fmt::sprintf("Ignoring map notify event for sequence %d.\n", state->sequence));
            } else {
                DLOG(fmt::sprintf("xkb map notify, sequence %d, time %d\n", state->sequence, state->time));
                add_ignore_event(event->sequence, type);
                global.keysyms->key_symbols_alloc();
                ungrab_all_keys(conn);
                translate_keysyms();
                grab_all_keys(conn);
                load_keymap();
            }
        } else if (state->xkbType == XCB_XKB_STATE_NOTIFY) {
            DLOG(fmt::sprintf("xkb state group = %d\n", state->group));
            if (global.xkb_current_group == state->group) {
                return;
            }
            global.xkb_current_group = state->group;
            ungrab_all_keys(conn);
            grab_all_keys(conn);
        }

        return;
    }

    if (global.shape->shape_supported && type == global.shape->shape_base + XCB_SHAPE_NOTIFY) {
        auto *shape = (xcb_shape_notify_event_t *)event;

        DLOG(fmt::sprintf("shape_notify_event for window 0x%08x, shape_kind = %d, shaped = %d\n",
                          shape->affected_window, shape->shape_kind, shape->shaped));

        Con *con = con_by_window_id(shape->affected_window);
        if (con == nullptr) {
            LOG(fmt::sprintf("Not a managed window 0x%08x, ignoring shape_notify_event\n",
                             shape->affected_window));
            return;
        }

        if (shape->shape_kind == XCB_SHAPE_SK_BOUNDING ||
            shape->shape_kind == XCB_SHAPE_SK_INPUT) {
            x_set_shape(con, (xcb_shape_sk_t)shape->shape_kind, shape->shaped);
        }

        return;
    }

    switch (type) {
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
            handle_key_press((xcb_key_press_event_t *)event);
            break;

        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            handle_button_press((xcb_button_press_event_t *)event);
            break;

        case XCB_MAP_REQUEST:
            handle_map_request((xcb_map_request_event_t *)event);
            break;

        case XCB_UNMAP_NOTIFY:
            handle_unmap_notify_event((xcb_unmap_notify_event_t *)event);
            break;

        case XCB_DESTROY_NOTIFY:
            handle_destroy_notify_event((xcb_destroy_notify_event_t *)event);
            break;

        case XCB_EXPOSE:
            handle_expose_event((xcb_expose_event_t *)event);
            break;

        case XCB_MOTION_NOTIFY:
            handle_motion_notify((xcb_motion_notify_event_t *)event);
            break;

        /* Enter window = user moved their mouse over the window */
        case XCB_ENTER_NOTIFY:
            handle_enter_notify((xcb_enter_notify_event_t *)event);
            break;

        /* Client message are sent to the root window. The only interesting
         * client message for us is _NET_WM_STATE, we honour
         * _NET_WM_STATE_FULLSCREEN and _NET_WM_STATE_DEMANDS_ATTENTION */
        case XCB_CLIENT_MESSAGE:
            handle_client_message((xcb_client_message_event_t *)event);
            break;

        /* Configure request = window tried to change size on its own */
        case XCB_CONFIGURE_REQUEST:
            handle_configure_request((xcb_configure_request_event_t *)event);
            break;

        /* Mapping notify = keyboard mapping changed (Xmodmap), re-grab bindings */
        case XCB_MAPPING_NOTIFY:
            handle_mapping_notify((xcb_mapping_notify_event_t *)event);
            break;

        case XCB_FOCUS_IN:
            handle_focus_in((xcb_focus_in_event_t *)event);
            break;

        case XCB_PROPERTY_NOTIFY: {
            property_notify((xcb_property_notify_event_t *)event);
            break;
        }

        case XCB_CONFIGURE_NOTIFY:
            handle_configure_notify((xcb_configure_notify_event_t *)event);
            break;

        default:
            // LOG(fmt::sprintf("Unhandled event of type %d\n",  type));
            break;
    }
}
