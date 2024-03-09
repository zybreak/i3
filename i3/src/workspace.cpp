/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * workspace.c: Modifying workspaces, accessing them, moving containers to
 *              workspaces.
 *
 */
module;
#include <cstdlib>
#include <cstring>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <algorithm>
#include <ranges>

#include "i3_ipc/i3-ipc.h"
#include "i3.h"
module i3;

import :output;
import log;

import utils;

/* NULL-terminated list of workspace names (in order) extracted from
 * keybindings. */
static std::deque<std::string> binding_workspace_names{};

/*
 * Returns the workspace with the given name or NULL if such a workspace does
 * not exist.
 *
 */
WorkspaceCon *get_existing_workspace_by_name(const std::string &name) {
    WorkspaceCon *workspace = nullptr;
    for (auto &output : global.croot->nodes) {
        auto ws = std::ranges::find_if(output->output_get_content()->nodes, [&name](auto &child) { return !strcasecmp(child->name.c_str(), name.c_str()); });
        if (ws != output->output_get_content()->nodes.end()) {
            workspace = dynamic_cast<WorkspaceCon*>(*ws);
        }
    }

    return workspace;
}

/*
 * Returns the workspace with the given number or NULL if such a workspace does
 * not exist.
 *
 */
WorkspaceCon *get_existing_workspace_by_num(int num) {
    WorkspaceCon *workspace = nullptr;
    for (auto &output : global.croot->nodes) {
        auto ws = std::ranges::find_if(output->output_get_content()->nodes, [&num](auto &child) { return child->num == num; });
        if (ws != output->output_get_content()->nodes.end()) {
            workspace = dynamic_cast<WorkspaceCon*>(*ws);
        }
    }

    return workspace;
}

/*
 * Sets ws->layout to splith/splitv if default_orientation was specified in the
 * configfile. Otherwise, it uses splith/splitv depending on whether the output
 * is higher than wide.
 *
 */
static void _workspace_apply_default_orientation(Con *ws) {
    /* If default_orientation is set to NO_ORIENTATION we determine
     * orientation depending on output resolution. */
    if (config.default_orientation == NO_ORIENTATION) {
        Con *output = ws->con_get_output();
        ws->layout = (output->rect.height > output->rect.width) ? L_SPLITV : L_SPLITH;
        ws->rect = output->rect;
        DLOG(fmt::sprintf("Auto orientation. Workspace size set to (%d,%d), setting layout to %d.\n",
             output->rect.width, output->rect.height, ws->layout));
    } else {
        ws->layout = (config.default_orientation == HORIZ) ? L_SPLITH : L_SPLITV;
    }
}

/*
 * Returns the first output that is assigned to a workspace specified by the
 * given name or number. Returns NULL if no such output exists.
 *
 * If an assignment matches by number but there is an assignment later that
 * matches by name, the second one is preferred.
 * The order of the 'ws_assignments' queue is respected: if multiple
 * assignments match the criteria, the first one is returned.
 * 'name' is ignored when NULL, 'parsed_num' is ignored when it is -1.
 *
 */
Con *get_assigned_output(const char *name, long parsed_num) {
    Con *output = nullptr;
    for (const auto &assignment : global.ws_assignments) {
        if (assignment->output.empty()) {
            continue;
        }

        if (name && strcmp(assignment->name.c_str(), name) == 0) {
             DLOG(fmt::sprintf("Found workspace name=\"%s\" assignment to output \"%s\"\n",
                 name, assignment->output));
            Output *assigned_by_name = global.randr->get_output_by_name(assignment->output, true);
            if (assigned_by_name) {
                /* When the name matches exactly, skip numbered assignments. */
                return assigned_by_name->con;
            }
        } else if (!output && /* Only keep the first numbered assignment. */
                   parsed_num != -1 &&
                   name_is_digits(assignment->name.c_str()) &&
                   ws_name_to_number(assignment->name) == parsed_num) {
             DLOG(fmt::sprintf("Found workspace number=%ld assignment to output \"%s\"\n",
                 parsed_num, assignment->output));
            Output *assigned_by_num = global.randr->get_output_by_name(assignment->output, true);
            if (assigned_by_num) {
                output = assigned_by_num->con;
            }
        }
    }

    return output;
}

/*
 * Returns true if the first output assigned to a workspace with the given
 * workspace assignment is the same as the given output.
 */
bool output_triggers_assignment(Output *output, const Workspace_Assignment *assignment) {
    Con *assigned = get_assigned_output(assignment->name.c_str(), -1);
    return assigned && assigned == output->con;
}

