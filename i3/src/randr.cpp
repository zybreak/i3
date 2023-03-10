/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * For more information on RandR, please see the X.org RandR specification at
 * https://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 * (take your time to read it completely, it answers all questions).
 *
 */
#include <cassert>
#include <err.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "log.h"
#include "wrapper.h"

#include "util.h"
#include "tree.h"
#include "workspace.h"
#include "i3.h"
#include "x.h"
#include "floating.h"
#include "configuration.h"
#include "randr.h"
#include "con.h"
#include "match.h"
#include "output.h"
#include "ewmh.h"
#include "global.h"

#include <xcb/randr.h>

#include <vector>
#include <algorithm>
#include <fmt/core.h>

/* Pointer to the result of the query for primary output */
xcb_randr_get_output_primary_reply_t *primary;

/* Stores all outputs available in your current session. */
std::deque<Output*> outputs{};

/* This is the output covering the root window */
static Output *root_output;
static bool has_randr_1_5 = false;

/*
 * Get a specific output by its internal X11 id. Used by randr_query_outputs
 * to check if the output is new (only in the first scan) or if we are
 * re-scanning.
 *
 */
static Output *get_output_by_id(xcb_randr_output_t id) {
    for (Output *output : outputs) {
        if (output->id == id) {
            return output;
        }
    }

    return nullptr;
}

/*
 * Returns the output with the given name or NULL.
 * If require_active is true, only active outputs are considered.
 *
 */
Output *get_output_by_name(const std::string &name, const bool require_active) {
    bool get_primary = (strcasecmp("primary", name.c_str()) == 0);
    for (Output *output : outputs) {
        if (require_active && !output->active) {
            continue;
        }
        if (output->primary && get_primary) {
            return output;
        }
        for (auto &output_name : output->names) {
            if (strcasecmp(output_name.c_str(), name.c_str()) == 0) {
                return output;
            }
        }
    }

    return nullptr;
}

/*
 * Returns the first output which is active.
 *
 */
Output *get_first_output() {
    Output *result = nullptr;

    for (Output *output : outputs) {
        if (output->active) {
            if (output->primary) {
                return output;
            }
            if (!result) {
                result = output;
            }
        }
    }

    if (result) {
        return result;
    }

    errx(EXIT_FAILURE, "No usable outputs available.\n");;
}

/*
 * Check whether there are any active outputs (excluding the root output).
 *
 */
static bool any_randr_output_active() {
    return std::any_of(outputs.begin(), outputs.end(), [](auto &output) { return output != root_output && !output->to_be_disabled && output->active; });
}

/*
 * Returns the active (!) output which contains the coordinates x, y or NULL
 * if there is no output which contains these coordinates.
 *
 */
Output *get_output_containing(unsigned int x, unsigned int y) {
    for (Output *output : outputs) {
        if (!output->active)
            continue;
        DLOG(fmt::sprintf("comparing x=%d y=%d with x=%d and y=%d width %d height %d\n",
             x, y, output->rect.x, output->rect.y, output->rect.width, output->rect.height));
        if (x >= output->rect.x && x < (output->rect.x + output->rect.width) &&
            y >= output->rect.y && y < (output->rect.y + output->rect.height))
            return output;
    }

    return nullptr;
}

/*
 * Returns the active output which contains the midpoint of the given rect. If
 * such an output doesn't exist, returns the output which contains most of the
 * rectangle or NULL if there is no output which intersects with it.
 *
 */
Output *get_output_from_rect(Rect rect) {
    unsigned int mid_x = rect.x + rect.width / 2;
    unsigned int mid_y = rect.y + rect.height / 2;
    Output *output = get_output_containing(mid_x, mid_y);

    return output ? output : output_containing_rect(rect);
}

/*
 * Returns the active output which spans exactly the area specified by
 * rect or NULL if there is no output like this.
 *
 */
Output *get_output_with_dimensions(Rect rect) {
    for (Output *output : outputs) {
        if (!output->active)
            continue;
        DLOG(fmt::sprintf("comparing x=%d y=%d %dx%d with x=%d and y=%d %dx%d\n",
             rect.x, rect.y, rect.width, rect.height,
             output->rect.x, output->rect.y, output->rect.width, output->rect.height));
        if (rect.x == output->rect.x && rect.width == output->rect.width &&
            rect.y == output->rect.y && rect.height == output->rect.height)
            return output;
    }

    return nullptr;
}

/*
 * In output_containing_rect, we check if any active output contains part of the container.
 * We do this by checking if the output rect is intersected by the Rect.
 * This is the 2-dimensional counterpart of get_output_containing.
 * Returns the output with the maximum intersecting area.
 *
 */
