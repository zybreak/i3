/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * manage.c: Initially managing new windows (or existing ones on restart).
 *
 */
module;
#include <xpp/proto/x.hpp>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>
#include <xcb/xcb_icccm.h>
#include <fmt/core.h>
#include <fmt/printf.h>

#include <xcb/shape.h>
#include <xcb/xcb_keysyms.h>
module i3;

import std;
import utils;
import log;
import rect;

template<typename T>
struct window_data {
    T wm_type_cookie;
    T strut_cookie;
    T state_cookie;
    T utf8_title_cookie;
    T leader_cookie;
    T transient_cookie;
    T title_cookie;
    T class_cookie;
    T role_cookie;
    T startup_id_cookie;
    T wm_hints_cookie;
    T wm_normal_hints_cookie;
    T motif_wm_hints_cookie;
    T wm_user_time_cookie;
    T wm_desktop_cookie;
    T wm_machine_cookie;
    T wm_icon_cookie;
};

static auto get_window_data(xcb_window_t window) {
    return window_data{
            .wm_type_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .strut_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_STRUT_PARTIAL], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .state_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_STATE], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .utf8_title_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_NAME], XCB_GET_PROPERTY_TYPE_ANY, 0, 128),
            .leader_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::WM_CLIENT_LEADER], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .transient_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_TRANSIENT_FOR, XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .title_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 0, 128),
            .class_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_CLASS, XCB_GET_PROPERTY_TYPE_ANY, 0, 128),
            .role_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::WM_WINDOW_ROLE], XCB_GET_PROPERTY_TYPE_ANY, 0, 128),
            .startup_id_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_STARTUP_ID], XCB_GET_PROPERTY_TYPE_ANY, 0, 512),
            .wm_hints_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_HINTS, XCB_ATOM_WM_HINTS, 0L, XCB_ICCCM_NUM_WM_HINTS_ELEMENTS),
            .wm_normal_hints_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 0L, XCB_ICCCM_NUM_WM_SIZE_HINTS_ELEMENTS),
            .motif_wm_hints_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_MOTIF_WM_HINTS], XCB_GET_PROPERTY_TYPE_ANY, 0, 5 * sizeof(uint64_t)),
            .wm_user_time_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_USER_TIME], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .wm_desktop_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_DESKTOP], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .wm_machine_cookie = global.x->conn->get_property(false, window, XCB_ATOM_WM_CLIENT_MACHINE, XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX),
            .wm_icon_cookie = global.x->conn->get_property(false, window, i3::atoms[i3::Atom::_NET_WM_ICON], XCB_GET_PROPERTY_TYPE_ANY, 0, UINT32_MAX)
    };
}

/*
 * Match frame and window depth. This is needed because X will refuse to reparent a
 * window whose background is ParentRelative under a window with a different depth.
 *
 */
static xcb_window_t _match_depth(i3Window const *win, Con *con) {
    xcb_window_t old_frame = XCB_NONE;
    if (con->depth != win->depth) {
        old_frame = con->frame->id;
        con->depth = win->depth;
        x_con_reframe(con);
    }
    return old_frame;
}

/*
 * Remove all match criteria, the first swallowed window wins.
 *
 */
static void _remove_matches(Con *con) {
    con->swallow.clear();
}

/*
 * Go through all existing windows (if the window manager is restarted) and manage them
 *
 */
void manage_existing_windows(xcb_window_t root) {
    /* Get the tree of windows whose parent is the root window (= all) */
    auto reply = global.x->conn->query_tree(root);

    for (auto child : reply.children()) {
        try {
            auto attr = global.x->conn->get_window_attributes(child);
            if (attr->map_state == XCB_MAP_STATE_VIEWABLE && !attr->override_redirect) {
                manage_window(child, attr->visual);
            } else {
                DLOG(std::format("Existing window {} is already mapped, unviewable or has override redirect", child));
            }
        } catch (...) {
            DLOG(std::format("Could not get attributes on window {}", child));
        }
    }
}

/*
 * Restores the geometry of each window by reparenting it to the root window
 * at the position of its frame.
 *
 * This is to be called *only* before exiting/restarting i3 because of evil
 * side-effects which are to be expected when continuing to run i3.
 *
 */
