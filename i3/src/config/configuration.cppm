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
        std::vector<Binding> bindings{};

        explicit Mode(std::string s) : name(std::move(s)) {}

        Mode(std::string s, const bool pango_markup) : name(std::move(s)), pango_markup(pango_markup) {}
        
        auto operator<=>(const Mode &) const = default;
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
        HEBM_NONE = 0,
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

    struct config_client {
        color_t background;
        Colortriple focused;
        Colortriple focused_inactive;
        Colortriple focused_tab_title;
        Colortriple unfocused;
        Colortriple urgent;
        Colortriple placeholder;
        bool got_focused_tab_title{false};
        
        auto operator<=>(const config_client &r) const = default;
    };

    struct config_bar {
        Colortriple focused;
        Colortriple unfocused;
        Colortriple urgent;
        
        auto operator<=>(const config_bar &r) const = default;
    };
    
    /**
     * Holds part of the configuration
     *
     */
    class Config {
    public:

        /* NULL-terminated list of workspace names (in order) extracted from
         * keybindings. */
        std::vector<std::string> binding_workspace_names{};

        std::unique_ptr<i3Font> font{};

        std::optional<std::string> ipc_socket_path{};
        std::optional<std::string> restart_state_path{};

        layout_t default_layout{layout_t::L_DEFAULT};
        int container_stack_limit;
        int container_stack_limit_value;
        int default_border_width;
        int default_floating_border_width;

        /** Default orientation for new containers */
        orientation_t default_orientation{orientation_t::NO_ORIENTATION};

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
        warping_t mouse_warping{warping_t::POINTER_WARPING_OUTPUT};

        /** Remove borders if they are adjacent to the screen edge.
         * This is useful if you are reaching scrollbar on the edge of the
         * screen or do not want to waste a single pixel of displayspace.
         * By default, this is disabled. */
        hide_edge_borders_mode_t hide_edge_borders{hide_edge_borders_mode_t::HEBM_NONE};

        /** By default, a workspace bar is drawn at the bottom of the screen.
         * If you want to have a more fancy bar, it is recommended to replace
         * the whole bar by dzen2, for example using the i3-wsbar script which
         * comes with i3. Thus, you can turn it off entirely. */
        bool disable_workspace_bar; // TODO: remove

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
        focus_wrapping_t focus_wrapping{focus_wrapping_t::FOCUS_WRAPPING_ON};

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
        float workspace_urgency_timer{0.5};

        /** Behavior when a window sends a NET_ACTIVE_WINDOW message. */
        conf_fowa_t focus_on_window_activation;

        /** Title alignment options. */
        title_align_t title_align;

        /** The default border style for new windows. */
        border_style_t default_border{border_style_t::BS_NORMAL};

        /** The default border style for new floating windows. */
        border_style_t default_floating_border{border_style_t::BS_NORMAL};

        /** The modifier which needs to be pressed in combination with your mouse
         * buttons to do things with floating windows (move, resize) */
        uint32_t floating_modifier;

        /** Maximum and minimum dimensions of a floating window */
        int32_t floating_maximum_width;
        int32_t floating_maximum_height;
        int32_t floating_minimum_width;
        int32_t floating_minimum_height;

        /* Color codes are stored here */
        config_client client{};
        
        config_bar bar; // TODO: remove

        /** What should happen when a new popup is opened during fullscreen mode */
        conf_pdf_t popup_during_fullscreen;

        tiling_drag_t tiling_drag{tiling_drag_t::TILING_DRAG_MODIFIER};

        /* Gap sizes */
        gaps_t gaps{};

        /* Should single containers on a workspace receive a border? */
        smart_borders_t smart_borders{smart_borders_t::SMART_BORDERS_OFF};

        /* Disable gaps if there is only one container on the workspace */
        smart_gaps_t smart_gaps{smart_gaps_t::SMART_GAPS_OFF};

        std::filesystem::path current_configpath{};
        std::map<std::string, Mode> modes{};
        std::vector<IncludedFile> included_files{};
        std::string _current_mode{"default"};
        
        Mode* current_mode() {
            return &modes.at(_current_mode);
        };
        
        Config();
    };
    
    class ConfigurationManager {
    public:
        std::unique_ptr<Config> config{};
        
        void set_config(std::unique_ptr<Config> _config);
    };

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
    std::unique_ptr<Config> load_configuration(const std::optional<std::filesystem::path> configfile = std::nullopt);
}