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
#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/shape.h>
#include <xcb/randr.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-launcher.h>
#include <libsn/sn-monitor.h>
#undef SN_API_NOT_YET_FROZEN

#include <config.h>
#include <fmt/printf.h>
module i3;

import std;
import log;
import rect;
import :output;
import utils;
import i3ipc;

static const uint32_t _NET_WM_STATE_REMOVE = 0;
static const uint32_t _NET_WM_STATE_ADD = 1;
static const uint32_t _NET_WM_STATE_TOGGLE = 2;

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
    event->added = std::chrono::system_clock::now();

    ignore_events.insert(ignore_events.begin(), std::move(event));
}

/*
 * Checks if the given sequence is ignored and returns true if so.
 *
 */
bool PropertyHandlers::event_is_ignored(const int sequence, const int response_type) {
    using namespace std::literals;
    std::lock_guard<std::mutex> guard(mtx);

    auto now = std::chrono::system_clock::now();
    ignore_events.erase(std::remove_if(ignore_events.begin(), ignore_events.end(), [now](const auto &it) { return (now - it->added) > 5s; }), ignore_events.end());

    return std::any_of(
        ignore_events.begin(),
        ignore_events.end(),
        [&response_type, &sequence](const auto &event) {
            return (event->sequence == sequence && (event->response_type == -1 || event->response_type == response_type));
        });
}

/*
 * Called with coordinates of an enter_notify event or motion_notify event
 * to check if the user crossed virtual screen boundaries and adjust the
 * current workspace, if so.
 *
 */
