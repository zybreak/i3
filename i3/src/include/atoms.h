#pragma once

#import <xcb/xproto.h>

#define I3_NET_SUPPORTED_ATOMS_XMACRO \
xmacro(_NET_SUPPORTED) \
xmacro(_NET_SUPPORTING_WM_CHECK) \
xmacro(_NET_WM_NAME) \
xmacro(_NET_WM_VISIBLE_NAME) \
xmacro(_NET_WM_MOVERESIZE) \
xmacro(_NET_WM_STATE_STICKY) \
xmacro(_NET_WM_STATE_FULLSCREEN) \
xmacro(_NET_WM_STATE_DEMANDS_ATTENTION) \
xmacro(_NET_WM_STATE_MODAL) \
xmacro(_NET_WM_STATE_HIDDEN) \
xmacro(_NET_WM_STATE_FOCUSED) \
xmacro(_NET_WM_STATE) \
xmacro(_NET_WM_WINDOW_TYPE) \
xmacro(_NET_WM_WINDOW_TYPE_NORMAL) \
xmacro(_NET_WM_WINDOW_TYPE_DOCK) \
xmacro(_NET_WM_WINDOW_TYPE_DIALOG) \
xmacro(_NET_WM_WINDOW_TYPE_UTILITY) \
xmacro(_NET_WM_WINDOW_TYPE_TOOLBAR) \
xmacro(_NET_WM_WINDOW_TYPE_SPLASH) \
xmacro(_NET_WM_WINDOW_TYPE_MENU) \
xmacro(_NET_WM_WINDOW_TYPE_DROPDOWN_MENU) \
xmacro(_NET_WM_WINDOW_TYPE_POPUP_MENU) \
xmacro(_NET_WM_WINDOW_TYPE_TOOLTIP) \
xmacro(_NET_WM_WINDOW_TYPE_NOTIFICATION) \
xmacro(_NET_WM_DESKTOP) \
xmacro(_NET_WM_STRUT_PARTIAL) \
xmacro(_NET_CLIENT_LIST) \
xmacro(_NET_CLIENT_LIST_STACKING) \
xmacro(_NET_CURRENT_DESKTOP) \
xmacro(_NET_NUMBER_OF_DESKTOPS) \
xmacro(_NET_DESKTOP_NAMES) \
xmacro(_NET_DESKTOP_VIEWPORT) \
xmacro(_NET_ACTIVE_WINDOW) \
xmacro(_NET_CLOSE_WINDOW) \
xmacro(_NET_MOVERESIZE_WINDOW)

#define I3_REST_ATOMS_XMACRO \
xmacro(_NET_WM_USER_TIME) \
xmacro(_NET_STARTUP_ID) \
xmacro(_NET_WORKAREA) \
xmacro(_NET_WM_ICON) \
xmacro(WM_PROTOCOLS) \
xmacro(WM_DELETE_WINDOW) \
xmacro(UTF8_STRING) \
xmacro(WM_STATE) \
xmacro(WM_CLIENT_LEADER) \
xmacro(WM_TAKE_FOCUS) \
xmacro(WM_WINDOW_ROLE) \
xmacro(I3_SOCKET_PATH) \
xmacro(I3_CONFIG_PATH) \
xmacro(I3_SYNC) \
xmacro(I3_PID) \
xmacro(I3_LOG_STREAM_SOCKET_PATH) \
xmacro(I3_FLOATING_WINDOW) \
xmacro(_NET_REQUEST_FRAME_EXTENTS) \
xmacro(_NET_FRAME_EXTENTS) \
xmacro(_MOTIF_WM_HINTS) \
xmacro(WM_CHANGE_STATE)

#define xmacro(atom) extern xcb_atom_t A_##atom;
I3_NET_SUPPORTED_ATOMS_XMACRO
I3_REST_ATOMS_XMACRO
#undef xmacro

extern std::array<xcb_atom_t, 35> supported_atoms[];

/* Setup NetWM atoms */
void setup_atoms();
