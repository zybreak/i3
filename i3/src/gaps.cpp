/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * gaps.c: gaps logic: whether to display gaps at all, and how big
 *         they should be.
 *
 */
module;
#include <cstring>
#include <fmt/printf.h>
module i3;

import log;

/**
 * Calculates the effective gap sizes for a container.
 */
gaps_t calculate_effective_gaps(Con *con) {
    WorkspaceCon *workspace = con->con_get_workspace();
    if (workspace == nullptr)
        return (gaps_t){0, 0, 0, 0, 0};

    bool one_child = workspace->con_num_visible_children() <= 1 ||
                     (workspace->con_num_children() == 1 &&
                      (con::first(workspace->nodes_head)->layout == L_TABBED ||
                       con::first(workspace->nodes_head)->layout == L_STACKED));

    if (config.smart_gaps == SMART_GAPS_ON && one_child)
        return (gaps_t){0, 0, 0, 0, 0};

    gaps_t gaps = {
        .inner = (workspace->gaps.inner + config.gaps.inner),
        .top = 0,
        .right = 0,
        .bottom = 0,
        .left = 0};

    if (config.smart_gaps != SMART_GAPS_INVERSE_OUTER || one_child) {
        gaps.top = workspace->gaps.top + config.gaps.top;
        gaps.right = workspace->gaps.right + config.gaps.right;
        gaps.bottom = workspace->gaps.bottom + config.gaps.bottom;
        gaps.left = workspace->gaps.left + config.gaps.left;
    }

    return gaps;
}

/*
 * Decides whether the container should be inset.
 */
bool gaps_should_inset_con(Con *con, int children) {
    /* No parent? None of the conditionals below can be true. */
    if (con->parent == nullptr) {
        return false;
    }

    /* Floating split containers should never have gaps inside them. */
    if (con->con_inside_floating()) {
        return false;
    }

    const bool leaf_or_stacked_tabbed =
        con->con_is_leaf() ||
        (con->layout == L_STACKED || con->layout == L_TABBED);

    /* Inset direct children of the workspace that are leaf containers or
       stacked/tabbed containers. */
    if (leaf_or_stacked_tabbed &&
        con->parent->type == CT_WORKSPACE) {
        return true;
    }

    /* Inset direct children of vertical or horizontal split containers at any
       depth in the tree. Do not inset as soon as any parent is a stacked or
       tabbed container, to avoid double insets. */
    if (leaf_or_stacked_tabbed &&
        !con_inside_stacked_or_tabbed(con) &&
        con->parent->type == CT_CON &&
        (con->parent->layout == L_SPLITH ||
         con->parent->layout == L_SPLITV)) {
        return true;
    }

    return false;
}

/*
 * Returns whether the given container has an adjacent container in the
 * specified direction. In other words, this returns true if and only if
 * the container is not touching the edge of the screen in that direction.
 */
bool gaps_has_adjacent_container(Con *con, direction_t direction) {
    Con *workspace = con->con_get_workspace();
    Con *fullscreen = workspace->con_get_fullscreen_con(CF_GLOBAL);
    if (fullscreen == nullptr)
        fullscreen = workspace->con_get_fullscreen_con(CF_OUTPUT);

    /* If this container is fullscreen by itself, there's no adjacent container. */
    if (con == fullscreen)
        return false;

    Con *first = con;
    Con *second = nullptr;
    bool found_neighbor = resize_find_tiling_participants(&first, &second, direction, false);
    if (!found_neighbor)
        return false;

    /* If we have an adjacent container and nothing is fullscreen, we consider it. */
    if (fullscreen == nullptr)
        return true;

    /* For fullscreen containers, only consider the adjacent container if it is also fullscreen. */
    return con->con_has_parent(fullscreen) && second->con_has_parent(fullscreen);
}

/*
 * Returns the configured gaps for this workspace based on the workspace name,
 * number, and configured workspace gap assignments.
 */
gaps_t gaps_for_workspace(WorkspaceCon *ws) {
    gaps_t gaps = (gaps_t){0, 0, 0, 0, 0};
    gaps_mask_t mask = gaps_mask_t::GAPS_INNER;
    for (auto &assignment : global.ws_assignments) {
        if (strcmp(assignment->name.c_str(), ws->name.c_str()) == 0) {
            gaps = assignment->gaps;
            mask = assignment->gaps_mask;
            break;
        } else if (ws->num != -1 && name_is_digits(assignment->name.c_str()) && ws_name_to_number(assignment->name.c_str()) == ws->num) {
            gaps = assignment->gaps;
            mask = assignment->gaps_mask;
        }
    }
    if (mask == 0) {
        return gaps;
    }

    if (mask & GAPS_INNER) {
        gaps.inner -= config.gaps.inner;
    }
    if (mask & GAPS_TOP) {
        gaps.top -= config.gaps.top;
    }
    if (mask & GAPS_RIGHT) {
        gaps.right -= config.gaps.right;
    }
    if (mask & GAPS_BOTTOM) {
        gaps.bottom -= config.gaps.bottom;
    }
    if (mask & GAPS_LEFT) {
        gaps.left -= config.gaps.left;
    }

    return gaps;
}

/*
 * Re-applies all workspace gap assignments to existing workspaces after
 * reloading the configuration file.
 *
 */
void gaps_reapply_workspace_assignments(void) {
    for (auto &output : global.croot->nodes_head) {
        Con *content = output->output_get_content();
        for (auto &workspace : content->nodes_head) {
            WorkspaceCon *ws = dynamic_cast<WorkspaceCon *>(workspace);
            if (ws == nullptr)
                continue;
            DLOG(fmt::sprintf("updating gap assignments for workspace %s\n", ws->name));
            ws->gaps = gaps_for_workspace(ws);
        }
    }
}