/*
 * Returns a pointer to the workspace with the given number (starting at 0),
 * creating the workspace if necessary (by allocating the necessary amount of
 * memory and initializing the data structures correctly).
 *
 */
WorkspaceCon *workspace_get(const std::string &num) {
    WorkspaceCon *workspace = get_existing_workspace_by_name(num);
    if (workspace) {
        return workspace;
    }

     LOG(fmt::sprintf("Creating new workspace \"%s\"\n", num));

    /* We set workspace->num to the number if this workspace’s name begins with
     * a positive number. Otherwise it’s a named ws and num will be 1. */
    const int parsed_num = ws_name_to_number(num);

    Con *output = get_assigned_output(num.c_str(), parsed_num);
    /* if an assignment is not found, we create this workspace on the current output */
    if (!output) {
        output = global.focused->con_get_output();
    }

    /* No parent because we need to attach this container after setting its
     * type. con_attach will handle CT_WORKSPACEs differently. */
    workspace = new WorkspaceCon();

    x_set_name(workspace, fmt::format("[i3 con] workspace {}", num));

    workspace->name = num;
    workspace->workspace_layout = config.default_layout;
    workspace->num = parsed_num;
    workspace->gaps = gaps_for_workspace(workspace);

    workspace->con_attach(output->output_get_content(), false);
    _workspace_apply_default_orientation(workspace);

    ipc_send_workspace_event("init", workspace, nullptr);
    ewmh_update_desktop_properties();

    return workspace;
}

/*
 * Extracts workspace names from keybindings (e.g. “web” from “bindsym $mod+1
 * workspace web”), so that when an output needs a workspace, i3 can start with
 * the first configured one. Needs to be called before reorder_bindings() so
 * that the config-file order is used, not the i3-internal order.
 *
 */
void extract_workspace_names_from_bindings() {
    int n = 0;
    binding_workspace_names.clear();
    for (auto &bind : current_mode->bindings) {
        DLOG(fmt::sprintf("binding with command %s\n",  bind->command));
        if (bind->command.length() < strlen("workspace ") ||
            strncasecmp(bind->command.c_str(), "workspace", strlen("workspace")) != 0) {
            continue;
        }
        DLOG(fmt::sprintf("relevant command = %s\n",  bind->command));
        const char *target = bind->command.c_str() + strlen("workspace ");
        while (*target == ' ' || *target == '\t') {
            target++;
        }
        /* We check if this is the workspace
         * next/prev/next_on_output/prev_on_output/back_and_forth command.
         * Beware: The workspace names "next", "prev", "next_on_output",
         * "prev_on_output", "back_and_forth" and "current" are OK,
         * so we check before stripping the double quotes */
        if (strncasecmp(target, "next", strlen("next")) == 0 ||
            strncasecmp(target, "prev", strlen("prev")) == 0 ||
            strncasecmp(target, "next_on_output", strlen("next_on_output")) == 0 ||
            strncasecmp(target, "prev_on_output", strlen("prev_on_output")) == 0 ||
            strncasecmp(target, "back_and_forth", strlen("back_and_forth")) == 0 ||
            strncasecmp(target, "current", strlen("current")) == 0) {
            continue;
        }
        if (strncasecmp(target, "--no-auto-back-and-forth", strlen("--no-auto-back-and-forth")) == 0) {
            target += strlen("--no-auto-back-and-forth");
            while (*target == ' ' || *target == '\t') {
                target++;
            }
        }
        if (strncasecmp(target, "number", strlen("number")) == 0) {
            target += strlen("number");
            while (*target == ' ' || *target == '\t') {
                target++;
            }
        }
        char *target_name = utils::parse_string(&target, false);
        if (target_name == nullptr) {
            continue;
        }
        if (strncasecmp(target_name, "__", strlen("__")) == 0) {
             LOG(fmt::sprintf("Cannot create workspace \"%s\". Names starting with __ are i3-internal.\n", target));
            free(target_name);
            continue;
        }
         DLOG(fmt::sprintf("Saving workspace name \"%s\"\n", target_name));

        binding_workspace_names.emplace_back(target_name);
    }
}

/*
 * Returns a pointer to a new workspace in the given output. The workspace
 * is created attached to the tree hierarchy through the given content
 * container.
 *
 */
