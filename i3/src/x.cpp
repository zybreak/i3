/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * x.c: Interface to X11, transfers our in-memory state to X11 (see also
 *      render.c). Basically a big state machine.
 *
 */
#include <cassert>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <sys/types.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>

#include "libi3.h"

#include "util.h"
#include "i3_ipc/include/i3-ipc.h"
#include "tree.h"
#include "xcb.h"
#include "i3.h"
#include "x.h"
#include "configuration.h"
#include "randr.h"
#include "con.h"
#include "xcursor.h"
#include "ewmh.h"
#include "global.h"

#include <unistd.h>
#include <ranges>

#define COLOR_TRANSPARENT ((color_t){.red = 0.0, .green = 0.0, .blue = 0.0, .colorpixel = 0})

/* Stores the X11 window ID of the currently focused window */
xcb_window_t focused_id = XCB_NONE;

/* Because 'focused_id' might be reset to force input focus, we separately keep
 * track of the X11 window ID to be able to always tell whether the focused
 * window actually changed. */
static xcb_window_t last_focused = XCB_NONE;

/* Stores coordinates to warp mouse pointer to if set */
static Rect *warp_to;

std::deque<con_state*> state_head{};
std::deque<con_state*> old_state_head{};
std::deque<con_state*> initial_mapping_head{};

/*
 * Returns the container state for the given frame. This function always
 * returns a container state (otherwise, there is a bug in the code and the
 * container state of a container for which x_con_init() was not called was
 * requested).
 *
 */
con_state *state_for_frame(xcb_window_t window) {
    auto it = std::ranges::find_if(state_head, [window](auto &state) { return state->id == window; });

    if (it != state_head.end()) {
        return *it;
    }

    /* TODO: better error handling? */
    ELOG(fmt::sprintf("No state found for window 0x%08x\n",  window));
    assert(false);
    return nullptr;
}

/*
 * Changes the atoms on the root window and the windows themselves to properly
 * reflect the current focus for ewmh compliance.
 *
 */
static void change_ewmh_focus(xcb_window_t new_focus, xcb_window_t old_focus) {
    if (new_focus == old_focus) {
        return;
    }

    ewmh_update_active_window(new_focus);

    if (new_focus != XCB_WINDOW_NONE) {
        ewmh_update_focused(new_focus, true);
    }

    if (old_focus != XCB_WINDOW_NONE) {
        ewmh_update_focused(old_focus, false);
    }
}

/*
 * Initializes the X11 part for the given container. Called exactly once for
 * every container from new Con().
 *
 */
void x_con_init(Con *con) {
    /* TODO: maybe create the window when rendering first? we could then even
     * get the initial geometry right */

    uint32_t mask = 0;
    uint32_t values[5];

    xcb_visualid_t visual = get_visualid_by_depth(con->depth);
    xcb_colormap_t win_colormap;
    if (con->depth != root_depth) {
        /* We need to create a custom colormap. */
        win_colormap = xcb_generate_id(global.conn);
        xcb_create_colormap(global.conn, XCB_COLORMAP_ALLOC_NONE, win_colormap, root, visual);
        con->colormap = win_colormap;
    } else {
        /* Use the default colormap. */
        win_colormap = colormap;
        con->colormap = XCB_NONE;
    }

    /* We explicitly set a background color and border color (even though we
     * don’t even have a border) because the X11 server requires us to when
     * using 32 bit color depths, see
     * https://stackoverflow.com/questions/3645632 */
    mask |= XCB_CW_BACK_PIXEL;
    values[0] = global.root_screen->black_pixel;

    mask |= XCB_CW_BORDER_PIXEL;
    values[1] = global.root_screen->black_pixel;

    /* our own frames should not be managed */
    mask |= XCB_CW_OVERRIDE_REDIRECT;
    values[2] = 1;

    /* see include/xcb.h for the FRAME_EVENT_MASK */
    mask |= XCB_CW_EVENT_MASK;
    values[3] = FRAME_EVENT_MASK & ~XCB_EVENT_MASK_ENTER_WINDOW;

    mask |= XCB_CW_COLORMAP;
    values[4] = win_colormap;

    Rect dims = {(uint32_t)-15, (uint32_t)-15, 10, 10};
    xcb_window_t frame_id = create_window(global.conn, dims, con->depth, visual, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCURSOR_CURSOR_POINTER, false, mask, values);
    draw_util_surface_init(global.conn, &(con->frame), frame_id, get_visualtype_by_id(visual), dims.width, dims.height);
    xcb_change_property(global.conn,
                        XCB_PROP_MODE_REPLACE,
                        con->frame.id,
                        XCB_ATOM_WM_CLASS,
                        XCB_ATOM_STRING,
                        8,
                        (strlen("i3-frame") + 1) * 2,
                        "i3-frame\0i3-frame\0");

    auto *state = new con_state();
    state->id = con->frame.id;
    state->mapped = false;
    state->initial = true;
    DLOG(fmt::sprintf("Adding window 0x%08x to lists\n",  state->id));

    state_head.push_front(state);
    old_state_head.push_front(state);
    initial_mapping_head.push_back(state);
    DLOG(fmt::sprintf("adding new state for window id 0x%08x\n",  state->id));
}

/*
 * Reparents the child window of the given container (necessary for sticky
 * containers). The reparenting happens in the next call of x_push_changes().
 *
 */
void x_reparent_child(Con *con, Con *old) {
    struct con_state *state;
    if ((state = state_for_frame(con->frame.id)) == nullptr) {
        ELOG("window state for con not found\n");
        return;
    }

    state->need_reparent = true;
    state->old_frame = old->frame.id;
}

static void _x_con_kill(Con *con) {
    con_state *state;

    if (con->colormap != XCB_NONE) {
        xcb_free_colormap(global.conn, con->colormap);
    }

    draw_util_surface_free(global.conn, &(con->frame));
    draw_util_surface_free(global.conn, &(con->frame_buffer));
    xcb_free_pixmap(global.conn, con->frame_buffer.id);
    con->frame_buffer.id = XCB_NONE;
    state = state_for_frame(con->frame.id);
    std::erase(state_head, state);
    std::erase(old_state_head, state);
    std::erase(initial_mapping_head, state);
    delete state;

    /* Invalidate focused_id to correctly focus new windows with the same ID */
    if (con->frame.id == focused_id) {
        focused_id = XCB_NONE;
    }
    if (con->frame.id == last_focused) {
        last_focused = XCB_NONE;
    }
}

/*
 * Kills the window decoration associated with the given container.
 *
 */
