/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * commands.c: all command functions (see commands_parser.c)
 *
 */
#include <cassert>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>

#include "i3_ipc/i3-ipc.h"

#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include "commands_applier.h"
#include "command_result_ir.h"

#include "base_parser.h"

#include <fmt/printf.h>

import utils;
import i3;

static void ysuccess(nlohmann::json *json_gen, bool success) {
    if (json_gen != nullptr) {
        json_gen->push_back({
                { "success", success }
        });
    }
}

static void yerror(nlohmann::json *json_gen, std::string message) {
    if (json_gen != nullptr) {
        json_gen->push_back({
                { "success", false },
                { "error", message }
        });
    }
}


/*
 * Checks whether we switched to a new workspace and returns false in that case,
 * signaling that further workspace switching should be done by the calling function
 * If not, calls workspace_back_and_forth() if workspace_auto_back_and_forth is set
 * and return true, signaling that no further workspace switching should occur in the calling function.
 *
 */
static bool maybe_back_and_forth(struct CommandResultIR &cmd_output, const char *name) {
    Con *ws = global.focused->con_get_workspace();

    /* If we switched to a different workspace, do nothing */
    if (strcmp(ws->name.c_str(), name) != 0)
        return false;

    DLOG("This workspace is already focused.\n");
    if (config.workspace_auto_back_and_forth) {
        workspace_back_and_forth();
        cmd_output.needs_tree_render = true;
    }
    return true;
}

/*
 * Return the passed workspace unless it is the current one and auto back and
 * forth is enabled, in which case the back_and_forth workspace is returned.
 */
static Con *maybe_auto_back_and_forth_workspace(Con *workspace) {
    Con *current, *baf;

    if (!config.workspace_auto_back_and_forth)
        return workspace;

    current = global.focused->con_get_workspace();

    if (current == workspace) {
        baf = workspace_back_and_forth_get();
        if (baf != nullptr) {
            DLOG("Substituting workspace with back_and_forth, as it is focused.\n");
            return baf;
        }
    }

    return workspace;
}

/*******************************************************************************
 * Criteria functions.
 ******************************************************************************/

/** When the command did not include match criteria (!), we use the currently
 * focused container. Do not confuse this case with a command which included
 * criteria but which did not match any windows. This macro has to be called in
 * every command.
 */
static void HANDLE_EMPTY_MATCH(struct criteria_state *criteria_state) {
    if (criteria_state->current_match.error != nullptr) {
        throw std::runtime_error(fmt::sprintf("Invalid match: %s", criteria_state->current_match.error));
    }

    if (criteria_state->current_match.match_is_empty()) {
        criteria_state->owindows.clear();
        criteria_state->owindows.push_back(global.focused);
    }
}


/*
 * This function toggles the layout of a given container. toggle_mode can be
 * either 'default' (toggle only between stacked/tabbed/last_split_layout),
 * 'split' (toggle only between splitv/splith) or 'all' (toggle between all
 * layouts).
 *
 */
static void con_toggle_layout(Con *con, const char *toggle_mode) {
    Con *parent = con;
    /* Users can focus workspaces, but not any higher in the hierarchy.
     * Focus on the workspace is a special case, since in every other case, the
     * user means "change the layout of the parent split container". */
    if (con->type != CT_WORKSPACE)
        parent = con->parent;
    DLOG(fmt::sprintf("con_toggle_layout(%p, %s), parent = %p\n",  (void*)con, toggle_mode, (void*)parent));

    const char delim[] = " ";

    if (strcasecmp(toggle_mode, "split") == 0 || strstr(toggle_mode, delim)) {
        /* L_DEFAULT is used as a placeholder value to distinguish if
         * the first layout has already been saved. (it can never be L_DEFAULT) */
        layout_t new_layout = L_DEFAULT;
        bool current_layout_found = false;
        char *tm_dup = sstrdup(toggle_mode);
        char *cur_tok = strtok(tm_dup, delim);

        for (layout_t layout; cur_tok != nullptr; cur_tok = strtok(nullptr, delim)) {
            if (strcasecmp(cur_tok, "split") == 0) {
                /* Toggle between splits. When the current layout is not a split
                 * layout, we just switch back to last_split_layout. Otherwise, we
                 * change to the opposite split layout. */
                if (parent->layout != L_SPLITH && parent->layout != L_SPLITV) {
                    layout = parent->last_split_layout;
                    /* In case last_split_layout was not initialized… */
                    if (layout == L_DEFAULT) {
                        layout = L_SPLITH;
                    }
                } else {
                    layout = (parent->layout == L_SPLITH) ? L_SPLITV : L_SPLITH;
                }
            } else {
                bool success = layout_from_name(cur_tok, &layout);
                if (!success || layout == L_DEFAULT) {
                    ELOG(fmt::sprintf("The token '%s' was not recognized and has been skipped.\n",  cur_tok));
                    continue;
                }
            }

            /* If none of the specified layouts match the current,
             * fall back to the first layout in the list */
            if (new_layout == L_DEFAULT) {
                new_layout = layout;
            }

            /* We found the active layout in the last iteration, so
             * now let's activate the current layout (next in list) */
            if (current_layout_found) {
                new_layout = layout;
                break;
            }

            if (parent->layout == layout) {
                current_layout_found = true;
            }
        }
        free(tm_dup);

        if (new_layout != L_DEFAULT) {
            con_set_layout(con, new_layout);
        }
    } else if (strcasecmp(toggle_mode, "all") == 0 || strcasecmp(toggle_mode, "default") == 0) {
        if (parent->layout == L_STACKED)
            con_set_layout(con, L_TABBED);
        else if (parent->layout == L_TABBED) {
            if (strcasecmp(toggle_mode, "all") == 0)
                con_set_layout(con, L_SPLITH);
            else
                con_set_layout(con, parent->last_split_layout);
        } else if (parent->layout == L_SPLITH || parent->layout == L_SPLITV) {
            if (strcasecmp(toggle_mode, "all") == 0) {
                /* When toggling through all modes, we toggle between
                 * splith/splitv, whereas normally we just directly jump to
                 * stacked. */
                if (parent->layout == L_SPLITH)
                    con_set_layout(con, L_SPLITV);
                else
                    con_set_layout(con, L_STACKED);
            } else {
                con_set_layout(con, L_STACKED);
            }
        }
    }
}


criteria_state* CommandsApplier::create_criteria_state() {
    return new criteria_state();
};

/*
 * Initializes the specified 'Match' data structure and the initial state of
 * commands.c for matching target windows of a command.
 *
 */
void CommandsApplier::criteria_init(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    //DLOG(fmt::sprintf("Initializing criteria, current_match = %p\n",  (void*)criteria_state->current_match));
    criteria_state->current_match = Match();
    criteria_state->owindows.clear();
}

/*
 * A match specification just finished (the closing square bracket was found),
 * so we filter the list of owindows.
 *
 */
void CommandsApplier::criteria_match_windows(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    DLOG("match specification finished, matching...\n");
    /* copy the old list head to iterate through it and start with a fresh
     * list which will contain only matching windows */
    criteria_state->owindows.clear();
    for (auto &current : global.all_cons) {
        /* make a copy of the next pointer and advance the pointer to the
         * next element as we are going to invalidate the element’s
         * next/prev pointers by calling TAILQ_INSERT_TAIL later */
        DLOG(fmt::sprintf("checking if con %p / %s matches\n",  (void*)current, current->name));

        /* We use this flag to prevent matching on window-less containers if
         * only window-specific criteria were specified. */
        bool accept_match = false;

        if (criteria_state->current_match.con_id != nullptr) {
            accept_match = true;

            if (criteria_state->current_match.con_id == current) {
                DLOG("con_id matched.\n");
            } else {
                DLOG("con_id does not match.\n");
                continue;
            }
        }

        if (current->window != nullptr) {
            if (criteria_state->current_match.match_matches_window(current->window)) {
                DLOG("matches window!\n");
                accept_match = true;
            } else {
                DLOG("doesn't match\n");
                continue;
            }
        }

        if (accept_match) {
            criteria_state->owindows.push_back(current);
        } else {
            continue;
        }
    }

    for (auto c: criteria_state->owindows) {
        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)c, c->name));
    }
}