WorkspaceCon *create_workspace_on_output(Output *output, Con *content) {
    /* add a workspace to this output */
    bool exists = true;
    auto *ws = new WorkspaceCon();

    /* try the configured workspace bindings first to find a free name */
    for (const auto &target_name : binding_workspace_names) {
        /* Ensure that this workspace is not assigned to a different output —
         * otherwise we would create it, then move it over to its output, then
         * find a new workspace, etc… */
        Con *assigned = get_assigned_output(target_name.c_str(), -1);
        if (assigned && assigned != output->con) {
            continue;
        }

        const int num = ws_name_to_number(target_name);
        exists = (num == -1)
                     ? get_existing_workspace_by_name(target_name)
                     : get_existing_workspace_by_num(num);
        if (!exists) {
            ws->name = target_name;
            /* Set ->num to the number of the workspace, if the name actually
             * is a number or starts with a number */
            ws->num = num;
            DLOG(fmt::sprintf("Used number %d for workspace with name %s\n",  ws->num, ws->name));

            break;
        }
    }

    if (exists) {
        /* get the next unused workspace number */
        DLOG("Getting next unused workspace by number\n");
        int c = 0;
        while (exists) {
            c++;
            Con *assigned = get_assigned_output(nullptr, c);
            exists = (get_existing_workspace_by_num(c) || (assigned && assigned != output->con));
            DLOG(fmt::sprintf("result for ws %d: exists = %d\n",  c, exists));
        }
        ws->num = c;
        ws->name = fmt::format("{}", c);
    }

    ws->con_attach(content, false);

    x_set_name(ws, fmt::format("[i3 con] workspace %s", ws->name));

    ws->gaps = gaps_for_workspace(ws);

    ws->fullscreen_mode = CF_OUTPUT;

    ws->workspace_layout = config.default_layout;
    _workspace_apply_default_orientation(ws);

    ipc_send_workspace_event("init", ws, nullptr);
    return ws;
}

/*
 * Returns true if the workspace is currently visible. Especially important for
 * multi-monitor environments, as they can have multiple currently active
 * workspaces.
 *
 */
bool workspace_is_visible(Con *ws) {
    Con *output = ws->con_get_output();
    if (output == nullptr) {
        return false;
    }
    Con *fs = output->con_get_fullscreen_con(CF_OUTPUT);
    return (fs == ws);
}

/*
 * XXX: we need to clean up all this recursive walking code.
 *
 */
static Con *_get_sticky(Con *con, const std::string &sticky_group, Con *exclude) {
    for (auto &current : con->nodes) {
        if (current != exclude &&
            !current->sticky_group.empty() &&
            current->window != nullptr &&
            current->sticky_group == sticky_group) {
            return current;
        }

        Con *recurse = _get_sticky(current, sticky_group, exclude);
        if (recurse != nullptr) {
            return recurse;
        }
    }

    if (dynamic_cast<WorkspaceCon*>(con) != nullptr) {
        for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
            if (current != exclude &&
                !current->sticky_group.empty() &&
                current->window != nullptr &&
                current->sticky_group == sticky_group) {
                return current;
            }

            Con *recurse = _get_sticky(current, sticky_group, exclude);
            if (recurse != nullptr) {
                return recurse;
            }
        }
    }

    return nullptr;
}



/*
 * Reassigns all child windows in sticky containers. Called when the user
 * changes workspaces.
 *
 * XXX: what about sticky containers which contain containers?
 *
 */
static void workspace_reassign_sticky(Con *con) {
    /* 1: go through all containers */

    /* handle all children and floating windows of this node */
    for (auto &current : con->nodes) {
        if (current->sticky_group.empty()) {
            workspace_reassign_sticky(current);
            continue;
        }

        LOG(fmt::sprintf("Ah, this one is sticky: %s / %p\n",  current->name, fmt::ptr(current)));
        /* 2: find a window which we can re-assign */
        Con *output = current->con_get_output();
        Con *src = _get_sticky(output, current->sticky_group, current);

        if (src == nullptr) {
            LOG("No window found for this sticky group\n");
            workspace_reassign_sticky(current);
            continue;
        }

        x_move_win(src, current);
        current->window = src->window;
        current->mapped = true;
        src->window = nullptr;
        src->mapped = false;

        x_reparent_child(current, src);

        LOG(fmt::sprintf("re-assigned window from src %p to dest %p\n", fmt::ptr(src), fmt::ptr(current)));
    }

    if (dynamic_cast<WorkspaceCon*>(con)) {
        for (auto &current : dynamic_cast<WorkspaceCon*>(con)->floating_windows) {
            workspace_reassign_sticky(current);
        }
    }
}

/*
 * Callback to reset the urgent flag of the given con to false. May be started by
 * workspace_show to avoid urgency hints being lost by switching to a workspace
 * focusing the con.
 *
 */