void x_con_kill(Con *con) {
    _x_con_kill(con);
    xcb_destroy_window(global.conn, con->frame.id);
}

/*
 * Completely reinitializes the container's frame, without destroying the old window.
 *
 */
void x_con_reframe(Con *con) {
    _x_con_kill(con);
    x_con_init(con);
}

/*
 * Returns true if the client supports the given protocol atom (like WM_DELETE_WINDOW)
 *
 */
bool window_supports_protocol(xcb_window_t window, xcb_atom_t atom) {
    xcb_get_property_cookie_t cookie;
    xcb_icccm_get_wm_protocols_reply_t protocols;
    bool result = false;

    cookie = xcb_icccm_get_wm_protocols(global.conn, window, A_WM_PROTOCOLS);
    if (xcb_icccm_get_wm_protocols_reply(global.conn, cookie, &protocols, nullptr) != 1)
        return false;

    /* Check if the client’s protocols have the requested atom set */
    for (uint32_t i = 0; i < protocols.atoms_len; i++)
        if (protocols.atoms[i] == atom)
            result = true;

    xcb_icccm_get_wm_protocols_reply_wipe(&protocols);

    return result;
}

/*
 * Kills the given X11 window using WM_DELETE_WINDOW (if supported).
 *
 */
void x_window_kill(xcb_window_t window, kill_window_t kill_window) {
    /* if this window does not support WM_DELETE_WINDOW, we kill it the hard way */
    if (!window_supports_protocol(window, A_WM_DELETE_WINDOW)) {
        if (kill_window == KILL_WINDOW) {
            LOG(fmt::sprintf("Killing specific window 0x%08x\n",  window));
            xcb_destroy_window(global.conn, window);
        } else {
            LOG(fmt::sprintf("Killing the X11 client which owns window 0x%08x\n",  window));
            xcb_kill_client(global.conn, window);
        }
        return;
    }

    /* Every X11 event is 32 bytes long. Therefore, XCB will copy 32 bytes.
     * In order to properly initialize these bytes, we allocate 32 bytes even
     * though we only need less for an xcb_configure_notify_event_t */
    void *event = scalloc(32, 1);
    auto *ev = (xcb_client_message_event_t*)event;

    ev->response_type = XCB_CLIENT_MESSAGE;
    ev->window = window;
    ev->type = A_WM_PROTOCOLS;
    ev->format = 32;
    ev->data.data32[0] = A_WM_DELETE_WINDOW;
    ev->data.data32[1] = XCB_CURRENT_TIME;

    LOG("Sending WM_DELETE to the client\n");
    xcb_send_event(global.conn, false, window, XCB_EVENT_MASK_NO_EVENT, (char *)ev);
    xcb_flush(global.conn);
    free(event);
}

static void x_draw_title_border(Con *con, struct deco_render_params *p) {
    assert(con->parent != nullptr);

    Rect *dr = &(con->deco_rect);

    /* Left */
    draw_util_rectangle(&(con->parent->frame_buffer), p->color->border,
                        dr->x, dr->y, 1, dr->height);

    /* Right */
    draw_util_rectangle(&(con->parent->frame_buffer), p->color->border,
                        dr->x + dr->width - 1, dr->y, 1, dr->height);

    /* Top */
    draw_util_rectangle(&(con->parent->frame_buffer), p->color->border,
                        dr->x, dr->y, dr->width, 1);

    /* Bottom */
    draw_util_rectangle(&(con->parent->frame_buffer), p->color->border,
                        dr->x, dr->y + dr->height - 1, dr->width, 1);
}

static void x_draw_decoration_after_title(Con *con, struct deco_render_params *p) {
    assert(con->parent != nullptr);

    Rect *dr = &(con->deco_rect);

    /* Redraw the right border to cut off any text that went past it.
     * This is necessary when the text was drawn using XCB since cutting text off
     * automatically does not work there. For pango rendering, this isn't necessary. */
    if (!font_is_pango()) {
        /* We actually only redraw the far right two pixels as that is the
         * distance we keep from the edge (not the entire border width).
         * Redrawing the entire border would cause text to be cut off. */
        draw_util_rectangle(&(con->parent->frame_buffer), p->color->background,
                            dr->x + dr->width - 2 * logical_px(global.root_screen, 1),
                            dr->y,
                            2 * logical_px(global.root_screen, 1),
                            dr->height);
    }

    /* Redraw the border. */
    x_draw_title_border(con, p);
}

/*
 * Get rectangles representing the border around the child window. Some borders
 * are adjacent to the screen-edge and thus not returned. Return value is the
 * number of rectangles.
 *
 */
static size_t x_get_border_rectangles(Con *con, xcb_rectangle_t rectangles[4]) {
    size_t count = 0;
    int border_style = con_border_style(con);

    if (border_style != BS_NONE && con->con_is_leaf()) {
        auto borders_to_hide = (adjacent_t)(con_adjacent_borders(con) & config.hide_edge_borders);
        Rect br = con_border_style_rect(con);

        if (!(borders_to_hide & ADJ_LEFT_SCREEN_EDGE)) {
            rectangles[count++] = (xcb_rectangle_t){
                .x = 0,
                .y = 0,
                .width = static_cast<uint16_t>(br.x),
                .height = static_cast<uint16_t>(con->rect.height),
            };
        }
        if (!(borders_to_hide & ADJ_RIGHT_SCREEN_EDGE)) {
            rectangles[count++] = (xcb_rectangle_t){
                .x = static_cast<int16_t>(con->rect.width + (br.width + br.x)),
                .y = 0,
                .width = static_cast<uint16_t>(-(br.width + br.x)),
                .height = static_cast<uint16_t>(con->rect.height),
            };
        }
        if (!(borders_to_hide & ADJ_LOWER_SCREEN_EDGE)) {
            rectangles[count++] = (xcb_rectangle_t){
                .x = static_cast<int16_t>(br.x),
                .y = static_cast<int16_t>(con->rect.height + (br.height + br.y)),
                .width = static_cast<uint16_t>(con->rect.width + br.width),
                .height = static_cast<uint16_t>(-(br.height + br.y)),
            };
        }
        /* pixel border have an additional line at the top */
        if (border_style == BS_PIXEL && !(borders_to_hide & ADJ_UPPER_SCREEN_EDGE)) {
            rectangles[count++] = (xcb_rectangle_t){
                .x = static_cast<int16_t>(br.x),
                .y = 0,
                .width = static_cast<uint16_t>(con->rect.width + br.width),
                .height = static_cast<uint16_t>(br.y),
            };
        }
    }

    return count;
}

