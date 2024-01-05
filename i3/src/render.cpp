/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * render.c: Renders (determines position/sizes) the layout tree, updating the
 *           various rects. Needs to be pushed to X11 (see x.c) to be visible.
 *
 */
module;
#include <cassert>

#include <cstdlib>

#include <xcb/xcb.h>

#include <cmath>
#include <ranges>

#include <fmt/printf.h>
module i3;

/* Forward declarations */
static std::vector<int> precalculate_sizes(Con *con, render_params *p);
static void render_root(Con *con, Con *fullscreen);
static void render_output(Con *con);
static void render_con_split(Con *con, Con *child, render_params *p, int i);
static void render_con_stacked(Con *con, Con *child, render_params *p, int i);
static void render_con_tabbed(Con *con, Con *child, render_params *p, int i);
static void render_con_dockarea(Con *con, Con *child, render_params *p);

/*
 * Returns the height for the decorations
 */
int render_deco_height() {
    int deco_height = config.font->height + 4;
    if (config.font->height & 0x01)
        ++deco_height;
    return deco_height;
}

/*
 * "Renders" the given container (and its children), meaning that all rects are
 * updated correctly. Note that this function does not call any xcb_*
 * functions, so the changes are completely done in memory only (and
 * side-effect free). As soon as you call x_push_changes(), the changes will be
 * updated in X11.
 *
 */
void render_con(Con *con) {
    render_params params = {
        .x = con->rect.x,
        .y = con->rect.y,
        .rect = con->rect,
        .children = con->con_num_children()
    };

    DLOG(fmt::sprintf("Rendering node %p / %s / layout %d / children %d\n",  (void*)con, con->name,
         con->layout, params.children));

    int i = 0;
    con->mapped = true;

    /* if this container contains a window, set the coordinates */
    if (con->window) {
        /* depending on the border style, the rect of the child window
         * needs to be smaller */
        Rect *inset = &(con->window_rect);
        *inset = (Rect){0, 0, con->rect.width, con->rect.height};
        if (con->fullscreen_mode == CF_NONE) {
            *inset += con_border_style_rect(con);
        }

        /* Obey x11 border */
        inset->width -= (2 * con->border_width);
        inset->height -= (2 * con->border_width);

        inset->rect_sanitize_dimensions();

        /* NB: We used to respect resize increment size hints for tiling
         * windows up until commit 0db93d9 here. However, since all terminal
         * emulators cope with ignoring the size hints in a better way than we
         * can (by providing their fake-transparency or background color), this
         * code was removed. See also https://bugs.i3wm.org/540 */

        DLOG(fmt::sprintf("child will be at %dx%d with size %dx%d\n",  inset->x, inset->y, inset->width, inset->height));
    }

    /* Check for fullscreen nodes */
    Con *fullscreen = nullptr;
    if (con->type != CT_OUTPUT) {
        fullscreen = con->con_get_fullscreen_con((con->type == CT_ROOT ? CF_GLOBAL : CF_OUTPUT));
    }
    if (fullscreen) {
        fullscreen->rect = params.rect;
        x_raise_con(fullscreen);
        render_con(fullscreen);
        /* Fullscreen containers are either global (underneath the CT_ROOT
         * container) or per-output (underneath the CT_CONTENT container). For
         * global fullscreen containers, we cannot abort rendering here yet,
         * because the floating windows (with popup_during_fullscreen smart)
         * have not yet been rendered (see the CT_ROOT code path below). See
         * also https://bugs.i3wm.org/1393 */
        if (con->type != CT_ROOT) {
            return;
        }
    }

    /* find the height for the decorations */
    params.deco_height = render_deco_height();

    /* precalculate the sizes to be able to correct rounding errors */
    params.sizes = precalculate_sizes(con, &params);

    if (con->layout == L_OUTPUT) {
        render_output(con);
    } else if (con->type == CT_ROOT) {
        render_root(con, fullscreen);
    } else {
        for (auto &child : con->nodes_head) {
            assert(params.children > 0);

            if (con->layout == L_SPLITH || con->layout == L_SPLITV) {
                render_con_split(con, child, &params, i);
            } else if (con->layout == L_STACKED) {
                render_con_stacked(con, child, &params, i);
            } else if (con->layout == L_TABBED) {
                render_con_tabbed(con, child, &params, i);
            } else if (con->layout == L_DOCKAREA) {
                render_con_dockarea(con, child, &params);
            }

            child->rect.rect_sanitize_dimensions();

            DLOG(fmt::sprintf("child at (%d, %d) with (%d x %d)\n",
                 child->rect.x, child->rect.y, child->rect.width, child->rect.height));
            x_raise_con(child);
            render_con(child);
            i++;
        }

        /* in a stacking or tabbed container, we ensure the focused client is raised */
        if (con->layout == L_STACKED || con->layout == L_TABBED) {
            for (auto &it : con->focus_head | std::views::reverse) {
                x_raise_con(it);
            }
            Con *child = con::first(con->focus_head);
            if (child != nullptr) {
                /* By rendering the stacked container again, we handle the case
                 * that we have a non-leaf-container inside the stack. In that
                 * case, the children of the non-leaf-container need to be
                 * raised as well. */
                render_con(child);
            }

            if (params.children != 1)
                /* Raise the stack con itself. This will put the stack
                 * decoration on top of every stack window. That way, when a
                 * new window is opened in the stack, the old window will not
                 * obscure part of the decoration (it’s unmapped afterwards). */
                x_raise_con(con);
        }
    }
}