static void check_crossing_screen_boundary(uint32_t x, uint32_t y) {
    Output *output;

    /* If the user disable focus follows mouse, we have nothing to do here */
    if (global.configManager->config->disable_focus_follows_mouse) {
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
    global.workspaceManager->workspace_show(next->con_get_workspace());
    next->con_focus();

    /* If the focus changed, we re-render to get updated decorations */
    if (old_focused != global.focused) {
        tree_render();
    }
}

/*
 * When the user moves the mouse pointer onto a window, this callback gets called.
 *
 */
void PropertyHandlers::handle_enter_notify(xcb_enter_notify_event_t *event) {
    Con *con;

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("enter_notify for %08x, mode = %d, detail %d, serial %d\n",
                      event->event, event->mode, event->detail, event->sequence));
    DLOG(fmt::sprintf("coordinates %d, %d\n", event->event_x, event->event_y));
    if (event->mode != XCB_NOTIFY_MODE_NORMAL) {
        DLOG("This was not a normal notify, ignoring\n");
        return;
    }
    /* Some events are not interesting, because they were not generated
     * actively by the user, but by reconfiguration of windows */
    if (event_is_ignored(event->sequence, XCB_ENTER_NOTIFY)) {
        DLOG("Event ignored\n");
        return;
    }

    bool enter_child = false;
    /* Get container by frame or by child window */
    if ((con = con_by_frame_id(event->event)) == nullptr) {
        con = con_by_window_id(event->event);
        enter_child = true;
    }

    /* If we cannot find the container, the user moved their cursor to the root
     * window. In this case and if they used it to a dock, we need to focus the
     * workspace on the correct output. */
    if (con == nullptr || con->parent->type == CT_DOCKAREA) {
        DLOG(fmt::sprintf("Getting screen at %d x %d\n", event->root_x, event->root_y));
        check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    /* see if the user entered the window on a certain window decoration */
    layout_t layout = (enter_child ? con->parent->layout : con->layout);
    if (layout == L_DEFAULT) {
        for (auto &child : con->nodes | std::views::reverse) {
            if (child->deco_rect.rect_contains(event->event_x, event->event_y)) {
                LOG(fmt::sprintf("using child %p / %s instead!\n", fmt::ptr(child), child->name));
                con = child;
                break;
            }
        }
    }

    if (configManager.config->disable_focus_follows_mouse) {
        return;
    }

    /* if this container is already focused, there is nothing to do. */
    if (con == global.focused) {
        return;
    }

    /* Get the currently focused workspace to check if the focus change also
     * involves changing workspaces. If so, we need to call workspace_show() to
     * correctly update state and send the IPC event. */
    WorkspaceCon *ws = con->con_get_workspace();
    if (ws != global.focused->con_get_workspace()) {
        workspaceManager.workspace_show(ws);
    }

    x.focused_id = XCB_NONE;
    con_descend_focused(con)->con_focus();
    tree_render();
}

/*
 * When the user moves the mouse but does not change the active window
 * (e.g. when having no windows opened but moving mouse on the root screen
 * and crossing virtual screen boundaries), this callback gets called.
 *
 */
void PropertyHandlers::handle_motion_notify(xcb_motion_notify_event_t *event) {
    global.last_timestamp = event->time;

    /* Skip events where the pointer was over a child window, we are only
     * interested in events on the root window. */
    if (event->child != XCB_NONE)
        return;

    Con *con = con_by_frame_id(event->event);
    if (con == nullptr) {
        DLOG("MotionNotify for an unknown container, checking if it crosses screen boundaries.\n");
        check_crossing_screen_boundary(event->root_x, event->root_y);
        return;
    }

    if (configManager.config->disable_focus_follows_mouse) {
        return;
    }

    if (con->layout != L_DEFAULT && con->layout != L_SPLITV && con->layout != L_SPLITH) {
        return;
    }

    /* see over which rect the user is */
    if (con->get_window() != nullptr) {
        if (con->deco_rect.rect_contains(event->event_x, event->event_y)) {
            /* We found the rect, let’s see if this window is focused */
            if (con::first(con->parent->focused) == con) {
                return;
            }

            con->con_focus();
            x_push_changes(global.croot);
            return;
        }
    } else {
        for (auto &current : con->nodes | std::views::reverse) {
            if (!current->deco_rect.rect_contains(event->event_x, event->event_y)) {
                continue;
            }

            /* We found the rect, let’s see if this window is focused */
            if (con::first(con->focused) == current) {
                return;
            }

            current->con_focus();
            x_push_changes(global.croot);
            return;
        }
    }
}

/*
 * Called when the keyboard mapping changes (for example by using Xmodmap),
 * we need to update our key bindings then (re-translate symbols).
 *
 */
void PropertyHandlers::handle_mapping_notify(xcb_mapping_notify_event_t *event) {
    if (event->request != XCB_MAPPING_KEYBOARD &&
        event->request != XCB_MAPPING_MODIFIER) {
        return;
    }

    DLOG("Received mapping_notify for keyboard or modifier mapping, re-grabbing keys\n");
    global.xkb->refresh_keyboard_mapping(event);

    x.xcb_numlock_mask = global.xkb->get_numlock_mask();

    ungrab_all_keys(&*x);
    translate_keysyms(xkb.keymap());
    grab_all_keys(&*x);
}

/*
 * A new window appeared on the screen (=was mapped), so let’s manage it.
 *
 */
void PropertyHandlers::handle_map_request(xcb_map_request_event_t *event) {
    try {
        auto attr = x.conn->get_window_attributes(event->window);

        DLOG(fmt::sprintf("window = 0x%08x, serial is %d.\n", event->window, event->sequence));
        add_ignore_event(event->sequence, -1);

        /* Don’t manage clients with the override_redirect flag */
        if (attr->override_redirect) {
            DLOG(fmt::format("Could not manage window = 0x%08x, has override redirect", event->window));
            return;
        }
        
        manage_window(event->window, attr->visual);
    } catch (...) {
        DLOG(fmt::format("Could not manage window = 0x%08x", event->window));
    }
}

/*
 * Configure requests are received when the application wants to resize windows
 * on their own.
 *
 * We generate a synthetic configure notify event to signalize the client its
 * "new" position.
 *
 */
void PropertyHandlers::handle_configure_request(xcb_configure_request_event_t *event) {
    DLOG(fmt::sprintf("window 0x%08x wants to be at %dx%d with %dx%d\n",
                      event->window, event->x, event->y, event->width, event->height));

    ConCon *con = con_by_window_id(event->window);

    /* For unmanaged windows, we just execute the configure request. As soon as
     * it gets mapped, we will take over anyways. */
    if (con == nullptr) {
        DLOG("Configure request for unmanaged window, can do that.\n");

        uint32_t mask = 0;
        uint32_t values[7];
        int c = 0;
#define COPY_MASK_MEMBER(mask_member, event_member) \
    do {                                            \
        if (event->value_mask & mask_member) {      \
            mask |= mask_member;                    \
            values[c++] = event->event_member;      \
        }                                           \
    } while (0)

        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_X, x);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_Y, y);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_WIDTH, width);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_HEIGHT, height);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_BORDER_WIDTH, border_width);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_SIBLING, sibling);
        COPY_MASK_MEMBER(XCB_CONFIG_WINDOW_STACK_MODE, stack_mode);

        xcb_configure_window(*x, event->window, mask, values);
        xcb_flush(*x);

        return;
    }

    DLOG("Configure request!\n");

    WorkspaceCon *workspace = con->con_get_workspace();
    Con *fullscreen = workspace ? workspace->con_get_fullscreen_covering_ws() : nullptr;

    if (fullscreen != con && con->con_is_floating() && con->con_is_leaf()) {
        /* we actually need to apply the size/position changes to the *parent*
         * container */
        Rect bsr = con_border_style_rect(con);
        Con *floatingcon = con->parent;
        Rect newrect = floatingcon->rect;

        if (event->value_mask & XCB_CONFIG_WINDOW_X) {
            newrect.x = event->x + (-1) * bsr.x;
            DLOG(fmt::sprintf("proposed x = %d, new x is %d\n", event->x, newrect.x));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_Y) {
            newrect.y = event->y + (-1) * bsr.y;
            DLOG(fmt::sprintf("proposed y = %d, new y is %d\n", event->y, newrect.y));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
            newrect.width = event->width + (-1) * bsr.width;
            newrect.width += con->border_width * 2;
            DLOG(fmt::sprintf("proposed width = %d, new width is %d (x11 border %d)\n",
                              event->width, newrect.width, con->border_width));
        }
        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
            newrect.height = event->height + (-1) * bsr.height;
            newrect.height += con->border_width * 2;
            DLOG(fmt::sprintf("proposed height = %d, new height is %d (x11 border %d)\n",
                              event->height, newrect.height, con->border_width));
        }

        DLOG("Container is a floating leaf node, will do that.\n");
        floating_reposition(floatingcon, newrect);
        return;
    }

    /* Dock windows can be reconfigured in their height and moved to another output. */
    if (con->parent && con->parent->type == CT_DOCKAREA) {
        DLOG(fmt::sprintf("Reconfiguring dock window (con = %p).\n", fmt::ptr(con)));
        if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
            DLOG(fmt::sprintf("Dock client wants to change height to %d, we can do that.\n", event->height));

            con->get_geometry().height = event->height;
            tree_render();
        }

        if (event->value_mask & XCB_CONFIG_WINDOW_X || event->value_mask & XCB_CONFIG_WINDOW_Y) {
            int16_t x = event->value_mask & XCB_CONFIG_WINDOW_X ? event->x : static_cast<int16_t>(con->get_geometry().x);
            int16_t y = event->value_mask & XCB_CONFIG_WINDOW_Y ? event->y : static_cast<int16_t>(con->get_geometry().y);

            Con *current_output = con->con_get_output();
            Output *target = randr.get_output_containing(x, y);
            if (target != nullptr && current_output != target->con) {
                DLOG(fmt::sprintf("Dock client is requested to be moved to output %s, moving it there.\n", target->output_primary_name()));
                Match *match;
                Con *nc = con_for_window(target->con, con->get_window(), &match);
                DLOG(fmt::sprintf("Dock client will be moved to container %p.\n", fmt::ptr(nc)));
                con->con_detach();
                con->con_attach(nc, false);

                tree_render();
            } else {
                DLOG("Dock client will not be moved, we only support moving it to another output.\n");
            }
        }
        goto out;
    }

    if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE) {
        DLOG(fmt::sprintf("window 0x%08x wants to be stacked %d\n", event->window, event->stack_mode));

        /* Emacs and IntelliJ Idea “request focus” by stacking their window
         * above all others. */
        if (event->stack_mode != XCB_STACK_MODE_ABOVE) {
            DLOG("stack_mode != XCB_STACK_MODE_ABOVE, ignoring ConfigureRequest\n");
            goto out;
        }

        if (fullscreen || !con->con_is_leaf()) {
            DLOG("fullscreen or not a leaf, ignoring ConfigureRequest\n");
            goto out;
        }

        if (workspace == nullptr) {
            DLOG("Window is not being managed, ignoring ConfigureRequest\n");
            goto out;
        }

        if (configManager.config->focus_on_window_activation == FOWA_FOCUS || (configManager.config->focus_on_window_activation == FOWA_SMART && workspace_is_visible(workspace))) {
            DLOG(fmt::sprintf("Focusing con = %p\n", fmt::ptr(con)));
            workspaceManager.workspace_show(workspace);
            con->con_activate_unblock();
            tree_render();
        } else if (configManager.config->focus_on_window_activation == FOWA_URGENT || (configManager.config->focus_on_window_activation == FOWA_SMART && !workspace_is_visible(workspace))) {
            DLOG(fmt::sprintf("Marking con = %p urgent\n", fmt::ptr(con)));
            con->con_set_urgency(true);
            con = remanage_window(con);
            tree_render();
        } else {
            DLOG(fmt::sprintf("Ignoring request for con = %p.\n", fmt::ptr(con)));
        }
    }

