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
module;
#include <err.h>
#include <cassert>
#include <sys/types.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_aux.h>
#include <xcb/shape.h>

#include <xpp/connection.hpp>
#include <xpp/proto/x.hpp>

#include <unistd.h>

#include <fmt/printf.h>
module i3;

import std;
import utils;
import log;
import rect;

void X::xcursor_load_cursors() {
    if (xcb_cursor_context_new(**global.x, global.x->root_screen, &ctx) < 0) {
        errx(EXIT_FAILURE, "Cannot allocate xcursor context");
    }
    cursors[XCURSOR_CURSOR_POINTER] = xcb_cursor_load_cursor(ctx, "left_ptr");;
    cursors[XCURSOR_CURSOR_RESIZE_HORIZONTAL] = xcb_cursor_load_cursor(ctx, "sb_h_double_arrow");;
    cursors[XCURSOR_CURSOR_RESIZE_VERTICAL] = xcb_cursor_load_cursor(ctx, "sb_v_double_arrow");;
    cursors[XCURSOR_CURSOR_WATCH] = xcb_cursor_load_cursor(ctx, "watch");;
    cursors[XCURSOR_CURSOR_MOVE] = xcb_cursor_load_cursor(ctx, "fleur");;
    cursors[XCURSOR_CURSOR_TOP_LEFT_CORNER] = xcb_cursor_load_cursor(ctx, "top_left_corner");;
    cursors[XCURSOR_CURSOR_TOP_RIGHT_CORNER] = xcb_cursor_load_cursor(ctx, "top_right_corner");;
    cursors[XCURSOR_CURSOR_BOTTOM_LEFT_CORNER] = xcb_cursor_load_cursor(ctx, "bottom_left_corner");;
    cursors[XCURSOR_CURSOR_BOTTOM_RIGHT_CORNER] = xcb_cursor_load_cursor(ctx, "bottom_right_corner");;
}

/*
 * Sets the cursor of the root window to the 'pointer' cursor.
 *
 * This function is called when i3 is initialized, because with some login
 * managers, the root window will not have a cursor otherwise.
 *
 */
void X::xcursor_set_root_cursor(xcursor_cursor_t cursor_id) {
    uint32_t value_list[]{global.x->xcursor_get_cursor(cursor_id)};
    xcb_change_window_attributes(**global.x, global.x->root, XCB_CW_CURSOR,
                                 value_list);
}

xcb_cursor_t X::xcursor_get_cursor(xcursor_cursor_t c) {
    return cursors.at(c);
}

xcb_visualtype_t* X::get_visual_type_for_root() {
    auto vt = xcb_aux_find_visual_by_attrs(root_screen, -1, 32);
    if (vt != nullptr) {
        root_depth = xcb_aux_get_depth_of_visual(root_screen, vt->visual_id);
        colormap = conn->generate_id();

        try {
            xpp::x::create_colormap_checked(*this->conn,
                XCB_COLORMAP_ALLOC_NONE,
                colormap,
                root,
                vt->visual_id);

        } catch (std::exception &e) {
            ELOG(fmt::sprintf("Could not create colormap. Error: %s\n",  e.what()));
            exit(EXIT_FAILURE);
        }
    } else {
        vt = get_visualtype(root_screen);
    }

    return vt;
}

X::X() : conn(new x_connection()) {
    this->conn_screen = conn->default_screen();
    this->root_screen = conn->screen_of_display(this->conn_screen);
    this->root = conn->root();

    root_depth = root_screen->root_depth;
    colormap = root_screen->default_colormap;
    visual_type = get_visual_type_for_root();

    DLOG(fmt::sprintf("root_depth = %d, visual_id = 0x%08x.\n", root_depth, visual_type->visual_id));
    DLOG(fmt::sprintf("root_screen->height_in_pixels = %d, root_screen->height_in_millimeters = %d\n",
        root_screen->height_in_pixels, root_screen->height_in_millimeters));
    DLOG(fmt::sprintf("One logical pixel corresponds to %ld physical pixels on this display.\n",  logical_px(root_screen, 1)));
}

/*
 * Returns the container state for the given frame. This function always
 * returns a container state (otherwise, there is a bug in the code and the
 * container state of a container for which con_init() was not called was
 * requested).
 *
 */
