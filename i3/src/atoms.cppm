module;
#include <xcb/xcb.h>
#include "xpp/xpp.hpp"
export module i3:atoms;

import std;

class X;

export namespace i3 {
    enum class Atom {
        _NET_SUPPORTED,
        _NET_SUPPORTING_WM_CHECK,
        _NET_WM_NAME,
        _NET_WM_VISIBLE_NAME,
        _NET_WM_MOVERESIZE,
        _NET_WM_STATE_STICKY,
        _NET_WM_STATE_FULLSCREEN,
        _NET_WM_STATE_DEMANDS_ATTENTION,
        _NET_WM_STATE_MODAL,
        _NET_WM_STATE_HIDDEN,
        _NET_WM_STATE_FOCUSED,
        _NET_WM_STATE_MAXIMIZED_VERT,
        _NET_WM_STATE_MAXIMIZED_HORZ,
        _NET_WM_STATE,
        _NET_WM_WINDOW_TYPE,
        _NET_WM_WINDOW_TYPE_NORMAL,
        _NET_WM_WINDOW_TYPE_DOCK,
        _NET_WM_WINDOW_TYPE_DIALOG,
        _NET_WM_WINDOW_TYPE_UTILITY,
        _NET_WM_WINDOW_TYPE_TOOLBAR,
        _NET_WM_WINDOW_TYPE_SPLASH,
        _NET_WM_WINDOW_TYPE_MENU,
        _NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        _NET_WM_WINDOW_TYPE_POPUP_MENU,
        _NET_WM_WINDOW_TYPE_TOOLTIP,
        _NET_WM_WINDOW_TYPE_NOTIFICATION,
        _NET_WM_DESKTOP,
        _NET_WM_STRUT_PARTIAL,
        _NET_CLIENT_LIST,
        _NET_CLIENT_LIST_STACKING,
        _NET_CURRENT_DESKTOP,
        _NET_NUMBER_OF_DESKTOPS,
        _NET_DESKTOP_NAMES,
        _NET_DESKTOP_VIEWPORT,
        _NET_ACTIVE_WINDOW,
        _NET_CLOSE_WINDOW,
        _NET_MOVERESIZE_WINDOW,
        _NET_WM_USER_TIME,
        _NET_STARTUP_ID,
        _NET_WORKAREA,
        _NET_WM_ICON,
        WM_PROTOCOLS,
        WM_DELETE_WINDOW,
        UTF8_STRING,
        WM_STATE,
        WM_CLIENT_LEADER,
        WM_TAKE_FOCUS,
        WM_WINDOW_ROLE,
        I3_SOCKET_PATH,
        I3_CONFIG_PATH,
        I3_SYNC,
        I3_PID,
        I3_LOG_STREAM_SOCKET_PATH,
        I3_FLOATING_WINDOW,
        _NET_REQUEST_FRAME_EXTENTS,
        _NET_FRAME_EXTENTS,
        _MOTIF_WM_HINTS,
        WM_CHANGE_STATE,
        MANAGER
    };

    std::map<Atom, xcb_atom_t> atoms{};
}  // namespace i3

/* Setup NetWM atoms */
export void setup_atoms(X &x);
