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
#include <xcb/xcb.h>

#include <fmt/printf.h>
module i3;

import std;
import :con;
import log;
import rect;

/**
 * This is used to keep a state to pass around when rendering a con in render_con().
 *
 */
struct render_params {
    /* A copy of the coordinates of the container which is being rendered. */
    unsigned int x{};
    unsigned int y{};

    /* The computed height for decorations. */
    int deco_height{};
    /* Container rect, subtract container border. This is the actually usable space
         * inside this container for clients. */
    Rect rect{};
    /* The number of children of the container which is being rendered. */
    unsigned long children{};
    /* A precalculated list of sizes of each child. */
    std::vector<int> sizes{};
};

/* Forward declarations */
static std::vector<int> precalculate_sizes(Con *con, render_params *p);
static void render_root(RootCon *con, Con *fullscreen);
static void render_output(Con *con);
static void render_con_split(Con *con, Con *child, render_params *p, int i);
static void render_con_stacked(Con *con, Con *child, render_params *p, int i);
static void render_con_tabbed(Con *con, Con *child, render_params *p, int i);
static void render_con_dockarea(Con *con, Con *child, render_params *p);

/*
 * Returns the height for the decorations
 */
int render_deco_height() {
    int deco_height = global.configManager->config->font->height + 4;
    if (global.configManager->config->font->height & 0x01) {
        ++deco_height;
    }
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
        .children = con->con_num_children()};

    DLOG(fmt::sprintf("Rendering node %p / %s / layout %d / children %d\n", fmt::ptr(con), con->name,
                     std::to_underlying(con->layout), params.children));

    if (con->type == CT_WORKSPACE) {
        gaps_t gaps = calculate_effective_gaps(con);
        Rect inset = (Rect){
            static_cast<uint32_t>(gaps.left),
            static_cast<uint32_t>(gaps.top),
            static_cast<uint32_t>(-gaps.left + gaps.right),
            static_cast<uint32_t>(-gaps.top + gaps.bottom),
        };
        con->rect += inset;
        params.rect += inset;
        params.x += gaps.left;
        params.y += gaps.top;
    }

    if (gaps_should_inset_con(con, params.children)) {
        gaps_t gaps = calculate_effective_gaps(con);
        Rect inset = (Rect){
            static_cast<uint32_t>(gaps_has_adjacent_container(con, D_LEFT) ? gaps.inner / 2 : gaps.inner),
            static_cast<uint32_t>(gaps_has_adjacent_container(con, D_UP) ? gaps.inner / 2 : gaps.inner),
            static_cast<uint32_t>(gaps_has_adjacent_container(con, D_RIGHT) ? -(gaps.inner / 2) : -gaps.inner),
            static_cast<uint32_t>(gaps_has_adjacent_container(con, D_DOWN) ? -(gaps.inner / 2) : -gaps.inner),
        };
        inset.width -= inset.x;
        inset.height -= inset.y;

        if (con->fullscreen_mode == CF_NONE) {
            params.rect += inset;
            con->rect += inset;
        }
        inset.height = 0;

        params.x = con->rect.x;
        params.y = con->rect.y;
    }

    int i = 0;
    con->mapped = true;

    /* if this container contains a window, set the coordinates */
    if (con->get_window()) {
        /* depending on the border style, the rect of the child window
         * needs to be smaller */
        Rect inset = (Rect){
            .x = 0,
            .y = 0,
            .width = con->rect.width,
            .height = con->rect.height,
        };
        if (con->fullscreen_mode == CF_NONE) {
            DLOG(fmt::sprintf("deco_rect.height = %d\n", con->deco_rect.height));
            Rect bsr = con_border_style_rect(con);
            DLOG(fmt::sprintf("bsr at %dx%d with size %dx%d\n",
                 bsr.x, bsr.y, bsr.width, bsr.height));

            inset += bsr;
        }

        /* Obey x11 border */
        inset.width -= (2 * con->border_width);
        inset.height -= (2 * con->border_width);

        inset.rect_sanitize_dimensions();
        con->get_window_rect() = inset;

        /* NB: We used to respect resize increment size hints for tiling
         * windows up until commit 0db93d9 here. However, since all terminal
         * emulators cope with ignoring the size hints in a better way than we
         * can (by providing their fake-transparency or background color), this
         * code was removed. See also https://bugs.i3wm.org/540 */

        DLOG(fmt::sprintf("child will be at %dx%d with size %dx%d\n",
             inset.x, inset.y, inset.width, inset.height));
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
        render_root(dynamic_cast<RootCon*>(con), fullscreen);
    } else {
        for (auto &child : con->nodes) {
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

            /* render_con_split() sets the deco_rect width based on the rect
             * width, but the render_con() call updates the rect width by
             * applying gaps, so we need to update deco_rect. */
            if (con->layout == L_SPLITH || con->layout == L_SPLITV) {
                if (child->con_is_leaf()) {
                    if (child->border_style == BS_NORMAL) {
                        child->deco_rect.width = child->rect.width;
                    }
                }
            }

            i++;
        }

        /* in a stacking or tabbed container, we ensure the focused client is raised */
        if (con->layout == L_STACKED || con->layout == L_TABBED) {
            // TODO: swap is ambigous when using the views::reverse
            for (auto &it : con->focused | std::views::reverse) {
                x_raise_con(it);
            }
            //for (auto it = con->focused.rbegin(); it != con->focused.rend(); ++it) {
            //    x_raise_con(*it);
            //}
            Con *child = con::first(con->focused);
            if (child != nullptr) {
                /* By rendering the stacked container again, we handle the case
                 * that we have a non-leaf-container inside the stack. In that
                 * case, the children of the non-leaf-container need to be
                 * raised as well. */
                render_con(child);
            }

            if (params.children != 1) {
                /* Raise the stack con itself. This will put the stack
                 * decoration on top of every stack window. That way, when a
                 * new window is opened in the stack, the old window will not
                 * obscure part of the decoration (it’s unmapped afterwards). */
                x_raise_con(con);
            }
        }
    }
}