/**
 * This function is a convenience wrapper and takes care of flushing the
 * surface as well as restoring the cairo state.
 *
 */
static void draw_util_image(cairo_surface_t *image, surface_t *surface, int x, int y, int width, int height) {
    if ((surface)->id == XCB_NONE) {
        ELOG(fmt::sprintf("Surface %p is not initialized, skipping drawing.\n",  (void*)surface));
        return;
    }

    cairo_save(surface->cr);

    cairo_translate(surface->cr, x, y);

    const int src_width = cairo_image_surface_get_width(image);
    const int src_height = cairo_image_surface_get_height(image);
    double scale = std::min((double)width / src_width, (double)height / src_height);
    cairo_scale(surface->cr, scale, scale);

    cairo_set_source_surface(surface->cr, image, 0, 0);
    cairo_paint(surface->cr);

    cairo_restore(surface->cr);
}

/*
 * Draws the decoration of the given container onto its parent.
 *
 */
void x_draw_decoration(Con *con) {
    Con *parent = con->parent;
    bool leaf = con->con_is_leaf();

    /* This code needs to run for:
     *  • leaf containers
     *  • non-leaf containers which are in a stacked/tabbed container
     *
     * It does not need to run for:
     *  • direct children of outputs or dockareas
     *  • floating containers (they don’t have a decoration)
     */
    if ((!leaf &&
         parent->layout != L_STACKED &&
         parent->layout != L_TABBED) ||
        parent->type == CT_OUTPUT ||
        parent->type == CT_DOCKAREA ||
        con->type == CT_FLOATING_CON)
        return;

    /* Skip containers whose height is 0 (for example empty dockareas) */
    if (con->rect.height == 0)
        return;

    /* Skip containers whose pixmap has not yet been created (can happen when
     * decoration rendering happens recursively for a window for which
     * x_push_node() was not yet called) */
    if (leaf && con->frame_buffer.id == XCB_NONE)
        return;

    /* 1: build deco_params and compare with cache */
    auto *p = new deco_render_params();

    /* find out which colors to use */
    if (con->urgent) {
        p->color = &config.client.urgent;
    } else if (con == focused || con->con_inside_focused()) {
        p->color = &config.client.focused;
    } else if (con == con::first(parent->focus_head)) {
        p->color = &config.client.focused_inactive;
    } else {
        p->color = &config.client.unfocused;
    }

    p->border_style = con_border_style(con);

    Rect *r = &(con->rect);
    Rect *w = &(con->window_rect);
    p->con_rect = {r->width, r->height};
    p->con_window_rect = {w->width, w->height};
    p->con_deco_rect = con->deco_rect;
    p->background = config.client.background;
    p->con_is_leaf = con->con_is_leaf();
    p->parent_layout = con->parent->layout;

    if (con->deco_render_params != nullptr &&
        (con->window == nullptr || !con->window->name_x_changed) &&
        !parent->pixmap_recreated &&
        !con->pixmap_recreated &&
        p == con->deco_render_params) {
        delete p;
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    for (auto c_itr = std::ranges::find(con->parent->nodes_head, con); c_itr != con->parent->nodes_head.end(); ++c_itr) {
        delete (*c_itr)->deco_render_params;
        (*c_itr)->deco_render_params = nullptr;
    }

    delete con->deco_render_params;
    con->deco_render_params = p;

    if (con->window != nullptr && con->window->name_x_changed)
        con->window->name_x_changed = false;

    parent->pixmap_recreated = false;
    con->pixmap_recreated = false;

    /* 2: draw the client.background, but only for the parts around the window_rect */
    if (con->window != nullptr) {
        /* Clear visible windows before beginning to draw */
        draw_util_clear_surface(&(con->frame_buffer), (color_t){.red = 0.0, .green = 0.0, .blue = 0.0});

        /* top area */
        draw_util_rectangle(&(con->frame_buffer), config.client.background,
                            0, 0, r->width, w->y);
        /* bottom area */
        draw_util_rectangle(&(con->frame_buffer), config.client.background,
                            0, w->y + w->height, r->width, r->height - (w->y + w->height));
        /* left area */
        draw_util_rectangle(&(con->frame_buffer), config.client.background,
                            0, 0, w->x, r->height);
        /* right area */
        draw_util_rectangle(&(con->frame_buffer), config.client.background,
                            w->x + w->width, 0, r->width - (w->x + w->width), r->height);
    }

    /* 3: draw a rectangle in border color around the client */
    if (p->border_style != BS_NONE && p->con_is_leaf) {
        /* Fill the border. We don’t just fill the whole rectangle because some
         * children are not freely resizable and we want their background color
         * to "shine through". */
        xcb_rectangle_t rectangles[4];
        size_t rectangles_count = x_get_border_rectangles(con, rectangles);
        for (size_t i = 0; i < rectangles_count; i++) {
            draw_util_rectangle(&(con->frame_buffer), p->color->child_border,
                                rectangles[i].x,
                                rectangles[i].y,
                                rectangles[i].width,
                                rectangles[i].height);
        }

        /* Highlight the side of the border at which the next window will be
         * opened if we are rendering a single window within a split container
         * (which is undistinguishable from a single window outside a split
         * container otherwise. */
        Rect br = con_border_style_rect(con);
        if (con::next(con, con->parent->nodes_head) == nullptr &&
            con::previous(con, con->parent->nodes_head) == nullptr &&
            con->parent->type != CT_FLOATING_CON) {
            if (p->parent_layout == L_SPLITH) {
                draw_util_rectangle(&(con->frame_buffer), p->color->indicator,
                                    r->width + (br.width + br.x), br.y, -(br.width + br.x), r->height + br.height);
            } else if (p->parent_layout == L_SPLITV) {
                draw_util_rectangle(&(con->frame_buffer), p->color->indicator,
                                    br.x, r->height + (br.height + br.y), r->width + br.width, -(br.height + br.y));
            }
        }
    }

    /* If the parent hasn't been set up yet, skip the decoration rendering
     * for now. */
    if (parent->frame_buffer.id == XCB_NONE) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    /* For the first child, we clear the parent pixmap to ensure there's no
     * garbage left on there. This is important to avoid tearing when using
     * transparency. */
    if (con == con::first(con->parent->nodes_head)) {
        draw_util_clear_surface(&(con->parent->frame_buffer), COLOR_TRANSPARENT);
        delete con->parent->deco_render_params;
        con->parent->deco_render_params = nullptr;
    }

    /* if this is a borderless/1pixel window, we don’t need to render the
     * decoration. */
    if (p->border_style != BS_NORMAL) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    /* 4: paint the bar */
    draw_util_rectangle(&(parent->frame_buffer), p->color->background,
                        con->deco_rect.x, con->deco_rect.y, con->deco_rect.width, con->deco_rect.height);

    /* 5: draw title border */
    x_draw_title_border(con, p);

    /* 6: draw the icon and title */
    int text_offset_y = (con->deco_rect.height - config.font.height) / 2;
    int text_offset_x = 0;

    i3Window *win = con->window;

    const int title_padding = logical_px(global.root_screen, 2);
    const int deco_width = (int)con->deco_rect.width;

    /* Draw the icon */
    if (con->window_icon_padding > -1 && win && win->icon) {
        /* icon_padding is applied horizontally only,
         * the icon will always use all available vertical space. */
        const int icon_padding = logical_px(global.root_screen, 1 + con->window_icon_padding);

        const uint16_t icon_size = con->deco_rect.height - 2 * logical_px(global.root_screen, 1);

        const int icon_offset_y = logical_px(global.root_screen, 1);

        text_offset_x += icon_size + 2 * icon_padding;

        draw_util_image(
            win->icon,
            &(parent->frame_buffer),
            con->deco_rect.x + icon_padding,
            con->deco_rect.y + icon_offset_y,
            icon_size,
            icon_size);
    }

    i3String *title = nullptr;
    if (win == nullptr) {
        if (con->title_format.empty()) {
            std::string _title = fmt::format("i3: {}", con_get_tree_representation(con));
            title = i3string_from_utf8(_title);
        } else {
            title = con_parse_title_format(con);
        }
    } else {
        title = con->title_format.empty() ? win->name : con_parse_title_format(con);
    }
    if (title == nullptr) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    int title_offset_x;
    switch (config.title_align) {
        case Config::ALIGN_LEFT:
            /* (pad)[text    ](pad) */
            title_offset_x = title_padding;
            break;
        case Config::ALIGN_CENTER:
            /* (pad)[  text  ](pad)
             * To center the text inside its allocated space, the surface
             * between the brackets, we use the formula
             * (surface_width - predict_text_width) / 2
             * where surface_width = deco_width - 2 * pad
             * so, offset = pad + (surface_width - predict_text_width) / 2 =
             * = … = (deco_width - predict_text_width) / 2 */
            title_offset_x = std::max(title_padding, (deco_width - predict_text_width(global.conn, global.root_screen, title)) / 2);
            break;
        case Config::ALIGN_RIGHT:
            /* (pad)[    text](pad) */
            title_offset_x = std::max(title_padding, deco_width - title_padding - predict_text_width(global.conn, global.root_screen, title));
            break;
    }

    draw_util_text(global.conn, title, &(parent->frame_buffer),
                   p->color->text, p->color->background,
                   con->deco_rect.x + text_offset_x + title_offset_x,
                   con->deco_rect.y + text_offset_y,
                   deco_width - text_offset_x - 2 * title_padding);

    if (win == nullptr || !con->title_format.empty()) {
        delete title;
        title = nullptr;
    }

    x_draw_decoration_after_title(con, p);
    draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
}

/*
 * Recursively calls x_draw_decoration. This cannot be done in x_push_node
 * because x_push_node uses focus order to recurse (see the comment above)
 * while drawing the decoration needs to happen in the actual order.
 *
 */
void x_deco_recurse(Con *con) {
    bool leaf = con->nodes_head.empty() && con->floating_windows.empty();
    con_state *state = state_for_frame(con->frame.id);

    if (!leaf) {
        for (auto &current : con->nodes_head) {
            x_deco_recurse(current);
        }

        for (auto &current : con->floating_windows) {
            x_deco_recurse(current);
        }

        if (state->mapped) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
    }

    if ((con->type != CT_ROOT && con->type != CT_OUTPUT) &&
        (!leaf || con->mapped))
        x_draw_decoration(con);
}

/*
 * Sets or removes the _NET_WM_STATE_HIDDEN property on con if necessary.
 *
 */
static void set_hidden_state(Con *con) {
    if (con->window == nullptr) {
        return;
    }

    con_state *state = state_for_frame(con->frame.id);
    bool should_be_hidden = con->con_is_hidden();
    if (should_be_hidden == state->is_hidden)
        return;

    if (should_be_hidden) {
        DLOG(fmt::sprintf("setting _NET_WM_STATE_HIDDEN for con = %p\n",  (void*)con));
        xcb_add_property_atom(global.conn, con->window->id, A__NET_WM_STATE, A__NET_WM_STATE_HIDDEN);
    } else {
        DLOG(fmt::sprintf("removing _NET_WM_STATE_HIDDEN for con = %p\n",  (void*)con));
        xcb_remove_property_atom(global.conn, con->window->id, A__NET_WM_STATE, A__NET_WM_STATE_HIDDEN);
    }

    state->is_hidden = should_be_hidden;
}

/*
 * Set the container frame shape as the union of the window shape and the
 * shape of the frame borders.
 */
static void x_shape_frame(Con *con, xcb_shape_sk_t shape_kind) {
    assert(con->window);

    xcb_shape_combine(global.conn, XCB_SHAPE_SO_SET, shape_kind, shape_kind,
                      con->frame.id,
                      con->window_rect.x + con->border_width,
                      con->window_rect.y + con->border_width,
                      con->window->id);
    xcb_rectangle_t rectangles[4];
    size_t rectangles_count = x_get_border_rectangles(con, rectangles);
    if (rectangles_count) {
        xcb_shape_rectangles(global.conn, XCB_SHAPE_SO_UNION, shape_kind,
                             XCB_CLIP_ORDERING_UNSORTED, con->frame.id,
                             0, 0, rectangles_count, rectangles);
    }
}

/*
 * Reset the container frame shape.
 */
static void x_unshape_frame(Con *con, xcb_shape_sk_t shape_kind) {
    assert(con->window);

    xcb_shape_mask(global.conn, XCB_SHAPE_SO_SET, shape_kind, con->frame.id, 0, 0, XCB_PIXMAP_NONE);
}

/*
 * Shape or unshape container frame based on the con state.
 */
static void set_shape_state(Con *con, bool need_reshape) {
    if (!shape_supported || con->window == nullptr) {
        return;
    }

    struct con_state *state;
    if ((state = state_for_frame(con->frame.id)) == nullptr) {
        ELOG(fmt::sprintf("window state for con %p not found\n",  (void*)con));
        return;
    }

    if (need_reshape && con->con_is_floating()) {
        /* We need to reshape the window frame only if it already has shape. */
        if (con->window->shaped) {
            x_shape_frame(con, XCB_SHAPE_SK_BOUNDING);
        }
        if (con->window->input_shaped) {
            x_shape_frame(con, XCB_SHAPE_SK_INPUT);
        }
    }

    if (state->was_floating && !con->con_is_floating()) {
        /* Remove the shape when container is no longer floating. */
        if (con->window->shaped) {
            x_unshape_frame(con, XCB_SHAPE_SK_BOUNDING);
        }
        if (con->window->input_shaped) {
            x_unshape_frame(con, XCB_SHAPE_SK_INPUT);
        }
    }
}

/*
 * This function pushes the properties of each node of the layout tree to
 * X11 if they have changed (like the map state, position of the window, …).
 * It recursively traverses all children of the given node.
 *
 */
void x_push_node(Con *con) {
    con_state *state;
    Rect rect = con->rect;

    // LOG(fmt::sprintf("Pushing changes for node %p / %s\n",  con, con->name));
    state = state_for_frame(con->frame.id);

    if (state->name.empty()) {
        DLOG(fmt::sprintf("pushing name %s for con %p\n",  state->name, (void*)con));

        xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, con->frame.id,
                            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, state->name.length(), state->name.c_str());
        state->name.clear();
    }

    if (con->window == nullptr) {
        /* Calculate the height of all window decorations which will be drawn on to
         * this frame. */
        uint32_t max_y = 0, max_height = 0;
        for (auto &current : con->nodes_head) {
            Rect *dr = &(current->deco_rect);
            if (dr->y >= max_y && dr->height >= max_height) {
                max_y = dr->y;
                max_height = dr->height;
            }
        }
        rect.height = max_y + max_height;
        if (rect.height == 0)
            con->mapped = false;
    }

    bool need_reshape = false;

    /* reparent the child window (when the window was moved due to a sticky
     * container) */
    if (state->need_reparent && con->window != nullptr) {
        DLOG("Reparenting child window\n");

        /* Temporarily set the event masks to XCB_NONE so that we won’t get
         * UnmapNotify events (otherwise the handler would close the container).
         * These events are generated automatically when reparenting. */
        uint32_t values[] = {XCB_NONE};
        xcb_change_window_attributes(global.conn, state->old_frame, XCB_CW_EVENT_MASK, values);
        xcb_change_window_attributes(global.conn, con->window->id, XCB_CW_EVENT_MASK, values);

        xcb_reparent_window(global.conn, con->window->id, con->frame.id, 0, 0);

        values[0] = FRAME_EVENT_MASK;
        xcb_change_window_attributes(global.conn, state->old_frame, XCB_CW_EVENT_MASK, values);
        values[0] = CHILD_EVENT_MASK;
        xcb_change_window_attributes(global.conn, con->window->id, XCB_CW_EVENT_MASK, values);

        state->old_frame = XCB_NONE;
        state->need_reparent = false;

        con->ignore_unmap++;
        DLOG(fmt::sprintf("ignore_unmap for reparenting of con %p (win 0x%08x) is now %d\n",
             (void*)con, con->window->id, con->ignore_unmap));

        need_reshape = true;
    }

    /* We need to update shape when window frame dimensions is updated. */
    need_reshape |= state->rect.width != rect.width ||
                    state->rect.height != rect.height ||
                    state->window_rect.width != con->window_rect.width ||
                    state->window_rect.height != con->window_rect.height;

    /* We need to set shape when container becomes floating. */
    need_reshape |= con->con_is_floating() && !state->was_floating;

    /* The pixmap of a borderless leaf container will not be used except
     * for the titlebar in a stack or tabs (issue #1013). */
    bool is_pixmap_needed = (con->border_style != BS_NONE ||
                             !con->con_is_leaf() ||
                             con->parent->layout == L_STACKED ||
                             con->parent->layout == L_TABBED);

    /* The root con and output cons will never require a pixmap. In particular for the
     * __i3 output, this will likely not work anyway because it might be ridiculously
     * large, causing an XCB_ALLOC error. */
    if (con->type == CT_ROOT || con->type == CT_OUTPUT)
        is_pixmap_needed = false;

    bool fake_notify = false;
    /* Set new position if rect changed (and if height > 0) or if the pixmap
     * needs to be recreated */
    if ((is_pixmap_needed && con->frame_buffer.id == XCB_NONE) || (state->rect != rect &&
                                                                   rect.height > 0)) {
        /* We first create the new pixmap, then render to it, set it as the
         * background and only afterwards change the window size. This reduces
         * flickering. */

        bool has_rect_changed = (state->rect.x != rect.x || state->rect.y != rect.y ||
                                 state->rect.width != rect.width || state->rect.height != rect.height);

        /* Check if the container has an unneeded pixmap left over from
         * previously having a border or titlebar. */
        if (!is_pixmap_needed && con->frame_buffer.id != XCB_NONE) {
            draw_util_surface_free(global.conn, &(con->frame_buffer));
            xcb_free_pixmap(global.conn, con->frame_buffer.id);
            con->frame_buffer.id = XCB_NONE;
        }

        if (is_pixmap_needed && (has_rect_changed || con->frame_buffer.id == XCB_NONE)) {
            if (con->frame_buffer.id == XCB_NONE) {
                con->frame_buffer.id = xcb_generate_id(global.conn);
            } else {
                draw_util_surface_free(global.conn, &(con->frame_buffer));
                xcb_free_pixmap(global.conn, con->frame_buffer.id);
            }

            uint16_t win_depth = root_depth;
            if (con->window)
                win_depth = con->window->depth;

            /* Ensure we have valid dimensions for our surface. */
            // TODO This is probably a bug in the condition above as we should never enter this path
            //      for height == 0. Also, we should probably handle width == 0 the same way.
            int width = std::max((int32_t)rect.width, 1);
            int height = std::max((int32_t)rect.height, 1);

            xcb_create_pixmap(global.conn, win_depth, con->frame_buffer.id, con->frame.id, width, height);
            draw_util_surface_init(global.conn, &(con->frame_buffer), con->frame_buffer.id,
                                   get_visualtype_by_id(get_visualid_by_depth(win_depth)), width, height);

            /* For the graphics context, we disable GraphicsExposure events.
             * Those will be sent when a CopyArea request cannot be fulfilled
             * properly due to parts of the source being unmapped or otherwise
             * unavailable. Since we always copy from pixmaps to windows, this
             * is not a concern for us. */
            uint32_t value_list[]{0};
            xcb_change_gc(global.conn, con->frame_buffer.gc, XCB_GC_GRAPHICS_EXPOSURES, value_list);

            draw_util_surface_set_size(&(con->frame), width, height);
            con->pixmap_recreated = true;

            /* Don’t render the decoration for windows inside a stack which are
             * not visible right now */
            // TODO Should this work the same way for L_TABBED?
            if (!con->parent ||
                con->parent->layout != L_STACKED ||
                con::first(con->parent->focus_head) == con)
                /* Render the decoration now to make the correct decoration visible
                 * from the very first moment. Later calls will be cached, so this
                 * doesn’t hurt performance. */
                x_deco_recurse(con);
        }

        DLOG(fmt::sprintf("setting rect (%d, %d, %d, %d)\n",  rect.x, rect.y, rect.width, rect.height));
        /* flush to ensure that the following commands are sent in a single
         * buffer and will be processed directly afterwards (the contents of a
         * window get lost when resizing it, therefore we want to provide it as
         * fast as possible) */
        xcb_flush(global.conn);
        xcb_set_window_rect(global.conn, con->frame.id, rect);
        if (con->frame_buffer.id != XCB_NONE) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
        xcb_flush(global.conn);

        memcpy(&(state->rect), &rect, sizeof(Rect));
        fake_notify = true;
    }

    /* dito, but for child windows */
    if (con->window != nullptr &&
        state->window_rect != con->window_rect) {
        DLOG(fmt::sprintf("setting window rect (%d, %d, %d, %d)\n",
             con->window_rect.x, con->window_rect.y, con->window_rect.width, con->window_rect.height));
        xcb_set_window_rect(global.conn, con->window->id, con->window_rect);
        memcpy(&(state->window_rect), &(con->window_rect), sizeof(Rect));
        fake_notify = true;
    }

    set_shape_state(con, need_reshape);

    /* Map if map state changed, also ensure that the child window
     * is changed if we are mapped and there is a new, unmapped child window.
     * Unmaps are handled in x_push_node_unmaps(). */
    if ((state->mapped != con->mapped || (con->window != nullptr && !state->child_mapped)) &&
        con->mapped) {
        xcb_void_cookie_t cookie;

        if (con->window != nullptr) {
            /* Set WM_STATE_NORMAL because GTK applications don’t want to
             * drag & drop if we don’t. Also, xprop(1) needs it. */
            long data[] = {XCB_ICCCM_WM_STATE_NORMAL, XCB_NONE};
            xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, con->window->id,
                                A_WM_STATE, A_WM_STATE, 32, 2, data);
        }

        uint32_t values[1];
        if (!state->child_mapped && con->window != nullptr) {
            cookie = xcb_map_window(global.conn, con->window->id);

            /* We are interested in EnterNotifys as soon as the window is
             * mapped */
            values[0] = CHILD_EVENT_MASK;
            xcb_change_window_attributes(global.conn, con->window->id, XCB_CW_EVENT_MASK, values);
            DLOG(fmt::sprintf("mapping child window (serial %d)\n",  cookie.sequence));
            state->child_mapped = true;
        }

        cookie = xcb_map_window(global.conn, con->frame.id);

        values[0] = FRAME_EVENT_MASK;
        xcb_change_window_attributes(global.conn, con->frame.id, XCB_CW_EVENT_MASK, values);

        /* copy the pixmap contents to the frame window immediately after mapping */
        if (con->frame_buffer.id != XCB_NONE) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
        xcb_flush(global.conn);

        DLOG(fmt::sprintf("mapping container %08x (serial %d)\n",  con->frame.id, cookie.sequence));
        state->mapped = con->mapped;
    }

    state->unmap_now = (state->mapped != con->mapped) && !con->mapped;
    state->was_floating = con->con_is_floating();

    if (fake_notify) {
        DLOG("Sending fake configure notify\n");
        fake_absolute_configure_notify(con);
    }

    set_hidden_state(con);

    /* Handle all children and floating windows of this node. We recurse
     * in focus order to display the focused client in a stack first when
     * switching workspaces (reduces flickering). */
    for (auto &current : con->focus_head) {
        x_push_node(current);
    }
}