Output *output_containing_rect(Rect rect) {
    int lx = rect.x, uy = rect.y;
    int rx = rect.x + rect.width, by = rect.y + rect.height;
    long max_area = 0;
    Output *result = nullptr;
    for (Output *output : outputs) {
        if (!output->active)
            continue;
        int lx_o = (int)output->rect.x, uy_o = (int)output->rect.y;
        int rx_o = (int)(output->rect.x + output->rect.width), by_o = (int)(output->rect.y + output->rect.height);
        DLOG(fmt::sprintf("comparing x=%d y=%d with x=%d and y=%d width %d height %d\n",
             rect.x, rect.y, output->rect.x, output->rect.y, output->rect.width, output->rect.height));
        int left = std::max(lx, lx_o);
        int right = std::min(rx, rx_o);
        int bottom = std::min(by, by_o);
        int top = std::max(uy, uy_o);
        if (left < right && bottom > top) {
            long area = (right - left) * (bottom - top);
            if (area > max_area) {
                result = output;
            }
        }
    }
    return result;
}

/*
 * Like get_output_next with close_far == CLOSEST_OUTPUT, but wraps.
 *
 * For example if get_output_next(D_DOWN, x, FARTHEST_OUTPUT) = NULL, then
 * get_output_next_wrap(D_DOWN, x) will return the topmost output.
 *
 * This function always returns a output: if no active outputs can be found,
 * current itself is returned.
 *
 */
Output *get_output_next_wrap(direction_t direction, Output *current) {
    Output *best = get_output_next(direction, current, CLOSEST_OUTPUT);
    /* If no output can be found, wrap */
    if (!best) {
        direction_t opposite;
        if (direction == D_RIGHT)
            opposite = D_LEFT;
        else if (direction == D_LEFT)
            opposite = D_RIGHT;
        else if (direction == D_DOWN)
            opposite = D_UP;
        else
            opposite = D_DOWN;
        best = get_output_next(opposite, current, FARTHEST_OUTPUT);
    }
    if (!best)
        best = current;
    DLOG(fmt::sprintf("current = %s, best = %s\n",  current->output_primary_name(), best->output_primary_name()));
    return best;
}

/*
 * Gets the output which is the next one in the given direction.
 *
 * If close_far == CLOSEST_OUTPUT, then the output next to the current one will
 * selected. If close_far == FARTHEST_OUTPUT, the output which is the last one
 * in the given direction will be selected.
 *
 * NULL will be returned when no active outputs are present in the direction
 * specified (note that “current” counts as such an output).
 *
 */
Output *get_output_next(direction_t direction, Output *current, output_close_far_t close_far) {
    Rect *cur = &(current->rect),
         *other;
    Output *best = nullptr;
    for (Output *output : outputs) {
        if (!output->active)
            continue;

        other = &(output->rect);

        if ((direction == D_RIGHT && other->x > cur->x) ||
            (direction == D_LEFT && other->x < cur->x)) {
            /* Skip the output when it doesn’t overlap the other one’s y
             * coordinate at all. */
            if ((other->y + other->height) <= cur->y ||
                (cur->y + cur->height) <= other->y)
                continue;
        } else if ((direction == D_DOWN && other->y > cur->y) ||
                   (direction == D_UP && other->y < cur->y)) {
            /* Skip the output when it doesn’t overlap the other one’s x
             * coordinate at all. */
            if ((other->x + other->width) <= cur->x ||
                (cur->x + cur->width) <= other->x)
                continue;
        } else
            continue;

        /* No candidate yet? Start with this one. */
        if (!best) {
            best = output;
            continue;
        }

        if (close_far == CLOSEST_OUTPUT) {
            /* Is this output better (closer to the current output) than our
             * current best bet? */
            if ((direction == D_RIGHT && other->x < best->rect.x) ||
                (direction == D_LEFT && other->x > best->rect.x) ||
                (direction == D_DOWN && other->y < best->rect.y) ||
                (direction == D_UP && other->y > best->rect.y)) {
                best = output;
                continue;
            }
        } else {
            /* Is this output better (farther to the current output) than our
             * current best bet? */
            if ((direction == D_RIGHT && other->x > best->rect.x) ||
                (direction == D_LEFT && other->x < best->rect.x) ||
                (direction == D_DOWN && other->y > best->rect.y) ||
                (direction == D_UP && other->y < best->rect.y)) {
                best = output;
                continue;
            }
        }
    }

    DLOG(fmt::sprintf("current = %s, best = %s\n",  current->output_primary_name(), (best ? best->output_primary_name() : "NULL")));
    return best;
}