static con_state *state_for_frame(xcb_window_t window) {
    auto it = std::ranges::find_if(global.x->state_head, [window](auto &state) { return state->id == window; });

    if (it != global.x->state_head.end()) {
        return *it;
    }

    /* TODO: better error handling? */
    ELOG(fmt::sprintf("No state found for window 0x%08x\n",  window));
    std::terminate();
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
void X::con_init(Con *con) {
    /* TODO: maybe create the window when rendering first? we could then even
     * get the initial geometry right */

    uint32_t mask = 0;
    uint32_t values[5];

    xcb_visualid_t visual = get_visualid_by_depth(con->depth);
    xcb_colormap_t win_colormap;
    if (con->depth != this->root_depth) {
        /* We need to create a custom colormap. */
        win_colormap = xcb_generate_id(**this);
        xcb_create_colormap(**this, XCB_COLORMAP_ALLOC_NONE, win_colormap, this->root, visual);
        con->colormap = win_colormap;
    } else {
        /* Use the default colormap. */
        win_colormap = this->colormap;
        con->colormap = XCB_NONE;
    }

    /* We explicitly set a background color and border color (even though we
     * don’t even have a border) because the X11 server requires us to when
     * using 32 bit color depths, see
     * https://stackoverflow.com/questions/3645632 */
    mask |= XCB_CW_BACK_PIXEL;
    values[0] = this->root_screen->black_pixel;

    mask |= XCB_CW_BORDER_PIXEL;
    values[1] = this->root_screen->black_pixel;

    /* our own frames should not be managed */
    mask |= XCB_CW_OVERRIDE_REDIRECT;
    values[2] = 1;

    /* see include/xcb.h for the FRAME_EVENT_MASK */
    mask |= XCB_CW_EVENT_MASK;
    values[3] = FRAME_EVENT_MASK & ~XCB_EVENT_MASK_ENTER_WINDOW;

    mask |= XCB_CW_COLORMAP;
    values[4] = win_colormap;

    Rect dims = {(uint32_t)-15, (uint32_t)-15, 10, 10};
    xcb_window_t frame_id = create_window((xcb_connection_t*)*this->conn, dims, con->depth, visual, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCURSOR_CURSOR_POINTER, false, mask, values);
    draw_util_surface_init(*this->conn, &(con->frame), frame_id, get_visualtype_by_id(visual), dims.width, dims.height);
    xcb_change_property(*this->conn,
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

    this->state_head.push_front(state);
    this->old_state_head.push_front(state);
    this->initial_mapping_head.push_back(state);
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
        xcb_free_colormap(**global.x, con->colormap);
    }

    draw_util_surface_free(**global.x, &(con->frame));
    draw_util_surface_free(**global.x, &(con->frame_buffer));
    xcb_free_pixmap(**global.x, con->frame_buffer.id);
    con->frame_buffer.id = XCB_NONE;
    state = state_for_frame(con->frame.id);
    std::erase(global.x->state_head, state);
    std::erase(global.x->old_state_head, state);
    std::erase(global.x->initial_mapping_head, state);
    delete state;

    /* Invalidate focused_id to correctly focus new windows with the same ID */
    if (con->frame.id == global.x->focused_id) {
        global.x->focused_id = XCB_NONE;
    }
    if (con->frame.id == global.x->last_focused) {
        global.x->last_focused = XCB_NONE;
    }
}

/*
 * Kills the window decoration associated with the given container.
 *
 */
void x_con_kill(Con *con) {
    _x_con_kill(con);
    xcb_destroy_window(**global.x, con->frame.id);
}

/*
 * Completely reinitializes the container's frame, without destroying the old window.
 *
 */
void x_con_reframe(Con *con) {
    _x_con_kill(con);
    global.x->con_init(con);
}

/*
 * Returns true if the client supports the given protocol atom (like WM_DELETE_WINDOW)
 *
 */
bool window_supports_protocol(xcb_window_t window, xcb_atom_t atom) {
    xcb_get_property_cookie_t cookie;
    xcb_icccm_get_wm_protocols_reply_t protocols;
    bool result = false;

    cookie = xcb_icccm_get_wm_protocols(**global.x, window, i3::atoms[i3::Atom::WM_PROTOCOLS]);
    if (xcb_icccm_get_wm_protocols_reply(**global.x, cookie, &protocols, nullptr) != 1) {
        return false;
    }

    /* Check if the client’s protocols have the requested atom set */
    for (uint32_t i = 0; i < protocols.atoms_len; i++) {
        if (protocols.atoms[i] == atom) {
            result = true;
        }
    }

    xcb_icccm_get_wm_protocols_reply_wipe(&protocols);

    return result;
}

/*
 * Kills the given X11 window using WM_DELETE_WINDOW (if supported).
 *
 */
void x_window_kill(xcb_connection_t *c, xcb_window_t window, kill_window_t kill_window) {
    /* if this window does not support WM_DELETE_WINDOW, we kill it the hard way */
    if (!window_supports_protocol(window, i3::atoms[i3::Atom::WM_DELETE_WINDOW])) {
        if (kill_window == kill_window_t::KILL_WINDOW) {
            LOG(fmt::sprintf("Killing specific window 0x%08x\n",  window));
            xcb_destroy_window(**global.x, window);
        } else {
            LOG(fmt::sprintf("Killing the X11 client which owns window 0x%08x\n",  window));
            xcb_kill_client(**global.x, window);
        }
        return;
    }

    xcb_client_message_event_t ev{};

    ev.response_type = XCB_CLIENT_MESSAGE;
    ev.window = window;
    ev.type = i3::atoms[i3::Atom::WM_PROTOCOLS];
    ev.format = 32;
    ev.data.data32[0] = i3::atoms[i3::Atom::WM_DELETE_WINDOW];
    ev.data.data32[1] = XCB_CURRENT_TIME;

    LOG("Sending WM_DELETE to the client\n");
    xcb_send_event(c, false, window, XCB_EVENT_MASK_NO_EVENT, (char *)&ev);
    xcb_flush(c);
}

static void x_draw_title_border(Con *con, surface_t *dest_surface) {
    deco_render_params *p = con->deco_render_params.value().get();
    Rect *dr = &(con->deco_rect);

    /* Left */
    draw_util_rectangle(dest_surface, p->color->border,
                        dr->x, dr->y, 1, dr->height);

    /* Right */
    draw_util_rectangle(dest_surface, p->color->border,
                        dr->x + dr->width - 1, dr->y, 1, dr->height);

    /* Top */
    draw_util_rectangle(dest_surface, p->color->border,
                        dr->x, dr->y, dr->width, 1);

    /* Bottom */
    draw_util_rectangle(dest_surface, p->color->border,
                        dr->x, dr->y + dr->height - 1, dr->width, 1);
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
        auto borders_to_hide = static_cast<adjacent_t>(std::to_underlying(con_adjacent_borders(con)) & std::to_underlying(config.hide_edge_borders));
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

static Colortriple* get_color(Con *con, bool leaf) {
    Con *parent = con->parent;
    /* find out which colors to use */
    if (con->urgent) {
        return &config.client.urgent;
    } else if (con == global.focused || con->con_inside_focused()) {
        return &config.client.focused;
    } else if (con == con::first(parent->focused)) {
        if (config.client.got_focused_tab_title && !leaf && con_descend_focused(con) == global.focused) {
            /* Stacked/tabbed parent of focused container */
            return &config.client.focused_tab_title;
        } else {
            return &config.client.focused_inactive;
        }
    } else {
        return &config.client.unfocused;
    }
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
        con->type == CT_FLOATING_CON) {
        return;
    }

    /* Skip containers whose height is 0 (for example empty dockareas) */
    if (con->rect.height == 0) {
        return;
    }

    /* Skip containers whose pixmap has not yet been created (can happen when
     * decoration rendering happens recursively for a window for which
     * x_push_node() was not yet called) */
    if (leaf && con->frame_buffer.id == XCB_NONE) {
        return;
    }

    Rect *r = &(con->rect);
    Rect *w = &(con->window_rect);
    
    /* 1: build deco_params and compare with cache */
    auto p = std::make_unique<deco_render_params>();

    p->color = get_color(con, leaf);
    p->border_style = con_border_style(con);
    p->con_rect = {r->width, r->height};
    p->con_window_rect = {w->width, w->height};
    p->con_deco_rect = con->deco_rect;
    p->background = config.client.background;
    p->con_is_leaf = con->con_is_leaf();
    p->parent_layout = con->parent->layout;

    if (con->deco_render_params &&
        (con->window == nullptr || !con->window->name_x_changed) &&
        !parent->pixmap_recreated &&
        !con->pixmap_recreated &&
        *p == *con->deco_render_params.value().get()) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    for (auto c_itr = std::ranges::find(con->parent->nodes, con); c_itr != con->parent->nodes.end(); ++c_itr) {
        (*c_itr)->deco_render_params.reset();
    }

    con->deco_render_params = std::move(p);

    if (con->window != nullptr && con->window->name_x_changed) {
        con->window->name_x_changed = false;
    }

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
    if (con->deco_render_params.value()->border_style != BS_NONE && con->deco_render_params.value()->con_is_leaf) {
        /* Fill the border. We don’t just fill the whole rectangle because some
         * children are not freely resizable and we want their background color
         * to "shine through". */
        xcb_rectangle_t rectangles[4];
        size_t rectangles_count = x_get_border_rectangles(con, rectangles);
        for (size_t i = 0; i < rectangles_count; i++) {
            draw_util_rectangle(&(con->frame_buffer), con->deco_render_params.value()->color->child_border,
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
        if (con::next(con, con->parent->nodes) == nullptr &&
            con::previous(con, con->parent->nodes) == nullptr &&
            con->parent->type != CT_FLOATING_CON) {
            if (con->deco_render_params.value()->parent_layout == L_SPLITH) {
                draw_util_rectangle(&(con->frame_buffer), con->deco_render_params.value()->color->indicator,
                                    r->width + (br.width + br.x), br.y, -(br.width + br.x), r->height + br.height);
            } else if (con->deco_render_params.value()->parent_layout == L_SPLITV) {
                draw_util_rectangle(&(con->frame_buffer), con->deco_render_params.value()->color->indicator,
                                    br.x, r->height + (br.height + br.y), r->width + br.width, -(br.height + br.y));
            }
        }
    }

    surface_t *dest_surface = &(parent->frame_buffer);
    if (con_draw_decoration_into_frame(con)) {
        DLOG(fmt::sprintf("using con->frame_buffer (for con->name=%s) as dest_surface\n", con->name));
        dest_surface = &(con->frame_buffer);
    } else {
        DLOG(fmt::sprintf("sticking to parent->frame_buffer = %p\n", fmt::ptr(dest_surface)));
    }
    DLOG(fmt::sprintf("dest_surface %p is %d x %d (id=0x%08x)\n", fmt::ptr(dest_surface), dest_surface->width, dest_surface->height, dest_surface->id));

    /* If the parent hasn't been set up yet, skip the decoration rendering
     * for now. */
    if (dest_surface->id == XCB_NONE) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    /* For the first child, we clear the parent pixmap to ensure there's no
     * garbage left on there. This is important to avoid tearing when using
     * transparency. */
    if (con == con::first(con->parent->nodes)) {
        con->parent->deco_render_params.reset();
    }

    /* if this is a borderless/1pixel window, we don’t need to render the
     * decoration. */
    if (con->deco_render_params.value()->border_style != BS_NORMAL) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    /* 4: paint the bar */
    DLOG(fmt::sprintf("con->deco_rect = (x=%d, y=%d, w=%d, h=%d) for con->name=%s\n",
         con->deco_rect.x, con->deco_rect.y, con->deco_rect.width, con->deco_rect.height, con->name));
    draw_util_rectangle(dest_surface, con->deco_render_params.value()->color->background,
                        con->deco_rect.x, con->deco_rect.y, con->deco_rect.width, con->deco_rect.height);

    /* 5: draw title border */
    x_draw_title_border(con, dest_surface);

    /* 6: draw the icon and title */
    int text_offset_y = (con->deco_rect.height - config.font->height) / 2;

    i3Window *win = con->window;

    const int deco_width = static_cast<int>(con->deco_rect.width);
    const int title_padding = logical_px(global.x->root_screen, 2);

    int mark_width = 0;

    std::string title{};
    if (win == nullptr) {
        if (con->title_format.empty()) {
            title = fmt::format("i3: {}", con_get_tree_representation(con));
        } else {
            title = con_parse_title_format(con);
        }
    } else {
        title = con->title_format.empty() ? win->name : con_parse_title_format(con);
    }
    if (title.empty()) {
        draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        return;
    }

    /* icon_padding is applied horizontally only, the icon will always use all
     * available vertical space. */
    int icon_size = std::max(0, static_cast<int>(con->deco_rect.height - logical_px(global.x->root_screen, 2)));
    int icon_padding = logical_px(global.x->root_screen, std::max(1, con->window_icon_padding));
    int total_icon_space = icon_size + 2 * icon_padding;
    const bool has_icon = (con->window_icon_padding > -1) && win && win->icon && (total_icon_space < deco_width);
    if (!has_icon) {
        icon_size = icon_padding = total_icon_space = 0;
    }
    /* Determine x offsets according to title alignment */
    int icon_offset_x;
    int title_offset_x;
    switch (config.title_align) {
        case title_align_t::ALIGN_LEFT:
            /* (pad)[(pad)(icon)(pad)][text    ](pad)[mark + its pad)
             *             ^           ^--- title_offset_x
             *             ^--- icon_offset_x */
            icon_offset_x = icon_padding;
            title_offset_x = title_padding + total_icon_space;
            break;
        case title_align_t::ALIGN_CENTER:
            /* (pad)[  ][(pad)(icon)(pad)][text  ](pad)[mark + its pad)
             *                 ^           ^--- title_offset_x
             *                 ^--- icon_offset_x
             * Text should come right after the icon (+padding). We calculate
             * the offset for the icon (white space in the title) by dividing
             * by two the total available area. That's the decoration width
             * minus the elements that come after icon_offset_x (icon, its
             * padding, text, marks). */
            icon_offset_x = std::max(icon_padding, (deco_width - icon_padding - icon_size - predict_text_width(config.font, **global.x, global.x->root_screen, title) - title_padding - mark_width) / 2);
            title_offset_x = std::max(title_padding, icon_offset_x + icon_padding + icon_size);
            break;
        case title_align_t::ALIGN_RIGHT:
            /* [mark + its pad](pad)[    text][(pad)(icon)(pad)](pad)
             *                           ^           ^--- icon_offset_x
             *                           ^--- title_offset_x */
            title_offset_x = std::max(title_padding + mark_width, deco_width - title_padding - predict_text_width(config.font, **global.x, global.x->root_screen, title) - total_icon_space);
            /* Make sure the icon does not escape title boundaries */
            icon_offset_x = std::min(deco_width - icon_size - icon_padding - title_padding, title_offset_x + predict_text_width(config.font, **global.x, global.x->root_screen, title) + icon_padding);
            break;
        default:
            ELOG(fmt::sprintf("BUG: invalid config.title_align value %d\n", std::to_underlying(config.title_align)));
            return;
    }

    draw_util_text(**global.x, config.font, title, dest_surface,
                   con->deco_render_params.value()->color->text, con->deco_render_params.value()->color->background,
                   con->deco_rect.x + title_offset_x,
                   con->deco_rect.y + text_offset_y,
                   deco_width - mark_width - 2 * title_padding - total_icon_space);
    if (has_icon) {
        draw_util_image(
            win->icon,
            dest_surface,
            con->deco_rect.x + icon_offset_x,
            con->deco_rect.y + logical_px(global.x->root_screen, 1),
            icon_size,
            icon_size);
    }

    x_draw_title_border(con, dest_surface);
    draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
}

/*
 * Recursively calls x_draw_decoration. This cannot be done in x_push_node
 * because x_push_node uses focus order to recurse (see the comment above)
 * while drawing the decoration needs to happen in the actual order.
 *
 */
void x_deco_recurse(Con *con) {
    bool leaf = con->nodes.empty() && (dynamic_cast<WorkspaceCon*>(con) == nullptr || dynamic_cast<WorkspaceCon*>(con)->floating_windows.empty());
    con_state *state = state_for_frame(con->frame.id);

    if (!leaf) {
        for (auto &current : con->nodes) {
            x_deco_recurse(current);
        }

        if (dynamic_cast<WorkspaceCon*>(con)) {
            for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
                x_deco_recurse(current);
            }
        }

        if (state->mapped) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
    }

    if ((con->type != CT_ROOT && con->type != CT_OUTPUT) &&
        (!leaf || con->mapped)) {
        x_draw_decoration(con);
    }
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
    if (should_be_hidden == state->is_hidden) {
            return;
    }

    if (should_be_hidden) {
        DLOG(fmt::sprintf("setting _NET_WM_STATE_HIDDEN for con = %p\n", fmt::ptr(con)));
        xcb_add_property_atom(**global.x, con->window->id, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_HIDDEN]);
    } else {
        DLOG(fmt::sprintf("removing _NET_WM_STATE_HIDDEN for con = %p\n", fmt::ptr(con)));
        xcb_remove_property_atom(**global.x, con->window->id, i3::atoms[i3::Atom::_NET_WM_STATE], i3::atoms[i3::Atom::_NET_WM_STATE_HIDDEN]);
    }

    state->is_hidden = should_be_hidden;
}

/*
 * Set the container frame shape as the union of the window shape and the
 * shape of the frame borders.
 */
static void x_shape_frame(Con *con, xcb_shape_sk_t shape_kind) {
    assert(con->window);

    xcb_shape_combine(**global.x, XCB_SHAPE_SO_SET, shape_kind, shape_kind,
                      con->frame.id,
                      con->window_rect.x + con->border_width,
                      con->window_rect.y + con->border_width,
                      con->window->id);
    xcb_rectangle_t rectangles[4];
    size_t rectangles_count = x_get_border_rectangles(con, rectangles);
    if (rectangles_count) {
        xcb_shape_rectangles(**global.x, XCB_SHAPE_SO_UNION, shape_kind,
                             XCB_CLIP_ORDERING_UNSORTED, con->frame.id,
                             0, 0, rectangles_count, rectangles);
    }
}

/*
 * Reset the container frame shape.
 */
static void x_unshape_frame(Con *con, xcb_shape_sk_t shape_kind) {
    assert(con->window);

    xcb_shape_mask(**global.x, XCB_SHAPE_SO_SET, shape_kind, con->frame.id, 0, 0, XCB_PIXMAP_NONE);
}

/*
 * Shape or unshape container frame based on the con state.
 */
static void set_shape_state(Con *con, bool need_reshape) {
    if (!global.shape->shape_supported || con->window == nullptr) {
        return;
    }

    struct con_state *state;
    if ((state = state_for_frame(con->frame.id)) == nullptr) {
        ELOG(fmt::sprintf("window state for con %p not found\n", fmt::ptr(con)));
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
 * Moves a child window from Container src to Container dest.
 *
 */
void x_move_win(Con *src, Con *dest) {
    struct con_state *state_src, *state_dest;

    if ((state_src = state_for_frame(src->frame.id)) == nullptr) {
        ELOG("window state for src not found\n");
        return;
    }

    if ((state_dest = state_for_frame(dest->frame.id)) == nullptr) {
        ELOG("window state for dest not found\n");
        return;
    }

    state_dest->con = state_src->con;
    state_src->con = nullptr;

    if (state_dest->window_rect == (Rect){0, 0, 0, 0}) {
        memcpy(&(state_dest->window_rect), &(state_src->window_rect), sizeof(Rect));
        DLOG("COPYING RECT\n");
    }
}

/*
 * Re-initializes the associated X window state for this container. You have
 * to call this when you assign a client to an empty container to ensure that
 * its state gets updated correctly.
 *
 */
void x_reinit(Con *con) {
    con_state *state;

    if ((state = state_for_frame(con->frame.id)) == nullptr) {
        ELOG("window state not found\n");
        return;
    }

    DLOG(fmt::sprintf("resetting state %p to initial\n", fmt::ptr(state)));
    state->initial = true;
    state->child_mapped = false;
    state->con = con;
    state->window_rect = Rect{};
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

    state = state_for_frame(con->frame.id);

    if (state->name.empty()) {
        DLOG(fmt::sprintf("pushing name %s for con %p\n",  state->name, fmt::ptr(con)));

        xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, con->frame.id,
                            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, state->name.length(), state->name.c_str());
        state->name.clear();
    }

    if (con->window == nullptr && (con->layout == L_STACKED || con->layout == L_TABBED)) {
        /* Calculate the height of all window decorations which will be drawn on to
         * this frame. */
        uint32_t max_y = 0, max_height = 0;
        for (auto &current : con->nodes) {
            Rect *dr = &(current->deco_rect);
            if (dr->y >= max_y && dr->height >= max_height) {
                max_y = dr->y;
                max_height = dr->height;
            }
        }
        rect.height = max_y + max_height;
        if (rect.height == 0) {
            con->mapped = false;
        }
    } else if (con->window == nullptr) {
        /* not a stacked or tabbed split container */
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
        xcb_change_window_attributes(**global.x, state->old_frame, XCB_CW_EVENT_MASK, values);
        xcb_change_window_attributes(**global.x, con->window->id, XCB_CW_EVENT_MASK, values);

        xcb_reparent_window(**global.x, con->window->id, con->frame.id, 0, 0);

        values[0] = FRAME_EVENT_MASK;
        xcb_change_window_attributes(**global.x, state->old_frame, XCB_CW_EVENT_MASK, values);
        values[0] = CHILD_EVENT_MASK;
        xcb_change_window_attributes(**global.x, con->window->id, XCB_CW_EVENT_MASK, values);

        state->old_frame = XCB_NONE;
        state->need_reparent = false;

        con->ignore_unmap++;
        DLOG(fmt::sprintf("ignore_unmap for reparenting of con %p (win 0x%08x) is now %d\n",
             fmt::ptr(con), con->window->id, con->ignore_unmap));

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
    bool is_pixmap_needed = ((con->con_is_leaf() && con->border_style != BS_NONE) ||
                             con->layout == L_STACKED ||
                             con->layout == L_TABBED);
    DLOG(fmt::sprintf("Con %p (layout %d), is_pixmap_needed = %s, rect.height = %d\n",
         fmt::ptr(con), std::to_underlying(con->layout), is_pixmap_needed ? "yes" : "no", con->rect.height));

    /* The root con and output cons will never require a pixmap. In particular for the
     * __i3 output, this will likely not work anyway because it might be ridiculously
     * large, causing an XCB_ALLOC error. */
    if (con->type == CT_ROOT || con->type == CT_OUTPUT) {
        is_pixmap_needed = false;
    }

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
            draw_util_surface_free(**global.x, &(con->frame_buffer));
            xcb_free_pixmap(**global.x, con->frame_buffer.id);
            con->frame_buffer.id = XCB_NONE;
        }

        if (is_pixmap_needed && (has_rect_changed || con->frame_buffer.id == XCB_NONE)) {
            if (con->frame_buffer.id == XCB_NONE) {
                con->frame_buffer.id = xcb_generate_id(**global.x);
            } else {
                draw_util_surface_free(**global.x, &(con->frame_buffer));
                xcb_free_pixmap(**global.x, con->frame_buffer.id);
            }

            uint16_t win_depth = global.x->root_depth;
            if (con->window) {
                win_depth = con->window->depth;
            }

            /* Ensure we have valid dimensions for our surface. */
            /* TODO: This is probably a bug in the condition above as we should
             * never enter this path for height == 0. Also, we should probably
             * handle width == 0 the same way. */
            int width = std::max(static_cast<int32_t>(rect.width), 1);
            int height = std::max(static_cast<int32_t>(rect.height), 1);

            DLOG(fmt::sprintf("creating %d x %d pixmap for con %p (con->frame_buffer.id = (pixmap_t)0x%08x) (con->frame.id (drawable_t)0x%08x)\n", width, height, fmt::ptr(con), con->frame_buffer.id, con->frame.id));
            xcb_create_pixmap(**global.x, win_depth, con->frame_buffer.id, con->frame.id, width, height);
            draw_util_surface_init(**global.x, &(con->frame_buffer), con->frame_buffer.id,
                                   get_visualtype_by_id(get_visualid_by_depth(win_depth)), width, height);
            draw_util_clear_surface(&(con->frame_buffer), (color_t){.red = 0.0, .green = 0.0, .blue = 0.0});

            /* For the graphics context, we disable GraphicsExposure events.
             * Those will be sent when a CopyArea request cannot be fulfilled
             * properly due to parts of the source being unmapped or otherwise
             * unavailable. Since we always copy from pixmaps to windows, this
             * is not a concern for us. */
            uint32_t value_list[]{0};
            xcb_change_gc(**global.x, con->frame_buffer.gc, XCB_GC_GRAPHICS_EXPOSURES, value_list);

            draw_util_surface_set_size(&(con->frame), width, height);
            con->pixmap_recreated = true;

            /* Don’t render the decoration for windows inside a stack which are
             * not visible right now
             * TODO: Should this work the same way for L_TABBED? */
            if (!con->parent ||
                con->parent->layout != L_STACKED ||
                con::first(con->parent->focused) == con) {
                /* Render the decoration now to make the correct decoration visible
                 * from the very first moment. Later calls will be cached, so this
                 * doesn’t hurt performance. */
                x_deco_recurse(con);
            }
        }

        DLOG(fmt::sprintf("setting rect (%d, %d, %d, %d)\n",  rect.x, rect.y, rect.width, rect.height));
        /* flush to ensure that the following commands are sent in a single
         * buffer and will be processed directly afterwards (the contents of a
         * window get lost when resizing it, therefore we want to provide it as
         * fast as possible) */
        xcb_flush(**global.x);
        xcb_set_window_rect(**global.x, con->frame.id, rect);
        if (con->frame_buffer.id != XCB_NONE) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
        xcb_flush(**global.x);

        memcpy(&(state->rect), &rect, sizeof(Rect));
        fake_notify = true;
    }

    /* dito, but for child windows */
    if (con->window != nullptr &&
        state->window_rect != con->window_rect) {
        DLOG(fmt::sprintf("setting window rect (%d, %d, %d, %d)\n",
             con->window_rect.x, con->window_rect.y, con->window_rect.width, con->window_rect.height));
        xcb_set_window_rect(**global.x, con->window->id, con->window_rect);
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
            xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, con->window->id,
                                i3::atoms[i3::Atom::WM_STATE], i3::atoms[i3::Atom::WM_STATE], 32, 2, data);
        }

        uint32_t values[1];
        if (!state->child_mapped && con->window != nullptr) {
            cookie = xcb_map_window(**global.x, con->window->id);

            /* We are interested in EnterNotifys as soon as the window is
             * mapped */
            values[0] = CHILD_EVENT_MASK;
            xcb_change_window_attributes(**global.x, con->window->id, XCB_CW_EVENT_MASK, values);
            DLOG(fmt::sprintf("mapping child window (serial %d)\n",  cookie.sequence));
            state->child_mapped = true;
        }

        cookie = xcb_map_window(**global.x, con->frame.id);

        values[0] = FRAME_EVENT_MASK;
        xcb_change_window_attributes(**global.x, con->frame.id, XCB_CW_EVENT_MASK, values);

        /* copy the pixmap contents to the frame window immediately after mapping */
        if (con->frame_buffer.id != XCB_NONE) {
            draw_util_copy_surface(&(con->frame_buffer), &(con->frame), 0, 0, 0, 0, con->rect.width, con->rect.height);
        }
        xcb_flush(**global.x);

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
    for (auto &current : con->focused) {
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

    state = state_for_frame(con->frame.id);

    /* map/unmap if map state changed, also ensure that the child window
     * is changed if we are mapped *and* in initial state (meaning the
     * container was empty before, but now got a child) */
    if (state->unmap_now) {
        xcb_void_cookie_t cookie;
        if (con->window != nullptr) {
            /* Set WM_STATE_WITHDRAWN, it seems like Java apps need it */
            long data[] = {XCB_ICCCM_WM_STATE_WITHDRAWN, XCB_NONE};
            xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, con->window->id,
                                i3::atoms[i3::Atom::WM_STATE], i3::atoms[i3::Atom::WM_STATE], 32, 2, data);
        }

        cookie = xcb_unmap_window(**global.x, con->frame.id);
        DLOG(fmt::sprintf("unmapping container %p / %s (serial %d)\n", fmt::ptr(con), con->name, cookie.sequence));
        /* we need to increase ignore_unmap for this container (if it
         * contains a window) and for every window "under" this one which
         * contains a window */
        if (con->window != nullptr) {
            con->ignore_unmap++;
            DLOG(fmt::sprintf("ignore_unmap for con %p (frame 0x%08x) now %d\n", fmt::ptr(con), con->frame.id, con->ignore_unmap));
        }
        state->mapped = con->mapped;
    }

    /* handle all children and floating windows of this node */
    for (auto &current : con->nodes) {
        x_push_node_unmaps(current);
    }

    if (dynamic_cast<WorkspaceCon*>(con)) {
        for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
            x_push_node_unmaps(current);
        }
    }
}

