/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * include/configuration.h: Contains all structs/variables for the configurable
 * part of i3 as well as functions handling the configuration file (calling
 * the parser (src/config_parse.c) with the correct path, switching key
 * bindings mode).
 *
 */
export module i3:configuration;

import std;
import :draw;
import :bindings;
import :con;
import :font;
import :internal;
import rect;
import utils;

import i3_config_base;

using std::uint32_t;
using std::int32_t;

export {


    /**
     * Mouse pointer warping modes.
     */
    enum warping_t {
        POINTER_WARPING_OUTPUT = 0,
        POINTER_WARPING_NONE = 1
    };

    /**
     * Focus wrapping modes.
     */
    enum focus_wrapping_t {
        FOCUS_WRAPPING_OFF = 0,
        FOCUS_WRAPPING_ON = 1,
        FOCUS_WRAPPING_FORCE = 2,
        FOCUS_WRAPPING_WORKSPACE = 3
    };

    /**
     * The configuration file can contain multiple sets of bindings. Apart from the
     * default set (name == "default"), you can specify other sets and change the
     * currently active set of bindings by using the "mode <name>" command.
     *
     */
    struct Mode {
        std::string name{};
        bool pango_markup{};
        std::vector<std::unique_ptr<Binding>> bindings{};

        explicit Mode(std::string s)
            : name(std::move(s)) {
        }

        Mode(std::string s, const bool pango_markup)
            : name(std::move(s)), pango_markup(pango_markup) {
        }

        ~Mode() = default;
    };

    enum conf_fowa_t {
        /* Focus if the target workspace is visible, set urgency hint otherwise. */
        FOWA_SMART,
        /* Always set the urgency hint. */
        FOWA_URGENT,
        /* Always focus the window. */
        FOWA_FOCUS,
        /* Ignore the request (no focus, no urgency hint). */
        FOWA_NONE
    };

    enum conf_pdf_t {
        /* display (and focus) the popup when it belongs to the fullscreen
         * window only. */
        PDF_SMART = 0,

        /* leave fullscreen mode unconditionally */
        PDF_LEAVE_FULLSCREEN = 1,

        /* just ignore the popup, that is, don’t map it */
        PDF_IGNORE = 2,
    };

    enum smart_borders_t {
        SMART_BORDERS_OFF,
        SMART_BORDERS_ON,
        SMART_BORDERS_NO_GAPS
    };

    enum smart_gaps_t {
        SMART_GAPS_OFF,
        SMART_GAPS_ON,
        SMART_GAPS_INVERSE_OUTER
    };

    enum hide_edge_borders_mode_t : unsigned int {
        HEBM_NONE = adjacent_t::ADJ_NONE,
        HEBM_VERTICAL = adjacent_t::ADJ_LEFT_SCREEN_EDGE | adjacent_t::ADJ_RIGHT_SCREEN_EDGE,
        HEBM_HORIZONTAL = ADJ_UPPER_SCREEN_EDGE | adjacent_t::ADJ_LOWER_SCREEN_EDGE,
        HEBM_BOTH = HEBM_VERTICAL | HEBM_HORIZONTAL,
        HEBM_SMART = (1 << 5),
        HEBM_SMART_NO_GAPS = (1 << 6)
    };
    
    enum title_align_t {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };

    /**
     * Holds part of the configuration
     *
     */
    struct Config {

        /* NULL-terminated list of workspace names (in order) extracted from
         * keybindings. */
        std::vector<std::string> binding_workspace_names{};

        const char *terminal;
        i3Font *font;

        std::optional<std::string> ipc_socket_path = std::nullopt;
        char *restart_state_path;

        layout_t default_layout;
        int container_stack_limit;
        int container_stack_limit_value;
        int default_border_width;
        int default_floating_border_width;

        /** Default orientation for new containers */
        int default_orientation;

        /** By default, focus follows mouse. If the user explicitly wants to
         * turn this off (and instead rely only on the keyboard for changing
         * focus), we allow them to do this with this relatively special option.
         * It is not planned to add any different focus models. */
        bool disable_focus_follows_mouse;

        /** By default, when switching focus to a window on a different output
         * (e.g. focusing a window on workspace 3 on output VGA-1, coming from
         * workspace 2 on LVDS-1), the mouse cursor is warped to the center of
         * that window.
         *
         * With the mouse_warping option, you can control when the mouse cursor
         * should be warped. "none" disables warping entirely, whereas "output"
         * is the default behavior described above. */
        warping_t mouse_warping;

        /** Remove borders if they are adjacent to the screen edge.
         * This is useful if you are reaching scrollbar on the edge of the
         * screen or do not want to waste a single pixel of displayspace.
         * By default, this is disabled. */
        hide_edge_borders_mode_t hide_edge_borders;

        /** By default, a workspace bar is drawn at the bottom of the screen.
         * If you want to have a more fancy bar, it is recommended to replace
         * the whole bar by dzen2, for example using the i3-wsbar script which
         * comes with i3. Thus, you can turn it off entirely. */
        bool disable_workspace_bar;

        /** When focus wrapping is enabled (the default), attempting to
         * move focus past the edge of the screen (in other words, in a
         * direction in which there are no more containers to focus) will
         * cause the focus to wrap to the opposite edge of the current
         * container. When it is disabled, nothing happens; the current
         * focus is preserved.
         *
         * Additionally, focus wrapping may be forced. Think of the
         * following layout: Horizontal workspace with a tabbed con on the
         * left of the screen and a terminal on the right of the
         * screen. You are in the second container in the tabbed container
         * and focus to the right. By default, i3 will set focus to the
         * terminal on the right. If you are in the first container in the
         * tabbed container however, focusing to the left will
         * wrap. Setting focus_wrapping to FOCUS_WRAPPING_FORCE forces i3
         * to always wrap, which will result in you having to use "focus
         * parent" more often. */
        focus_wrapping_t focus_wrapping;

        /** Automatic workspace back and forth switching. If this is set, a
         * switch to the currently active workspace will switch to the
         * previously focused one instead, making it possible to fast toggle
         * between two workspaces. */
        bool workspace_auto_back_and_forth;

        /** By default, urgency is cleared immediately when switching to another
         * workspace leads to focusing the con with the urgency hint. When having
         * multiple windows on that workspace, the user needs to guess which
         * application raised the event. To prevent this, the reset of the urgency
         * flag can be delayed using an urgency timer. */
        float workspace_urgency_timer;

        /** Behavior when a window sends a NET_ACTIVE_WINDOW message. */
        enum conf_fowa_t focus_on_window_activation;

        /** Title alignment options. */
        title_align_t title_align;

        /** The default border style for new windows. */
        border_style_t default_border;

        /** The default border style for new floating windows. */
        border_style_t default_floating_border;

        /** The modifier which needs to be pressed in combination with your mouse
         * buttons to do things with floating windows (move, resize) */
        uint32_t floating_modifier;

        /** Maximum and minimum dimensions of a floating window */
        int32_t floating_maximum_width;
        int32_t floating_maximum_height;
        int32_t floating_minimum_width;
        int32_t floating_minimum_height;

        /* Color codes are stored here */
        struct config_client {
            color_t background;
            Colortriple focused;
            Colortriple focused_inactive;
            Colortriple focused_tab_title;
            Colortriple unfocused;
            Colortriple urgent;
            Colortriple placeholder;
            bool got_focused_tab_title;
        } client;
        struct config_bar {
            Colortriple focused;
            Colortriple unfocused;
            Colortriple urgent;
        } bar;

        /** What should happen when a new popup is opened during fullscreen mode */
        enum conf_pdf_t popup_during_fullscreen;

        /* The number of currently parsed barconfigs */
        int number_barconfigs;

        tiling_drag_t tiling_drag;

        /* Gap sizes */
        gaps_t gaps;

        /* Should single containers on a workspace receive a border? */
        smart_borders_t smart_borders;

        /* Disable gaps if there is only one container on the workspace */
        smart_gaps_t smart_gaps;
    };

    enum config_hidden_state_t {
        S_HIDE = 0,
        S_SHOW = 1
    };

    enum config_mode_t {
        M_DOCK = 0,
        M_HIDE = 1,
        M_INVISIBLE = 2
    };

    enum config_bar_position_t { P_BOTTOM = 0,
                                 P_TOP = 1 };

    /**
     * Defines a mouse command to be executed instead of the default behavior when
     * clicking on the non-statusline part of i3bar.
     *
     */
    struct Barbinding {
        /** The button to be used (e.g., 1 for "button1"). */
        int input_code;

        /** The command which is to be executed for this button. */
        std::string command;

        /** If true, the command will be executed after the button is released. */
        bool release;
    };

    /**
     * Holds the status bar configuration (i3bar). One of these structures is
     * created for each 'bar' block in the config.
     *
     */
    struct Barconfig {
        /** Automatically generated ID for this bar config. Used by the bar process
         * to request a specific configuration. */
        char *id;

        /** Outputs on which this bar should show up on. We use an array for
         * simplicity (since we store just strings). */
        std::vector<std::string> outputs{};

        /** List of outputs on which the tray is allowed to be shown, in order.
         * The special value "none" disables it (per default, it will be shown) and
         * the special value "primary" enabled it on the primary output. */
        std::vector<std::string> tray_outputs{};

        /* Padding around the tray icons. */
        int tray_padding;

        /** Path to the i3 IPC socket. This option is discouraged since programs
         * can find out the path by looking for the I3_SOCKET_PATH property on the
         * root window! */
        char *socket_path;

        /** Bar display mode (hide unless modifier is pressed or show in dock mode or always hide in invisible mode) */
        enum config_mode_t mode;

        /* The current hidden_state of the bar, which indicates whether it is hidden or shown */
        enum config_hidden_state_t hidden_state;

        /** Bar modifier (to show bar when in hide mode). */
        uint32_t modifier;

        std::vector<std::unique_ptr<Barbinding>> bar_bindings{};

        /** Bar position (bottom by default). */
        enum config_bar_position_t position;

        /** Command that should be run to execute i3bar, give a full path if i3bar is not
         * in your $PATH.
         * By default just 'i3bar' is executed. */
        char *i3bar_command;

        /** Command that should be run to get a statusline, for example 'i3status'.
         * Will be passed to the shell. */
        char *status_command;

        /** Command that should be run to get the workspace buttons. Will be passed
         * to the shell. */
        char *workspace_command;

        /** i3Font specification for all text rendered on the bar. */
        char *font;

        /** A custom separator to use instead of a vertical line. */
        char *separator_symbol;

        /** Hide workspace buttons? Configuration option is 'workspace_buttons no'
         * but we invert the bool to get the correct default when initializing with
         * zero. */
        bool hide_workspace_buttons;

        /** The minimal width for workspace buttons. */
        int workspace_min_width;

        /** Strip workspace numbers? Configuration option is
         * 'strip_workspace_numbers yes'. */
        bool strip_workspace_numbers;

        /** Strip workspace name? Configuration option is
         * 'strip_workspace_name yes'. */
        bool strip_workspace_name;

        /** Hide mode button? Configuration option is 'binding_mode_indicator no'
         * but we invert the bool for the same reason as hide_workspace_buttons.*/
        bool hide_binding_mode_indicator;

        /** Enable verbose mode? Useful for debugging purposes. */
        bool verbose;

        /** Defines the height of the bar in pixels. */
        uint32_t bar_height;

        Rect padding;

        struct bar_colors {
            char *background;
            char *statusline;
            char *separator;

            char *focused_background;
            char *focused_statusline;
            char *focused_separator;

            char *focused_workspace_border;
            char *focused_workspace_bg;
            char *focused_workspace_text;

            char *active_workspace_border;
            char *active_workspace_bg;
            char *active_workspace_text;

            char *inactive_workspace_border;
            char *inactive_workspace_bg;
            char *inactive_workspace_text;

            char *urgent_workspace_border;
            char *urgent_workspace_bg;
            char *urgent_workspace_text;

            char *binding_mode_border;
            char *binding_mode_bg;
            char *binding_mode_text;
        } colors;

        ~Barconfig();
    };

    std::string current_configpath{};
    Config config;
    std::vector<std::unique_ptr<Mode>> modes{};
    std::vector<std::unique_ptr<Barconfig>> barconfigs{};
    std::vector<std::unique_ptr<IncludedFile>> included_files{};
    /* The list of key bindings */
    Mode *current_mode{};

    /**
     * (Re-)loads the configuration file (sets useful defaults before).
     *
     * If you specify override_configpath, only this path is used to look for a
     * configuration file.
     *
     * load_type specifies the type of loading: C_VALIDATE is used to only verify
     * the correctness of the config file (used with the flag -C). C_LOAD will load
     * the config for normal use and display errors in the nagbar. C_RELOAD will
     * also clear the previous config.
     */
    bool load_configuration(const std::string *override_configfile, config_load_t load_type);
}