/*
 * Same idea as in x_push_node(), but this function only unmaps windows. It is
 * necessary to split this up to handle new fullscreen clients properly: The
 * new window needs to be mapped and focus needs to be set *before* the
 * underlying windows are unmapped. Otherwise, focus will revert to the
 * PointerRoot and will then be set to the new window, generating unnecessary
 * FocusIn/FocusOut events.
 *
 */
static void x_push_node_unmaps(Con *con) {
    con_state *state;

    // LOG(fmt::sprintf("Pushing changes (with unmaps) for node %p / %s\n",  con, con->name));
    state = state_for_frame(con->frame.id);

    /* map/unmap if map state changed, also ensure that the child window
     * is changed if we are mapped *and* in initial state (meaning the
     * container was empty before, but now got a child) */
    if (state->unmap_now) {
        xcb_void_cookie_t cookie;
        if (con->window != nullptr) {
            /* Set WM_STATE_WITHDRAWN, it seems like Java apps need it */
            long data[] = {XCB_ICCCM_WM_STATE_WITHDRAWN, XCB_NONE};
            xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, con->window->id,
                                A_WM_STATE, A_WM_STATE, 32, 2, data);
        }

        cookie = xcb_unmap_window(global.conn, con->frame.id);
        DLOG(fmt::sprintf("unmapping container %p / %s (serial %d)\n",  (void*)con, con->name, cookie.sequence));
        /* we need to increase ignore_unmap for this container (if it
         * contains a window) and for every window "under" this one which
         * contains a window */
        if (con->window != nullptr) {
            con->ignore_unmap++;
            DLOG(fmt::sprintf("ignore_unmap for con %p (frame 0x%08x) now %d\n",  (void*)con, con->frame.id, con->ignore_unmap));
        }
        state->mapped = con->mapped;
    }

    /* handle all children and floating windows of this node */
    for (auto &current : con->nodes_head) {
        x_push_node_unmaps(current);
    }

    for (auto &current : con->floating_windows) {
        x_push_node_unmaps(current);
    }
}

