module;
#include <xcb/xcb.h>
#include "xpp/xpp.hpp"
module i3;

import log;

static std::array const I3_ATOMS{
    std::make_pair(i3::Atom::_NET_SUPPORTED, "_NET_SUPPORTED"),
    std::make_pair(i3::Atom::_NET_SUPPORTING_WM_CHECK, "_NET_SUPPORTING_WM_CHECK"),
    std::make_pair(i3::Atom::_NET_WM_NAME, "_NET_WM_NAME"),
    std::make_pair(i3::Atom::_NET_WM_VISIBLE_NAME, "_NET_WM_VISIBLE_NAME"),
    std::make_pair(i3::Atom::_NET_WM_MOVERESIZE, "_NET_WM_MOVERESIZE"),
    std::make_pair(i3::Atom::_NET_WM_STATE_STICKY, "_NET_WM_STATE_STICKY"),
    std::make_pair(i3::Atom::_NET_WM_STATE_FULLSCREEN, "_NET_WM_STATE_FULLSCREEN"),
    std::make_pair(i3::Atom::_NET_WM_STATE_DEMANDS_ATTENTION, "_NET_WM_STATE_DEMANDS_ATTENTION"),
    std::make_pair(i3::Atom::_NET_WM_STATE_MODAL, "_NET_WM_STATE_MODAL"),
    std::make_pair(i3::Atom::_NET_WM_STATE_HIDDEN, "_NET_WM_STATE_HIDDEN"),
    std::make_pair(i3::Atom::_NET_WM_STATE_FOCUSED, "_NET_WM_STATE_FOCUSED"),
    std::make_pair(i3::Atom::_NET_WM_STATE, "_NET_WM_STATE"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL, "_NET_WM_WINDOW_TYPE_NORMAL"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_DOCK, "_NET_WM_WINDOW_TYPE_DOCK"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG, "_NET_WM_WINDOW_TYPE_DIALOG"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY, "_NET_WM_WINDOW_TYPE_UTILITY"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR, "_NET_WM_WINDOW_TYPE_TOOLBAR"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH, "_NET_WM_WINDOW_TYPE_SPLASH"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_MENU, "_NET_WM_WINDOW_TYPE_MENU"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_POPUP_MENU, "_NET_WM_WINDOW_TYPE_POPUP_MENU"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_TOOLTIP, "_NET_WM_WINDOW_TYPE_TOOLTIP"),
    std::make_pair(i3::Atom::_NET_WM_WINDOW_TYPE_NOTIFICATION, "_NET_WM_WINDOW_TYPE_NOTIFICATION"),
    std::make_pair(i3::Atom::_NET_WM_DESKTOP, "_NET_WM_DESKTOP"),
    std::make_pair(i3::Atom::_NET_WM_STRUT_PARTIAL, "_NET_WM_STRUT_PARTIAL"),
    std::make_pair(i3::Atom::_NET_CLIENT_LIST, "_NET_CLIENT_LIST"),
    std::make_pair(i3::Atom::_NET_CLIENT_LIST_STACKING, "_NET_CLIENT_LIST_STACKING"),
    std::make_pair(i3::Atom::_NET_CURRENT_DESKTOP, "_NET_CURRENT_DESKTOP"),
    std::make_pair(i3::Atom::_NET_NUMBER_OF_DESKTOPS, "_NET_NUMBER_OF_DESKTOPS"),
    std::make_pair(i3::Atom::_NET_DESKTOP_NAMES, "_NET_DESKTOP_NAMES"),
    std::make_pair(i3::Atom::_NET_DESKTOP_VIEWPORT, "_NET_DESKTOP_VIEWPORT"),
    std::make_pair(i3::Atom::_NET_ACTIVE_WINDOW, "_NET_ACTIVE_WINDOW"),
    std::make_pair(i3::Atom::_NET_CLOSE_WINDOW, "_NET_CLOSE_WINDOW"),
    std::make_pair(i3::Atom::_NET_MOVERESIZE_WINDOW, "_NET_MOVERESIZE_WINDOW"),
    std::make_pair(i3::Atom::_NET_WM_USER_TIME, "_NET_WM_USER_TIME"),
    std::make_pair(i3::Atom::_NET_STARTUP_ID, "_NET_STARTUP_ID"),
    std::make_pair(i3::Atom::_NET_WORKAREA, "_NET_WORKAREA"),
    std::make_pair(i3::Atom::_NET_WM_ICON, "_NET_WM_ICON"),
    std::make_pair(i3::Atom::WM_PROTOCOLS, "WM_PROTOCOLS"),
    std::make_pair(i3::Atom::WM_DELETE_WINDOW, "WM_DELETE_WINDOW"),
    std::make_pair(i3::Atom::UTF8_STRING, "UTF8_STRING"),
    std::make_pair(i3::Atom::WM_STATE, "WM_STATE"),
    std::make_pair(i3::Atom::WM_CLIENT_LEADER, "WM_CLIENT_LEADER"),
    std::make_pair(i3::Atom::WM_TAKE_FOCUS, "WM_TAKE_FOCUS"),
    std::make_pair(i3::Atom::WM_WINDOW_ROLE, "WM_WINDOW_ROLE"),
    std::make_pair(i3::Atom::I3_SOCKET_PATH, "I3_SOCKET_PATH"),
    std::make_pair(i3::Atom::I3_CONFIG_PATH, "I3_CONFIG_PATH"),
    std::make_pair(i3::Atom::I3_SYNC, "I3_SYNC"),
    std::make_pair(i3::Atom::I3_PID, "I3_PID"),
    std::make_pair(i3::Atom::I3_LOG_STREAM_SOCKET_PATH, "I3_LOG_STREAM_SOCKET_PATH"),
    std::make_pair(i3::Atom::I3_FLOATING_WINDOW, "I3_FLOATING_WINDOW"),
    std::make_pair(i3::Atom::_NET_REQUEST_FRAME_EXTENTS, "_NET_REQUEST_FRAME_EXTENTS"),
    std::make_pair(i3::Atom::_NET_FRAME_EXTENTS, "_NET_FRAME_EXTENTS"),
    std::make_pair(i3::Atom::_MOTIF_WM_HINTS, "_MOTIF_WM_HINTS"),
    std::make_pair(i3::Atom::WM_CHANGE_STATE, "WM_CHANGE_STATE"),
    std::make_pair(i3::Atom::MANAGER, "MANAGER")};

/* Setup NetWM atoms */
void setup_atoms(X &x) {
    for (auto &atom : I3_ATOMS) {
        try {
            i3::atoms[atom.first] = xpp::x::intern_atom(*x, false, atom.second).atom();
        } catch (std::exception &e) {
            ELOG(std::format("Could not get atom {}", atom.second));
            exit(-1);
        }
    }
}