static std::vector<int> precalculate_sizes(Con *con, render_params *p) {
    if ((con->layout != L_SPLITH && con->layout != L_SPLITV) || p->children <= 0) {
        return {};
    }

    std::vector<int> sizes(p->children);

    assert(!con->nodes_head.empty());

    int i = 0, assigned = 0;
    int total = con_rect_size_in_orientation(con);
    for (auto &child : con->nodes_head) {
        double percentage = child->percent > 0.0 ? child->percent : 1.0 / p->children;
        assigned += sizes[i++] = lround(percentage * total);
    }
    assert(assigned == total ||
           (assigned > total && assigned - total <= p->children * 2) ||
           (assigned < total && total - assigned <= p->children * 2));
    int signal = assigned < total ? 1 : -1;
    while (assigned != total) {
        for (i = 0; i < p->children && assigned != total; ++i) {
            sizes[i] += signal;
            assigned += signal;
        }
    }

    return sizes;
}

static void render_root(Con *con, Con *fullscreen) {
    if (!fullscreen) {
        for (auto &output : con->nodes_head) {
            render_con(output);
        }
    }

    /* We need to render floating windows after rendering all outputs’
     * tiling windows because they need to be on top of *every* output at
     * all times. This is important when the user places floating
     * windows/containers so that they overlap on another output. */
    DLOG("Rendering floating windows:\n");
    for (auto &output : con->nodes_head) {
        /* Get the active workspace of that output */
        Con *content = output->output_get_content();
        if (!content || content->focus_head.empty()) {
            DLOG("Skipping this output because it is currently being destroyed.\n");
            continue;
        }
        Con *workspace = con::first(content->focus_head);
        if (workspace == nullptr) {
            continue;
        }
        Con *fullscreen = workspace->con_get_fullscreen_covering_ws();
        if (dynamic_cast<WorkspaceCon*>(workspace)) {
            for (auto &child : dynamic_cast<WorkspaceCon*>(workspace)->floating_windows) {
                if (fullscreen != nullptr) {
                    /* Don’t render floating windows when there is a fullscreen
                 * window on that workspace. Necessary to make floating
                 * fullscreen work correctly (ticket #564). Exception to the
                 * above rule: smart popup_during_fullscreen handling (popups
                 * belonging to the fullscreen app will be rendered). */
                    if (config.popup_during_fullscreen != PDF_SMART || fullscreen->window == nullptr) {
                        continue;
                    }

                    Con *floating_child = con_descend_focused(child);
                    Con *transient_con = floating_child;
                    bool is_transient_for = false;
                    while (transient_con != nullptr &&
                           transient_con->window != nullptr &&
                           transient_con->window->transient_for != XCB_NONE) {
                        DLOG(fmt::sprintf("transient_con = 0x%08x, transient_con->window->transient_for = 0x%08x, fullscreen_id = 0x%08x\n",
                                          transient_con->window->id, transient_con->window->transient_for, fullscreen->window->id));
                        if (transient_con->window->transient_for == fullscreen->window->id) {
                            is_transient_for = true;
                            break;
                        }
                        Con *next_transient = con_by_window_id(transient_con->window->transient_for);
                        if (next_transient == nullptr)
                            break;
                        /* Some clients (e.g. x11-ssh-askpass) actually set
                     * WM_TRANSIENT_FOR to their own window id, so break instead of
                     * looping endlessly. */
                        if (transient_con == next_transient)
                            break;
                        transient_con = next_transient;
                    }

                    if (!is_transient_for)
                        continue;
                    else {
                        DLOG(fmt::sprintf("Rendering floating child even though in fullscreen mode: "
                                          "floating->transient_for (0x%08x) --> fullscreen->id (0x%08x)\n",
                                          floating_child->window->transient_for, fullscreen->window->id));
                    }
                }
                DLOG(fmt::sprintf("floating child at (%d,%d) with %d x %d\n",
                                  child->rect.x, child->rect.y, child->rect.width, child->rect.height));
                x_raise_con(child);
                render_con(child);
            }
        }
    }
}