/*
 * Returns true if the given container is currently attached to its parent.
 *
 * TODO: Remove once #1185 has been fixed
 */
static bool is_con_attached(Con *con) {
    if (con->parent == nullptr)
        return false;

    return std::ranges::any_of(con->parent->nodes_head, [&con](Con* current){ return current == con;});
}

/*
 * Pushes all changes (state of each node, see x_push_node() and the window
 * stack) to X11.
 *
 * NOTE: We need to push the stack first so that the windows have the correct
 * stacking order. This is relevant for workspace switching where we map the
 * windows because mapping may generate EnterNotify events. When they are
 * generated in the wrong order, this will cause focus problems when switching
 * workspaces.
 *
 */
void x_push_changes(Con *con) {
    xcb_query_pointer_cookie_t pointercookie;

    /* If we need to warp later, we request the pointer position as soon as possible */
    if (warp_to) {
        pointercookie = xcb_query_pointer(global.conn, root);
    }

    DLOG("-- PUSHING WINDOW STACK --\n");
    //DLOG("Disabling EnterNotify\n");
    /* We need to keep SubstructureRedirect around, otherwise clients can send
     * ConfigureWindow requests and get them applied directly instead of having
     * them become ConfigureRequests that i3 handles. */
    uint32_t values[1] = {XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
    for (auto &state : state_head | std::views::reverse) {
        if (state->mapped)
            xcb_change_window_attributes(global.conn, state->id, XCB_CW_EVENT_MASK, values);
    }
    //DLOG("Done, EnterNotify disabled\n");
    bool order_changed = false;
    bool stacking_changed = false;

    /* count first, necessary to (re)allocate memory for the bottom-to-top
     * stack afterwards */
    int cnt = 0;
    for (auto &it : state_head | std::views::reverse) {
        if (it->con && it->con->con_has_managed_window()) {
            cnt++;
        }
    }

    /* The bottom-to-top window stack of all windows which are managed by i3.
     * Used for x_get_window_stack(). */
    static xcb_window_t *client_list_windows = nullptr;
    static int client_list_count = 0;

    if (cnt != client_list_count) {
        client_list_windows = static_cast<xcb_window_t *>(srealloc(client_list_windows, sizeof(xcb_window_t) * cnt));
        client_list_count = cnt;
    }

    xcb_window_t *walk = client_list_windows;

    /* X11 correctly represents the stack if we push it from bottom to top */
    for (auto it = state_head.rbegin(); it != state_head.rend(); ++it) {
        auto &state = *it;
        if ((state->con) && state->con->con_has_managed_window())
            memcpy(walk++, &(state->con->window->id), sizeof(xcb_window_t));

        // LOG(fmt::sprintf("stack: 0x%08x\n",  state->id));
        auto prev = std::next(it);
        auto old_state_head_it = std::ranges::find(old_state_head, state);
        if (old_state_head_it == old_state_head.begin()) {
            old_state_head_it = old_state_head.end();
        }
        auto old_prev = std::prev(old_state_head_it);
        if (prev != state_head.rend() && old_prev != old_state_head.end() && *prev != *old_prev)
            order_changed = true;
        if ((state->initial || order_changed) && prev != state_head.rend()) {
            stacking_changed = true;
            // LOG(fmt::sprintf("Stacking 0x%08x above 0x%08x\n",  prev->id, state->id));
            uint32_t mask = 0;
            mask |= XCB_CONFIG_WINDOW_SIBLING;
            mask |= XCB_CONFIG_WINDOW_STACK_MODE;
            uint32_t xcb_values[] = {state->id, XCB_STACK_MODE_ABOVE};

            xcb_configure_window(global.conn, (*prev)->id, mask, xcb_values);
        }
        state->initial = false;
    }

    /* If we re-stacked something (or a new window appeared), we need to update
     * the _NET_CLIENT_LIST and _NET_CLIENT_LIST_STACKING hints */
    if (stacking_changed) {
        DLOG(fmt::sprintf("Client list changed (%i clients)\n",  cnt));
        ewmh_update_client_list_stacking(client_list_windows, client_list_count);

        walk = client_list_windows;

        /* reorder by initial mapping */
        for (auto &s : initial_mapping_head) {
            if (s->con && s->con->con_has_managed_window())
                *walk++ = s->con->window->id;
        }

        ewmh_update_client_list(client_list_windows, client_list_count);
    }

    DLOG("PUSHING CHANGES\n");
    x_push_node(con);

    if (warp_to) {
        xcb_query_pointer_reply_t *pointerreply = xcb_query_pointer_reply(global.conn, pointercookie, nullptr);
        if (!pointerreply) {
            ELOG("Could not query pointer position, not warping pointer\n");
        } else {
            int mid_x = warp_to->x + (warp_to->width / 2);
            int mid_y = warp_to->y + (warp_to->height / 2);

            Output *current = get_output_containing(pointerreply->root_x, pointerreply->root_y);
            Output *target = get_output_containing(mid_x, mid_y);
            if (current != target) {
                uint32_t value_list[]{ROOT_EVENT_MASK};
                uint32_t event_mask_value_list[]{XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
                /* Ignore MotionNotify events generated by warping */
                xcb_change_window_attributes(global.conn, root, XCB_CW_EVENT_MASK, event_mask_value_list);
                xcb_warp_pointer(global.conn, XCB_NONE, root, 0, 0, 0, 0, mid_x, mid_y);
                xcb_change_window_attributes(global.conn, root, XCB_CW_EVENT_MASK, value_list);
            }

            free(pointerreply);
        }
        warp_to = nullptr;
    }

    //DLOG("Re-enabling EnterNotify\n");
    values[0] = FRAME_EVENT_MASK;
    for (auto &state : state_head | std::views::reverse) {
        if (state->mapped)
            xcb_change_window_attributes(global.conn, state->id, XCB_CW_EVENT_MASK, values);
    }
    //DLOG("Done, EnterNotify re-enabled\n");

    x_deco_recurse(con);

    xcb_window_t to_focus = focused->frame.id;
    if (focused->window != nullptr)
        to_focus = focused->window->id;

    if (focused_id != to_focus) {
        if (!focused->mapped) {
            DLOG(fmt::sprintf("Not updating focus (to %p / %s), focused window is not mapped.\n",  (void*)focused, focused->name));
            /* Invalidate focused_id to correctly focus new windows with the same ID */
            focused_id = XCB_NONE;
        } else {
            if (focused->window != nullptr &&
                focused->window->needs_take_focus &&
                focused->window->doesnt_accept_focus) {
                DLOG(fmt::sprintf("Updating focus by sending WM_TAKE_FOCUS to window 0x%08x (focused: %p / %s)\n",
                     to_focus, (void*)focused, focused->name));
                send_take_focus(to_focus, global.last_timestamp);

                change_ewmh_focus((focused->con_has_managed_window() ? focused->window->id : XCB_WINDOW_NONE), last_focused);

                if (to_focus != last_focused && is_con_attached(focused))
                    ipc_send_window_event("focus", focused);
            } else {
                DLOG(fmt::sprintf("Updating focus (focused: %p / %s) to X11 window 0x%08x\n",  (void*)focused, focused->name, to_focus));
                /* We remove XCB_EVENT_MASK_FOCUS_CHANGE from the event mask to get
                 * no focus change events for our own focus changes. We only want
                 * these generated by the clients. */
                if (focused->window != nullptr) {
                    values[0] = CHILD_EVENT_MASK & ~(XCB_EVENT_MASK_FOCUS_CHANGE);
                    xcb_change_window_attributes(global.conn, focused->window->id, XCB_CW_EVENT_MASK, values);
                }
                xcb_set_input_focus(global.conn, XCB_INPUT_FOCUS_POINTER_ROOT, to_focus, global.last_timestamp);
                if (focused->window != nullptr) {
                    values[0] = CHILD_EVENT_MASK;
                    xcb_change_window_attributes(global.conn, focused->window->id, XCB_CW_EVENT_MASK, values);
                }

                change_ewmh_focus((focused->con_has_managed_window() ? focused->window->id : XCB_WINDOW_NONE), last_focused);

                if (to_focus != XCB_NONE && to_focus != last_focused && focused->window != nullptr && is_con_attached(focused))
                    ipc_send_window_event("focus", focused);
            }

            focused_id = last_focused = to_focus;
        }
    }

    if (focused_id == XCB_NONE) {
        /* If we still have no window to focus, we focus the EWMH window instead. We use this rather than the
         * root window in order to avoid an X11 fallback mechanism causing a ghosting effect (see #1378). */
        DLOG(fmt::sprintf("Still no window focused, better set focus to the EWMH support window (%d)\n",  ewmh_window));
        xcb_set_input_focus(global.conn, XCB_INPUT_FOCUS_POINTER_ROOT, ewmh_window, global.last_timestamp);
        change_ewmh_focus(XCB_WINDOW_NONE, last_focused);

        focused_id = ewmh_window;
        last_focused = XCB_NONE;
    }

    xcb_flush(global.conn);
    DLOG("ENDING CHANGES\n");

    /* Disable EnterWindow events for windows which will be unmapped in
     * x_push_node_unmaps() now. Unmapping windows happens when switching
     * workspaces. We want to avoid getting EnterNotifies during that phase
     * because they would screw up our focus. One of these cases is having a
     * stack with two windows. If the first window is focused and gets
     * unmapped, the second one appears under the cursor and therefore gets an
     * EnterNotify event. */
    values[0] = FRAME_EVENT_MASK & ~XCB_EVENT_MASK_ENTER_WINDOW;
    for (auto &state : state_head | std::views::reverse) {
        if (!state->unmap_now)
            continue;
        xcb_change_window_attributes(global.conn, state->id, XCB_CW_EVENT_MASK, values);
    }

    /* Push all pending unmaps */
    x_push_node_unmaps(con);

    /* save the current stack as old stack */
    for (auto &state : state_head) {
        std::erase(old_state_head, state);
        old_state_head.push_back(state);
    }
    //CIRCLEQ_FOREACH(state, &old_state_head, old_state) {
    //    DLOG(fmt::sprintf("old stack: 0x%08x\n",  state->id));
    //}

    xcb_flush(global.conn);
}

/*
 * Raises the specified container in the internal stack of X windows. The
 * next call to x_push_changes() will make the change visible in X11.
 *
 */
void x_raise_con(Con *con) {
    con_state *state;
    state = state_for_frame(con->frame.id);
    // LOG(fmt::sprintf("raising in new stack: %p / %s / %s / xid %08x\n",  con, con->name, con->window ? con->window->name_json : "", state->id));

    std::erase(state_head, state);
    state_head.push_front(state);
}

/*
 * Sets the WM_NAME property (so, no UTF8, but used only for debugging anyways)
 * of the given name. Used for properly tagging the windows for easily spotting
 * i3 windows in xwininfo -root -all.
 *
 */
void x_set_name(Con *con, const std::string &name) {
    struct con_state *state;

    if ((state = state_for_frame(con->frame.id)) == nullptr) {
        ELOG("window state not found\n");
        return;
    }

    state->name = name;
}

/*
 * Sets up i3 specific atoms (I3_SOCKET_PATH and I3_CONFIG_PATH)
 *
 */
void x_set_i3_atoms() {
    pid_t pid = getpid();
    xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, root, A_I3_SOCKET_PATH, A_UTF8_STRING, 8,
                        current_socketpath.length(),
                        current_socketpath.c_str());
    xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, root, A_I3_PID, XCB_ATOM_CARDINAL, 32, 1, &pid);
    xcb_change_property(global.conn, XCB_PROP_MODE_REPLACE, root, A_I3_CONFIG_PATH, A_UTF8_STRING, 8,
                        current_configpath.length(), current_configpath.c_str());
}