out:
    fake_absolute_configure_notify(con);
}

/*
 * Gets triggered upon a RandR screen change event, that is when the user
 * changes the screen configuration in any way (mode, position, …)
 *
 */
void PropertyHandlers::handle_screen_change(xcb_generic_event_t *e) {
    DLOG("RandR screen change\n");

    /* The geometry of the root window is used for “fullscreen global” and
     * changes when new outputs are added. */
    xcb_get_geometry_cookie_t cookie = xcb_get_geometry(*x, x.root);
    xcb_get_geometry_reply_t *reply = xcb_get_geometry_reply(*x, cookie, nullptr);
    if (reply == nullptr) {
        ELOG("Could not get geometry of the root window, exiting\n");
        exit(EXIT_FAILURE);
    }
    DLOG(fmt::sprintf("root geometry reply: (%d, %d) %d x %d\n", reply->x, reply->y, reply->width, reply->height));

    global.croot->rect.width = reply->width;
    global.croot->rect.height = reply->height;

    randr.randr_query_outputs();

    ipcManager.ipc_send_event("output", i3ipc::EVENT_OUTPUT, R"({"change":"unspecified"})");
}

/*
 * Our window decorations were unmapped. That means, the window will be killed
 * now, so we better clean up before.
 *
 */
void PropertyHandlers::handle_unmap_notify_event(xcb_unmap_notify_event_t *event) {
    DLOG(fmt::sprintf("UnmapNotify for 0x%08x (received from 0x%08x), serial %d\n", event->window, event->event, event->sequence));
    xcb_get_input_focus_cookie_t cookie;
    ConCon *con = con_by_window_id(event->window);
    if (con == nullptr) {
        /* This could also be an UnmapNotify for the frame. We need to
         * decrement the ignore_unmap counter. */
        Con *con = con_by_frame_id(event->window);
        if (con == nullptr) {
            LOG("Not a managed window, ignoring UnmapNotify event\n");
            return;
        }

        if (con->ignore_unmap > 0)
            con->ignore_unmap--;
        /* See the end of this function. */
        cookie = xcb_get_input_focus(*x);
        DLOG(fmt::sprintf("ignore_unmap = %d for frame of container %p\n", con->ignore_unmap, fmt::ptr(con)));
        goto ignore_end;
    }

    /* See the end of this function. */
    cookie = xcb_get_input_focus(**global.x);

    if (con->ignore_unmap > 0) {
        DLOG(fmt::sprintf("ignore_unmap = %d, dec\n", con->ignore_unmap));
        con->ignore_unmap--;
        goto ignore_end;
    }

    /* Since we close the container, we need to unset _NET_WM_DESKTOP and
     * _NET_WM_STATE according to the spec. */
    xcb_delete_property(**global.x, event->window, i3::atoms[i3::Atom::_NET_WM_DESKTOP]);
    xcb_delete_property(**global.x, event->window, i3::atoms[i3::Atom::_NET_WM_STATE]);

    tree_close_internal(con, kill_window_t::DONT_KILL_WINDOW, false);
    tree_render();

ignore_end:
    /* If the client (as opposed to i3) destroyed or unmapped a window, an
     * EnterNotify event will follow (indistinguishable from an EnterNotify
     * event caused by moving your mouse), causing i3 to set focus to whichever
     * window is now visible.
     *
     * In a complex stacked or tabbed layout (take two v-split containers in a
     * tabbed container), when the bottom window in tab2 is closed, the bottom
     * window of tab1 is visible instead. X11 will thus send an EnterNotify
     * event for the bottom window of tab1, while the focus should be set to
     * the remaining window of tab2.
     *
     * Therefore, we ignore all EnterNotify events which have the same sequence
     * as an UnmapNotify event. */
    add_ignore_event(event->sequence, XCB_ENTER_NOTIFY);

    /* Since we just ignored the sequence of this UnmapNotify, we want to make
     * sure that following events use a different sequence. When putting xterm
     * into fullscreen and moving the pointer to a different window, without
     * using GetInputFocus, subsequent (legitimate) EnterNotify events arrived
     * with the same sequence and thus were ignored (see ticket #609). */
    free(xcb_get_input_focus_reply(**global.x, cookie, nullptr));
}