static void workspace_defer_update_urgent_hint_cb(EV_P_ ev_timer *w, int revents) {
    Con *con = (Con*)w->data;

    ev_timer_stop(global.eventHandler->main_loop, con->urgency_timer);
    delete con->urgency_timer;
    con->urgency_timer = nullptr;

    if (con->urgent) {
        DLOG(fmt::sprintf("Resetting urgency flag of con %p by timer\n", fmt::ptr(con)));
        con_set_urgency(con, false);
        con_update_parents_urgency(con);
        workspace_update_urgent_flag(con->con_get_workspace());
        ipc_send_window_event("urgent", con);
        tree_render();
    }
}

/*
 * Switches to the given workspace
 *
 */
void workspace_show(Con *workspace) {
    Con *current, *old = nullptr;

    /* disable fullscreen for the other workspaces and get the workspace we are
     * currently on. */
    for (auto &c : workspace->parent->nodes) {
        current = c;
        if (current->fullscreen_mode == CF_OUTPUT) {
            old = current;
        }
        current->fullscreen_mode = CF_NONE;
    }

    /* enable fullscreen for the target workspace. If it happens to be the
     * same one we are currently on anyways, we can stop here. */
    workspace->fullscreen_mode = CF_OUTPUT;
    current = global.focused->con_get_workspace();
    if (workspace == current) {
        DLOG("Not switching, already there.\n");
        return;
    }

    /* Used to correctly update focus when pushing sticky windows. Holds the
     * previously focused container in the same output as workspace. For
     * example, if a sticky window is focused and then we switch focus to a
     * workspace in another output and then switch to a third workspace in the
     * first output, the sticky window needs to be refocused. */
    Con *old_focus = old ? con_descend_focused(old) : nullptr;

    /* Remember currently focused workspace for switching back to it later with
     * the 'workspace back_and_forth' command.
     * NOTE: We have to duplicate the name as the original will be freed when
     * the corresponding workspace is cleaned up.
     * NOTE: Internal cons such as __i3_scratch (when a scratchpad window is
     * focused) are skipped, see bug #868. */
    if (current) {
        previous_workspace_name.assign(current->name);
        DLOG(fmt::sprintf("Setting previous_workspace_name = %s\n",  previous_workspace_name));
    }

    workspace_reassign_sticky(workspace);

    DLOG(fmt::sprintf("switching to %p / %s\n", fmt::ptr(workspace), workspace->name));
    Con *next = con_descend_focused(workspace);

    /* Memorize current output */
    Con *old_output = global.focused->con_get_output();

    /* Display urgency hint for a while if the newly visible workspace would
     * focus and thereby immediately destroy it */
    if (next->urgent && static_cast<int>(config.workspace_urgency_timer * 1000) > 0) {
        /* focus for now… */
        next->urgent = false;
        next->con_focus();

        /* … but immediately reset urgency flags; they will be set to false by
         * the timer callback in case the container is focused at the time of
         * its expiration */
        global.focused->urgent = true;
        workspace->urgent = true;

        if (global.focused->urgency_timer == nullptr) {
            DLOG(fmt::sprintf("Deferring reset of urgency flag of con %p on newly shown workspace %p\n",
                              fmt::ptr(global.focused), fmt::ptr(workspace)));
            global.focused->urgency_timer = new ev_timer{};
            /* use a repeating timer to allow for easy resets */
            ev_timer_init(global.focused->urgency_timer, workspace_defer_update_urgent_hint_cb,
                          config.workspace_urgency_timer, config.workspace_urgency_timer);
            global.focused->urgency_timer->data = global.focused;
            ev_timer_start(global.eventHandler->main_loop, global.focused->urgency_timer);
        } else {
            DLOG(fmt::sprintf("Resetting urgency timer of con %p on workspace %p\n",
                              fmt::ptr(global.focused), fmt::ptr(workspace)));
            ev_timer_again(global.eventHandler->main_loop, global.focused->urgency_timer);
        }
    } else {
        next->con_focus();
    }

    ipc_send_workspace_event("focus", workspace, current);

    DLOG(fmt::sprintf("old = %p / %s\n", fmt::ptr(old), (old ? old->name : "(null)")));
    /* Close old workspace if necessary. This must be done *after* doing
     * urgency handling, because tree_close_internal() will do a con_focus() on the next
     * client, which will clear the urgency flag too early. Also, there is no
     * way for con_focus() to know about when to clear urgency immediately and
     * when to defer it. */
    if (old && old->nodes.empty() && dynamic_cast<WorkspaceCon*>(old) != nullptr && dynamic_cast<WorkspaceCon*>(old)->floating_windows.empty()) {
        /* check if this workspace is currently visible */
        if (!workspace_is_visible(old)) {
            LOG(fmt::sprintf("Closing old workspace (%p / %s), it is empty\n", fmt::ptr(old), old->name));
            auto gen = ipc_marshal_workspace_event("empty", old, nullptr);
            tree_close_internal(old, DONT_KILL_WINDOW, false);

            auto payload = gen.dump();
            ipc_send_event("workspace", I3_IPC_EVENT_WORKSPACE, payload);

            /* Avoid calling output_push_sticky_windows later with a freed container. */
            if (old == old_focus) {
                old_focus = nullptr;
            }

            ewmh_update_desktop_properties();
        }
    }

    workspace->fullscreen_mode = CF_OUTPUT;
    LOG(fmt::sprintf("focused now = %p / %s\n", fmt::ptr(global.focused), global.focused->name));

    /* Set mouse pointer */
    Con *new_output = global.focused->con_get_output();
    if (old_output != new_output) {
        x_set_warp_to(next->rect);
    }

    /* Update the EWMH hints */
    ewmh_update_current_desktop();

    /* Push any sticky windows to the now visible workspace. */
    output_push_sticky_windows(old_focus);
}