/*
 * Interprets a ctype=cvalue pair and adds it to the current match
 * specification.
 *
 */
void CommandsApplier::criteria_add(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *ctype, const char *cvalue) {
    criteria_state->current_match.parse_property(ctype, cvalue);
}

static void move_matches_to_workspace(struct criteria_state *criteria_state, Con *ws) {
    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
        con_move_to_workspace(current, ws, true, false, false);
    }
}

static void CHECK_MOVE_CON_TO_WORKSPACE(struct criteria_state *criteria_state) {
    HANDLE_EMPTY_MATCH(criteria_state);

    if (criteria_state->owindows.empty()) {
        throw std::runtime_error("Nothing to move: specified criteria don't match any window");
    } else {
        bool found = false;
        for (auto it = criteria_state->owindows.begin(); it != criteria_state->owindows.end();) {
            Con *current = *it;

            if (current->type == CT_WORKSPACE && !current->con_has_children()) {
                it = criteria_state->owindows.erase(it);
            } else {
                ++it;
                found = true;
            }
        }
        if (!found) {
            throw std::runtime_error("Nothing to move: workspace empty");
        }
    }
}

/*
 * Implementation of 'move [window|container] [to] workspace
 * next|prev|next_on_output|prev_on_output|current'.
 *
 */
void CommandsApplier::move_con_to_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) {
    DLOG(fmt::sprintf("which=%s\n",  which));

    CHECK_MOVE_CON_TO_WORKSPACE(criteria_state);

    /* get the workspace */
    Con *ws;
    if (strcmp(which, "next") == 0)
        ws = workspace_next();
    else if (strcmp(which, "prev") == 0)
        ws = workspace_prev();
    else if (strcmp(which, "next_on_output") == 0)
        ws = workspace_next_on_output();
    else if (strcmp(which, "prev_on_output") == 0)
        ws = workspace_prev_on_output();
    else if (strcmp(which, "current") == 0)
        ws = global.focused->con_get_workspace();
    else {
        throw std::runtime_error(fmt::sprintf("BUG: called with which=%s", which));
    }

    move_matches_to_workspace(criteria_state, ws);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [window|container] [to] workspace back_and_forth'.
 *
 */
void CommandsApplier::move_con_to_workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    Con *ws = workspace_back_and_forth_get();
    if (ws == nullptr) {
        throw std::runtime_error("No workspace was previously active.");
    }

    HANDLE_EMPTY_MATCH(criteria_state);

    move_matches_to_workspace(criteria_state, ws);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
 *
 */
void CommandsApplier::move_con_to_workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *no_auto_back_and_forth) {
    if (strncasecmp(name, "__", strlen("__")) == 0) {
        throw std::runtime_error(fmt::sprintf("You cannot move containers to i3-internal workspaces (\"%s\").", name));
    }

    CHECK_MOVE_CON_TO_WORKSPACE(criteria_state);

    LOG(fmt::sprintf("should move window to workspace %s\n",  name));
    /* get the workspace */
    Con *ws = workspace_get(name);

    if (no_auto_back_and_forth == nullptr) {
        ws = maybe_auto_back_and_forth_workspace(ws);
    }

    move_matches_to_workspace(criteria_state, ws);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
 *
 */
void CommandsApplier::move_con_to_workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *no_auto_back_and_forth) {
    CHECK_MOVE_CON_TO_WORKSPACE(criteria_state);

    LOG(fmt::sprintf("should move window to workspace %s\n",  which));

    int parsed_num = ws_name_to_number(which);
    if (parsed_num == -1) {
        LOG(fmt::sprintf("Could not parse initial part of \"%s\" as a number.\n", which));
        throw std::runtime_error(fmt::sprintf("Could not parse number \"%s\"", which));
    }

    Con *ws = get_existing_workspace_by_num(parsed_num);
    if (!ws) {
        ws = workspace_get(which);
    }

    if (no_auto_back_and_forth == nullptr) {
        ws = maybe_auto_back_and_forth_workspace(ws);
    }

    move_matches_to_workspace(criteria_state, ws);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Convert a string direction ("left", "right", etc.) to a direction_t. Assumes
 * valid direction string.
 */
static direction_t parse_direction(const char *str) {
    if (strcmp(str, "left") == 0) {
        return D_LEFT;
    } else if (strcmp(str, "right") == 0) {
        return D_RIGHT;
    } else if (strcmp(str, "up") == 0) {
        return D_UP;
    } else if (strcmp(str, "down") == 0) {
        return D_DOWN;
    } else {
        ELOG("Invalid direction. This is a parser bug.\n");
        assert(false);
    }
}

static void resize_floating(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str, Con *floating_con, int px) {
    Rect old_rect = floating_con->rect;
    Con *focused_con = con_descend_focused(floating_con);

    direction_t direction;
    if (strcmp(direction_str, "height") == 0) {
        direction = D_DOWN;
    } else if (strcmp(direction_str, "width") == 0) {
        direction = D_RIGHT;
    } else {
        direction = parse_direction(direction_str);
    }
    orientation_t orientation = orientation_from_direction(direction);

    /* ensure that resize will take place even if pixel increment is smaller than
     * height increment or width increment.
     * fixes #1011 */
    const i3Window *window = focused_con->window;
    if (window != nullptr) {
        if (orientation == VERT) {
            if (px < 0) {
                px = (-px < window->height_increment) ? -window->height_increment : px;
            } else {
                px = (px < window->height_increment) ? window->height_increment : px;
            }
        } else {
            if (px < 0) {
                px = (-px < window->width_increment) ? -window->width_increment : px;
            } else {
                px = (px < window->width_increment) ? window->width_increment : px;
            }
        }
    }

    if (orientation == VERT) {
        floating_con->rect.height += px;
    } else {
        floating_con->rect.width += px;
    }
    floating_check_size(floating_con, orientation == VERT);

    /* Did we actually resize anything or did the size constraints prevent us?
     * If we could not resize, exit now to not move the window. */
    if (old_rect == floating_con->rect) {
        return;
    }

    if (direction == D_UP) {
        floating_con->rect.y -= (floating_con->rect.height - old_rect.height);
    } else if (direction == D_LEFT) {
        floating_con->rect.x -= (floating_con->rect.width - old_rect.width);
    }

    /* If this is a scratchpad window, don't auto center it from now on. */
    if (floating_con->scratchpad_state == SCRATCHPAD_FRESH) {
        floating_con->scratchpad_state = SCRATCHPAD_CHANGED;
    }
}

static bool cmd_resize_tiling_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, Con *current, const char *direction, int px, int ppt) {
    Con *second = nullptr;
    Con *first = current;
    direction_t search_direction = parse_direction(direction);

    bool res = resize_find_tiling_participants(&first, &second, search_direction, false);
    if (!res) {
        yerror(cmd_output.json_gen, "No second container found in this direction.");
        return false;
    }

    if (ppt) {
        /* For backwards compatibility, 'X px or Y ppt' means that ppt is
         * preferred. */
        px = 0;
    }
    return resize_neighboring_cons(first, second, px, ppt);
}