/*
 * A destroy notify event is sent when the window is not unmapped, but
 * immediately destroyed (for example when starting a window and immediately
 * killing the program which started it).
 *
 * We just pass on the event to the unmap notify handler (by copying the
 * important fields in the event data structure).
 *
 */
void PropertyHandlers::handle_destroy_notify_event(xcb_destroy_notify_event_t *event) {
    DLOG(fmt::sprintf("destroy notify for 0x%08x, 0x%08x\n", event->event, event->window));

    xcb_unmap_notify_event_t unmap;
    unmap.sequence = event->sequence;
    unmap.event = event->event;
    unmap.window = event->window;

    handle_unmap_notify_event(&unmap);
}

/*
 * Expose event means we should redraw our windows (= title bar)
 *
 */
void PropertyHandlers::handle_expose_event(xcb_expose_event_t *event) {
    if (event->count != 0) {
        return;
    }

    Con *parent;

    DLOG(fmt::sprintf("window = %08x\n", event->window));

    if ((parent = con_by_frame_id(event->window)) == nullptr) {
        LOG("expose event for unknown window, ignoring\n");
        return;
    }

    /* Since we render to our surface on every change anyways, expose events
     * only tell us that the X server lost (parts of) the window contents. */
    draw_util_copy_surface(parent->frame_buffer.get(), parent->frame.get(),
            0, 0, 0, 0, parent->rect.width, parent->rect.height);
    xcb_flush(**global.x);
}