/*
 * Looks up the workspace by name and switches to it.
 *
 */
void workspace_show_by_name(const char *num) {
    workspace_show(workspace_get(num));
}

/*
 * Focuses the next workspace.
 *
 */
WorkspaceCon *workspace_next() {
    WorkspaceCon *current = global.focused->con_get_workspace();
    Con *next = nullptr, *first = nullptr, *first_opposite = nullptr;

    if (current->num == -1) {
        /* If currently a named workspace, find next named workspace. */
        auto it = std::ranges::find(current->parent->nodes, current);
        if (std::next(it) == current->parent->nodes.end()) {
            return dynamic_cast<WorkspaceCon*>(*(++it));
        }
        bool found_current = false;
        for (auto &output : global.croot->nodes) {
            for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
                for (auto &c : output->output_get_content()->nodes) {
                    child = c;
                    if (child->type != CT_WORKSPACE) {
                        continue;
                    }
                    if (!first) {
                        first = child;
                    }
                    if (!first_opposite || (child->num != -1 && child->num < first_opposite->num)) {
                        first_opposite = child;
                    }
                    if (child == current) {
                        found_current = true;
                    } else if (child->num == -1 && found_current) {
                        next = child;
                        return dynamic_cast<WorkspaceCon *>(next);
                    }
                }
            }
        }
    } else {
        /* If currently a numbered workspace, find next numbered workspace. */
        for (auto &output : global.croot->nodes) {
            for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
                for (auto &c : output->output_get_content()->nodes) {
                    child = c;
                    if (child->type != CT_WORKSPACE) {
                        continue;
                    }
                    if (!first || (child->num != -1 && child->num < first->num)) {
                        first = child;
                    }
                    if (!first_opposite && child->num == -1) {
                        first_opposite = child;
                    }
                    if (child->num == -1) {
                        break;
                    }
                    /* Need to check child against current and next because we are
                     * traversing multiple lists and thus are not guaranteed the
                     * relative order between the list of workspaces. */
                    if (current->num < child->num && (!next || child->num < next->num)) {
                        next = child;
                    }
                }
            }
        }
    }

    if (!next) {
        next = first_opposite ? first_opposite : first;
    }

    return dynamic_cast<WorkspaceCon*>(next);
}

/*
 * Focuses the previous workspace.
 *
 */
