export module i3:internal;

export {
    /**
     * Bitmask for matching XCB_XKB_GROUP_1 to XCB_XKB_GROUP_4.
     */
    enum i3_xkb_group_mask_t {
        I3_XKB_GROUP_MASK_ANY = 0,
        I3_XKB_GROUP_MASK_1 = (1 << 0),
        I3_XKB_GROUP_MASK_2 = (1 << 1),
        I3_XKB_GROUP_MASK_3 = (1 << 2),
        I3_XKB_GROUP_MASK_4 = (1 << 3)
    };

    enum xcursor_cursor_t {
        XCURSOR_CURSOR_POINTER = 0,
        XCURSOR_CURSOR_RESIZE_HORIZONTAL,
        XCURSOR_CURSOR_RESIZE_VERTICAL,
        XCURSOR_CURSOR_TOP_LEFT_CORNER,
        XCURSOR_CURSOR_TOP_RIGHT_CORNER,
        XCURSOR_CURSOR_BOTTOM_LEFT_CORNER,
        XCURSOR_CURSOR_BOTTOM_RIGHT_CORNER,
        XCURSOR_CURSOR_WATCH,
        XCURSOR_CURSOR_MOVE,
    };

    enum position_t {
        BEFORE,
        AFTER
    };

    enum con_scratchpad_t {
        /* Not a scratchpad window. */
        SCRATCHPAD_NONE = 0,

        /* Just moved to scratchpad, not resized by the user yet.
         * Window will be auto-centered and sized appropriately. */
        SCRATCHPAD_FRESH = 1,

        /* The user changed position/size of the scratchpad window. */
        SCRATCHPAD_CHANGED = 2
    };

    enum con_floating_t {
        FLOATING_AUTO_OFF = 0,
        FLOATING_USER_OFF = 1,
        FLOATING_AUTO_ON = 2,
        FLOATING_USER_ON = 3
    };

    /**
     * Container layouts. See Con::layout.
     */
    enum layout_t {
        L_DEFAULT = 0,
        L_STACKED = 1,
        L_TABBED = 2,
        L_DOCKAREA = 3,
        L_OUTPUT = 4,
        L_SPLITV = 5,
        L_SPLITH = 6
    };

    /** describes if the window is adjacent to the output (physical screen) edges. */
    enum adjacent_t : unsigned int { ADJ_NONE = 0,
                                     ADJ_LEFT_SCREEN_EDGE = (1 << 0),
                                     ADJ_RIGHT_SCREEN_EDGE = (1 << 1),
                                     ADJ_UPPER_SCREEN_EDGE = (1 << 2),
                                     ADJ_LOWER_SCREEN_EDGE = (1 << 4) };

    enum direction_t {
        D_LEFT,
        D_RIGHT,
        D_UP,
        D_DOWN
    };

    enum con_type_t {
        CT_ROOT = 0,
        CT_OUTPUT = 1,
        CT_CON = 2,
        CT_FLOATING_CON = 3,
        CT_WORKSPACE = 4,
        CT_DOCKAREA = 5
    };

    enum orientation_t {
        NO_ORIENTATION = 0,
        HORIZ,
        VERT
    };

    enum border_style_t : unsigned int { BS_NORMAL = 0,
                                         BS_NONE = 1,
                                         BS_PIXEL = 2 };

    /** Fullscreen modes. Used by Con.fullscreen_mode. */
    enum fullscreen_mode_t {
        CF_NONE = 0,
        CF_OUTPUT = 1,
        CF_GLOBAL = 2
    };

    /** parameter to specify whether tree_close_internal() and x_window_kill() should kill
     * only this specific window or the whole X11 client */
    enum kill_window_t {
        DONT_KILL_WINDOW = 0,
        KILL_WINDOW = 1,
        KILL_CLIENT = 2
    };
}