/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * ewmh.c: Get/set certain EWMH properties easily.
 *
 */
module;
#include <fmt/printf.h>
#include <xcb/xcb.h>

xcb_window_t ewmh_window;
module i3;

import std;
import log;

/*
 * Updates _NET_CURRENT_DESKTOP with the current desktop number.
 *
 * EWMH: The index of the current desktop. This is always an integer between 0
 * and _NET_NUMBER_OF_DESKTOPS - 1.
 *
 */
void ewmh_update_current_desktop() {
    static uint32_t old_idx = NET_WM_DESKTOP_NONE;
    const uint32_t idx = ewmh_get_workspace_index(global.focused);

    if (idx == old_idx || idx == NET_WM_DESKTOP_NONE) {
        return;
    }
    old_idx = idx;

    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::_NET_CURRENT_DESKTOP], XCB_ATOM_CARDINAL, 32, 1, &idx);
}

/*
 * Updates _NET_NUMBER_OF_DESKTOPS which we interpret as the number of
 * noninternal workspaces.
 */
static void ewmh_update_number_of_desktops() {
    static uint32_t old_idx = 0;
    uint32_t idx = 0;

    for (auto &output : global.croot->nodes) {
        idx += dynamic_cast<OutputCon*>(output)->output_get_content()->nodes.size();
    }

    if (idx == old_idx) {
        return;
    }
    old_idx = idx;

    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root,
                        i3::atoms[i3::Atom::_NET_NUMBER_OF_DESKTOPS], XCB_ATOM_CARDINAL, 32, 1, &idx);
}

/*
 * Updates _NET_DESKTOP_NAMES: "The names of all virtual desktops. This is a
 * list of NULL-terminated strings in UTF-8 encoding"
 */
static void ewmh_update_desktop_names() {
    int msg_length = 0;

    /* count the size of the property message to set */
    for (auto &output : global.croot->nodes) {
        for (auto &ws : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            msg_length += ws->name.length() + 1;
        }
    }

    std::vector<char> desktop_names{};
    desktop_names.reserve(msg_length);

    /* fill the buffer with the names of the i3 workspaces */
    for (auto &output : global.croot->nodes) {
        for (auto &ws : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            for (size_t i = 0; i < ws->name.length() + 1; i++) {
                desktop_names.push_back(ws->name[i]);
            }
        }
    }

    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root,
                        i3::atoms[i3::Atom::_NET_DESKTOP_NAMES], i3::atoms[i3::Atom::UTF8_STRING], 8, desktop_names.size(), desktop_names.data());
}

/*
 * Updates _NET_DESKTOP_VIEWPORT, which is an array of pairs of cardinals that
 * define the top left corner of each desktop's viewport.
 */
static void ewmh_update_desktop_viewport() {
    int num_desktops = 0;
    /* count number of desktops */
    for (auto &output : global.croot->nodes) {
        num_desktops += dynamic_cast<OutputCon*>(output)->output_get_content()->nodes.size();
    }

    std::vector<uint32_t> viewports{};
    viewports.reserve(num_desktops * 2);

    /* fill the viewport buffer */
    for (auto output : global.croot->nodes) {
        auto content = dynamic_cast<OutputCon*>(output)->output_get_content();
        if (content == nullptr) {
            continue;
        }
        std::ranges::for_each(
            content->nodes, [&](const auto) {
            viewports.push_back(output->rect.x);
            viewports.push_back(output->rect.y);
        });
    }

    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root,
        i3::atoms[i3::Atom::_NET_DESKTOP_VIEWPORT], XCB_ATOM_CARDINAL, 32, viewports.size(), viewports.data());
}

/*
 * Updates all the EWMH desktop properties.
 *
 */
void ewmh_update_desktop_properties() {
    ewmh_update_number_of_desktops();
    ewmh_update_desktop_viewport();
    ewmh_update_current_desktop();
    ewmh_update_desktop_names();
    ewmh_update_wm_desktop();
}