/*
 * Set warp_to coordinates.  This will trigger on the next call to
 * x_push_changes().
 *
 */
void x_set_warp_to(Rect *rect) {
    if (config.mouse_warping != POINTER_WARPING_NONE)
        warp_to = rect;
}

/*
 * Applies the given mask to the event mask of every i3 window decoration X11
 * window. This is useful to disable EnterNotify while resizing so that focus
 * is untouched.
 *
 */
void x_mask_event_mask(uint32_t mask) {
    uint32_t values[] = {FRAME_EVENT_MASK & mask};

    for (auto &state : std::ranges::reverse_view(state_head)) {
        if (state->mapped)
            xcb_change_window_attributes(global.conn, state->id, XCB_CW_EVENT_MASK, values);
    }
}

/*
 * Enables or disables nonrectangular shape of the container frame.
 */
void x_set_shape(Con *con, xcb_shape_sk_t kind, bool enable) {
    if (state_for_frame(con->frame.id) == nullptr) {
        ELOG(fmt::sprintf("window state for con %p not found\n",  (void*)con));
        return;
    }

    switch (kind) {
        case XCB_SHAPE_SK_BOUNDING:
            con->window->shaped = enable;
            break;
        case XCB_SHAPE_SK_INPUT:
            con->window->input_shaped = enable;
            break;
        default:
            ELOG(fmt::sprintf("Received unknown shape event kind for con %p. This is a bug.\n",
                 (void*)con));
            return;
    }

    if (con->con_is_floating()) {
        if (enable) {
            x_shape_frame(con, kind);
        } else {
            x_unshape_frame(con, kind);
        }

        xcb_flush(global.conn);
    }
}