WorkspaceCon *workspace_prev() {
    WorkspaceCon *current = global.focused->con_get_workspace();
    Con *prev = nullptr, *first_opposite = nullptr, *last = nullptr;

    if (current->num == -1) {
        /* If named workspace, find previous named workspace. */
        auto c_itr = std::ranges::find(current->parent->nodes, current);
        prev = c_itr == current->parent->nodes.begin() ? nullptr : *(--c_itr);
        if (prev && prev->num != -1) {
            prev = nullptr;
        }
        if (!prev) {
            bool found_current = false;
            for (auto &output : global.croot->nodes | std::views::reverse) {
                for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
                    for (auto &c : output->output_get_content()->nodes | std::views::reverse) {
                        child = c;
                        if (child->type != CT_WORKSPACE) {
                            continue;
                        }
                        if (!last) {
                            last = child;
                        }
                        if (!first_opposite || (child->num != -1 && child->num > first_opposite->num)) {
                            first_opposite = child;
                        }
                        if (child == current) {
                            found_current = true;
                        } else if (child->num == -1 && found_current) {
                            prev = child;
                            return dynamic_cast<WorkspaceCon*>(prev);
                        }
                    }
                }
            }
        }
    } else {
        /* If numbered workspace, find previous numbered workspace. */
        for (auto &output : global.croot->nodes | std::views::reverse) {
            for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
                for (auto &c : output->output_get_content()->nodes | std::views::reverse) {
                    child = c;
                    if (child->type != CT_WORKSPACE) {
                        continue;
                    }
                    if (!last || (child->num != -1 && last->num < child->num)) {
                        last = child;
                    }
                    if (!first_opposite && child->num == -1) {
                        first_opposite = child;
                    }
                    if (child->num == -1) {
                        continue;
                    }
                    /* Need to check child against current and previous because we
                     * are traversing multiple lists and thus are not guaranteed
                     * the relative order between the list of workspaces. */
                    if (current->num > child->num && (!prev || child->num > prev->num)) {
                        prev = child;
                    }
                }
            }
        }
    }

    if (!prev) {
        prev = first_opposite ? first_opposite : last;
    }

    return dynamic_cast<WorkspaceCon*>(prev);
}

/*
 * Focuses the next workspace on the same output.
 *
 */
WorkspaceCon *workspace_next_on_output() {
    WorkspaceCon *current = global.focused->con_get_workspace();
    Con *next = nullptr;
    Con *output = global.focused->con_get_output();

    if (current->num == -1) {
        /* If currently a named workspace, find next named workspace. */
        next = con::next(current, current->parent->nodes);
    } else {
        /* If currently a numbered workspace, find next numbered workspace. */
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes) {
                child = c;
                if (child->type != CT_WORKSPACE) {
                    continue;
                }
                if (child->num == -1) {
                    break;
                }
                /* Need to check child against current and next because we are
                 * traversing multiple lists and thus are not guaranteed the
                 * relative order between the list of workspaces. */
                if (current->num < child->num && (!next || child->num < next->num)) {
                    next = child;
                }
            }
        }
    }

    /* Find next named workspace. */
    if (!next) {
        bool found_current = false;
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes) {
                child = c;
                if (child->type != CT_WORKSPACE) {
                    continue;
                }
                if (child == current) {
                    found_current = true;
                } else if (child->num == -1 && (current->num != -1 || found_current)) {
                    next = child;
                    goto workspace_next_on_output_end;
                }
            }
        }
    }

    /* Find first workspace. */
    if (!next) {
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes) {
                child = c;
                if (child->type != CT_WORKSPACE) {
                    continue;
                }
                if (!next || (child->num != -1 && child->num < next->num)) {
                    next = child;
                }
            }
        }
    }
workspace_next_on_output_end:
    return dynamic_cast<WorkspaceCon*>(next);
}

/*
 * Focuses the previous workspace on same output.
 *
 */
WorkspaceCon *workspace_prev_on_output() {
    WorkspaceCon *current = global.focused->con_get_workspace();
    Con *prev = nullptr;
    Con *output = global.focused->con_get_output();
    DLOG(fmt::sprintf("output = %s\n",  output->name));

    if (current->num == -1) {
        /* If named workspace, find previous named workspace. */
        prev = con::previous(current, current->parent->nodes);
        if (prev && prev->num != -1) {
            prev = nullptr;
        }
    } else {
        /* If numbered workspace, find previous numbered workspace. */
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes | std::views::reverse) {
                child = c;
                if (child->type != CT_WORKSPACE || child->num == -1) {
                    continue;
                }
                /* Need to check child against current and previous because we
                 * are traversing multiple lists and thus are not guaranteed
                 * the relative order between the list of workspaces. */
                if (current->num > child->num && (!prev || child->num > prev->num)) {
                    prev = child;
                }
            }
        }
    }

    /* Find previous named workspace. */
    if (!prev) {
        bool found_current = false;
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes | std::views::reverse) {
                child = c;
                if (child->type != CT_WORKSPACE) {
                    continue;
                }
                if (child == current) {
                    found_current = true;
                } else if (child->num == -1 && (current->num != -1 || found_current)) {
                    prev = child;
                    goto workspace_prev_on_output_end;
                }
            }
        }
    }

    /* Find last workspace. */
    if (!prev) {
        for (Con *child = (Con *)-1; (child == (Con *)-1) && ((child = 0), true);) {
            for (auto &c : output->output_get_content()->nodes | std::views::reverse) {
                child = c;
                if (child->type != CT_WORKSPACE) {
                    continue;
                }
                if (!prev || child->num > prev->num) {
                    prev = child;
                }
            }
        }
    }