static const uint32_t _NET_WM_MOVERESIZE_SIZE_TOPLEFT = 0;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_TOP = 1;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_TOPRIGHT = 2;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_RIGHT = 3;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT = 4;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_BOTTOM = 5;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT = 6;
static const uint32_t _NET_WM_MOVERESIZE_SIZE_LEFT = 7;
static const uint32_t _NET_WM_MOVERESIZE_MOVE = 8;           /* movement only */
static const uint32_t _NET_WM_MOVERESIZE_SIZE_KEYBOARD = 9;  /* size via keyboard */
static const uint32_t _NET_WM_MOVERESIZE_MOVE_KEYBOARD = 10; /* move via keyboard */
static const uint32_t _NET_WM_MOVERESIZE_CANCEL = 11;        /* cancel operation */

static const uint32_t _NET_MOVERESIZE_WINDOW_X = (1 << 8);
static const uint32_t _NET_MOVERESIZE_WINDOW_Y = (1 << 9);
static const uint32_t _NET_MOVERESIZE_WINDOW_WIDTH = (1 << 10);
static const uint32_t _NET_MOVERESIZE_WINDOW_HEIGHT = (1 << 11);

/*
 * Handles FocusIn events which are generated by clients (i3’s focus changes
 * don’t generate FocusIn events due to a different EventMask) and updates the
 * decorations accordingly.
 *
 */