/*
 * Creates an output covering the root window.
 *
 */
Output *create_root_output(xcb_connection_t *conn) {
    auto *s = new Output();

    s->active = false;
    s->rect.x = 0;
    s->rect.y = 0;
    s->rect.width = global.root_screen->width_in_pixels;
    s->rect.height = global.root_screen->height_in_pixels;

    s->names.emplace_back("xroot-0");

    return s;
}

/*
 * Initializes a CT_OUTPUT Con (searches existing ones from inplace restart
 * before) to use for the given Output.
 *
 */
void output_init_con(Output *output) {
    Con *con = nullptr;
    bool reused = false;

    DLOG(fmt::sprintf("init_con for output %s\n",  output->output_primary_name()));

    /* Search for a Con with that name directly below the root node. There
     * might be one from a restored layout. */
    for (auto &current : croot->nodes_head) {
        if (strcmp(current->name.c_str(), output->output_primary_name().c_str()) != 0)
            continue;

        con = current;
        reused = true;
        DLOG(fmt::sprintf("Using existing con %p / %s\n",  (void*)con, con->name));
        break;
    }

    if (con == nullptr) {
        con = new Con(croot);
        con->name = output->output_primary_name();
        con->type = CT_OUTPUT;
        con->layout = L_OUTPUT;
        croot->con_fix_percent();
    }
    con->rect = output->rect;
    output->con = con;

    x_set_name(con, fmt::format("[i3 con] output {}", con->name));

    if (reused) {
        DLOG("Not adding workspace, this was a reused con\n");
        return;
    }

    DLOG("Changing layout, adding top/bottom dockarea\n");
    Con *topdock = new Con();
    topdock->type = CT_DOCKAREA;
    topdock->layout = L_DOCKAREA;
    /* this container swallows dock clients */
    auto match = std::make_unique<Match>();
    match->dock = M_DOCK_TOP;
    match->insert_where = M_BELOW;
    topdock->swallow.push_back(std::move(match));

    topdock->name.assign("topdock");

    x_set_name(topdock, fmt::format("[i3 con] top dockarea {}", con->name));
    DLOG("attaching\n");
    topdock->con_attach(con, false);

    /* content container */

    DLOG("adding main content container\n");
    Con *content = new Con();
    content->type = CT_CON;
    content->layout = L_SPLITH;
    content->name.assign("content");

    x_set_name(content, fmt::format("[i3 con] content {}", con->name));
    content->con_attach(con, false);

    /* bottom dock container */
    Con *bottomdock = new Con();
    bottomdock->type = CT_DOCKAREA;
    bottomdock->layout = L_DOCKAREA;
    /* this container swallows dock clients */
    auto match2 = std::make_unique<Match>();
    match2->dock = M_DOCK_BOTTOM;
    match2->insert_where = M_BELOW;
    bottomdock->swallow.push_back(std::move(match2));

    bottomdock->name.assign("bottomdock");

    x_set_name(bottomdock, fmt::format("[i3 con] bottom dockarea {}", con->name));
    DLOG("attaching\n");
    bottomdock->con_attach(con, false);

    /* Change focus to the content container */
    std::erase(con->focus_head, content);
    con->focus_head.push_front(content);
}

/*
 * Initializes at least one workspace for this output, trying the following
 * steps until there is at least one workspace:
 *
 * • Move existing workspaces, which are assigned to be on the given output, to
 *   the output.
 * • Create the first assigned workspace for this output.
 * • Create the first unused workspace.
 *
 */
