/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * output.c: Output (monitor) related functions.
 *
 */
module;
#include <xcb/randr.h>
export module i3:output;

import std;
import rect;

class Con;
class OutputCon;

export {
    /**
     * An Output is a physical output on your graphics driver. Outputs which
     * are currently in use have (output->active == true). Each output has a
     * position and a mode. An output usually corresponds to one connected
     * screen (except if you are running multiple screens in clone mode).
     *
     */
    class Output {
      public:
        /** Output id, so that we can requery the output directly later */
        xcb_randr_output_t id{};

        /** Whether the output is currently active (has a CRTC attached with a
         * valid mode) */
        bool active{};

        /** Internal flags, necessary for querying RandR screens (happens in
         * two stages) */
        bool changed{};
        bool to_be_disabled{};
        bool primary{};

        /** List of names for the output.
         * An output always has at least one name; the first name is
         * considered the primary one. */
        std::vector<std::string> names{};

        /** Pointer to the Con which represents this output */
        OutputCon *con{};

        /** x, y, width, height */
        Rect rect{};

        /**
         * Returns an 'output' corresponding to one of left/right/down/up or a specific
         * output name.
         *
         */
        Output *get_output_from_string(std::string const &output_str);

        /**
         * Retrieves the primary name of an output.
         *
         */
        std::string output_primary_name() const;
    };

    /**
     * Retrieves the output for a given container. Never returns NULL.
     * There is an assertion that _will_ fail if the container is inside an
     * internal workspace. Use con_is_internal() if needed before calling this
     * function.
     */
    Output *get_output_for_con(Con * con);

    /**
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
    void output_push_sticky_windows(Con * old_focus);
}