void PropertyHandlers::handle_focus_in(xcb_focus_in_event_t *event) {
    DLOG(fmt::sprintf("focus change in, for window 0x%08x\n", event->event));

    if (event->event == x.root) {
        DLOG("Received focus in for root window, refocusing the focused window.\n");
        global.focused->con_focus();
        x.focused_id = XCB_NONE;
        x_push_changes(global.croot);
    }

    ConCon *con = con_by_window_id(event->event);
    if (con == nullptr || con->get_window() == nullptr) {
        return;
    }
    DLOG(fmt::sprintf("That is con %p / %s\n", fmt::ptr(con), con->name));

    if (event->mode == XCB_NOTIFY_MODE_GRAB ||
        event->mode == XCB_NOTIFY_MODE_UNGRAB) {
        DLOG("FocusIn event for grab/ungrab, ignoring\n");
        return;
    }

    if (event->detail == XCB_NOTIFY_DETAIL_POINTER) {
        DLOG("notify detail is pointer, ignoring this event\n");
        return;
    }

    /* Floating windows should be refocused to ensure that they are on top of
     * other windows. */
    if (x.focused_id == event->event && !con->con_inside_floating()) {
        DLOG("focus matches the currently focused window, not doing anything\n");
        return;
    }

    /* Skip dock clients, they cannot get the i3 focus. */
    if (con->parent->type == CT_DOCKAREA) {
        DLOG("This is a dock client, not focusing.\n");
        return;
    }

    DLOG("focus is different / refocusing floating window: updating decorations\n");

    con->con_activate_unblock();

    /* We update focused_id because we don’t need to set focus again */
    x.focused_id = event->event;
    tree_render();
}

/*
 * Log FocusOut events.
 *
 */
void PropertyHandlers::handle_focus_out(xcb_focus_in_event_t *event) {
    ConCon *con = con_by_window_id(event->event);
    const char *window_name, *mode, *detail;

    if (con != nullptr) {
        window_name = con->name.c_str();
        if (window_name == nullptr) {
            window_name = "<unnamed con>";
        }
    } else if (event->event == x.root) {
        window_name = "<the root window>";
    } else {
        window_name = "<unknown window>";
    }

    switch (event->mode) {
        case XCB_NOTIFY_MODE_NORMAL:
            mode = "Normal";
            break;
        case XCB_NOTIFY_MODE_GRAB:
            mode = "Grab";
            break;
        case XCB_NOTIFY_MODE_UNGRAB:
            mode = "Ungrab";
            break;
        case XCB_NOTIFY_MODE_WHILE_GRABBED:
            mode = "WhileGrabbed";
            break;
        default:
            mode = "<unknown>";
            break;
    }

    switch (event->detail) {
        case XCB_NOTIFY_DETAIL_ANCESTOR:
            detail = "Ancestor";
            break;
        case XCB_NOTIFY_DETAIL_VIRTUAL:
            detail = "Virtual";
            break;
        case XCB_NOTIFY_DETAIL_INFERIOR:
            detail = "Inferior";
            break;
        case XCB_NOTIFY_DETAIL_NONLINEAR:
            detail = "Nonlinear";
            break;
        case XCB_NOTIFY_DETAIL_NONLINEAR_VIRTUAL:
            detail = "NonlinearVirtual";
            break;
        case XCB_NOTIFY_DETAIL_POINTER:
            detail = "Pointer";
            break;
        case XCB_NOTIFY_DETAIL_POINTER_ROOT:
            detail = "PointerRoot";
            break;
        case XCB_NOTIFY_DETAIL_NONE:
            detail = "NONE";
            break;
        default:
            detail = "unknown";
            break;
    }

    DLOG(fmt::sprintf("focus change out: window 0x%08x (con %p, %s) lost focus with detail=%s, mode=%s\n", event->event, fmt::ptr(con), window_name, detail, mode));
}

/*
 * Handles ConfigureNotify events for the root window, which are generated when
 * the monitor configuration changed.
 *
 */
