/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * load_layout.c: Restore (parts of) the layout, for example after an inplace
 *                restart.
 *
 */
export module i3:load_layout;

import std;

class Con;

export {
    enum json_content_t {
        // We could not determine the content of the JSON file. This typically
        // means it’s unreadable or contains garbage.
        JSON_CONTENT_UNKNOWN = 0,

        // The JSON file contains a “normal” container, i.e. a container to be
        // appended to an existing workspace (or split container!).
        JSON_CONTENT_CON = 1,

        // The JSON file contains a workspace container, which needs to be appended
        // to the output (next to the other workspaces) with special care to avoid
        // naming conflicts and ensuring that the workspace _has_ a name.
        JSON_CONTENT_WORKSPACE = 2,
    };

    /* Parses the given JSON file until it encounters the first “type” property to
     * determine whether the file contains workspaces or regular containers, which
     * is important to know when deciding where (and how) to append the contents.
     * */
    json_content_t json_determine_content(std::istream && fb);

    /**
     * Returns true if the provided JSON could be parsed by yajl.
     *
     */
    bool json_validate(std::istream && fb);

    void tree_append_json(Con * con, std::istream && fb);
}