static bool cmd_resize_tiling_width_height(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, Con *current, const char *direction, int px, double ppt) {
    LOG("width/height resize\n");

    /* get the appropriate current container (skip stacked/tabbed cons) */
    Con *dummy = nullptr;
    direction_t search_direction = (strcmp(direction, "width") == 0 ? D_LEFT : D_DOWN);
    bool search_result = resize_find_tiling_participants(&current, &dummy, search_direction, true);
    if (!search_result) {
        yerror(cmd_output.json_gen, "Failed to find appropriate tiling containers for resize operation");
        return false;
    }

    /* get the default percentage */
    int children = current->parent->con_num_children();
    LOG(fmt::sprintf("ins. %d children\n",  children));
    double percentage = 1.0 / children;
    LOG(fmt::sprintf("default percentage = %f\n",  percentage));

    /* Ensure all the other children have a percentage set. */
    for (auto &child : current->parent->nodes_head) {
        LOG(fmt::sprintf("child->percent = %f (child %p)\n",  child->percent, (void*)child));
        if (child->percent == 0.0)
            child->percent = percentage;
    }

    double new_current_percent;
    double subtract_percent;
    if (ppt != 0.0) {
        new_current_percent = current->percent + ppt;
    } else {
        /* Convert px change to change in percentages */
        ppt = (double) px / (double) con_rect_size_in_orientation(current->parent);
        new_current_percent = current->percent + ppt;
    }
    subtract_percent = ppt / (children - 1);
    if (ppt < 0.0 && new_current_percent < percent_for_1px(current)) {
        yerror(cmd_output.json_gen, "Not resizing, container would end with less than 1px");
        return false;
    }

    LOG(fmt::sprintf("new_current_percent = %f\n",  new_current_percent));
    LOG(fmt::sprintf("subtract_percent = %f\n",  subtract_percent));
    /* Ensure that the new percentages are positive. */
    if (subtract_percent >= 0.0) {
        for (auto &child : current->parent->nodes_head) {
            if (child == current) {
                continue;
            }
            if (child->percent - subtract_percent < percent_for_1px(child)) {
                yerror(cmd_output.json_gen, fmt::sprintf("Not resizing, already at minimum size (child %p would end up with a size of %.f", (void*)child,
                       child->percent - subtract_percent));
                return false;
            }
        }
    }

    current->percent = new_current_percent;
    LOG(fmt::sprintf("current->percent after = %f\n",  current->percent));

    for (auto &child : current->parent->nodes_head) {
        if (child == current)
            continue;
        child->percent -= subtract_percent;
        LOG(fmt::sprintf("child->percent after (%p) = %f\n",  (void*)child, child->percent));
    }

    return true;
}

/*
 * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
 *
 */