static void ewmh_update_wm_desktop_recursively(Con *con, const uint32_t desktop) {
    /* Recursively call this to descend through the entire subtree. */
    for (auto &child : con->nodes) {
        ewmh_update_wm_desktop_recursively(child, desktop);
    }

    /* If con is a workspace, we also need to go through the floating windows on it. */
    if (con->type == CT_WORKSPACE) {
        for (auto &child : dynamic_cast<WorkspaceCon *>(con)->floating_windows) {
            ewmh_update_wm_desktop_recursively(child, desktop);
        }
    }

    if (!con->con_has_managed_window())
        return;

    uint32_t wm_desktop = desktop;
    /* Sticky windows are only actually sticky when they are floating or inside
     * a floating container. This is technically still slightly wrong, since
     * sticky windows will only be on all workspaces on this output, but we
     * ignore multi-monitor situations for this since the spec isn't too
     * precise on this anyway. */
    if (con->con_is_sticky() && con->con_is_floating()) {
        wm_desktop = NET_WM_DESKTOP_ALL;
    }

    /* If this is the cached value, we don't need to do anything. */
    if (con->window->wm_desktop == wm_desktop) {
        return;
    }
    con->window->wm_desktop = wm_desktop;

    const xcb_window_t window = con->window->id;
    if (wm_desktop != NET_WM_DESKTOP_NONE) {
        DLOG(fmt::sprintf("Setting _NET_WM_DESKTOP = %d for window 0x%08x.\n", wm_desktop, window));
        xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, window, i3::atoms[i3::Atom::_NET_WM_DESKTOP], XCB_ATOM_CARDINAL, 32, 1, &wm_desktop);
    } else {
        /* If we can't determine the workspace index, delete the property. We'd
         * rather not set it than lie. */
        ELOG(fmt::sprintf("Failed to determine the proper EWMH desktop index for window 0x%08x, deleting _NET_WM_DESKTOP.\n", window));
        xcb_delete_property(**global.x, window, i3::atoms[i3::Atom::_NET_WM_DESKTOP]);
    }
}

/*
 * Updates _NET_WM_DESKTOP for all windows.
 * A request will only be made if the cached value differs from the calculated value.
 *
 */
void ewmh_update_wm_desktop() {
    uint32_t desktop = 0;

    for (auto &output : global.croot->nodes) {
        for (auto &workspace : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            ewmh_update_wm_desktop_recursively(workspace, desktop);
            ++desktop;
        }
    }
}

/*
 * Updates _NET_ACTIVE_WINDOW with the currently focused window.
 *
 * EWMH: The window ID of the currently active window or None if no window has
 * the focus.
 *
 */
void ewmh_update_active_window(xcb_window_t window) {
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root,
                        i3::atoms[i3::Atom::_NET_ACTIVE_WINDOW], XCB_ATOM_WINDOW, 32, 1, &window);
}

/*
 * Updates _NET_WM_VISIBLE_NAME.
 *
 */
void ewmh_update_visible_name(xcb_window_t window, const char *name) {
    if (name != nullptr) {
        xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, window, i3::atoms[i3::Atom::_NET_WM_VISIBLE_NAME], i3::atoms[i3::Atom::UTF8_STRING], 8, strlen(name), name);
    } else {
        xcb_delete_property(**global.x, window, i3::atoms[i3::Atom::_NET_WM_VISIBLE_NAME]);
    }
}

/*
 * i3 currently does not support _NET_WORKAREA, because it does not correspond
 * to i3’s concept of workspaces. See also:
 * https://bugs.i3wm.org/539
 * https://bugs.i3wm.org/301
 * https://bugs.i3wm.org/1038
 *
 * We need to actively delete this property because some display managers (e.g.
 * LightDM) set it.
 *
 * EWMH: Contains a geometry for each desktop. These geometries specify an area
 * that is completely contained within the viewport. Work area SHOULD be used by
 * desktop applications to place desktop icons appropriately.
 *
 */