void init_ws_for_output(Output *output) {
    Con *content = output->con->output_get_content();
    Con *previous_focus = focused->con_get_workspace();

    /* Iterate over all workspaces and check if any of them should be assigned
     * to this output.
     * Note: in order to do that we iterate over all_cons and not using another
     * list that would be updated during iteration by the
     * workspace_move_to_output function. */
    for (const auto &workspace : all_cons) {
        if (workspace->type != CT_WORKSPACE || workspace->con_is_internal()) {
            continue;
        }

        Con *workspace_out = get_assigned_output(workspace->name.c_str(), workspace->num);

        if (output->con != workspace_out) {
            continue;
        }

         DLOG(fmt::sprintf("Moving workspace \"%s\" from output \"%s\" to \"%s\" due to assignment\n",
             workspace->name, get_output_for_con(workspace)->output_primary_name(),
             output->output_primary_name()));

        /* Need to copy output's rect since content is not yet rendered. We
         * can't call render_con here because render_output only proceeds
         * if a workspace exists. */
        content->rect = output->con->rect;
        workspace_move_to_output(workspace, output);
    }

    /* Temporarily set the focused container, might not be initialized yet. */
    focused = content;

    /* if a workspace exists, we are done now */
    if (!content->nodes_head.empty()) {
        /* ensure that one of the workspaces is actually visible (in fullscreen
         * mode), if they were invisible before, this might not be the case. */
        auto visible = std::ranges::find_if(content->nodes_head, [](auto &child) { return child->fullscreen_mode == CF_OUTPUT; });
        if (visible == content->nodes_head.end()) {
            visible = content->nodes_head.begin();
            workspace_show(*visible);
        }
        goto restore_focus;
    }

    /* otherwise, we create the first assigned ws for this output */
    for (const auto &assignment : global.ws_assignments) {
        if (!output_triggers_assignment(output, assignment.get())) {
            continue;
        }

         LOG(fmt::sprintf("Initializing first assigned workspace \"%s\" for output \"%s\"\n",
            assignment->name, assignment->output));
        workspace_show_by_name(assignment->name.c_str());
        goto restore_focus;
    }

    /* if there is still no workspace, we create the first free workspace */
    DLOG("Now adding a workspace\n");
    workspace_show(create_workspace_on_output(output, content));

restore_focus:
    if (previous_focus) {
        workspace_show(previous_focus);
    }
}

/*
 * This function needs to be called when changing the mode of an output when
 * it already has some workspaces (or a bar window) assigned.
 *
 * It reconfigures the bar window for the new mode, copies the new rect into
 * each workspace on this output and forces all windows on the affected
 * workspaces to be reconfigured.
 *
 * It is necessary to call render_layout() afterwards.
 *
 */
static void output_change_mode(xcb_connection_t *conn, Output *output) {
    DLOG("Output mode changed, updating rect\n");
    assert(output->con != nullptr);
    output->con->rect = output->rect;

    Con *content;

    /* Point content to the container of the workspaces */
    content = output->con->output_get_content();

    /* Fix the position of all floating windows on this output.
     * The 'rect' of each workspace will be updated in src/render.c. */
    for (auto &workspace : content->nodes_head) {
        for (auto &child : workspace->floating_windows) {
            floating_fix_coordinates(child, &(workspace->rect), &(output->con->rect));
        }
    }

    /* If default_orientation is NO_ORIENTATION, we change the orientation of
     * the workspaces and their children depending on output resolution. This is
     * only done for workspaces with maximum one child. */
    if (config.default_orientation == NO_ORIENTATION) {
        for (auto &workspace : content->nodes_head) {
            /* Workspaces with more than one child are left untouched because
             * we do not want to change an existing layout. */
            if (workspace->con_num_children() > 1)
                continue;

            workspace->layout = (output->rect.height > output->rect.width) ? L_SPLITV : L_SPLITH;
            DLOG(fmt::sprintf("Setting workspace [%d,%s]'s layout to %d.\n",  workspace->num, workspace->name, workspace->layout));
            if (!workspace->nodes_head.empty()) {
                auto child = con::first(workspace->nodes_head);
                if (child->layout == L_SPLITV || child->layout == L_SPLITH)
                    child->layout = workspace->layout;
                DLOG(fmt::sprintf("Setting child [%d,%s]'s layout to %d.\n",  child->num, child->name, child->layout));
            }
        }
    }
}

/*
 * randr_query_outputs_15 uses RandR ≥ 1.5 to update outputs.
 *
 */