void restore_geometry() {
    DLOG("Restoring geometry\n");

    for (auto &c : global.all_cons) {
        if (auto con = dynamic_cast<ConCon*>(c); con && con->get_window() != nullptr) {
            DLOG(fmt::sprintf("Re-adding X11 border of %d px\n", con->border_width));
            con->get_window_rect().width += (2 * con->border_width);
            con->get_window_rect().height += (2 * con->border_width);
            xcb_set_window_rect(**global.x, con->get_window()->id, con->get_window_rect());
            DLOG(fmt::sprintf("placing window %08x at %d %d\n", con->get_window()->id, con->rect.x, con->rect.y));
            xcb_reparent_window(**global.x, con->get_window()->id, global.x->root,
                                con->rect.x, con->rect.y);
        }
    }

    /* Strictly speaking, this line doesn’t really belong here, but since we
     * are syncing, let’s un-register as a window manager first */
    uint32_t value_list[]{XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
    xcb_change_window_attributes(**global.x, global.x->root, XCB_CW_EVENT_MASK, value_list);

    /* Make sure our changes reach the X server, we restart/exit now */
    xcb_aux_sync(**global.x);
}

/*
 * Moves the given container to the currently focused container on the
 * visible workspace on the output specified by the given name.
 * The current output for the container is used to resolve relative names
 * such as left, right, up, down.
 *
 */
static bool con_move_to_output_name(Con *con, const std::string &name, bool fix_coordinates) {
    Output *current_output = get_output_for_con(con);
    Output *output = current_output->get_output_from_string(name);
    if (output == nullptr) {
        ELOG(fmt::sprintf("Could not find output \"%s\"\n", name));
        return false;
    }

    con_move_to_output(con, output, fix_coordinates);
    return true;
}

/*
 * Merges container specific data that should move with the window (e.g. marks,
 * title format, and the window itself) into another container, and closes the
 * old container.
 *
 */
static void con_merge_into(ConCon *old, ConCon *new_con) {
    new_con->set_window(old->release_window());

    if (!old->title_format.empty()) {
        new_con->title_format = old->title_format;
        old->title_format.clear();
    }

    if (!old->sticky_group.empty()) {
        new_con->sticky_group = old->sticky_group;
        old->sticky_group = "";
    }

    new_con->sticky = old->sticky;

    new_con->con_set_urgency(old->urgent);

    tree_close_internal(old, kill_window_t::DONT_KILL_WINDOW, false);
}

static std::optional<std::string> to_string(auto &prop) {
    if (prop->value_len > 0) {
        auto data = prop.template value<char>();
        return std::string{data.begin(), data.end()};
    }
    return std::nullopt;
}

static ConCon* find_con_for_window_not_found(i3Window *cwindow, Con const *search_at, bool &urgency_hint, std::optional<std::string> &startup_ws, bool &match_from_restart_mode, Match *match) {
    ConCon *nc = nullptr;
    auto assignmentOpt = global.assignmentManager->assignment_for<WorkspaceAssignment>(cwindow);

    /* If not, check if it is assigned to a specific workspace */
    if (assignmentOpt) {
        DLOG(fmt::sprintf("Assignment matches (%p)\n", fmt::ptr(match)));

        Con *assigned_ws = nullptr;
        if (assignmentOpt->get().type == workspace_assignment_type::WORKSPACE_NUMBER) {
            long parsed_num = utils::ws_name_to_number(assignmentOpt->get().workspace);

            assigned_ws = get_existing_workspace_by_num(parsed_num);
        }
        /* A_TO_WORKSPACE type assignment or fallback from A_TO_WORKSPACE_NUMBER
         * when the target workspace number does not exist yet. */
        if (!assigned_ws) {
            assigned_ws = global.workspaceManager->workspace_get_or_create(assignmentOpt->get().workspace);
        }

        auto descend_nc = con_descend_tiling_focused(assigned_ws);
        DLOG(fmt::sprintf("focused on ws %s: %p / %s\n", assigned_ws->name, fmt::ptr(descend_nc), descend_nc->name));
        if (descend_nc->type == CT_WORKSPACE) {
            nc = tree_open_con(descend_nc, cwindow);
        } else {
            nc = tree_open_con(descend_nc->parent, cwindow);
        }

        /* set the urgency hint on the window if the workspace is not visible */
        if (!workspace_is_visible(assigned_ws)) {
            urgency_hint = true;
        }
    } else if (WorkspaceCon *wm_desktop_ws = ewmh_get_workspace_by_index(cwindow->wm_desktop);
               cwindow->wm_desktop != NET_WM_DESKTOP_NONE &&
               cwindow->wm_desktop != NET_WM_DESKTOP_ALL &&
               wm_desktop_ws != nullptr) {
        /* If _NET_WM_DESKTOP is set to a specific desktop, we open it
         * there. Note that we ignore the special value 0xFFFFFFFF here
         * since such a window will be made sticky anyway. */

        DLOG(fmt::sprintf("Using workspace %p / %s because _NET_WM_DESKTOP = %d.\n",
                fmt::ptr(wm_desktop_ws), wm_desktop_ws->name, cwindow->wm_desktop));

        auto descend_nc = con_descend_tiling_focused(wm_desktop_ws);
        if (descend_nc->type == CT_WORKSPACE) {
            nc = tree_open_con(descend_nc, cwindow);
        } else {
            nc = tree_open_con(descend_nc->parent, cwindow);
        }
    } else if (startup_ws) {
        /* If it was started on a specific workspace, we want to open it there. */
        DLOG(fmt::format("Using workspace on which this application was started ({})", *startup_ws));
        auto descend_nc = con_descend_tiling_focused(global.workspaceManager->workspace_get_or_create(*startup_ws));
        DLOG(fmt::format("focused on ws {}: {} / {}", *startup_ws, fmt::ptr(descend_nc), descend_nc->name));
        if (descend_nc->type == CT_WORKSPACE) {
            nc = tree_open_con(descend_nc, cwindow);
        } else {
            nc = tree_open_con(descend_nc->parent, cwindow);
        }
    } else {
        /* If not, insert it at the currently focused position */
        if (global.focused->type == CT_CON && global.focused->con_accepts_window()) {
            LOG(fmt::sprintf("using current container, focused = %p, focused->name = %s\n",
                    fmt::ptr(global.focused), global.focused->name));
            nc = dynamic_cast<ConCon*>(global.focused);
            // TODO assign cwindow to nc?
        } else {
            nc = tree_open_con(nullptr, cwindow);
        }
    }
    auto outputAssignmentOpt = global.assignmentManager->assignment_for<OutputAssignment>(cwindow);
    if (outputAssignmentOpt) {
        con_move_to_output_name(nc, outputAssignmentOpt->get().output, true);
    }   
    
    return nc;
}

static ConCon* find_con_for_window_found(i3Window *cwindow, Con *nc_for_window, Match *match) {
    ConCon *nc;
    /* M_BELOW inserts the new window as a child of the one which was
         * matched (e.g. dock areas) */
    if (match != nullptr && match->insert_where == M_BELOW) {
        nc = tree_open_con(nc_for_window, cwindow);
    } else {
        nc = dynamic_cast<ConCon*>(nc_for_window); // TODO: bad to assume its a concon?
    }

    /* If M_BELOW is not used, the container is replaced. This happens with
     * "swallows" criteria that are used for stored layouts, in which case
     * we need to remove that criterion, because they should only be valid
     * once. */
    if (match != nullptr && match->insert_where != M_BELOW) {
        DLOG(fmt::sprintf("Removing match %p from container %p\n", fmt::ptr(match), fmt::ptr(nc)));
        std::erase_if(nc->swallow, [&match](auto &m) {
            return m.get() == match;
        });
    }

    
    return nc;
}

static ConCon* find_con_for_window(i3Window *cwindow, Con const *search_at, bool &urgency_hint, std::optional<std::string> &startup_ws, bool &match_from_restart_mode) {
    Match *match = nullptr;
    Con *nc_for_window = con_for_window(search_at, cwindow, &match);
    ConCon *nc = nullptr;
    match_from_restart_mode = (match && match->restart_mode);
    if (nc_for_window == nullptr) {
        nc = find_con_for_window_not_found(cwindow, search_at, urgency_hint, startup_ws, match_from_restart_mode, match);
    } else {
        cwindow->swallowed = true;
        nc = find_con_for_window_found(cwindow, nc_for_window, match);
        // TODO assign cwindow to nc?
    }
    
    return nc;
}

static bool should_set_focus(i3Window *cwindow, Con *fs, WorkspaceCon *ws, ConCon *nc, bool match_from_restart_mode) {
    bool set_focus = false;
    if (fs == nullptr) {
        DLOG("Not in fullscreen mode, focusing\n");
        if (cwindow->dock == window_dock_t::W_NODOCK) {
            /* Check that the workspace is visible and on the same output as
             * the current focused container. If the window was assigned to an
             * invisible workspace, we should not steal focus. */
            Con *current_output = global.focused->con_get_output();
            Con *target_output = ws->con_get_output();

            if (workspace_is_visible(ws) && current_output == target_output) {
                if (!match_from_restart_mode) {
                    set_focus = true;
                } else {
                    DLOG("not focusing, matched with restart_mode == true\n");
                }
            } else {
                DLOG("workspace not visible, not focusing\n");
            }
        } else {
            DLOG("dock, not focusing\n");
        }
    } else {
        DLOG(fmt::sprintf("fs = %p, ws = %p, not focusing\n", fmt::ptr(fs), fmt::ptr(ws)));
        /* Insert the new container in focus stack *after* the currently
         * focused (fullscreen) con. This way, the new container will be
         * focused after we return from fullscreen mode */
        Con *first = con::first(nc->parent->focused);
        if (first != nc) {
            /* We only modify the focus stack if the container is not already
             * the first one. This can happen when existing containers swallow
             * new windows, for example when restarting. */
            std::erase(nc->parent->focused, nc);
            nc->parent->focused.push_front(nc);
        }
    }
    
    return set_focus;
}

static void handle_dock(i3Window *cwindow, Con **search_at_p, xcb_get_geometry_reply_t *geom) {
    Output *output = global.randr->get_output_containing(geom->x, geom->y);
    if (output != nullptr) {
        DLOG(fmt::sprintf("Starting search at output %s\n", output->output_primary_name()));
        *search_at_p = output->con;
    }
    
    Con *search_at = *search_at_p;

    /* find out the desired position of this dock window */
    if (cwindow->reserved.top > 0 && cwindow->reserved.bottom == 0) {
        DLOG("Top dock client\n");
        cwindow->dock = window_dock_t::W_DOCK_TOP;
    } else if (cwindow->reserved.top == 0 && cwindow->reserved.bottom > 0) {
        DLOG("Bottom dock client\n");
        cwindow->dock = window_dock_t::W_DOCK_BOTTOM;
    } else {
        DLOG("Ignoring invalid reserved edges (_NET_WM_STRUT_PARTIAL), using position as fallback:\n");
        if (geom->y < static_cast<int16_t>(search_at->rect.height / 2)) {
            DLOG(fmt::sprintf("geom->y = %d < rect.height / 2 = %d, it is a top dock client\n",
                    geom->y, (search_at->rect.height / 2)));
            cwindow->dock = window_dock_t::W_DOCK_TOP;
        } else {
            DLOG(fmt::sprintf("geom->y = %d >= rect.height / 2 = %d, it is a bottom dock client\n",
                    geom->y, (search_at->rect.height / 2)));
            cwindow->dock = window_dock_t::W_DOCK_BOTTOM;
        }
    }
}

template<typename Window_data>
static i3Window* create_window(Window_data &w_data, xcb_window_t window, xcb_visualid_t visual, bool &urgency_hint, border_style_t &motif_border_style, bool &has_mwm_hints, xcb_get_geometry_reply_t *geom) {
    auto cwindow = new i3Window(window);
    cwindow->depth = get_visual_depth(visual);

    auto buttons = bindings_get_buttons_to_grab();
    xcb_grab_buttons(window, buttons);

    /* update as much information as possible so far (some replies may be NULL) */
    if (auto ret = handle_property::window_update_class(w_data.class_cookie.get().get()); ret) {
        cwindow->window_update_class(ret->class_name, ret->class_instance);
    }
    if (auto ret = handle_property::window_update_name_legacy(w_data.title_cookie.get().get()); ret) {
        cwindow->window_update_name_legacy(*ret);
    }
    if (auto ret = handle_property::window_update_name(w_data.utf8_title_cookie.get().get()); ret) {
        cwindow->window_update_name(*ret);
    }
    if (w_data.wm_icon_cookie->value_len > 0) {
        cwindow->window_update_icon(w_data.wm_icon_cookie.get().get());
    }
    if (w_data.leader_cookie->value_len > 0) {
        cwindow->window_update_leader(w_data.leader_cookie.get().get());
    }
    if (w_data.transient_cookie->value_len > 0) {
        cwindow->window_update_transient_for(w_data.transient_cookie.get().get());
    }
    if (w_data.strut_cookie->value_len > 0) {
        cwindow->window_update_strut_partial(w_data.strut_cookie.get().get());
    }
    if (w_data.role_cookie->value_len > 0) {
        cwindow->window_update_role(w_data.role_cookie.get().get());
    }
    if (w_data.wm_hints_cookie->value_len > 0) {
        cwindow->window_update_hints(w_data.wm_hints_cookie.get().get(), &urgency_hint);
    }
    if (w_data.motif_wm_hints_cookie->value_len > 0) {
        has_mwm_hints = cwindow->window_update_motif_hints(w_data.motif_wm_hints_cookie.get().get(), &motif_border_style);
    }
    if (w_data.wm_normal_hints_cookie->value_len > 0) {
        cwindow->window_update_normal_hints(w_data.wm_normal_hints_cookie.get().get(), geom);
    }
    if (auto machine = handle_property::window_update_machine(w_data.wm_machine_cookie.get().get()); machine) {
        cwindow->window_update_machine(*machine);
    }
    return cwindow;
}

/*
 * Do some sanity checks and then reparent the window.
 *
 */
void manage_window(xcb_window_t window, xcb_visualid_t visual) {
    DLOG(fmt::sprintf("window 0x%08x\n", window));

    /* Check if the window is already managed */
    if (con_by_window_id(window) != nullptr) {
        DLOG(fmt::sprintf("already managed (by con %p)\n", fmt::ptr(con_by_window_id(window))));
        return;
    }

    auto opt_geom = [window]() -> std::optional<xpp::x::reply::checked::get_geometry<x_connection>> {
        try {
            return global.x->conn->get_geometry(static_cast<xcb_drawable_t>(window));
        } catch (...) {
            return std::nullopt;
        }
    }();

    /* Get the initial geometry (position, size, …) */
    if (!opt_geom) {
        DLOG("could not get geometry\n");
        return;
    }
    
    auto &geom = opt_geom.value();

    uint32_t values[1];

    /* Set a temporary event mask for the new window, consisting only of
     * PropertyChange and StructureNotify. We need to be notified of
     * PropertyChanges because the client can change its properties *after* we
     * requested them but *before* we actually reparented it and have set our
     * final event mask.
     * We need StructureNotify because the client may unmap the window before
     * we get to re-parent it.
     * If this request fails, we assume the client has already unmapped the
     * window between the MapRequest and our event mask change. */
    values[0] = XCB_EVENT_MASK_PROPERTY_CHANGE |
                XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    xcb_void_cookie_t event_mask_cookie =
        xcb_change_window_attributes_checked(**global.x, window, XCB_CW_EVENT_MASK, values);
    if (xcb_request_check(**global.x, event_mask_cookie) != nullptr) {
        LOG("Could not change event mask, the window probably already disappeared.\n");
        return;
    }

    window_data w_data = get_window_data(window);

    bool urgency_hint;
    border_style_t motif_border_style = border_style_t::BS_NORMAL;
    bool has_mwm_hints = false;
    i3Window *cwindow = create_window(w_data, window, visual, urgency_hint, motif_border_style, has_mwm_hints, geom.get().get());
    
    xcb_get_property_reply_t *type_reply = (w_data.wm_type_cookie->value_len > 0 ? w_data.wm_type_cookie.get().get() : nullptr);
    xcb_get_property_reply_t *state_reply = (w_data.state_cookie->value_len > 0 ? w_data.state_cookie.get().get() : nullptr);

    std::optional<std::string> startup_ws = global.applicationLauncher->startup_workspace_for_window(cwindow, (w_data.startup_id_cookie->value_len > 0 ? w_data.startup_id_cookie.get().get() : nullptr));
    DLOG(fmt::format("startup workspace = {}", startup_ws.value_or("(null)")));

    /* Get _NET_WM_DESKTOP if it was set. */
    xcb_get_property_reply_t *wm_desktop_reply;
    wm_desktop_reply = (w_data.wm_desktop_cookie->value_len > 0) ? w_data.wm_desktop_cookie.get().get() : nullptr;
    cwindow->wm_desktop = NET_WM_DESKTOP_NONE;
    if (wm_desktop_reply != nullptr && xcb_get_property_value_length(wm_desktop_reply) != 0) {
        auto *wm_desktops = (uint32_t *)xcb_get_property_value(wm_desktop_reply);
        cwindow->wm_desktop = static_cast<int32_t>(wm_desktops[0]);
    }

    /* check if the window needs WM_TAKE_FOCUS */
    cwindow->needs_take_focus = window_supports_protocol(cwindow->id, i3::atoms[i3::Atom::WM_TAKE_FOCUS]);

    /* read the preferred _NET_WM_WINDOW_TYPE atom */
    cwindow->window_type = xcb_get_preferred_window_type(type_reply);

    /* Where to start searching for a container that swallows the new one? */
    Con *search_at = global.croot;

    if (xcb_reply_contains_atom(type_reply, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DOCK])) {
        LOG("This window is of type dock\n");
        handle_dock(cwindow, &search_at, geom.get().get());
    }

    DLOG(fmt::sprintf("Initial geometry: (%d, %d, %d, %d)\n", geom->x, geom->y, geom->width, geom->height));

    /* See if any container swallows this new window */
    cwindow->swallowed = false;
    bool match_from_restart_mode = false;
    ConCon *nc = find_con_for_window(cwindow, search_at, urgency_hint, startup_ws, match_from_restart_mode);

    DLOG(fmt::sprintf("new container = %p\n", fmt::ptr(nc)));
    
    // TODO: This should already been set by find_con_for_window
    
    if (nc->get_window() != nullptr && nc->get_window() != cwindow) {
        if (!restore_kill_placeholder(nc->get_window()->id)) {
            DLOG("Uh?! Container without a placeholder, but with a window, has swallowed this to-be-managed window?!\n");
        } else {
            /* Remove remaining criteria, the first swallowed window wins. */
            _remove_matches(nc);
        }
    }
    xcb_window_t old_frame = XCB_NONE;
    if (nc->get_window() != cwindow && nc->get_window() != nullptr) {
        old_frame = _match_depth(cwindow, nc);
    }
    if (nc->get_window() != cwindow) {
        nc->set_window(cwindow);
    }
    
    // END:
    
    x_reinit(nc);

    nc->border_width = geom->border_width;

    x_set_name(nc, fmt::format("[i3 con] container around {}", fmt::ptr(nc->get_window())));

    /* handle fullscreen containers */
    WorkspaceCon *ws = nc->con_get_workspace();
    Con *fs = ws ? ws->con_get_fullscreen_covering_ws() : nullptr;

    if (xcb_reply_contains_atom(state_reply, i3::atoms[i3::Atom::_NET_WM_STATE_FULLSCREEN])) {
        /* If this window is already fullscreen (after restarting!), skip
         * toggling fullscreen, that would drop it out of fullscreen mode. */
        if (fs != nc) {
            Output *output = global.randr->get_output_with_dimensions((Rect){static_cast<uint32_t>(geom->x), static_cast<uint32_t>(geom->y), geom->width, geom->height});
            /* If the requested window geometry spans the whole area
             * of an output, move the window to that output. This is
             * needed e.g. for LibreOffice Impress multi-monitor
             * presentations to work out of the box. */
            if (output != nullptr) {
                con_move_to_output(nc, output, false);
            }
            con_toggle_fullscreen(nc, CF_OUTPUT);
        }
        fs = nullptr;
    }

    bool set_focus = should_set_focus(nc->get_window(), fs, ws, nc, match_from_restart_mode);

    /* set floating if necessary */
    bool want_floating = false;
    if (xcb_reply_contains_atom(type_reply, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG]) ||
        xcb_reply_contains_atom(type_reply, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY]) ||
        xcb_reply_contains_atom(type_reply, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR]) ||
        xcb_reply_contains_atom(type_reply, i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH]) ||
        xcb_reply_contains_atom(state_reply, i3::atoms[i3::Atom::_NET_WM_STATE_MODAL]) ||
        (nc->get_window()->max_width > 0 && nc->get_window()->max_height > 0 &&
         nc->get_window()->min_height == nc->get_window()->max_height &&
         nc->get_window()->min_width == nc->get_window()->max_width)) {
        LOG("This window is a dialog window, setting floating\n");
        want_floating = true;
    }

    if (xcb_reply_contains_atom(state_reply, i3::atoms[i3::Atom::_NET_WM_STATE_STICKY])) {
        nc->sticky = true;
    }

    /* We ignore the hint for an internal workspace because windows in the
     * scratchpad also have this value, but upon restarting i3 we don't want
     * them to become sticky windows. */
    if (nc->get_window()->wm_desktop == NET_WM_DESKTOP_ALL) {
        DLOG("This window has _NET_WM_DESKTOP = 0xFFFFFFFF. Will float it and make it sticky.\n");
        nc->sticky = true;
        want_floating = true;
    }

    if (nc->get_window()->transient_for != XCB_NONE ||
        (nc->get_window()->leader != XCB_NONE &&
         nc->get_window()->leader != nc->get_window()->id &&
         con_by_window_id(nc->get_window()->leader) != nullptr)) {
        DLOG("This window is transient for another window, setting floating\n");
        want_floating = true;

        if (global.configManager->config->popup_during_fullscreen == PDF_LEAVE_FULLSCREEN &&
            fs != nullptr) {
            DLOG("There is a fullscreen window, leaving fullscreen mode\n");
            con_toggle_fullscreen(fs, CF_OUTPUT);
        } else if (global.configManager->config->popup_during_fullscreen == PDF_SMART &&
                   fs != nullptr &&
                   fs->get_window() != nullptr) {
            set_focus = con_find_transient_for_window(nc, fs->get_window()->id);
        }
    }

    /* dock clients cannot be floating, that makes no sense */
    if (nc->get_window()->dock != window_dock_t::W_NODOCK) {
        want_floating = false;
    }

    /* Store the requested geometry. The width/height gets raised to at least
     * 75x50 when entering floating mode, which is the minimum size for a
     * window to be useful (smaller windows are usually overlays/toolbars/…
     * which are not managed by the wm anyways). We store the original geometry
     * here because it’s used for dock clients. */
    if (nc->get_geometry().width == 0) {
        nc->set_geometry((Rect){static_cast<uint32_t>(geom->x), static_cast<uint32_t>(geom->y), geom->width, geom->height});
    }

    if (want_floating) {
        DLOG(fmt::sprintf("geometry = %d x %d\n", nc->get_geometry().width, nc->get_geometry().height));
        if (floating_enable(nc, true)) {
            nc->floating = FLOATING_AUTO_ON;
        }
    }

    if (has_mwm_hints) {
        DLOG(fmt::sprintf("MOTIF_WM_HINTS specifies decorations (border_style = %d)\n", std::to_underlying(motif_border_style)));
        if (want_floating) {
            con_set_border_style(nc, motif_border_style, global.configManager->config->default_floating_border_width);
        } else {
            con_set_border_style(nc, motif_border_style, global.configManager->config->default_border_width);
        }
    }

    /* explicitly set the border width to the default */
    if (nc->current_border_width == -1) {
        nc->current_border_width = (want_floating ? global.configManager->config->default_floating_border_width : global.configManager->config->default_border_width);
    }

    /* to avoid getting an UnmapNotify event due to reparenting, we temporarily
     * declare no interest in any state change event of this window */
    values[0] = XCB_NONE;
    xcb_change_window_attributes(**global.x, window, XCB_CW_EVENT_MASK, values);

    xcb_void_cookie_t rcookie = xcb_reparent_window_checked(**global.x, window, nc->frame->id, 0, 0);
    if (xcb_request_check(**global.x, rcookie) != nullptr) {
        LOG("Could not reparent the window, aborting\n");
        return;
    }

    values[0] = CHILD_EVENT_MASK & ~XCB_EVENT_MASK_ENTER_WINDOW;
    xcb_change_window_attributes(**global.x, window, XCB_CW_EVENT_MASK, values);
    xcb_flush(**global.x);

    /* Put the client inside the save set. Upon termination (whether killed or
     * normal exit does not matter) of the window manager, these clients will
     * be correctly reparented to their most closest living ancestor (=
     * cleanup) */
    xcb_change_save_set(**global.x, XCB_SET_MODE_INSERT, window);

    if (global.x->shape_supported) {
        /* Receive ShapeNotify events whenever the client altered its window
         * shape. */
        xcb_shape_select_input(**global.x, window, true);

        /* Check if the window is shaped. Sadly, we can check only for the
         * bounding shape, not for the input shape. */
        xcb_shape_query_extents_cookie_t cookie =
            xcb_shape_query_extents(**global.x, window);
        xcb_shape_query_extents_reply_t *reply =
            xcb_shape_query_extents_reply(**global.x, cookie, nullptr);
        if (reply != nullptr && reply->bounding_shaped) {
            nc->get_window()->shaped = true;
        }
        free(reply);
        reply = nullptr;
    }

    /* Check if any assignments match */
    global.assignmentManager->run_assignments(nc->get_window());

    /* 'ws' may be invalid because of the assignments, e.g. when the user uses
     * "move window to workspace 1", but had it assigned to workspace 2. */
    ws = nc->con_get_workspace();

    /* If this window was put onto an invisible workspace (via assignments), we
     * render this workspace. It wouldn’t be rendered in our normal code path
     * because only the visible workspaces get rendered.
     *
     * By rendering the workspace, we assign proper coordinates (read: not
     * width=0, height=0) to the window, which is important for windows who
     * actually use them to position their GUI elements, e.g. rhythmbox. */
    if (ws && !workspace_is_visible(ws)) {
        /* This is a bit hackish: we need to copy the content container’s rect
         * to the workspace, because calling render_con() on the content
         * container would also take the shortcut and not render the invisible
         * workspace at all. However, just calling render_con() on the
         * workspace isn’t enough either — it needs the rect. */
        ws->rect = ws->parent->rect;
        render_con(ws);
        /* Disable setting focus, otherwise we’d move focus to an invisible
         * workspace, which we generally prevent (e.g. in
         * con_move_to_workspace). */
        set_focus = false;
    }
    render_con(global.croot);

    nc->get_window()->managed_since = std::chrono::system_clock::now();

    /* Send an event about window creation */
    ipc_send_window_event("new", nc);

    if (set_focus && global.assignmentManager->assignment_for<NoFocusAssignment>(nc->get_window()).has_value()) {
        /* The first window on a workspace should always be focused. We have to
         * compare with == 1 because the container has already been inserted at
         * this point. */
        if (ws->con_num_windows() == 1) {
            DLOG("This is the first window on this workspace, ignoring no_focus.\n");
        } else {
            DLOG(fmt::sprintf("no_focus was set for con = %p, not setting focus.\n", fmt::ptr(nc)));
            set_focus = false;
        }
    }

    if (set_focus) {
        DLOG(fmt::sprintf("Checking con = %p for _NET_WM_USER_TIME.\n", fmt::ptr(nc)));

        uint32_t *wm_user_time;
        xcb_get_property_reply_t *wm_user_time_reply = (w_data.wm_user_time_cookie.get() != nullptr ? w_data.wm_user_time_cookie.get().get() : nullptr);
        if (wm_user_time_reply != nullptr && xcb_get_property_value_length(wm_user_time_reply) != 0 &&
            (wm_user_time = (uint32_t *)xcb_get_property_value(wm_user_time_reply)) &&
            wm_user_time[0] == 0) {
            DLOG(fmt::sprintf("_NET_WM_USER_TIME set to 0, not focusing con = %p.\n", fmt::ptr(nc)));
            set_focus = false;
        }
    } else {
        xcb_discard_reply(**global.x, w_data.wm_user_time_cookie->sequence);
    }

    if (set_focus) {
        /* Even if the client doesn't want focus, we still need to focus the
         * container to not break focus workflows. Our handling towards X will
         * take care of not setting the input focus. However, one exception to
         * this are clients using the globally active input model which we
         * don't want to focus at all. */
        if (nc->get_window()->doesnt_accept_focus && !nc->get_window()->needs_take_focus) {
            set_focus = false;
        }
    }

    /* Defer setting focus after the 'new' event has been sent to ensure the
     * proper window event sequence. */
    if (set_focus && nc->mapped) {
        DLOG("Now setting focus.\n");
        nc->con_activate();
    }

    tree_render();

    /* Destroy the old frame if we had to reframe the container. This needs to be done
     * after rendering in order to prevent the background from flickering in its place. */
    if (old_frame != XCB_NONE) {
        xcb_destroy_window(**global.x, old_frame);
    }

    /* Windows might get managed with the urgency hint already set (Pidgin is
     * known to do that), so check for that and handle the hint accordingly.
     * This code needs to be in this part of manage_window() because the window
     * needs to be on the final workspace first. */
    nc->con_set_urgency(urgency_hint);

    /* Update _NET_WM_DESKTOP. We invalidate the cached value first to force an update. */
    nc->get_window()->wm_desktop = NET_WM_DESKTOP_NONE;
    ewmh_update_wm_desktop();

    /* If a sticky window was mapped onto another workspace, make sure to pop it to the front. */
    output_push_sticky_windows(global.focused);
}