workspace_prev_on_output_end:
    return dynamic_cast<WorkspaceCon*>(prev);
}

/*
 * Focuses the previously focused workspace.
 *
 */
void workspace_back_and_forth() {
    if (previous_workspace_name.empty()) {
        DLOG("No previous workspace name set. Not switching.\n");
        return;
    }

    workspace_show_by_name(previous_workspace_name.c_str());
}

/*
 * Returns the previously focused workspace con, or NULL if unavailable.
 *
 */
WorkspaceCon *workspace_back_and_forth_get() {
    if (previous_workspace_name.empty()) {
        DLOG("No previous workspace name set.\n");
        return nullptr;
    }

    return workspace_get(previous_workspace_name);
}

static bool get_urgency_flag(Con *con) {
    for (auto &child : con->nodes) {
        if (child->urgent || get_urgency_flag(child)) {
            return true;
        }
    }

    auto workspace = dynamic_cast<WorkspaceCon*>(con);

    if (workspace == nullptr) {
        return false;
    }

    return std::ranges::any_of(dynamic_cast<WorkspaceCon*>(con)->floating_windows, [](auto &child) { return (child->urgent || get_urgency_flag(child)); });
}

/*
 * Goes through all clients on the given workspace and updates the workspace’s
 * urgent flag accordingly.
 *
 */
void workspace_update_urgent_flag(Con *ws) {
    bool old_flag = ws->urgent;
    ws->urgent = get_urgency_flag(ws);
    DLOG(fmt::sprintf("Workspace urgency flag changed from %d to %d\n",  old_flag, ws->urgent));

    if (old_flag != ws->urgent) {
        ipc_send_workspace_event("urgent", ws, nullptr);
    }
}

/*
 * 'Forces' workspace orientation by moving all cons into a new split-con with
 * the same layout as the workspace and then changing the workspace layout.
 *
 */
void ws_force_orientation(Con *ws, orientation_t orientation) {
    /* 1: create a new split container */
    Con *split = new ConCon();
    split->parent = ws;

    /* 2: copy layout from workspace */
    split->layout = ws->layout;

    /* 3: move the existing cons of this workspace below the new con */
    auto focus_order = ws->get_focus_order();

    DLOG("Moving cons\n");
    while (!ws->nodes.empty()) {
        Con *child = con::first(ws->nodes);
        child->con_detach();
        child->con_attach(split, true);
    }

    split->set_focus_order(focus_order);
    //free(focus_order); TODO: do we need to free?

    /* 4: switch workspace layout */
    ws->layout = (orientation == HORIZ) ? L_SPLITH : L_SPLITV;
    DLOG(fmt::sprintf("split->layout = %d, ws->layout = %d\n",  split->layout, ws->layout));

    /* 5: attach the new split container to the workspace */
    DLOG(fmt::sprintf("Attaching new split (%p) to ws (%p)\n", fmt::ptr(split), fmt::ptr(ws)));
    split->con_attach(ws, false);

    /* 6: fix the percentages */
    ws->con_fix_percent();
}

/*
 * Called when a new con (with a window, not an empty or split con) should be
 * attached to the workspace (for example when managing a new window or when
 * moving an existing window to the workspace level).
 *
 * Depending on the workspace_layout setting, this function either returns the
 * workspace itself (default layout) or creates a new stacked/tabbed con and
 * returns that.
 *
 */
Con *workspace_attach_to(WorkspaceCon *ws) {
    DLOG(fmt::sprintf("Attaching a window to workspace %p / %s\n", fmt::ptr(ws), ws->name));

    if (ws->workspace_layout == L_DEFAULT) {
        DLOG("Default layout, just attaching it to the workspace itself.\n");
        return ws;
    }

    DLOG("Non-default layout, creating a new split container\n");
    /* 1: create a new split container */
    Con *new_con = new ConCon();
    new_con->parent = ws;

    /* 2: set the requested layout on the split con */
    new_con->layout = ws->workspace_layout;

    /* 4: attach the new split container to the workspace */
    DLOG(fmt::sprintf("Attaching new split %p to workspace %p\n", fmt::ptr(new_con), fmt::ptr(ws)));
    new_con->con_attach(ws, false);

    /* 5: fix the percentages */
    ws->con_fix_percent();

    return new_con;
}

/*
 * Creates a new container and re-parents all of children from the given
 * workspace into it.
 *
 * The container inherits the layout from the workspace.
 */