static bool randr_query_outputs_15() {
#if XCB_RANDR_MINOR_VERSION < 5
    return false;
#else
    /* RandR 1.5 available at compile-time, i.e. libxcb is new enough */
    if (!has_randr_1_5) {
        return false;
    }
    /* RandR 1.5 available at run-time (supported by the server and not
     * disabled by the user) */
    DLOG("Querying outputs using RandR 1.5\n");
    xcb_generic_error_t *err;
    xcb_randr_get_monitors_reply_t *monitors =
        xcb_randr_get_monitors_reply(
            global.conn, xcb_randr_get_monitors(global.conn, root, true), &err);
    if (err != nullptr) {
        ELOG(fmt::sprintf("Could not get RandR monitors: X11 error code %d\n",  err->error_code));
        free(err);
        /* Fall back to RandR ≤ 1.4 */
        return false;
    }

    /* Mark all outputs as to_be_disabled, since xcb_randr_get_monitors() will
     * only return active outputs. */
    for (Output *output : outputs) {
        if (output != root_output) {
            output->to_be_disabled = true;
        }
    }

    DLOG(fmt::sprintf("%d RandR monitors found (timestamp %d)\n",
         xcb_randr_get_monitors_monitors_length(monitors),
         monitors->timestamp));

    xcb_randr_monitor_info_iterator_t iter;
    for (iter = xcb_randr_get_monitors_monitors_iterator(monitors);
         iter.rem;
         xcb_randr_monitor_info_next(&iter)) {
        const xcb_randr_monitor_info_t *monitor_info = iter.data;
        xcb_get_atom_name_reply_t *atom_reply =
            xcb_get_atom_name_reply(
                global.conn, xcb_get_atom_name(global.conn, monitor_info->name), &err);
        if (err != nullptr) {
            ELOG(fmt::sprintf("Could not get RandR monitor name: X11 error code %d\n",  err->error_code));
            free(err);
            continue;
        }
        char *name;
        sasprintf(&name, "%.*s",
                  xcb_get_atom_name_name_length(atom_reply),
                  xcb_get_atom_name_name(atom_reply));
        free(atom_reply);

        Output *new_output = get_output_by_name(name, false);
        if (new_output == nullptr) {
            new_output = new Output();

            /* Register associated output names in addition to the monitor name */
            xcb_randr_output_t *randr_outputs = xcb_randr_monitor_info_outputs(monitor_info);
            int randr_output_len = xcb_randr_monitor_info_outputs_length(monitor_info);
            for (int i = 0; i < randr_output_len; i++) {
                xcb_randr_output_t randr_output = randr_outputs[i];

                xcb_randr_get_output_info_reply_t *info =
                    xcb_randr_get_output_info_reply(global.conn,
                                                    xcb_randr_get_output_info(global.conn, randr_output, monitors->timestamp),
                                                    nullptr);

                if (info != nullptr && info->crtc != XCB_NONE) {
                    char *oname;
                    sasprintf(&oname, "%.*s",
                              xcb_randr_get_output_info_name_length(info),
                              xcb_randr_get_output_info_name(info));

                    if (strcmp(name, oname) != 0) {
                        new_output->names.insert(new_output->names.begin(), std::string(oname));
                    } else {
                        free(oname);
                    }
                }
                FREE(info);
            }

            /* Insert the monitor name last, so that it's used as the primary name */
            new_output->names.emplace_front(name);

            if (monitor_info->primary) {
                outputs.push_front(new_output);
            } else {
                outputs.push_back(new_output);
            }
        }
        /* We specified get_active == true in xcb_randr_get_monitors(), so we
         * will only receive active outputs. */
        new_output->active = true;
        new_output->to_be_disabled = false;

        new_output->primary = monitor_info->primary;

        new_output->changed =
            update_if_necessary(&(new_output->rect.x), monitor_info->x) |
            update_if_necessary(&(new_output->rect.y), monitor_info->y) |
            update_if_necessary(&(new_output->rect.width), monitor_info->width) |
            update_if_necessary(&(new_output->rect.height), monitor_info->height);

        DLOG(fmt::sprintf("name %s, x %d, y %d, width %d px, height %d px, width %d mm, height %d mm, primary %d, automatic %d\n",
             name,
             monitor_info->x, monitor_info->y, monitor_info->width, monitor_info->height,
             monitor_info->width_in_millimeters, monitor_info->height_in_millimeters,
             monitor_info->primary, monitor_info->automatic));
        free(name);
    }
    free(monitors);
    return true;
#endif
}

/*
 * Gets called by randr_query_outputs_14() for each output. The function adds
 * new outputs to the list of outputs, checks if the mode of existing outputs
 * has been changed or if an existing output has been disabled. It will then
 * change either the "changed" or the "to_be_deleted" flag of the output, if
 * appropriate.
 *
 */