/*
 * Renders a container with layout L_OUTPUT. In this layout, all CT_DOCKAREAs
 * get the height of their content and the remaining CT_CON gets the rest.
 *
 */
static void render_output(Con *con) {
    int x = con->rect.x;
    int y = con->rect.y;
    int height = con->rect.height;

    /* Find the content container and ensure that there is exactly one. Also
     * check for any non-CT_DOCKAREA clients. */
    Con *content = nullptr;
    for (auto &child : con->nodes_head) {
        if (child->type == CT_CON) {
            if (content != nullptr) {
                DLOG("More than one CT_CON on output container\n");
                assert(false);
            }
            content = child;
        } else if (child->type != CT_DOCKAREA) {
            DLOG(fmt::sprintf("Child %p of type %d is inside the OUTPUT con\n",  (void*)child, child->type));
            assert(false);
        }
    }

    if (content == nullptr) {
        DLOG("Skipping this output because it is currently being destroyed.\n");
        return;
    }

    /* We need to find out if there is a fullscreen con on the current workspace
     * and take the short-cut to render it directly (the user does not want to
     * see the dockareas in that case) */
    Con *ws = content->con_get_fullscreen_con(CF_OUTPUT);
    if (!ws) {
        DLOG("Skipping this output because it is currently being destroyed.\n");
        return;
    }
    Con *fullscreen = ws->con_get_fullscreen_con(CF_OUTPUT);
    if (fullscreen) {
        fullscreen->rect = con->rect;
        x_raise_con(fullscreen);
        render_con(fullscreen);
        return;
    }

    /* First pass: determine the height of all CT_DOCKAREAs (the sum of their
     * children) and figure out how many pixels we have left for the rest */
    for (auto &child : con->nodes_head) {
        if (child->type != CT_DOCKAREA)
            continue;

        child->rect.height = 0;
        for (auto &dockchild : child->nodes_head) {
            child->rect.height += dockchild->geometry.height;
        }

        height -= child->rect.height;
    }

    /* Second pass: Set the widths/heights */
    for (auto &child : con->nodes_head) {
        if (child->type == CT_CON) {
            child->rect.x = x;
            child->rect.y = y;
            child->rect.width = con->rect.width;
            child->rect.height = height;
        }

        child->rect.x = x;
        child->rect.y = y;
        child->rect.width = con->rect.width;

        child->deco_rect.x = 0;
        child->deco_rect.y = 0;
        child->deco_rect.width = 0;
        child->deco_rect.height = 0;

        y += child->rect.height;

        DLOG(fmt::sprintf("child at (%d, %d) with (%d x %d)\n",
             child->rect.x, child->rect.y, child->rect.width, child->rect.height));
        x_raise_con(child);
        render_con(child);
    }
}

