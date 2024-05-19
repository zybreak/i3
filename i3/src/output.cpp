/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * output.c: Output (monitor) related functions.
 *
 */
module;
#include <cassert>
module i3;

import std;

/*
 * Returns an 'output' corresponding to one of left/right/down/up or a specific
 * output name.
 *
 */
Output* Output::get_output_from_string(const std::string &output_str) {
    if (output_str == "current") {
        return get_output_for_con(global.focused);
    } else if (output_str == "left") {
        return get_output_next_wrap(D_LEFT, this);
    } else if (output_str == "right") {
        return get_output_next_wrap(D_RIGHT, this);
    } else if (output_str == "up") {
        return get_output_next_wrap(D_UP, this);
    } else if (output_str == "down") {
        return get_output_next_wrap(D_DOWN, this);
    }

    return global.randr->get_output_by_name(output_str, true);
}

/*
 * Retrieves the primary name of an output.
 *
 */
std::string Output::output_primary_name() const {
    return this->names.front();
}

Output *get_output_for_con(Con *con) {
    OutputCon *output_con = con->con_get_output();
    Output *output = global.randr->get_output_by_name(output_con->name, true);
    assert(output != nullptr);

    return output;
}

/*
 * Iterates over all outputs and pushes sticky windows to the currently visible
 * workspace on that output.
 *
 * old_focus is used to determine if a sticky window is going to be focused.
 * old_focus might be different than the currently focused container because the
 * caller might need to temporarily change the focus and then call
 * output_push_sticky_windows. For example, workspace_show needs to set focus to
 * one of its descendants first, then call output_push_sticky_windows that
 * should focus a sticky window if it was the focused in the previous workspace.
 *
 */
void output_push_sticky_windows(Con *old_focus) {
    for (auto &output : global.croot->focused) {
        auto visible_ws = std::ranges::find_if(output->output_get_content()->nodes, [](auto &child) { return workspace_is_visible(child); });

        /* We use this loop instead of TAILQ_FOREACH to avoid problems if the
         * sticky window was the last window on that workspace as moving it in
         * this case will close the workspace. */
        for (auto &current_ws : output->output_get_content()->focused) {
            /* Since moving the windows actually removes them from the list of
             * floating windows on this workspace, here too we need to use
             * another loop than TAILQ_FOREACH. */
            for (auto &current : current_ws->focused) {
                if (current->type != CT_FLOATING_CON || !current->con_is_sticky()) {
                    continue;
                }

                bool ignore_focus = (old_focus == nullptr) || (current != old_focus->parent);
                con_move_to_workspace(current, dynamic_cast<WorkspaceCon*>(*visible_ws), true, false, ignore_focus);
                if (!ignore_focus) {
                    Con *current_ws = global.focused->con_get_workspace();
                    con_descend_focused(current)->con_activate();
                    /* Pushing sticky windows shouldn't change the focused workspace. */
                    con_descend_focused(current_ws)->con_activate();
                }
            }
        }
    }
}
