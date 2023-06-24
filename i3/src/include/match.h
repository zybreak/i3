/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * A "match" is a data structure which acts like a mask or expression to match
 * certain windows or not. For example, when using commands, you can specify a
 * command like this: [title="*Firefox*"] kill. The title member of the match
 * data structure will then be filled and i3 will check each window using
 * match_matches_window() to find the windows affected by this command.
 *
 */
#pragma once

#include <config.h>
#include <xcb/xcb.h>
#include "i3_regex.h"
#include "con.h"

class i3Window;

enum match_dock_t {
    M_DONTCHECK = -1,
    M_NODOCK = 0,
    M_DOCK_ANY = 1,
    M_DOCK_TOP = 2,
    M_DOCK_BOTTOM = 3
};

enum match_insert_t { M_HERE = 0,
    M_ASSIGN_WS,
    M_BELOW };

enum match_urgent_t {
    U_DONTCHECK = -1,
    U_LATEST = 0,
    U_OLDEST = 1
};

enum match_window_mode_t {
    WM_ANY = 0,
    WM_TILING_AUTO,
    WM_TILING_USER,
    WM_TILING,
    WM_FLOATING_AUTO,
    WM_FLOATING_USER,
    WM_FLOATING
};

/**
 * A "match" is a data structure which acts like a mask or expression to match
 * certain windows or not. For example, when using commands, you can specify a
 * command like this: [title="*Firefox*"] kill. The title member of the match
 * data structure will then be filled and i3 will check each window using
 * match_matches_window() to find the windows affected by this command.
 *
 */
class Match {
public:
    /* Set if a criterion was specified incorrectly. */
    char *error{nullptr};

    Regex *title{nullptr};
    Regex *application{nullptr};
    Regex *window_class{nullptr};
    Regex *instance{nullptr};
    Regex *window_role{nullptr};
    Regex *workspace{nullptr};
    Regex *machine{nullptr};
    xcb_atom_t window_type{UINT32_MAX};
    match_urgent_t urgent{U_DONTCHECK};
    enum match_dock_t dock{M_NODOCK};
    xcb_window_t id{0};
    match_window_mode_t window_mode{WM_ANY};
    Con *con_id{nullptr};
    bool match_all_windows{false};

    /* Where the window looking for a match should be inserted:
     *
     * M_HERE   = the matched container will be replaced by the window
     *            (layout saving)
     * M_ASSIGN_WS = the matched container will be inserted in the target_ws.
     * M_BELOW  = the window will be inserted as a child of the matched container
     *            (dockareas)
     *
     */
    enum match_insert_t insert_where{M_HERE};

    /* Whether this match was generated when restarting i3 inplace.
     * Leads to not setting focus when managing a new window, because the old
     * focus stack should be restored. */
    bool restart_mode{false};

    Match() = default;
    Match(const Match &src);
    Match(const Match &&src);
    Match& operator=(Match &&src) noexcept;
    ~Match();
    /**
     * Interprets a ctype=cvalue pair and adds it to the given match specification.
     *
     */
    void parse_property(const char *ctype, const char *cvalue);

};

/**
 * Check if a match is empty. This is necessary while parsing commands to see
 * whether the user specified a match at all.
 *
 */
bool match_is_empty(Match &match);

/**
 * Check if a match data structure matches the given window.
 *
 */
bool match_matches_window(Match &match, i3Window *window);