static void handle_output(xcb_connection_t *conn, xcb_randr_output_t id,
                          xcb_randr_get_output_info_reply_t *output,
                          xcb_timestamp_t cts,
                          xcb_randr_get_screen_resources_current_reply_t *res) {
    /* each CRT controller has a position in which we are interested in */
    xcb_randr_get_crtc_info_reply_t *crtc;

    Output *new_output = get_output_by_id(id);
    bool existing = (new_output != nullptr);
    if (!existing) {
        new_output = new Output();
    }
    new_output->id = id;
    new_output->primary = (primary && primary->output == id);
    while (!new_output->names.empty()) {
        auto first = new_output->names.begin();
        new_output->names.erase(first);
        //FREE(*first); TODO: do i need to free?
    }
    char *output_name;
    sasprintf(&output_name, "%.*s",
              xcb_randr_get_output_info_name_length(output),
              xcb_randr_get_output_info_name(output));
    new_output->names.insert(new_output->names.begin(), output_name);

    DLOG(fmt::sprintf("found output with name %s\n",  new_output->output_primary_name()));

    /* Even if no CRTC is used at the moment, we store the output so that
     * we do not need to change the list ever again (we only update the
     * position/size) */
    if (output->crtc == XCB_NONE) {
        if (!existing) {
            if (new_output->primary)
                outputs.insert(outputs.begin(), new_output);
            else
                outputs.push_back(new_output);
        } else if (new_output->active)
            new_output->to_be_disabled = true;
        return;
    }

    xcb_randr_get_crtc_info_cookie_t icookie;
    icookie = xcb_randr_get_crtc_info(global.conn, output->crtc, cts);
    if ((crtc = xcb_randr_get_crtc_info_reply(global.conn, icookie, nullptr)) == nullptr) {
        DLOG(fmt::sprintf("Skipping output %s: could not get CRTC (%p)\n",
             new_output->output_primary_name(), (void*)crtc));
        free(new_output);
        return;
    }

    bool updated = update_if_necessary(&(new_output->rect.x), crtc->x) |
                   update_if_necessary(&(new_output->rect.y), crtc->y) |
                   update_if_necessary(&(new_output->rect.width), crtc->width) |
                   update_if_necessary(&(new_output->rect.height), crtc->height);
    free(crtc);
    new_output->active = (new_output->rect.width != 0 && new_output->rect.height != 0);
    if (!new_output->active) {
        DLOG("width/height 0/0, disabling output\n");
        return;
    }

    DLOG(fmt::sprintf("mode: %dx%d+%d+%d\n",  new_output->rect.width, new_output->rect.height,
         new_output->rect.x, new_output->rect.y));

    /* If we don’t need to change an existing output or if the output
     * does not exist in the first place, the case is simple: we either
     * need to insert the new output or we are done. */
    if (!updated || !existing) {
        if (!existing) {
            if (new_output->primary)
                outputs.insert(outputs.begin(), new_output);
            else
                outputs.push_back(new_output);
        }
        return;
    }

    new_output->changed = true;
}

/*
 * randr_query_outputs_14 uses RandR ≤ 1.4 to update outputs.
 *
 */
static void randr_query_outputs_14() {
    DLOG("Querying outputs using RandR ≤ 1.4\n");

    /* Get screen resources (primary output, crtcs, outputs, modes) */
    xcb_randr_get_screen_resources_current_cookie_t rcookie;
    rcookie = xcb_randr_get_screen_resources_current(global.conn, root);
    xcb_randr_get_output_primary_cookie_t pcookie;
    pcookie = xcb_randr_get_output_primary(global.conn, root);

    if ((primary = xcb_randr_get_output_primary_reply(global.conn, pcookie, nullptr)) == nullptr)
        ELOG("Could not get RandR primary output\n");
    else
        DLOG(fmt::sprintf("primary output is %08x\n",  primary->output));

    xcb_randr_get_screen_resources_current_reply_t *res =
        xcb_randr_get_screen_resources_current_reply(global.conn, rcookie, nullptr);
    if (res == nullptr) {
        ELOG("Could not query screen resources.\n");
        return;
    }

    /* timestamp of the configuration so that we get consistent replies to all
     * requests (if the configuration changes between our different calls) */
    const xcb_timestamp_t cts = res->config_timestamp;

    const int len = xcb_randr_get_screen_resources_current_outputs_length(res);

    /* an output is VGA-1, LVDS-1, etc. (usually physical video outputs) */
    xcb_randr_output_t *randr_outputs = xcb_randr_get_screen_resources_current_outputs(res);

    /* Request information for each output */
    xcb_randr_get_output_info_cookie_t ocookie[len];
    for (int i = 0; i < len; i++)
        ocookie[i] = xcb_randr_get_output_info(global.conn, randr_outputs[i], cts);

    /* Loop through all outputs available for this X11 screen */
    for (int i = 0; i < len; i++) {
        xcb_randr_get_output_info_reply_t *output;

        if ((output = xcb_randr_get_output_info_reply(global.conn, ocookie[i], nullptr)) == nullptr)
            continue;

        handle_output(global.conn, randr_outputs[i], output, cts, res);
        free(output);
    }

    FREE(res);
}

/*
 * Move the content of an outputs container to the first output.
 *
 * TODO: Maybe use an on_destroy callback which is implement differently for
 * different container types (CT_CONTENT vs. CT_DOCKAREA)?
 *
 */