void ewmh_update_workarea() {
    xcb_delete_property(**global.x, global.x->root, i3::atoms[i3::Atom::_NET_WORKAREA]);
}

/*
 * Updates the _NET_CLIENT_LIST hint.
 *
 */
void ewmh_update_client_list(xcb_window_t *list, int num_windows) {
    xcb_change_property(
        **global.x,
        XCB_PROP_MODE_REPLACE,
        global.x->root,
        i3::atoms[i3::Atom::_NET_CLIENT_LIST],
        XCB_ATOM_WINDOW,
        32,
        num_windows,
        list);
}

/*
 * Updates the _NET_CLIENT_LIST_STACKING hint.
 *
 */
void ewmh_update_client_list_stacking(xcb_window_t *stack, int num_windows) {
    xcb_change_property(
        **global.x,
        XCB_PROP_MODE_REPLACE,
        global.x->root,
        i3::atoms[i3::Atom::_NET_CLIENT_LIST_STACKING],
        XCB_ATOM_WINDOW,
        32,
        num_windows,
        stack);
}

/*
 * Set or remove _NET_WM_STATE_STICKY on the window.
 *
 */
void ewmh_update_sticky(xcb_window_t window, bool sticky) {
    if (sticky) {
        DLOG(fmt::sprintf("Setting _NET_WM_STATE_STICKY for window = %08x.\n", window));
        xcb_add_property_atom(**global.x, window, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_STICKY]);
    } else {
        DLOG(fmt::sprintf("Removing _NET_WM_STATE_STICKY for window = %08x.\n", window));
        xcb_remove_property_atom(**global.x, window, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_STICKY]);
    }
}

/*
 * Set or remove _NEW_WM_STATE_FOCUSED on the window.
 *
 */
void ewmh_update_focused(xcb_window_t window, bool is_focused) {
    if (is_focused) {
        DLOG(fmt::sprintf("Setting _NET_WM_STATE_FOCUSED for window = %08x.\n", window));
        xcb_add_property_atom(**global.x, window, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_FOCUSED]);
    } else {
        DLOG(fmt::sprintf("Removing _NET_WM_STATE_FOCUSED for window = %08x.\n", window));
        xcb_remove_property_atom(**global.x, window, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_FOCUSED]);
    }
}

/*
 * Set up the EWMH hints on the root window.
 *
 */