void CommandsApplier::resize(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *way, const char *direction, long resize_px, long resize_ppt) {
    DLOG(fmt::sprintf("resizing in way %s, direction %s, px %ld or ppt %ld\n",  way, direction, resize_px, resize_ppt));
    if (strcmp(way, "shrink") == 0) {
        resize_px *= -1;
        resize_ppt *= -1;
    }

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        /* Don't handle dock windows (issue #1201) */
        if (current->window && current->window->dock) {
            DLOG(fmt::sprintf("This is a dock window. Not resizing (con = %p)\n)",  (void*)current));
            continue;
        }

        Con *floating_con;
        if ((floating_con = current->con_inside_floating())) {
            resize_floating(criteria_state, cmd_output, direction, floating_con, resize_px);
        } else {
            if (strcmp(direction, "width") == 0 ||
                strcmp(direction, "height") == 0) {
                const double ppt = (double) resize_ppt / 100.0;
                if (!cmd_resize_tiling_width_height(criteria_state, cmd_output,
                                                    current, direction,
                                                    resize_px, ppt)) {
                    throw std::runtime_error("Cannot resize.");
                }
            } else {
                if (!cmd_resize_tiling_direction(criteria_state, cmd_output,
                                                 current, direction,
                                                 resize_px, resize_ppt)) {
                    throw std::runtime_error("Cannot resize.");
                }
            }
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

static bool
resize_set_tiling(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, Con *target, orientation_t resize_orientation, bool is_ppt, long target_size) {
    direction_t search_direction;
    char *mode;
    if (resize_orientation == HORIZ) {
        search_direction = D_LEFT;
        mode = (char *) "width";
    } else {
        search_direction = D_DOWN;
        mode = (char *) "height";
    }

    /* Get the appropriate current container (skip stacked/tabbed cons) */
    Con *dummy;
    resize_find_tiling_participants(&target, &dummy, search_direction, true);

    /* Calculate new size for the target container */
    double ppt = 0.0;
    int px = 0;
    if (is_ppt) {
        ppt = (double) target_size / 100.0 - target->percent;
    } else {
        px = target_size - (resize_orientation == HORIZ ? target->rect.width : target->rect.height);
    }

    /* Perform resizing and report failure if not possible */
    return cmd_resize_tiling_width_height(criteria_state, cmd_output,
                                          target, mode, px, ppt);
}

/*
 * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
 *
 */
void CommandsApplier::resize_set(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long cwidth, const char *mode_width, long cheight, const char *mode_height) {
    DLOG(fmt::sprintf("resizing to %ld %s x %ld %s\n",  cwidth, mode_width, cheight, mode_height));
    if (cwidth < 0 || cheight < 0) {
        throw std::runtime_error("Dimensions cannot be negative.");
    }

    HANDLE_EMPTY_MATCH(criteria_state);

    bool success = true;
    for (auto current: criteria_state->owindows) {
        Con *floating_con;
        if ((floating_con = current->con_inside_floating())) {
            Con *output = floating_con->con_get_output();
            if (cwidth == 0) {
                cwidth = floating_con->rect.width;
            } else if (mode_width && strcmp(mode_width, "ppt") == 0) {
                cwidth = output->rect.width * ((double) cwidth / 100.0);
            }
            if (cheight == 0) {
                cheight = floating_con->rect.height;
            } else if (mode_height && strcmp(mode_height, "ppt") == 0) {
                cheight = output->rect.height * ((double) cheight / 100.0);
            }
            floating_resize(floating_con, cwidth, cheight);
        } else {
            if (current->window && current->window->dock) {
                DLOG(fmt::sprintf("This is a dock window. Not resizing (con = %p)\n)",  (void*)current));
                continue;
            }

            if (cwidth > 0) {
                bool is_ppt = mode_width && strcmp(mode_width, "ppt") == 0;
                success &= resize_set_tiling(criteria_state, cmd_output, current,
                                             HORIZ, is_ppt, cwidth);
            }
            if (cheight > 0) {
                bool is_ppt = mode_height && strcmp(mode_height, "ppt") == 0;
                success &= resize_set_tiling(criteria_state, cmd_output, current,
                                             VERT, is_ppt, cheight);
            }
        }
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen,  success);
}

static int border_width_from_style(border_style_t border_style, long border_width, Con *con) {
    if (border_style == BS_NONE) {
        return 0;
    }
    if (border_width >= 0) {
        return logical_px(global.x->root_screen, border_width);
    }

    const bool is_floating = con->con_inside_floating() != nullptr;
    /* Load the configured defaults. */
    if (is_floating && border_style == config.default_floating_border) {
        return config.default_floating_border_width;
    } else if (!is_floating && border_style == config.default_border) {
        return config.default_border_width;
    } else {
        /* Use some hardcoded values. */
        return logical_px(global.x->root_screen, border_style == BS_NORMAL ? 2 : 1);
    }
}

/*
 * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
 *
 */
void CommandsApplier::border(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *border_style_str, long border_width) {
    DLOG(fmt::sprintf("border style should be changed to %s with border width %ld\n",  border_style_str, border_width));

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));

        border_style_t border_style;
        if (strcmp(border_style_str, "toggle") == 0) {
            border_style = static_cast<border_style_t>((current->border_style + 1) % 3);
        } else if (strcmp(border_style_str, "normal") == 0) {
            border_style = BS_NORMAL;
        } else if (strcmp(border_style_str, "pixel") == 0) {
            border_style = BS_PIXEL;
        } else if (strcmp(border_style_str, "none") == 0) {
            border_style = BS_NONE;
        } else {
            throw std::runtime_error(fmt::sprintf("BUG: called with border_style=%s", border_style_str));
        }

        const int con_border_width = border_width_from_style(border_style, border_width, current);
        con_set_border_style(current, border_style, con_border_width);
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'nop <comment>'.
 *
 */
void CommandsApplier::nop(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *comment) {
    LOG("-------------------------------------------------\n");
    LOG(fmt::sprintf("  NOP: %.4000s\n",  (comment != nullptr) ? comment : ""));
    LOG("-------------------------------------------------\n");
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'append_layout <path>'.
 *
 */
void CommandsApplier::append_layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *cpath) {
     LOG(fmt::sprintf("Appending layout \"%s\"\n", cpath));

    /* Make sure we allow paths like '~/.i3/layout.json' */
    auto path = utils::resolve_tilde(cpath);
    std::string buf = slurp(path);

    if (!json_validate(buf)) {
        ELOG(fmt::sprintf("Could not parse \"%s\" as JSON, not loading.\n", path));
        throw std::runtime_error(fmt::sprintf("Could not parse \"%s\" as JSON.", path));
    }

    json_content_t content = json_determine_content(buf);
    LOG(fmt::sprintf("JSON content = %d\n",  content));
    if (content == JSON_CONTENT_UNKNOWN) {
        ELOG(fmt::sprintf("Could not determine the contents of \"%s\", not loading.\n", path));
        throw std::runtime_error(fmt::sprintf("Could not determine the contents of \"%s\".", path));
    }

    Con *parent = global.focused;
    if (content == JSON_CONTENT_WORKSPACE) {
        parent = parent->con_get_output()->output_get_content();
    } else {
        /* We need to append the layout to a split container, since a leaf
         * container must not have any children (by definition).
         * Note that we explicitly check for workspaces, since they are okay for
         * this purpose, but con_accepts_window() returns false for workspaces. */
        while (parent->type != CT_WORKSPACE && !parent->con_accepts_window())
            parent = parent->parent;
    }
    DLOG(fmt::sprintf("Appending to parent=%p instead of focused=%p\n",  (void*)parent, (void*)global.focused));
    char *errormsg = nullptr;
    tree_append_json(parent, buf, &errormsg);
    if (errormsg != nullptr) {
        yerror(cmd_output.json_gen, errormsg);
        free(errormsg);
        /* Note that we continue executing since tree_append_json() has
         * side-effects — user-provided layouts can be partly valid, partly
         * invalid, leading to half of the placeholder containers being
         * created. */
    } else {
        ysuccess(cmd_output.json_gen,  true);
    }

    // XXX: This is a bit of a kludge. Theoretically, render_con(parent,
    // false); should be enough, but when sending 'workspace 4; append_layout
    // /tmp/foo.json', the needs_tree_render == true of the workspace command
    // is not executed yet and will be batched with append_layout’s
    // needs_tree_render after the parser finished. We should check if that is
    // necessary at all.
    render_con(global.croot);

    restore_open_placeholder_windows(parent);

    if (content == JSON_CONTENT_WORKSPACE)
        ipc_send_workspace_event("restored", parent, nullptr);

    cmd_output.needs_tree_render = true;
}

/*
 * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
 *
 */
void CommandsApplier::workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which) {
    Con *ws;

    DLOG(fmt::sprintf("which=%s\n",  which));

    if (global.croot->con_get_fullscreen_con(CF_GLOBAL)) {
        throw std::runtime_error("Cannot switch workspace while in global fullscreen");
    }

    if (strcmp(which, "next") == 0)
        ws = workspace_next();
    else if (strcmp(which, "prev") == 0)
        ws = workspace_prev();
    else if (strcmp(which, "next_on_output") == 0)
        ws = workspace_next_on_output();
    else if (strcmp(which, "prev_on_output") == 0)
        ws = workspace_prev_on_output();
    else {
        throw std::runtime_error(fmt::sprintf("BUG: called with which=%s", which));
    }

    workspace_show(ws);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
 *
 */
void CommandsApplier::workspace_number(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *which, const char *_no_auto_back_and_forth) {
    const bool no_auto_back_and_forth = (_no_auto_back_and_forth != nullptr);

    if (global.croot->con_get_fullscreen_con(CF_GLOBAL)) {
        throw std::runtime_error("Cannot switch workspace while in global fullscreen");
    }

    long parsed_num = ws_name_to_number(which);
    if (parsed_num == -1) {
        throw std::runtime_error(fmt::sprintf("Could not parse initial part of \"%s\" as a number.", which));
    }

    Con *workspace = get_existing_workspace_by_num(parsed_num);
    if (!workspace) {
        LOG(fmt::sprintf("There is no workspace with number %ld, creating a new one.\n",  parsed_num));
        ysuccess(cmd_output.json_gen,  true);
        workspace_show_by_name(which);
        cmd_output.needs_tree_render = true;
        return;
    }
    if (!no_auto_back_and_forth && maybe_back_and_forth(cmd_output, workspace->name.c_str())) {
        ysuccess(cmd_output.json_gen,  true);
        return;
    }
    workspace_show(workspace);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'workspace back_and_forth'.
 *
 */
void CommandsApplier::workspace_back_and_forth(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    if (global.croot->con_get_fullscreen_con(CF_GLOBAL)) {
        throw std::runtime_error("Cannot switch workspace while in global fullscreen");
    }

    ::workspace_back_and_forth();

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen,  true);
}

/*
 * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
 *
 */
void CommandsApplier::workspace_name(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, const char *_no_auto_back_and_forth) {
    const bool no_auto_back_and_forth = (_no_auto_back_and_forth != nullptr);

    if (strncasecmp(name, "__", strlen("__")) == 0) {
        throw std::runtime_error(fmt::sprintf("You cannot switch to the i3-internal workspaces (\"%s\").", name));
    }

    if (global.croot->con_get_fullscreen_con(CF_GLOBAL)) {
        throw std::runtime_error("Cannot switch workspace while in global fullscreen");
    }

    DLOG(fmt::sprintf("should switch to workspace %s\n",  name));
    if (!no_auto_back_and_forth && maybe_back_and_forth(cmd_output, name)) {
        ysuccess(cmd_output.json_gen,  true);
        return;
    }
    workspace_show_by_name(name);

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'mode <string>'.
 *
 */
void CommandsApplier::mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *mode) {
    DLOG(fmt::sprintf("mode=%s\n",  mode));
    switch_mode(mode);

    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [window|container|workspace] [to] output <strings>'.
 *
 */
void CommandsApplier::move_con_to_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name, bool move_workspace) {
    /* Initialize a data structure that is used to save multiple user-specified
     * output names since this function is called multiple types for each
     * command call. */
    std::vector<std::string> user_output_names{};

    if (name) {
        if (strcmp(name, "next") == 0) {
            /* "next" here works like a wildcard: It "expands" to all available
             * outputs. */
            for (Output *output: global.randr->outputs) {
                user_output_names.push_back(std::string(output->output_primary_name()));
            }
            return;
        }

        user_output_names.emplace_back(name);
        return;
    }

    HANDLE_EMPTY_MATCH(criteria_state);

    if (user_output_names.empty()) {
        throw std::runtime_error("At least one output must be specified");
    }

    bool success = false;
    for (auto current: criteria_state->owindows) {
        WorkspaceCon *ws = current->con_get_workspace();

        Output *current_output = get_output_for_con(ws);

        Output *target_output = nullptr;
        for (auto uo = user_output_names.begin(); uo != user_output_names.end(); ++uo) {
            if (strcasecmp(current_output->output_primary_name().c_str(), (*uo).c_str()) == 0) {
                /* The current output is in the user list */
                while (true) {
                    /* This corrupts the outer loop but it is ok since we are
                     * going to break anyway. */
                    auto next = ++uo;
                    if (next == user_output_names.end()) {
                        /* We reached the end of the list. We should use the
                         * first available output that, if it exists, is
                         * already saved in target_output. */
                        break;
                    }
                    Output *out = current_output->get_output_from_string(*next);
                    if (out) {
                        DLOG(fmt::sprintf("Found next target for workspace %s from user list: %s\n",  ws->name, *next));
                        target_output = out;
                        break;
                    }
                }
                break;
            }
            if (!target_output) {
                /* The first available output from the list is used in 2 cases:
                 * 1. When we must wrap around the user list. For example, if
                 * user specifies outputs A B C and C is `current_output`.
                 * 2. When the current output is not in the user list. For
                 * example, user specifies A B C and D is `current_output`.
                 */
                DLOG(fmt::sprintf("Found first target for workspace %s from user list: %s\n",  ws->name, *uo));
                target_output = current_output->get_output_from_string(*uo);
            }
        }
        if (target_output) {
            if (move_workspace) {
                workspace_move_to_output(ws, target_output);
            } else {
                con_move_to_output(current, target_output, true);
            }
            success = true;
        }
    }

    cmd_output.needs_tree_render = success;
    if (success) {
        ysuccess(cmd_output.json_gen, true);
    } else {
        yerror(cmd_output.json_gen, "No output matched");
    }
}

/*
 * Implementation of 'floating enable|disable|toggle'
 *
 */
void CommandsApplier::floating(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *floating_mode) {
    DLOG(fmt::sprintf("floating_mode=%s\n",  floating_mode));

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
        if (strcmp(floating_mode, "toggle") == 0) {
            DLOG("should toggle mode\n");
            toggle_floating_mode(current, false);
        } else {
            DLOG(fmt::sprintf("should switch mode to %s\n",  floating_mode));
            if (strcmp(floating_mode, "enable") == 0) {
                floating_enable(current, false);
            } else {
                floating_disable(current);
            }
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
 *
 */
void CommandsApplier::split(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction) {
    HANDLE_EMPTY_MATCH(criteria_state);

    LOG(fmt::sprintf("splitting in direction %c\n",  direction[0]));
    for (auto current: criteria_state->owindows) {
        if (current->con_is_docked()) {
            ELOG("Cannot split a docked container, skipping.\n");
            continue;
        }

        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
        if (direction[0] == 't') {
            layout_t current_layout;
            if (current->type == CT_WORKSPACE) {
                current_layout = current->layout;
            } else {
                current_layout = current->parent->layout;
            }
            /* toggling split orientation */
            if (current_layout == L_SPLITH) {
                tree_split(current, VERT);
            } else {
                tree_split(current, HORIZ);
            }
        } else {
            tree_split(current, (direction[0] == 'v' ? VERT : HORIZ));
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'kill [window|client]'.
 *
 */
void CommandsApplier::kill(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *kill_mode_str) {
    if (kill_mode_str == nullptr)
        kill_mode_str = "window";

    DLOG(fmt::sprintf("kill_mode=%s\n",  kill_mode_str));

    int kill_mode;
    if (strcmp(kill_mode_str, "window") == 0)
        kill_mode = KILL_WINDOW;
    else if (strcmp(kill_mode_str, "client") == 0)
        kill_mode = KILL_CLIENT;
    else {
        throw std::runtime_error(fmt::sprintf("BUG: called with kill_mode=%s", kill_mode_str));
    }

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        current->con_close((kill_window_t) kill_mode);
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'exec [--no-startup-id] <command>'.
 *
 */
void CommandsApplier::exec(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *nosn, const char *command) {
    bool no_startup_id = (nosn != nullptr);

    HANDLE_EMPTY_MATCH(criteria_state);

    int count = criteria_state->owindows.size();

    if (count > 1) {
         LOG(fmt::sprintf("WARNING: Your criteria for the exec command match %d containers, "
            "so the command will execute this many times.\n",
            count));
    }

    for (auto it = criteria_state->owindows.begin(); it != criteria_state->owindows.end(); ++it) {
        DLOG(fmt::sprintf("should execute %s, no_startup_id = %d\n",  command, no_startup_id));
        start_application(command, no_startup_id);
    }

    ysuccess(cmd_output.json_gen, true);
}

static void CMD_FOCUS_WARN_CHILDREN(struct criteria_state *criteria_state) {
    int count = criteria_state->owindows.size();

    if (count > 1) {
         LOG(fmt::sprintf("WARNING: Your criteria for the focus command matches %d containers, "
            "while only exactly one container can be focused at a time.\n",
            count));
    }
}

/*
 * Implementation of 'focus left|right|up|down|next|prev'.
 *
 */
void CommandsApplier::focus_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
    HANDLE_EMPTY_MATCH(criteria_state);
    CMD_FOCUS_WARN_CHILDREN(criteria_state);

    direction_t direction;
    position_t position;
    bool auto_direction = true;
    if (strcmp(direction_str, "prev") == 0) {
        position = BEFORE;
    } else if (strcmp(direction_str, "next") == 0) {
        position = AFTER;
    } else {
        auto_direction = false;
        direction = parse_direction(direction_str);
    }

    for (auto current: criteria_state->owindows) {
        Con *ws = current->con_get_workspace();
        if (!ws) {
            continue;
        }
        if (auto_direction) {
            orientation_t o = con_orientation(current->parent);
            direction = direction_from_orientation_position(o, position);
        }
        tree_next(current, direction);
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'focus next|prev sibling'
 *
 */
void CommandsApplier::focus_sibling(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str) {
    HANDLE_EMPTY_MATCH(criteria_state);
    CMD_FOCUS_WARN_CHILDREN(criteria_state);

    const position_t direction = (STARTS_WITH(direction_str, "prev")) ? BEFORE : AFTER;
    for (auto current: criteria_state->owindows) {
        Con *ws = current->con_get_workspace();
        if (!ws) {
            continue;
        }
        Con *next = get_tree_next_sibling(current, direction);
        if (next) {
            if (next->type == CT_WORKSPACE) {
                /* On the workspace level, we need to make sure that the
                 * workspace change happens properly. However, workspace_show
                 * descends focus so we also have to put focus on the workspace
                 * itself to maintain consistency. See #3997. */
                workspace_show(next);
                next->con_focus();
            } else {
                next->con_activate();
            }
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'focus tiling|floating|mode_toggle'.
 *
 */
void CommandsApplier::focus_window_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *window_mode) {
    DLOG(fmt::sprintf("window_mode = %s\n",  window_mode));

    bool to_floating = false;
    if (strcmp(window_mode, "mode_toggle") == 0) {
        to_floating = !global.focused->con_inside_floating();
    } else if (strcmp(window_mode, "floating") == 0) {
        to_floating = true;
    } else if (strcmp(window_mode, "tiling") == 0) {
        to_floating = false;
    }

    Con *ws = global.focused->con_get_workspace();
    bool success = false;
    for (auto &current : ws->focus_head) {
        if ((to_floating && current->type != CT_FLOATING_CON) ||
            (!to_floating && current->type == CT_FLOATING_CON))
            continue;

        con_descend_focused(current)->con_activate_unblock();
        success = true;
        break;
    }

    if (success) {
        cmd_output.needs_tree_render = true;
        ysuccess(cmd_output.json_gen, true);
    } else {
        throw std::runtime_error(fmt::sprintf("Failed to find a %s container in workspace.", to_floating ? "floating" : "tiling"));
    }
}

/*
 * Implementation of 'focus parent|child'.
 *
 */
void CommandsApplier::focus_level(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *level) {
    DLOG(fmt::sprintf("level = %s\n",  level));
    bool success = false;

    /* Focusing the parent can only be allowed if the newly
     * focused container won't escape the fullscreen container. */
    if (strcmp(level, "parent") == 0) {
        if (global.focused && global.focused->parent) {
            if (con_fullscreen_permits_focusing(global.focused->parent))
                success = level_up();
            else
                ELOG("'focus parent': Currently in fullscreen, not going up\n");
        }
    }

        /* Focusing a child should always be allowed. */
    else
        success = level_down();

    cmd_output.needs_tree_render = success;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, success);
}

/*
 * Implementation of 'focus'.
 *
 */
void CommandsApplier::focus(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    //DLOG(fmt::sprintf("criteria_state->current_match = %p\n",  (void*)criteria_state->current_match));

    if (criteria_state->current_match.match_is_empty()) {
        ELOG("You have to specify which window/container should be focused.\n");
        ELOG("Example: [class=\"urxvt\" title=\"irssi\"] focus\n");

        throw std::runtime_error("You have to specify which window/container should be focused");
    } else if (criteria_state->owindows.empty()) {
        throw std::runtime_error("No window matches given criteria");
    }

    CMD_FOCUS_WARN_CHILDREN(criteria_state);

    for (auto current: criteria_state->owindows) {
        Con *ws = current->con_get_workspace();
        /* If no workspace could be found, this was a dock window.
         * Just skip it, you cannot focus dock windows. */
        if (!ws)
            continue;

        LOG(fmt::sprintf("focusing %p / %s\n",  (void*)current, current->name));
        current->con_activate_unblock();
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'fullscreen enable|toggle [global]' and
 *                   'fullscreen disable'
 *
 */
void CommandsApplier::fullscreen(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action, const char *fullscreen_mode) {
    fullscreen_mode_t mode = strcmp(fullscreen_mode, "global") == 0 ? CF_GLOBAL : CF_OUTPUT;
    DLOG(fmt::sprintf("%s fullscreen, mode = %s\n",  action, fullscreen_mode));

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
        if (strcmp(action, "toggle") == 0) {
            con_toggle_fullscreen(current, mode);
        } else if (strcmp(action, "enable") == 0) {
            con_enable_fullscreen(current, mode);
        } else if (strcmp(action, "disable") == 0) {
            con_disable_fullscreen(current);
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'sticky enable|disable|toggle'.
 *
 */
void CommandsApplier::sticky(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *action) {
    DLOG(fmt::sprintf("%s sticky on window\n",  action));
    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        if (current->window == nullptr) {
            ELOG(fmt::sprintf("only containers holding a window can be made sticky, skipping con = %p\n",  (void*)current));
            continue;
        }
        DLOG(fmt::sprintf("setting sticky for container = %p / %s\n",  (void*)current, current->name));

        bool sticky = false;
        if (strcmp(action, "enable") == 0)
            sticky = true;
        else if (strcmp(action, "disable") == 0)
            sticky = false;
        else if (strcmp(action, "toggle") == 0)
            sticky = !current->sticky;

        current->sticky = sticky;
        ewmh_update_sticky(current->window->id, sticky);
    }

    /* A window we made sticky might not be on a visible workspace right now, so we need to make
     * sure it gets pushed to the front now. */
    output_push_sticky_windows(global.focused);

    ewmh_update_wm_desktop();

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move <direction> [<amount> [px|ppt]]'.
 *
 */
void CommandsApplier::move_direction(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *direction_str, long amount, const char *mode) {
    HANDLE_EMPTY_MATCH(criteria_state);

    Con *initially_focused = global.focused;
    direction_t direction = parse_direction(direction_str);

    const bool is_ppt = mode && strcmp(mode, "ppt") == 0;

    DLOG(fmt::sprintf("moving in direction %s, %ld %s\n",  direction_str, amount, mode));
    for (auto current: criteria_state->owindows) {
        if (current->con_is_floating()) {
            DLOG(fmt::sprintf("floating move with %ld %s\n",  amount, mode));
            Rect newrect = current->parent->rect;
            Con *output = current->con_get_output();

            switch (direction) {
                case D_LEFT:
                    newrect.x -= is_ppt ? output->rect.width * ((double) amount / 100.0) : amount;
                    break;
                case D_RIGHT:
                    newrect.x += is_ppt ? output->rect.width * ((double) amount / 100.0) : amount;
                    break;
                case D_UP:
                    newrect.y -= is_ppt ? output->rect.height * ((double) amount / 100.0) : amount;
                    break;
                case D_DOWN:
                    newrect.y += is_ppt ? output->rect.height * ((double) amount / 100.0) : amount;
                    break;
            }

            cmd_output.needs_tree_render = floating_reposition(current->parent, newrect);
        } else {
            tree_move(current, direction);
            cmd_output.needs_tree_render = true;
        }
    }

    /* The move command should not disturb focus. con_exists is called because
     * tree_move calls tree_flatten. */
    if (global.focused != initially_focused && initially_focused != nullptr && initially_focused->exists()) {
        initially_focused->con_activate();
    }

    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
 *
 */
void CommandsApplier::layout(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *layout_str) {
    HANDLE_EMPTY_MATCH(criteria_state);

    layout_t layout;
    if (!layout_from_name(layout_str, &layout)) {
        throw std::runtime_error(fmt::sprintf("Unknown layout \"%s\", this is a mismatch between code and parser spec.", layout_str));
    }

    DLOG(fmt::sprintf("changing layout to %s (%d)\n",  layout_str, layout));

    for (auto current: criteria_state->owindows) {
        if (current->con_is_docked()) {
            ELOG("cannot change layout of a docked container, skipping it.\n");
            continue;
        }

        DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
        con_set_layout(current, layout);
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'layout toggle [all|split]'.
 *
 */
void CommandsApplier::layout_toggle(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *toggle_mode) {
    if (toggle_mode == nullptr)
        toggle_mode = "default";

    DLOG(fmt::sprintf("toggling layout (mode = %s)\n",  toggle_mode));

    /* check if the match is empty, not if the result is empty */
    if (criteria_state->current_match.match_is_empty())
        con_toggle_layout(global.focused, toggle_mode);
    else {
        for (auto current: criteria_state->owindows) {
            DLOG(fmt::sprintf("matching: %p / %s\n",  (void*)current, current->name));
            con_toggle_layout(current, toggle_mode);
        }
    }

    cmd_output.needs_tree_render = true;
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'exit'.
 *
 */
void CommandsApplier::exit(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    LOG("Exiting due to user command.\n");
    ::exit(EXIT_SUCCESS);

    /* unreached */
}

/*
 * Implementation of 'reload'.
 *
 */
void CommandsApplier::reload(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    LOG("reloading\n");

    kill_nagbar(global.config_error_nagbar_pid, false);
    kill_nagbar(global.command_error_nagbar_pid, false);
    /* start_nagbar() will refuse to start a new process if the passed pid is
     * set. This will happen when our child watcher is triggered by libev when
     * the loop re-starts. However, config errors might be detected before
     * that since we will read the config right now with load_configuration.
     * See #4104. */
    global.config_error_nagbar_pid = global.command_error_nagbar_pid = -1;

    load_configuration(nullptr, config_load_t::C_RELOAD);
    x_set_i3_atoms();
    /* Send an IPC event just in case the ws names have changed */
    ipc_send_workspace_event("reload", nullptr, nullptr);
    /* Send an update event for each barconfig just in case it has changed */
    for (auto &current: barconfigs) {
        ipc_send_barconfig_update_event(current.get());
    }

    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'restart'.
 *
 */
void CommandsApplier::restart(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    LOG("restarting i3\n");
    int exempt_fd = -1;
    if (cmd_output.client != nullptr) {
        exempt_fd = cmd_output.client->fd;
        LOG(fmt::sprintf("Carrying file descriptor %d across restart\n",  exempt_fd));
        int flags;
        if ((flags = fcntl(exempt_fd, F_GETFD)) < 0 ||
            fcntl(exempt_fd, F_SETFD, flags & ~FD_CLOEXEC) < 0) {
            ELOG(fmt::sprintf("Could not disable FD_CLOEXEC on fd %d\n",  exempt_fd));
        }
        char *fdstr = nullptr;
        sasprintf(&fdstr, "%d", exempt_fd);
        setenv("_I3_RESTART_FD", fdstr, 1);
    }
    ipc_shutdown(SHUTDOWN_REASON_RESTART, exempt_fd);
    unlink(config.ipc_socket_path);
    i3_restart(false);
    /* unreached */
    assert(false);
}

/*
 * Implementation of 'open'.
 *
 */
void CommandsApplier::open(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    LOG("opening new container\n");
    Con *con = tree_open_con(nullptr, nullptr);
    con->layout = L_SPLITH;
    con->con_activate();
    if (cmd_output.json_gen != nullptr) {
        cmd_output.json_gen->push_back({
            { "success", true },
            { "id", (uintptr_t) con }
        });

    }

    cmd_output.needs_tree_render = true;
}

/*
 * Implementation of 'focus output <output>'.
 *
 */
void CommandsApplier::focus_output(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *name) {
    HANDLE_EMPTY_MATCH(criteria_state);

    if (criteria_state->owindows.empty()) {
        ysuccess(cmd_output.json_gen, true);
        return;
    }

    Output *current_output = get_output_for_con(criteria_state->owindows.front());
    Output *output = current_output->get_output_from_string(name);

    if (!output) {
        throw std::runtime_error(fmt::sprintf("Output %s not found.", name));
    }

    /* get visible workspace on output */
    auto ws = std::ranges::find_if(output->con->output_get_content()->nodes_head, [](auto &child) { return workspace_is_visible(child); });
    if (ws == output->con->output_get_content()->nodes_head.end()) {
        throw std::runtime_error("BUG: No workspace found on output.");
    }

    workspace_show(*ws);

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
 *
 */
void CommandsApplier::move_window_to_position(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, long x, const char *mode_x, long y, const char *mode_y) {
    bool has_error = false;

    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        if (!current->con_is_floating()) {
            ELOG("Cannot change position. The window/container is not floating\n");

            if (!has_error) {
                yerror(cmd_output.json_gen, "Cannot change position of a window/container because it is not floating.");
                has_error = true;
            }

            continue;
        }

        Rect newrect = current->parent->rect;
        Con *output = current->con_get_output();

        newrect.x = mode_x && strcmp(mode_x, "ppt") == 0 ? output->rect.width * ((double) x / 100.0) : x;
        newrect.y = mode_y && strcmp(mode_y, "ppt") == 0 ? output->rect.height * ((double) y / 100.0) : y;
        DLOG(fmt::sprintf("moving to position %d %s %d %s\n",  newrect.x, mode_x, newrect.y, mode_y));

        if (!floating_reposition(current->parent, newrect)) {
            yerror(cmd_output.json_gen, "Cannot move window/container out of bounds.");
            has_error = true;
        }
    }

    if (!has_error)
        ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [window|container] [to] [absolute] position center
 *
 */
void CommandsApplier::move_window_to_center(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *method) {
    bool has_error = false;
    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        Con *floating_con = current->con_inside_floating();
        if (floating_con == nullptr) {
            ELOG(fmt::sprintf("con %p / %s is not floating, cannot move it to the center.\n",
                 (void*)current, current->name));

            if (!has_error) {
                yerror(cmd_output.json_gen, "Cannot change position of a window/container because it is not floating.");
                has_error = true;
            }

            continue;
        }

        if (strcmp(method, "absolute") == 0) {
            DLOG("moving to absolute center\n");
            floating_center(floating_con, global.croot->rect);

            floating_maybe_reassign_ws(floating_con);
            cmd_output.needs_tree_render = true;
        }

        if (strcmp(method, "position") == 0) {
            DLOG("moving to center\n");
            floating_center(floating_con, floating_con->con_get_workspace()->rect);

            cmd_output.needs_tree_render = true;
        }
    }

    // XXX: default reply for now, make this a better reply
    if (!has_error)
        ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'move [window|container] [to] position mouse'
 *
 */
void CommandsApplier::move_window_to_mouse(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output) {
    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        Con *floating_con = current->con_inside_floating();
        if (floating_con == nullptr) {
            DLOG(fmt::sprintf("con %p / %s is not floating, cannot move it to the mouse position.\n",
                 (void*)current, current->name));
            continue;
        }

        DLOG(fmt::sprintf("moving floating container %p / %s to cursor position\n",  (void*)floating_con, floating_con->name));
        floating_move_to_pointer(floating_con);
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'title_format <format>'
 *
 */
void CommandsApplier::title_format(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *format) {
     DLOG(fmt::sprintf("setting title_format to \"%s\"\n", format));
    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("setting title_format for %p / %s\n",  (void*)current, current->name));
        current->title_format.clear();

        /* If we only display the title without anything else, we can skip the parsing step,
         * so we remove the title format altogether. */
        if (strcasecmp(format, "%title") != 0) {
            current->title_format = format;

            if (current->window != nullptr) {
                i3String *formatted_title = con_parse_title_format(current);
                ewmh_update_visible_name(current->window->id, formatted_title->get_utf8());
                delete formatted_title;
                formatted_title = nullptr;
            }
        } else {
            if (current->window != nullptr) {
                /* We can remove _NET_WM_VISIBLE_NAME since we don't display a custom title. */
                ewmh_update_visible_name(current->window->id, nullptr);
            }
        }

        if (current->window != nullptr) {
            /* Make sure the window title is redrawn immediately. */
            current->window->name_x_changed = true;
        } else {
            /* For windowless containers we also need to force the redrawing. */
            delete current->deco_render_params;
            current->deco_render_params = nullptr;
        }
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'title_window_icon <yes|no>' and 'title_window_icon padding <px>'
 *
 */
void CommandsApplier::title_window_icon(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *enable, int padding) {
    if (enable != nullptr && !boolstr(enable)) {
        padding = -1;
    }
    DLOG(fmt::sprintf("setting window_icon=%d\n",  padding));
    HANDLE_EMPTY_MATCH(criteria_state);

    for (auto current: criteria_state->owindows) {
        DLOG(fmt::sprintf("setting window_icon for %p / %s\n",  (void*)current, current->name));
        current->window_icon_padding = padding;

        if (current->window != nullptr) {
            /* Make sure the window title is redrawn immediately. */
            current->window->name_x_changed = true;
        } else {
            /* For windowless containers we also need to force the redrawing. */
            delete current->deco_render_params;
            current->deco_render_params = nullptr;
        }
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'rename workspace [<name>] to <name>'
 *
 */
void CommandsApplier::rename_workspace(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *old_name, const char *new_name) {
    if (strncasecmp(new_name, "__", strlen("__")) == 0) {
        throw std::runtime_error(fmt::sprintf("Cannot rename workspace to \"%s\": names starting with __ are i3-internal.", new_name));
    }
    if (old_name) {
         LOG(fmt::sprintf("Renaming workspace \"%s\" to \"%s\"\n", old_name, new_name));
    } else {
         LOG(fmt::sprintf("Renaming current workspace to \"%s\"\n", new_name));
    }

    WorkspaceCon *workspace;
    if (old_name) {
        workspace = get_existing_workspace_by_name(old_name);
    } else {
        workspace = global.focused->con_get_workspace();
        old_name = workspace->name.c_str();
    }

    if (!workspace) {
        throw std::runtime_error(fmt::sprintf("Old workspace \"%s\" not found", old_name));
    }

    Con *check_dest = get_existing_workspace_by_name(new_name);

    /* If check_dest == workspace, the user might be changing the case of the
     * workspace, or it might just be a no-op. */
    if (check_dest != nullptr && check_dest != workspace) {
        throw std::runtime_error(fmt::sprintf("New workspace \"%s\" already exists", new_name));
    }

    /* Change the name and try to parse it as a number. */
    /* old_name might refer to workspace->name, so copy it before free()ing */
    char *old_name_copy = sstrdup(old_name);
    workspace->name = new_name;

    workspace->num = ws_name_to_number(new_name);
    LOG(fmt::sprintf("num = %d\n",  workspace->num));

    /* By re-attaching, the sort order will be correct afterwards. */
    Con *previously_focused = global.focused;
    Con *previously_focused_content = global.focused->type == CT_WORKSPACE ? global.focused->parent : nullptr;
    Con *parent = workspace->parent;
    workspace->con_detach();
    workspace->con_attach(parent, false);
    ipc_send_workspace_event("rename", workspace, nullptr);

    Con *assigned = get_assigned_output(workspace->name.c_str(), workspace->num);
    if (assigned) {
        workspace_move_to_output(workspace, get_output_for_con(assigned));
    }

    bool can_restore_focus = previously_focused != nullptr;
    /* NB: If previously_focused is a workspace we can't work directly with it
     * since it might have been cleaned up by workspace_show() already,
     * depending on the focus order/number of other workspaces on the output.
     * Instead, we loop through the available workspaces and only focus
     * previously_focused if we still find it. */
    if (previously_focused_content) {
        auto workspace = std::ranges::find_if(previously_focused_content->nodes_head, [&previously_focused](auto &child) { return child == previously_focused; });
        can_restore_focus &= (workspace != previously_focused->nodes_head.end());
    }

    if (can_restore_focus) {
        /* Restore the previous focus since con_attach messes with the focus. */
        workspace_show(previously_focused->con_get_workspace());
        previously_focused->con_focus();
    }

    /* Let back-and-forth work after renaming the previous workspace.
     * See #3694. */
    if (!previous_workspace_name.empty() && !strcmp(previous_workspace_name.c_str(), old_name_copy)) {
        previous_workspace_name.assign(new_name);
    }

    cmd_output.needs_tree_render = true;
    ysuccess(cmd_output.json_gen, true);

    ewmh_update_desktop_properties();

    startup_sequence_rename_workspace(old_name_copy, new_name);
    free(old_name_copy);
}

/*
 * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
 *
 */
void CommandsApplier::bar_mode(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_mode, const char *bar_id) {
    int mode = M_DOCK;
    bool toggle = false;
    if (strcmp(bar_mode, "dock") == 0)
        mode = M_DOCK;
    else if (strcmp(bar_mode, "hide") == 0)
        mode = M_HIDE;
    else if (strcmp(bar_mode, "invisible") == 0)
        mode = M_INVISIBLE;
    else if (strcmp(bar_mode, "toggle") == 0)
        toggle = true;
    else {
         ELOG(fmt::sprintf("Unknown bar mode \"%s\", this is a mismatch between code and parser spec.\n", bar_mode));
        assert(false);
    }

    if (barconfigs.empty()) {
        throw std::runtime_error("No bars found\n");
    }

    for (auto &current: barconfigs) {
        if (bar_id && strcmp(current->id, bar_id) != 0) {
            continue;
        }

        if (toggle) {
            mode = (current->mode + 1) % 2;
        }

        DLOG(fmt::sprintf("Changing bar mode of bar_id '%s' from '%d' to '%s (%d)'\n",
             current->id, current->mode, bar_mode, mode));
        if ((int) current->mode != mode) {
            current->mode = static_cast<config_mode_t>(mode);
            ipc_send_barconfig_update_event(current.get());
        }

        if (bar_id) {
            /* We are looking for a specific bar and we found it */
            ysuccess(cmd_output.json_gen, true);
            return;
        }
    }

    if (bar_id) {
        /* We are looking for a specific bar and we did not find it */
        throw std::runtime_error(fmt::sprintf("Changing bar mode of bar_id %s failed, bar_id not found.\n", bar_id));
    } else {
        /* We have already handled the case of no bars, so we must have
         * updated all active bars now. */
        ysuccess(cmd_output.json_gen, true);
    }
}

/*
 * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
 *
 */
void CommandsApplier::bar_hidden_state(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *bar_hidden_state, const char *bar_id) {
    enum config_hidden_state_t hidden_state = S_SHOW;
    bool toggle = false;
    if (strcmp(bar_hidden_state, "hide") == 0)
        hidden_state = S_HIDE;
    else if (strcmp(bar_hidden_state, "show") == 0)
        hidden_state = S_SHOW;
    else if (strcmp(bar_hidden_state, "toggle") == 0)
        toggle = true;
    else {
         ELOG(fmt::sprintf("Unknown bar state \"%s\", this is a mismatch between code and parser spec.\n", bar_hidden_state));
        assert(false);
    }

    if (barconfigs.empty()) {
        throw std::runtime_error("No bars found\n");
    }

    for (auto &current: barconfigs) {
        if (bar_id && strcmp(current->id, bar_id) != 0) {
            continue;
        }

        if (toggle) {
            hidden_state = static_cast<config_hidden_state_t>((current->hidden_state + 1) % 2);
        }

        DLOG(fmt::sprintf("Changing bar hidden_state of bar_id '%s' from '%d' to '%s (%d)'\n",
             current->id, current->hidden_state, bar_hidden_state, hidden_state));
        if ((int) current->hidden_state != hidden_state) {
            current->hidden_state = hidden_state;
            ipc_send_barconfig_update_event(current.get());
        }

        if (bar_id) {
            /* We are looking for a specific bar and we found it */
            ysuccess(cmd_output.json_gen, true);
            return;
        }
    }

    if (bar_id) {
        /* We are looking for a specific bar and we did not find it */
        throw std::runtime_error(fmt::sprintf("Changing bar hidden_state of bar_id %s failed, bar_id not found.\n", bar_id));
    } else {
        /* We have already handled the case of no bars, so we must have
         * updated all active bars now. */
        ysuccess(cmd_output.json_gen, true);
    }
}

/*
 * Implementation of 'debuglog toggle|on|off'
 *
 */
void CommandsApplier::debuglog(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char *argument) {
    bool logging = get_debug_logging();
    if (!strcmp(argument, "toggle")) {
        LOG(fmt::sprintf("%s debug logging\n",  logging ? "Disabling" : "Enabling"));
        set_debug_logging(!logging);
    } else if (!strcmp(argument, "on") && !logging) {
        LOG("Enabling debug logging\n");
        set_debug_logging(true);
    } else if (!strcmp(argument, "off") && logging) {
        LOG("Disabling debug logging\n");
        set_debug_logging(false);
    }
    // XXX: default reply for now, make this a better reply
    ysuccess(cmd_output.json_gen, true);
}

/*
 * Implementation of 'nagbar'
 *
 */
void CommandsApplier::nagbar(struct criteria_state *criteria_state, struct CommandResultIR &cmd_output, const char* type, const char* message, const char* font, const char* primary, const char* labels, const char* actions, const char* terminals) {
    std::vector<button_t> buttons{};

    if (labels != nullptr) {
        buttons.push_back((button_t) {
                .label = new i3String{labels},
                .action = sstrdup(actions),
                .terminal = strcmp(terminals, "true") == 0,
        });
    }

    start_nagbar(nullptr, buttons, message, font, strcmp(type, "warning") == 0 ? TYPE_WARNING : TYPE_ERROR, strcmp(primary, "true") == 0);
    ysuccess(cmd_output.json_gen, true);
}