static void move_content(Con *con) {
    Con *first = get_first_output()->con;
    Con *first_content = first->output_get_content();

    /* We need to move the workspaces from the disappearing output to the first output */
    /* 1: Get the con to focus next */
    Con *next = focused;

    /* 2: iterate through workspaces and re-assign them, fixing the coordinates
     * of floating containers as we go */
    Con *current;
    Con *old_content = con->output_get_content();
    while (!old_content->nodes_head.empty()) {
        current = con::first(old_content->nodes_head);
        if (current != next && current->focus_head.empty()) {
            /* the workspace is empty and not focused, get rid of it */
            DLOG(fmt::sprintf("Getting rid of current = %p / %s (empty, unfocused)\n",  (void*)current, current->name));
            tree_close_internal(current, DONT_KILL_WINDOW, false);
            continue;
        }
        DLOG(fmt::sprintf("Detaching current = %p / %s\n",  (void*)current, current->name));
        current->con_detach();
        DLOG(fmt::sprintf("Re-attaching current = %p / %s\n",  (void*)current, current->name));
        current->con_attach(first_content, false);
        DLOG("Fixing the coordinates of floating containers\n");
        for (auto &floating_con : current->floating_windows) {
            floating_fix_coordinates(floating_con, &(con->rect), &(first->rect));
        }
    }

    /* Restore focus after con_detach / con_attach. next can be NULL, see #3523. */
    if (next) {
        DLOG(fmt::sprintf("now focusing next = %p\n",  (void*)next));
        next->con_focus();
        workspace_show(next->con_get_workspace());
    }

    /* 3: move the dock clients to the first output */
    for (auto &child : con->nodes_head) {
        if (child->type != CT_DOCKAREA) {
            continue;
        }
        DLOG(fmt::sprintf("Handling dock con %p\n",  (void*)child));
        Con *dock;
        while (!child->nodes_head.empty()) {
            dock = con::first(child->nodes_head);
            Con *nc;
            Match *match;
            nc = con_for_window(first, dock->window, &match);
            DLOG(fmt::sprintf("Moving dock client %p to nc %p\n",  (void*)dock, (void*)nc));
            dock->con_detach();
            DLOG("Re-attaching\n");
            dock->con_attach(nc, false);
            DLOG("Done\n");
        }
    }

    DLOG(fmt::sprintf("Destroying disappearing con %p\n",  (void*)con));
    tree_close_internal(con, DONT_KILL_WINDOW, true);
}

/*
 * (Re-)queries the outputs via RandR and stores them in the list of outputs.
 *
 * If no outputs are found use the root window.
 *
 */