void PropertyHandlers::handle_configure_notify(xcb_configure_notify_event_t *event) {
    if (event->event != x.root) {
        DLOG(fmt::sprintf("ConfigureNotify for non-root window 0x%08x, ignoring\n", event->event));
        return;
    }
    DLOG(fmt::sprintf("ConfigureNotify for root window 0x%08x\n", event->event));

    randr.randr_query_outputs();

    ipcManager.ipc_send_event("output", i3ipc::EVENT_OUTPUT, "{\"change\":\"unspecified\"}");
}

/*
 * Handles SelectionClear events for the root window, which are generated when
 * we lose ownership of a selection.
 */
static void handle_selection_clear(xcb_selection_clear_event_t *event) {
    if (event->selection != global.x->wm_sn) {
        DLOG(fmt::sprintf("SelectionClear for unknown selection %d, ignoring\n", event->selection));
        return;
    }
    LOG("Lost WM_Sn selection, exiting.\n");
    exit(EXIT_SUCCESS);

    /* unreachable */
}

/*
 * Sets the appropriate atoms for the property handlers after the atoms were
 * received from X11
 *
 */
PropertyHandlers::PropertyHandlers(X &x, WorkspaceManager &workspaceManager, ConfigurationManager &configManager, RandR &randr, Xkb &xkb, InputManager &inputManager, ApplicationLauncher &applicationLauncher, IPCManager &ipcManager) : x{x}, workspaceManager(workspaceManager), configManager(configManager), randr(randr), xkb(xkb), inputManager(inputManager), ipcManager(ipcManager) {


}

/*
 * There was a KeyPress or KeyRelease (both events have the same fields). We
 * compare this key code with our bindings table and pass the bound action to
 * parse_command().
 *
 */
void PropertyHandlers::handle_key_press(xcb_key_press_event_t *event) {
    const bool key_release = (event->response_type == XCB_KEY_RELEASE);

    global.last_timestamp = event->time;

    DLOG(fmt::sprintf("%s %d, state raw = 0x%x\n", (key_release ? "KeyRelease" : "KeyPress"), event->detail, event->state));

    Binding *bind = get_binding_from_xcb_event((xcb_generic_event_t *)event);

    /* if we couldn't find a binding, we are done */
    if (bind == nullptr)
        return;

    run_binding(bind, nullptr);
}

/*
 * Takes an xcb_generic_event_t and calls the appropriate handler, based on the
 * event type.
 *
 */