void ewmh_setup_hints() {

    std::array supported_atoms = {
        i3::atoms[i3::Atom::_NET_SUPPORTED],
        i3::atoms[i3::Atom::_NET_SUPPORTING_WM_CHECK],
        i3::atoms[i3::Atom::_NET_WM_NAME],
        i3::atoms[i3::Atom::_NET_WM_VISIBLE_NAME],
        i3::atoms[i3::Atom::_NET_WM_MOVERESIZE],
        i3::atoms[i3::Atom::_NET_WM_STATE_STICKY],
        i3::atoms[i3::Atom::_NET_WM_STATE_FULLSCREEN],
        i3::atoms[i3::Atom::_NET_WM_STATE_DEMANDS_ATTENTION],
        i3::atoms[i3::Atom::_NET_WM_STATE_MODAL],
        i3::atoms[i3::Atom::_NET_WM_STATE_HIDDEN],
        i3::atoms[i3::Atom::_NET_WM_STATE_FOCUSED],
        i3::atoms[i3::Atom::_NET_WM_STATE],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DOCK],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_MENU],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_POPUP_MENU],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLTIP],
        i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NOTIFICATION],
        i3::atoms[i3::Atom::_NET_WM_DESKTOP],
        i3::atoms[i3::Atom::_NET_WM_STRUT_PARTIAL],
        i3::atoms[i3::Atom::_NET_CLIENT_LIST],
        i3::atoms[i3::Atom::_NET_CLIENT_LIST_STACKING],
        i3::atoms[i3::Atom::_NET_CURRENT_DESKTOP],
        i3::atoms[i3::Atom::_NET_NUMBER_OF_DESKTOPS],
        i3::atoms[i3::Atom::_NET_DESKTOP_NAMES],
        i3::atoms[i3::Atom::_NET_DESKTOP_VIEWPORT],
        i3::atoms[i3::Atom::_NET_ACTIVE_WINDOW],
        i3::atoms[i3::Atom::_NET_CLOSE_WINDOW],
        i3::atoms[i3::Atom::_NET_MOVERESIZE_WINDOW]
    };

    /* Set up the window manager’s name. According to EWMH, section "Root Window
     * Properties", to indicate that an EWMH-compliant window manager is
     * present, a child window has to be created (and kept alive as long as the
     * window manager is running) which has the _NET_SUPPORTING_WM_CHECK and
     * _NET_WM_ATOMS. */
    ewmh_window = xcb_generate_id(**global.x);
    /* We create the window and put it at (-1, -1) so that it is off-screen. */
    uint32_t v_list[]{1};
    xcb_create_window(
        **global.x,
        XCB_COPY_FROM_PARENT,        /* depth */
        ewmh_window,                 /* window id */
        global.x->root,              /* parent */
        -1, -1, 1, 1,                /* dimensions (x, y, w, h) */
        0,                           /* border */
        XCB_WINDOW_CLASS_INPUT_ONLY, /* window class */
        XCB_COPY_FROM_PARENT,        /* visual */
        XCB_CW_OVERRIDE_REDIRECT,
        v_list);
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, ewmh_window, i3::atoms[i3::Atom::_NET_SUPPORTING_WM_CHECK], XCB_ATOM_WINDOW, 32, 1, &ewmh_window);
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, ewmh_window, i3::atoms[i3::Atom::_NET_WM_NAME], i3::atoms[i3::Atom::UTF8_STRING], 8, strlen("i3"), "i3");
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::_NET_SUPPORTING_WM_CHECK], XCB_ATOM_WINDOW, 32, 1, &ewmh_window);

    /* I’m not entirely sure if we need to keep _NET_WM_NAME on root. */
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::_NET_WM_NAME], i3::atoms[i3::Atom::UTF8_STRING], 8, strlen("i3"), "i3");

    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::_NET_SUPPORTED], XCB_ATOM_ATOM, 32, /* number of atoms */ supported_atoms.size(), supported_atoms.data());

    /* We need to map this window to be able to set the input focus to it if no other window is available to be focused. */
    xcb_map_window(**global.x, ewmh_window);
    uint32_t value_list[]{XCB_STACK_MODE_BELOW};
    xcb_configure_window(**global.x, ewmh_window, XCB_CONFIG_WINDOW_STACK_MODE, value_list);
}

/*
 * Returns the workspace container as enumerated by the EWMH desktop model.
 * Returns NULL if no workspace could be found for the index.
 *
 * This is the reverse of ewmh_get_workspace_index.
 *
 */
WorkspaceCon *ewmh_get_workspace_by_index(uint32_t idx) {
    if (idx == NET_WM_DESKTOP_NONE) {
        return nullptr;
    }

    uint32_t current_index = 0;

    for (auto &output : global.croot->nodes) {
        for (auto &ws : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            if (current_index == idx) {
                return dynamic_cast<WorkspaceCon*>(ws);
            }
            current_index++;
        }
    }

    return nullptr;
}

/*
 * Returns the EWMH desktop index for the workspace the given container is on.
 * Returns NET_WM_DESKTOP_NONE if the desktop index cannot be determined.
 *
 * This is the reverse of ewmh_get_workspace_by_index.
 *
 */
uint32_t ewmh_get_workspace_index(Con *con) {
    uint32_t index = 0;

    Con *target_workspace = con->con_get_workspace();
    for (auto &output : global.croot->nodes) {
        for (auto &ws : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            if (ws == target_workspace) {
                return index;
            }

            index++;
        }
    }

    return NET_WM_DESKTOP_NONE;
}