static void render_con_split(Con *con, Con *child, render_params *p, int i) {
    assert(con->layout == L_SPLITH || con->layout == L_SPLITV);

    if (con->layout == L_SPLITH) {
        child->rect.x = p->x;
        child->rect.y = p->y;
        child->rect.width = p->sizes[i];
        child->rect.height = p->rect.height;
        p->x += child->rect.width;
    } else {
        child->rect.x = p->x;
        child->rect.y = p->y;
        child->rect.width = p->rect.width;
        child->rect.height = p->sizes[i];
        p->y += child->rect.height;
    }

    /* first we have the decoration, if this is a leaf node */
    if (child->con_is_leaf()) {
        if (child->border_style == BS_NORMAL) {
            /* TODO: make a function for relative coords? */
            child->deco_rect.x = child->rect.x - con->rect.x;
            child->deco_rect.y = child->rect.y - con->rect.y;

            child->rect.y += p->deco_height;
            child->rect.height -= p->deco_height;

            child->deco_rect.width = child->rect.width;
            child->deco_rect.height = p->deco_height;
        } else {
            child->deco_rect.x = 0;
            child->deco_rect.y = 0;
            child->deco_rect.width = 0;
            child->deco_rect.height = 0;
        }
    }
}

static void render_con_stacked(Con *con, Con *child, render_params *p, int i) {
    assert(con->layout == L_STACKED);

    child->rect.x = p->x;
    child->rect.y = p->y;
    child->rect.width = p->rect.width;
    child->rect.height = p->rect.height;

    child->deco_rect.x = p->x - con->rect.x;
    child->deco_rect.y = p->y - con->rect.y + (i * p->deco_height);
    child->deco_rect.width = child->rect.width;
    child->deco_rect.height = p->deco_height;

    if (p->children > 1 || (child->border_style != BS_PIXEL && child->border_style != BS_NONE)) {
        child->rect.y += (p->deco_height * p->children);
        child->rect.height -= (p->deco_height * p->children);
    }
}

static void render_con_tabbed(Con *con, Con *child, render_params *p, int i) {
    assert(con->layout == L_TABBED);

    child->rect.x = p->x;
    child->rect.y = p->y;
    child->rect.width = p->rect.width;
    child->rect.height = p->rect.height;

    child->deco_rect.width = floor((double)child->rect.width / p->children);
    child->deco_rect.x = p->x - con->rect.x + i * child->deco_rect.width;
    child->deco_rect.y = p->y - con->rect.y;

    /* Since the tab width may be something like 31,6 px per tab, we
     * let the last tab have all the extra space (0,6 * children). */
    if (i == (p->children - 1)) {
        child->deco_rect.width = child->rect.width - child->deco_rect.x;
    }

    if (p->children > 1 || (child->border_style != BS_PIXEL && child->border_style != BS_NONE)) {
        child->rect.y += p->deco_height;
        child->rect.height -= p->deco_height;
        child->deco_rect.height = p->deco_height;
    } else {
        child->deco_rect.height = (child->border_style == BS_PIXEL ? 1 : 0);
    }
}

static void render_con_dockarea(Con *con, Con *child, render_params *p) {
    assert(con->layout == L_DOCKAREA);

    child->rect.x = p->x;
    child->rect.y = p->y;
    child->rect.width = p->rect.width;
    child->rect.height = child->geometry.height;

    child->deco_rect.x = 0;
    child->deco_rect.y = 0;
    child->deco_rect.width = 0;
    child->deco_rect.height = 0;
    p->y += child->rect.height;
}