/*
 * Returns true if the given container is currently attached to its parent.
 *
 * TODO: Remove once #1185 has been fixed
 */
static bool is_con_attached(Con *con) {
    if (con->parent == nullptr) {
        return false;
    }

    return std::ranges::any_of(con->parent->nodes, [&con](Con* current){ return current == con;});
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
    if (global.x->warp_to) {
        pointercookie = xcb_query_pointer(**global.x, global.x->root);
    }

    DLOG("-- PUSHING WINDOW STACK --\n");
    /* We need to keep SubstructureRedirect around, otherwise clients can send
     * ConfigureWindow requests and get them applied directly instead of having
     * them become ConfigureRequests that i3 handles. */
    uint32_t values[1] = {XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
    for (auto &state : global.x->state_head | std::views::reverse) {
        if (state->mapped)
            xcb_change_window_attributes(**global.x, state->id, XCB_CW_EVENT_MASK, values);
    }
    bool order_changed = false;
    bool stacking_changed = false;

    /* count first, necessary to (re)allocate memory for the bottom-to-top
     * stack afterwards */
    int cnt = 0;
    for (auto &it : global.x->state_head | std::views::reverse) {
        if (it->con && it->con->con_has_managed_window()) {
            cnt++;
        }
    }

    /* The bottom-to-top window stack of all windows which are managed by i3.
     * Used for x_get_window_stack(). */
    std::vector<xcb_window_t> client_list_windows{};
    client_list_windows.reserve(cnt);

    /* X11 correctly represents the stack if we push it from bottom to top */
    for (auto it = global.x->state_head.rbegin(); it != global.x->state_head.rend(); ++it) {
        auto &state = *it;
        if ((state->con) && state->con->con_has_managed_window()) {
            client_list_windows.push_back(state->con->window->id);
        }

        auto prev = std::next(it);
        auto old_state_head_it = std::ranges::find(global.x->old_state_head, state);
        if (old_state_head_it == global.x->old_state_head.begin()) {
            old_state_head_it = global.x->old_state_head.end();
        }
        auto old_prev = std::prev(old_state_head_it);
        if (prev != global.x->state_head.rend() && old_prev != global.x->old_state_head.end() && *prev != *old_prev) {
            order_changed = true;
        }
        if ((state->initial || order_changed) && prev != global.x->state_head.rend()) {
            stacking_changed = true;
            uint32_t mask = 0;
            mask |= XCB_CONFIG_WINDOW_SIBLING;
            mask |= XCB_CONFIG_WINDOW_STACK_MODE;
            uint32_t xcb_values[] = {state->id, XCB_STACK_MODE_ABOVE};

            xcb_configure_window(**global.x, (*prev)->id, mask, xcb_values);
        }
        state->initial = false;
    }

    /* If we re-stacked something (or a new window appeared), we need to update
     * the _NET_CLIENT_LIST and _NET_CLIENT_LIST_STACKING hints */
    if (stacking_changed) {
        DLOG(fmt::sprintf("Client list changed (%i clients)\n",  cnt));
        ewmh_update_client_list_stacking(client_list_windows.data(), client_list_windows.size());

        auto walk = client_list_windows.begin();

        /* reorder by initial mapping */
        for (auto &s : global.x->initial_mapping_head) {
            if (s->con && s->con->con_has_managed_window()) {
                *walk = s->con->window->id;
                walk++;
            }
        }

        ewmh_update_client_list(client_list_windows.data(), client_list_windows.size());
    }

    DLOG("PUSHING CHANGES\n");
    x_push_node(con);

    if (global.x->warp_to) {
        xcb_query_pointer_reply_t *pointerreply = xcb_query_pointer_reply(**global.x, pointercookie, nullptr);
        if (!pointerreply) {
            ELOG("Could not query pointer position, not warping pointer\n");
        } else {
            int mid_x = global.x->warp_to->x + (global.x->warp_to->width / 2);
            int mid_y = global.x->warp_to->y + (global.x->warp_to->height / 2);

            Output *current = global.randr->get_output_containing(pointerreply->root_x, pointerreply->root_y);
            Output *target = global.randr->get_output_containing(mid_x, mid_y);
            if (current != target) {
                uint32_t value_list[]{ROOT_EVENT_MASK};
                uint32_t event_mask_value_list[]{XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
                /* Ignore MotionNotify events generated by warping */
                xcb_change_window_attributes(**global.x, global.x->root, XCB_CW_EVENT_MASK, event_mask_value_list);
                xcb_warp_pointer(**global.x, XCB_NONE, global.x->root, 0, 0, 0, 0, mid_x, mid_y);
                xcb_change_window_attributes(**global.x, global.x->root, XCB_CW_EVENT_MASK, value_list);
            }

            free(pointerreply);
        }
        global.x->warp_to = std::nullopt;
    }

    values[0] = FRAME_EVENT_MASK;
    for (auto &state : global.x->state_head | std::views::reverse) {
        if (state->mapped)
            xcb_change_window_attributes(**global.x, state->id, XCB_CW_EVENT_MASK, values);
    }

    x_deco_recurse(con);

    xcb_window_t to_focus = global.focused->frame.id;
    if (global.focused->window != nullptr) {
        to_focus = global.focused->window->id;
    }

    if (global.x->focused_id != to_focus) {
        if (!global.focused->mapped) {
            DLOG(fmt::sprintf("Not updating focus (to %p / %s), focused window is not mapped.\n", fmt::ptr(global.focused), global.focused->name));
            /* Invalidate focused_id to correctly focus new windows with the same ID */
            global.x->focused_id = XCB_NONE;
        } else {
            if (global.focused->window != nullptr &&
                global.focused->window->needs_take_focus &&
                global.focused->window->doesnt_accept_focus) {
                DLOG(fmt::sprintf("Updating focus by sending WM_TAKE_FOCUS to window 0x%08x (focused: %p / %s)\n",
                     to_focus, fmt::ptr(global.focused), global.focused->name));
                send_take_focus(to_focus, global.last_timestamp);

                change_ewmh_focus((global.focused->con_has_managed_window() ? global.focused->window->id : XCB_WINDOW_NONE), global.x->last_focused);

                if (to_focus != global.x->last_focused && is_con_attached(global.focused)) {
                    ipc_send_window_event("focus", global.focused);
                }
            } else {
                DLOG(fmt::sprintf("Updating focus (focused: %p / %s) to X11 window 0x%08x\n", fmt::ptr(global.focused), global.focused->name, to_focus));
                /* We remove XCB_EVENT_MASK_FOCUS_CHANGE from the event mask to get
                 * no focus change events for our own focus changes. We only want
                 * these generated by the clients. */
                if (global.focused->window != nullptr) {
                    values[0] = CHILD_EVENT_MASK & ~(XCB_EVENT_MASK_FOCUS_CHANGE);
                    xcb_change_window_attributes(**global.x, global.focused->window->id, XCB_CW_EVENT_MASK, values);
                }
                xcb_set_input_focus(**global.x, XCB_INPUT_FOCUS_POINTER_ROOT, to_focus, global.last_timestamp);
                if (global.focused->window != nullptr) {
                    values[0] = CHILD_EVENT_MASK;
                    xcb_change_window_attributes(**global.x, global.focused->window->id, XCB_CW_EVENT_MASK, values);
                }

                change_ewmh_focus((global.focused->con_has_managed_window() ? global.focused->window->id : XCB_WINDOW_NONE), global.x->last_focused);

                if (to_focus != XCB_NONE && to_focus != global.x->last_focused && global.focused->window != nullptr && is_con_attached(global.focused)) {
                    ipc_send_window_event("focus", global.focused);
                }
            }

            global.x->focused_id = global.x->last_focused = to_focus;
        }
    }

    if (global.x->focused_id == XCB_NONE) {
        /* If we still have no window to focus, we focus the EWMH window instead. We use this rather than the
         * root window in order to avoid an X11 fallback mechanism causing a ghosting effect (see #1378). */
        DLOG(fmt::sprintf("Still no window focused, better set focus to the EWMH support window (%d)\n",  global.x->ewmh_window));
        xcb_set_input_focus(**global.x, XCB_INPUT_FOCUS_POINTER_ROOT, global.x->ewmh_window, global.last_timestamp);
        change_ewmh_focus(XCB_WINDOW_NONE, global.x->last_focused);

        global.x->focused_id = global.x->ewmh_window;
        global.x->last_focused = XCB_NONE;
    }

    xcb_flush(**global.x);
    DLOG("ENDING CHANGES\n");

    /* Disable EnterWindow events for windows which will be unmapped in
     * x_push_node_unmaps() now. Unmapping windows happens when switching
     * workspaces. We want to avoid getting EnterNotifies during that phase
     * because they would screw up our focus. One of these cases is having a
     * stack with two windows. If the first window is focused and gets
     * unmapped, the second one appears under the cursor and therefore gets an
     * EnterNotify event. */
    values[0] = FRAME_EVENT_MASK & ~XCB_EVENT_MASK_ENTER_WINDOW;
    for (auto &state : global.x->state_head | std::views::reverse) {
        if (!state->unmap_now)
            continue;
        xcb_change_window_attributes(**global.x, state->id, XCB_CW_EVENT_MASK, values);
    }

    /* Push all pending unmaps */
    x_push_node_unmaps(con);

    /* save the current stack as old stack */
    for (auto &state : global.x->state_head) {
        std::erase(global.x->old_state_head, state);
        global.x->old_state_head.push_back(state);
    }

    xcb_flush(**global.x);
}

/*
 * Raises the specified container in the internal stack of X windows. The
 * next call to x_push_changes() will make the change visible in X11.
 *
 */
void x_raise_con(Con *con) {
    con_state *state;
    state = state_for_frame(con->frame.id);

    std::erase(global.x->state_head, state);
    global.x->state_head.push_front(state);
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
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::I3_SOCKET_PATH], i3::atoms[i3::Atom::UTF8_STRING], 8,
                        global.current_socketpath.length(),
                        global.current_socketpath.c_str());
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::I3_PID], XCB_ATOM_CARDINAL, 32, 1, &pid);
    xcb_change_property(**global.x, XCB_PROP_MODE_REPLACE, global.x->root, i3::atoms[i3::Atom::I3_CONFIG_PATH], i3::atoms[i3::Atom::UTF8_STRING], 8,
                        current_configpath.length(), current_configpath.c_str());
}

