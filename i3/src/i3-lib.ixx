module;
#include <xcb/xproto.h>
export module i3;

export import :assignments;
export import :autostarts;
export import :bindings;
export import :click;
export import :configuration;
export import :con;
export import :util;
export import :x;
export import :workspace;
export import :keysyms;
export import :event_handler;
export import :shape;
export import :xkb;
export import :handlers;
export import :criteria_state;
export import :resize;
export import :window;
export import :match;
export import :tree;
export import :startup;
export import :draw;
export import :sync;
export import :output;
export import :rect;
export import :render;
export import :randr;
export import :font;
export import :format_placeholders;
export import :xcb;
export import :drag;
export import :dpi;
export import :get_visualtype;
export import :ewmh;
export import :floating;
export import :move;

export namespace i3 {

}

export bool is_background_set(x_connection *conn, xcb_screen_t *screen);