Con *workspace_encapsulate(Con *ws) {
    if (ws->nodes.empty()) {
        ELOG(fmt::sprintf("Workspace %p / %s has no children to encapsulate\n", fmt::ptr(ws), ws->name));
        return nullptr;
    }

    Con *new_con = new ConCon();
    new_con->parent = ws;
    new_con->layout = ws->layout;

    auto focus_order = ws->get_focus_order();

    DLOG(fmt::sprintf("Moving children of workspace %p / %s into container %p\n", fmt::ptr(ws), ws->name, fmt::ptr(new_con)));
    Con *child;
    while (!ws->nodes.empty()) {
        child = con::first(ws->nodes);
        child->con_detach();
        child->con_attach(new_con, true);
    }

    new_con->set_focus_order(focus_order);
    //free(focus_order); TODO: do we need to free?

    new_con->con_attach(ws, true);

    return new_con;
}

/*
 * Move the given workspace to the specified output.
 */
void workspace_move_to_output(WorkspaceCon *ws, Output *output) {
     DLOG(fmt::sprintf("Moving workspace %p / %s to output %p / \"%s\".\n", fmt::ptr(ws), ws->name, fmt::ptr(output), output->output_primary_name()));

    Output *current_output = get_output_for_con(ws);
    Con *content = output->con->output_get_content();
    DLOG(fmt::sprintf("got output %p with content %p\n", fmt::ptr(output), fmt::ptr(content)));

    if (ws->parent == content) {
        DLOG("Nothing to do, workspace already there\n");
        return;
    }

    Con *previously_visible_ws = con::first(content->focused);
    if (previously_visible_ws) {
        DLOG(fmt::sprintf("Previously visible workspace = %p / %s\n", fmt::ptr(previously_visible_ws), previously_visible_ws->name));
    } else {
        DLOG("No previously visible workspace on output.\n");
    }

    bool workspace_was_visible = workspace_is_visible(ws);
    if (ws->parent->con_num_children() == 1) {
         DLOG(fmt::sprintf("Creating a new workspace to replace \"%s\" (last on its output).\n", ws->name));

        /* check if we can find a workspace assigned to this output */
        bool used_assignment = false;
        for (const auto &assignment : global.ws_assignments) {
            if (!output_triggers_assignment(current_output, assignment.get())) {
                continue;
            }
            /* check if this workspace's name or num is already attached to the tree */
            const int num = ws_name_to_number(assignment->name);
            const bool attached = (num == -1)
                                      ? get_existing_workspace_by_name(assignment->name)
                                      : get_existing_workspace_by_num(num);
            if (attached) {
                continue;
            }

            /* so create the workspace referenced to by this assignment */
            DLOG(fmt::sprintf("Creating workspace from assignment %s.\n",  assignment->name));
            workspace_get(assignment->name);
            used_assignment = true;
            break;
        }

        /* if we couldn't create the workspace using an assignment, create it on
         * the output. Workspace init IPC events are sent either by
         * workspace_get or create_workspace_on_output. */
        if (!used_assignment) {
            create_workspace_on_output(current_output, ws->parent);
        }
    }
    DLOG("Detaching\n");

    /* detach from the old output and attach to the new output */
    Con *old_content = ws->parent;
    ws->con_detach();
    if (workspace_was_visible) {
        /* The workspace which we just detached was visible, so focus the next
         * one in the focus-stack. */
        Con *focus_ws = con::first(old_content->focused);
        DLOG(fmt::sprintf("workspace was visible, focusing %p / %s now\n", fmt::ptr(focus_ws), focus_ws->name));
        workspace_show(focus_ws);
    }
    ws->con_attach(content, false);

    /* fix the coordinates of the floating containers */
    for (auto &floating_con : ws->floating_windows) {
        floating_fix_coordinates(floating_con, old_content->rect, content->rect);
    }

    ipc_send_workspace_event("move", ws, nullptr);
    if (workspace_was_visible) {
        /* Focus the moved workspace on the destination output. */
        workspace_show(ws);
    }

    ewmh_update_desktop_properties();

    if (!previously_visible_ws) {
        return;
    }

    /* NB: We cannot simply work with previously_visible_ws since it might have
     * been cleaned up by workspace_show() already, depending on the focus
     * order/number of other workspaces on the output. Instead, we loop through
     * the available workspaces and only work with previously_visible_ws if we
     * still find it. */
    for (auto &ws : content->nodes) {
        if (ws != previously_visible_ws) {
            continue;
        }

        /* Call the on_remove_child callback of the workspace which previously
         * was visible on the destination output. Since it is no longer visible,
         * it might need to get cleaned up. */
        previously_visible_ws->on_remove_child();
        break;
    }
}