void PropertyHandlers::handle_event(int type, xcb_generic_event_t *event) {
    x_connection *conn = &*x;
    if (type != XCB_MOTION_NOTIFY)
        DLOG(fmt::sprintf("event type %d, xkb_base %d\n", type, global.xkb->xkb_base));

    if (randr.randr_base > -1 &&
        type == randr.randr_base + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
        handle_screen_change(event);
        return;
    }

    if (global.xkb->xkb_base > -1 && type == global.xkb->xkb_base) {
        DLOG("xkb event, need to handle it.\n");

        auto *state = (xcb_xkb_state_notify_event_t *)event;
        if (state->xkbType == XCB_XKB_NEW_KEYBOARD_NOTIFY) {
            DLOG(fmt::sprintf("xkb new keyboard notify, sequence %d, time %d\n", state->sequence, state->time));
            xkb.key_symbols_alloc();
            if (((xcb_xkb_new_keyboard_notify_event_t *)event)->changed & XCB_XKB_NKN_DETAIL_KEYCODES) {
                xkb.load_keymap();
            }
            ungrab_all_keys(conn);
            translate_keysyms(xkb.keymap());
            grab_all_keys(conn);
        } else if (state->xkbType == XCB_XKB_MAP_NOTIFY) {
            if (event_is_ignored(event->sequence, type)) {
                DLOG(fmt::sprintf("Ignoring map notify event for sequence %d.\n", state->sequence));
            } else {
                DLOG(fmt::sprintf("xkb map notify, sequence %d, time %d\n", state->sequence, state->time));
                add_ignore_event(event->sequence, type);
                xkb.key_symbols_alloc();
                ungrab_all_keys(conn);
                translate_keysyms(xkb.keymap());
                grab_all_keys(conn);
                xkb.load_keymap();
            }
        } else if (state->xkbType == XCB_XKB_STATE_NOTIFY) {
            DLOG(fmt::sprintf("xkb state group = %d\n", state->group));
            if (xkb.xkb_current_group == state->group) {
                return;
            }
            xkb.xkb_current_group = state->group;
            ungrab_all_keys(conn);
            grab_all_keys(conn);
        }

        return;
    }

    if (x.shape_supported && type == x.shape_base + XCB_SHAPE_NOTIFY) {
        auto *shape = reinterpret_cast<xcb_shape_notify_event_t*>(event);

        DLOG(fmt::sprintf("shape_notify_event for window 0x%08x, shape_kind = %d, shaped = %d\n",
                          shape->affected_window, shape->shape_kind, shape->shaped));

        ConCon *con = con_by_window_id(shape->affected_window);
        if (con == nullptr) {
            LOG(fmt::sprintf("Not a managed window 0x%08x, ignoring shape_notify_event\n",
                             shape->affected_window));
            return;
        }

        if (shape->shape_kind == XCB_SHAPE_SK_BOUNDING ||
            shape->shape_kind == XCB_SHAPE_SK_INPUT) {
            x_set_shape(con, static_cast<xcb_shape_sk_t>(shape->shape_kind), shape->shaped);
        }

        return;
    }

    switch (type) {
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
            handle_key_press(reinterpret_cast<xcb_key_press_event_t *>(event));
            break;

        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            handle_button_press(reinterpret_cast<xcb_button_press_event_t *>(event));
            break;

        case XCB_MAP_REQUEST:
            handle_map_request(reinterpret_cast<xcb_map_request_event_t *>(event));
            break;

        case XCB_UNMAP_NOTIFY:
            handle_unmap_notify_event(reinterpret_cast<xcb_unmap_notify_event_t *>(event));
            break;

        case XCB_DESTROY_NOTIFY:
            handle_destroy_notify_event(reinterpret_cast<xcb_destroy_notify_event_t *>(event));
            break;

        case XCB_EXPOSE:
            handle_expose_event(reinterpret_cast<xcb_expose_event_t *>(event));
            break;

        case XCB_MOTION_NOTIFY:
            handle_motion_notify(reinterpret_cast<xcb_motion_notify_event_t *>(event));
            break;

        /* Enter window = user moved their mouse over the window */
        case XCB_ENTER_NOTIFY:
            handle_enter_notify(reinterpret_cast<xcb_enter_notify_event_t *>(event));
            break;

        /* Client message are sent to the root window. The only interesting
         * client message for us is _NET_WM_STATE, we honour
         * _NET_WM_STATE_FULLSCREEN and _NET_WM_STATE_DEMANDS_ATTENTION */
        case XCB_CLIENT_MESSAGE:
            handle_client_message(reinterpret_cast<xcb_client_message_event_t *>(event));
            break;

        /* Configure request = window tried to change size on its own */
        case XCB_CONFIGURE_REQUEST:
            handle_configure_request(reinterpret_cast<xcb_configure_request_event_t *>(event));
            break;

        /* Mapping notify = keyboard mapping changed (Xmodmap), re-grab bindings */
        case XCB_MAPPING_NOTIFY:
            handle_mapping_notify(reinterpret_cast<xcb_mapping_notify_event_t *>(event));
            break;

        case XCB_FOCUS_IN:
            handle_focus_in(reinterpret_cast<xcb_focus_in_event_t *>(event));
            break;

        case XCB_FOCUS_OUT:
            handle_focus_out(reinterpret_cast<xcb_focus_out_event_t *>(event));
            break;

        case XCB_PROPERTY_NOTIFY: {
            property_notify(reinterpret_cast<xcb_property_notify_event_t *>(event));
            break;
        }

        case XCB_CONFIGURE_NOTIFY:
            handle_configure_notify(reinterpret_cast<xcb_configure_notify_event_t *>(event));
            break;

        case XCB_SELECTION_CLEAR:
            handle_selection_clear(reinterpret_cast<xcb_selection_clear_event_t *>(event));
            break;

        default:
            // LOG(fmt::sprintf("Unhandled event of type %d\n",  type));
            break;
    }
}