static ConCon *placeholder_for_con(ConCon *con) {
    /* Make sure this windows hasn't already been swallowed. */
    if (con->get_window()->swallowed) {
        return nullptr;
    }
    Match *match;
    Con *nc = con_for_window(global.croot, con->get_window(), &match);
    if (nc == nullptr || nc->get_window() == nullptr || nc->get_window() == con->get_window()) {
        return nullptr;
    }
    /* Make sure the placeholder that wants to swallow this window didn't spawn
     * after the window to follow current behavior: adding a placeholder won't
     * swallow windows currently managed. */
    if (nc->get_window()->managed_since > con->get_window()->managed_since) {
        return nullptr;
    }
    
    auto concon = dynamic_cast<ConCon*>(nc);
    
    if (concon == nullptr) {
        ELOG("Placeholder is not a container! This is because of a erroneous assumption on my part\n");
        return nullptr;
    }
    
    return concon;
}

/*
 * Remanages a window: performs a swallow check and runs assignments.
 * Returns con for the window regardless if it updated.
 *
 */
ConCon *remanage_window(ConCon *con) {
    ConCon *nc = placeholder_for_con(con);
    if (!nc) {
        /* The con is not updated, just run assignments */
        global.assignmentManager->run_assignments(con->get_window());
        return con;
    }

    if (!restore_kill_placeholder(nc->get_window()->id)) {
        DLOG("Uh?! Container without a placeholder, but with a window, has swallowed this managed window?!\n");
    } else {
        _remove_matches(nc);
    }
    nc->set_window(nullptr);

    xcb_window_t old_frame = _match_depth(con->get_window(), nc);

    x_reparent_child(nc, con);

    bool moved_workpaces = (nc->con_get_workspace() != con->con_get_workspace());

    con_merge_into(con, nc);

    /* Destroy the old frame if we had to reframe the container. This needs to be done
     * after rendering in order to prevent the background from flickering in its place. */
    if (old_frame != XCB_NONE) {
        xcb_destroy_window(**global.x, old_frame);
    }

    global.assignmentManager->run_assignments(nc->get_window());

    if (moved_workpaces) {
        /* If the window is associated with a startup sequence, delete it so
         * child windows won't be created on the old workspace. */
        global.applicationLauncher->startup_sequence_delete_by_window(nc->get_window());

        ewmh_update_wm_desktop();
    }

    nc->get_window()->swallowed = true;
    return nc;
}