/*
 * Set warp_to coordinates.  This will trigger on the next call to
 * x_push_changes().
 *
 */
void x_set_warp_to(std::optional<Rect> rect) {
    if (config.mouse_warping != POINTER_WARPING_NONE) {
        global.x->warp_to = rect;
    }
}

/*
 * Applies the given mask to the event mask of every i3 window decoration X11
 * window. This is useful to disable EnterNotify while resizing so that focus
 * is untouched.
 *
 */
void x_mask_event_mask(uint32_t mask) {
    uint32_t values[] = {FRAME_EVENT_MASK & mask};

    for (auto &state : std::ranges::reverse_view(global.x->state_head)) {
        if (state->mapped)
            xcb_change_window_attributes(**global.x, state->id, XCB_CW_EVENT_MASK, values);
    }
}

/*
 * Enables or disables nonrectangular shape of the container frame.
 */
void x_set_shape(Con *con, xcb_shape_sk_t kind, bool enable) {
    if (state_for_frame(con->frame.id) == nullptr) {
        ELOG(fmt::sprintf("window state for con %p not found\n", fmt::ptr(con)));
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
                 fmt::ptr(con)));
            return;
    }

    if (con->con_is_floating()) {
        if (enable) {
            x_shape_frame(con, kind);
        } else {
            x_unshape_frame(con, kind);
        }

        xcb_flush(**global.x);
    }
}