void randr_query_outputs() {
    if (!randr_query_outputs_15()) {
        randr_query_outputs_14();
    }

    /* If there's no randr output, enable the output covering the root window. */
    if (any_randr_output_active()) {
        DLOG("Active RandR output found. Disabling root output.\n");
        if (root_output && root_output->active) {
            root_output->to_be_disabled = true;
        }
    } else {
        DLOG("No active RandR output found. Enabling root output.\n");
        root_output->active = true;
    }

    /* Check for clones, disable the clones and reduce the mode to the
     * lowest common mode */
    for (auto it = outputs.begin(); it != outputs.end(); ++it) {
        auto output = *it;

        if (!output->active || output->to_be_disabled)
            continue;
        DLOG(fmt::sprintf("output %p / %s, position (%d, %d), checking for clones\n",
             (void*)output, output->output_primary_name(), output->rect.x, output->rect.y));

        for (auto other_it = it; other_it != outputs.end(); ++other_it) {
            auto other = *other_it;
            if (other == output || !other->active || other->to_be_disabled)
                continue;

            if (other->rect.x != output->rect.x ||
                other->rect.y != output->rect.y)
                continue;

            DLOG(fmt::sprintf("output %p has the same position, its mode = %d x %d\n",
                 (void*)other, other->rect.width, other->rect.height));
            uint32_t width = std::min(other->rect.width, output->rect.width);
            uint32_t height = std::min(other->rect.height, output->rect.height);

            if (update_if_necessary(&(output->rect.width), width) |
                update_if_necessary(&(output->rect.height), height))
                output->changed = true;

            update_if_necessary(&(other->rect.width), width);
            update_if_necessary(&(other->rect.height), height);

            DLOG(fmt::sprintf("disabling output %p (%s)\n",  (void*)other, other->output_primary_name()));
            other->to_be_disabled = true;

            DLOG(fmt::sprintf("new output mode %d x %d, other mode %d x %d\n",
                 output->rect.width, output->rect.height,
                 other->rect.width, other->rect.height));
        }
    }

    /* Ensure that all outputs which are active also have a con. This is
     * necessary because in the next step, a clone might get disabled. Example:
     * LVDS1 active, VGA1 gets activated as a clone of LVDS1 (has no con).
     * LVDS1 gets disabled. */
    for (Output *output : outputs) {
        if (output->active && output->con == nullptr) {
            DLOG(fmt::sprintf("Need to initialize a Con for output %s\n",  output->output_primary_name()));
            output_init_con(output);
            output->changed = false;
        }
    }

    /* Ensure that all containers with type CT_OUTPUT have a valid
     * corresponding entry in outputs. This can happen in situations related to
     * those mentioned #3767 e.g. when a CT_OUTPUT is created from an in-place
     * restart's layout but the output is disabled by a randr query happening
     * at the same time. */
    for (auto &con : croot->nodes_head) {
        if (!con->con_is_internal() && get_output_by_name(con->name, true) == nullptr) {
            DLOG(fmt::sprintf("No output %s found, moving its old content to first output\n",  con->name));
            move_content(con);
        }
    }

    /* Handle outputs which have a new mode or are disabled now (either
     * because the user disabled them or because they are clones) */
    for (Output *output : outputs) {
        if (output->to_be_disabled) {
            randr_disable_output(output);
        }

        if (output->changed) {
            output_change_mode(global.conn, output);
            output->changed = false;
        }
    }

    /* Just go through each active output and assign one workspace */
    for (Output *output : outputs) {
        if (!output->active)
            continue;
        Con *content = output->con->output_get_content();
        if (!content->nodes_head.empty())
            continue;
        DLOG(fmt::sprintf("Should add ws for output %s\n",  output->output_primary_name()));
        init_ws_for_output(output);
    }

    /* Focus the primary screen, if possible */
    for (Output *output : outputs) {
        if (!output->primary || !output->con)
            continue;

        DLOG(fmt::sprintf("Focusing primary output %s\n",  output->output_primary_name()));
        Con *content = output->con->output_get_content();
        Con *ws = con::first(content->focus_head);
        workspace_show(ws);
    }

    /* render_layout flushes */
    ewmh_update_desktop_properties();
    tree_render();

    FREE(primary);
}

/*
 * Disables the output and moves its content.
 *
 */
void randr_disable_output(Output *output) {
    assert(output->to_be_disabled);

    output->active = false;
    DLOG(fmt::sprintf("Output %s disabled, re-assigning workspaces/docks\n",  output->output_primary_name()));

    if (output->con != nullptr) {
        /* clear the pointer before move_content calls tree_close_internal in which the memory is freed */
        Con *con = output->con;
        output->con = nullptr;
        move_content(con);
    }

    output->to_be_disabled = false;
    output->changed = false;
}

static void fallback_to_root_output() {
    root_output->active = true;
    output_init_con(root_output);
    init_ws_for_output(root_output);
}

/*
 * We have just established a connection to the X server and need the initial
 * XRandR information to setup workspaces for each screen.
 *
 */
void randr_init(int *event_base) {
    const xcb_query_extension_reply_t *extreply;

    root_output = create_root_output(global.conn);
    outputs.push_back(root_output);

    extreply = xcb_get_extension_data(global.conn, &xcb_randr_id);
    if (!extreply->present) {
        DLOG("RandR is not present, activating root output.\n");
        fallback_to_root_output();
        return;
    }

    xcb_generic_error_t *err;
    xcb_randr_query_version_reply_t *randr_version =
        xcb_randr_query_version_reply(
            global.conn, xcb_randr_query_version(global.conn, XCB_RANDR_MAJOR_VERSION, XCB_RANDR_MINOR_VERSION), &err);
    if (err != nullptr) {
        ELOG(fmt::sprintf("Could not query RandR version: X11 error code %d\n",  err->error_code));
        free(err);
        fallback_to_root_output();
        return;
    }

    has_randr_1_5 = (randr_version->major_version >= 1) &&
                    (randr_version->minor_version >= 5);

    free(randr_version);

    randr_query_outputs();

    if (event_base != nullptr)
        *event_base = extreply->first_event;

    xcb_randr_select_input(global.conn, root,
                           XCB_RANDR_NOTIFY_MASK_SCREEN_CHANGE |
                               XCB_RANDR_NOTIFY_MASK_OUTPUT_CHANGE |
                               XCB_RANDR_NOTIFY_MASK_CRTC_CHANGE |
                               XCB_RANDR_NOTIFY_MASK_OUTPUT_PROPERTY);

    xcb_flush(global.conn);
}