static std::vector<int> precalculate_sizes(Con *con, render_params *p) {
    if ((con->layout != L_SPLITH && con->layout != L_SPLITV) || p->children <= 0) {
        return {};
    }

    std::vector<int> sizes(p->children);

    assert(!con->nodes.empty());

    int i = 0, assigned = 0;
    int total = con_rect_size_in_orientation(con);
    for (auto &child : con->nodes) {
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

static void render_root(RootCon *con, Con *fullscreen) {
    if (!fullscreen) {
        for (auto &output : con->nodes) {
            render_con(output);
        }
    }

    /* We need to render floating windows after rendering all outputs’
     * tiling windows because they need to be on top of *every* output at
     * all times. This is important when the user places floating
     * windows/containers so that they overlap on another output. */
    DLOG("Rendering floating windows:\n");
    for (auto &output : con->nodes) {
        /* Get the active workspace of that output */
        Con *content = dynamic_cast<OutputCon*>(output)->output_get_content();
        if (!content || content->focused.empty()) {
            DLOG("Skipping this output because it is currently being destroyed.\n");
            continue;
        }
        Con *workspace = con::first(content->focused);
        if (workspace == nullptr) {
            continue;
        }
        Con *fullscreen = workspace->con_get_fullscreen_covering_ws();
        if (dynamic_cast<WorkspaceCon *>(workspace)) {
            for (auto &child : dynamic_cast<WorkspaceCon *>(workspace)->floating_windows) {
                if (fullscreen != nullptr) {
                    /* Don’t render floating windows when there is a fullscreen
                     * window on that workspace. Necessary to make floating
                     * fullscreen work correctly (ticket #564). Exception to the
                     * above rule: smart popup_during_fullscreen handling (popups
                     * belonging to the fullscreen app will be rendered). */
                    if (global.configManager->config->popup_during_fullscreen != PDF_SMART || fullscreen->get_window() == nullptr) {
                        continue;
                    }

                    Con *floating_child = con_descend_focused(child);
                    if (con_find_transient_for_window(floating_child, fullscreen->get_window()->id)) {
                        DLOG(fmt::sprintf("Rendering floating child even though in fullscreen mode: "
                             "floating->transient_for (0x%08x) --> fullscreen->id (0x%08x)\n",
                             floating_child->get_window()->transient_for, fullscreen->get_window()->id));
                    } else {
                        continue;
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
    for (auto &child : con->nodes) {
        if (child->type == CT_CON) {
            if (content != nullptr) {
                DLOG("More than one CT_CON on output container\n");
                std::terminate();
            }
            content = child;
        } else if (child->type != CT_DOCKAREA) {
            DLOG(fmt::sprintf("Child %p of type %d is inside the OUTPUT con\n", fmt::ptr(child), std::to_underlying(child->type)));
            std::terminate();
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
    for (auto &child : con->nodes) {
        if (child->type != CT_DOCKAREA) {
            continue;
        }

        child->rect.height = 0;
        for (auto &dockchild : child->nodes) {
            child->rect.height += dockchild->get_geometry().height;
        }

        height -= child->rect.height;
    }

    /* Second pass: Set the widths/heights */
    for (auto &child : con->nodes) {
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
            child->deco_rect.x = 0;
            child->deco_rect.y = 0;

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

    child->deco_rect.width = floor(static_cast<double>(child->rect.width) / p->children);
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
    child->rect.height = child->get_geometry().height;

    child->deco_rect.x = 0;
    child->deco_rect.y = 0;
    child->deco_rect.width = 0;
    child->deco_rect.height = 0;
    p->y += child->rect.height;
}
