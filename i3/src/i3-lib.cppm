module;
#include <unistd.h>
#include <sys/types.h>
#include <xcb/randr.h>
#include <nlohmann/json.hpp>
#include <sys/resource.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>
#include <err.h>
#include <config.h>
#include <xcb/xcb.h>
#include <xcb/shape.h>

#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_aux.h>
#include <ev.h>
#include <fmt/printf.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xkbcommon/xkbcommon.h>
#include <cairo.h>
#include <xcb/xcb_icccm.h>
#define SN_API_NOT_YET_FROZEN 1
#include <libsn/sn-monitor.h>
#undef SN_API_NOT_YET_FROZEN
#include <pango/pango.h>
#include <cairo/cairo-xcb.h>
#include <pango/pangocairo.h>
#include <wordexp.h>
#include <xcb/xcb_xrm.h>
#include <cassert>
export module i3;

import i3_config_base;
import i3_commands_base;
import criteria_state;
import program_arguments;
import std;
import rect;
import utils;
import bindings;
import i3ipc;
import regex;
import log;
import xpp;
import xpp.proto.randr;
import xpp.proto.xkb;
import xpp.proto.shape;
import xpp.proto.big_requests;

using std::int32_t;
using std::uint32_t;

export class Assignment;
export class AssignmentManager;
export class Config;
export class ConfigurationManager;
export class Con;
export class X;
export class WorkspaceConfig;
export class WorkspaceManager;
export class EventHandler;
export class PropertyHandlers;
export class i3WindowIcon;
export class i3Window;
export class Match;
export class ApplicationLauncher;
export class Output;
export class RandR;
export class i3Font;
export class InputManager;
export class IPCManager;
export class TreeManager;

// internal.cppm
export {
    
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

    enum class border_style_t {
        BS_NONE = 0,
        BS_PIXEL = 1,
        BS_NORMAL = 2,
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

    /**
     * Tiling drag initiation modes.
     */
    enum tiling_drag_t {
        TILING_DRAG_OFF = 0,
        TILING_DRAG_MODIFIER = 1,
        TILING_DRAG_TITLEBAR = 2,
        TILING_DRAG_MODIFIER_OR_TITLEBAR = 3
    };

    struct gaps_t {
        int inner{0};
        int top{0};
        int right{0};
        int bottom{0};
        int left{0};

        auto operator<=>(gaps_t const&) const = default;
    };

    enum gaps_mask_t : int {
        GAPS_INNER = (1 << 0),
        GAPS_TOP = (1 << 1),
        GAPS_RIGHT = (1 << 2),
        GAPS_BOTTOM = (1 << 3),
        GAPS_LEFT = (1 << 4),
        GAPS_VERTICAL = (GAPS_TOP | GAPS_BOTTOM),
        GAPS_HORIZONTAL = (GAPS_RIGHT | GAPS_LEFT),
        GAPS_OUTER = (GAPS_VERTICAL | GAPS_HORIZONTAL),
    };

    /** Fullscreen modes. Used by Con.fullscreen_mode. */
    enum fullscreen_mode_t : int {
        CF_NONE = 0,
        CF_OUTPUT = 1,
        CF_GLOBAL = 2
    };

    /** parameter to specify whether tree_close_internal() and x_window_kill() should kill
     * only this specific window or the whole X11 client */
    enum class kill_window_t : int {
        DONT_KILL_WINDOW = 0,
        KILL_WINDOW = 1,
        KILL_CLIENT = 2
    };
}
// atoms.cppm
export namespace i3 {
    enum class Atom {
        _NET_SUPPORTED,
        _NET_SUPPORTING_WM_CHECK,
        _NET_WM_NAME,
        _NET_WM_VISIBLE_NAME,
        _NET_WM_MOVERESIZE,
        _NET_WM_STATE_STICKY,
        _NET_WM_STATE_FULLSCREEN,
        _NET_WM_STATE_DEMANDS_ATTENTION,
        _NET_WM_STATE_MODAL,
        _NET_WM_STATE_HIDDEN,
        _NET_WM_STATE_FOCUSED,
        _NET_WM_STATE_MAXIMIZED_VERT,
        _NET_WM_STATE_MAXIMIZED_HORZ,
        _NET_WM_STATE,
        _NET_WM_WINDOW_TYPE,
        _NET_WM_WINDOW_TYPE_NORMAL,
        _NET_WM_WINDOW_TYPE_DOCK,
        _NET_WM_WINDOW_TYPE_DIALOG,
        _NET_WM_WINDOW_TYPE_UTILITY,
        _NET_WM_WINDOW_TYPE_TOOLBAR,
        _NET_WM_WINDOW_TYPE_SPLASH,
        _NET_WM_WINDOW_TYPE_MENU,
        _NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        _NET_WM_WINDOW_TYPE_POPUP_MENU,
        _NET_WM_WINDOW_TYPE_TOOLTIP,
        _NET_WM_WINDOW_TYPE_NOTIFICATION,
        _NET_WM_DESKTOP,
        _NET_WM_STRUT_PARTIAL,
        _NET_CLIENT_LIST,
        _NET_CLIENT_LIST_STACKING,
        _NET_CURRENT_DESKTOP,
        _NET_NUMBER_OF_DESKTOPS,
        _NET_DESKTOP_NAMES,
        _NET_DESKTOP_VIEWPORT,
        _NET_ACTIVE_WINDOW,
        _NET_CLOSE_WINDOW,
        _NET_MOVERESIZE_WINDOW,
        _NET_WM_USER_TIME,
        _NET_STARTUP_ID,
        _NET_WORKAREA,
        _NET_WM_ICON,
        WM_PROTOCOLS,
        WM_DELETE_WINDOW,
        UTF8_STRING,
        WM_STATE,
        WM_CLIENT_LEADER,
        WM_TAKE_FOCUS,
        WM_WINDOW_ROLE,
        I3_SOCKET_PATH,
        I3_CONFIG_PATH,
        I3_SYNC,
        I3_PID,
        I3_LOG_STREAM_SOCKET_PATH,
        I3_FLOATING_WINDOW,
        _NET_REQUEST_FRAME_EXTENTS,
        _NET_FRAME_EXTENTS,
        _MOTIF_WM_HINTS,
        WM_CHANGE_STATE,
        MANAGER
    };
    
    std::map<Atom, xcb_atom_t> atoms{};
}  // namespace i3

/* Setup NetWM atoms */
export void setup_atoms(X &x);
// match.cppm
export {
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
        std::unique_ptr<Regex> title{};
        std::unique_ptr<Regex> application{};
        std::unique_ptr<Regex> window_class{};
        std::unique_ptr<Regex> instance{};
        std::unique_ptr<Regex> window_role{};
        std::unique_ptr<Regex> workspace{};
        std::unique_ptr<Regex> machine{};
        xcb_atom_t window_type{UINT32_MAX};
        match_urgent_t urgent{match_urgent_t::U_DONTCHECK};
        match_dock_t dock{match_dock_t::M_NODOCK};
        xcb_window_t id{0};
        match_window_mode_t window_mode{match_window_mode_t::WM_ANY};
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
        match_insert_t insert_where{match_insert_t::M_HERE};

        /* Whether this match was generated when restarting i3 inplace.
         * Leads to not setting focus when managing a new window, because the old
         * focus stack should be restored. */
        bool restart_mode{false};

        /**
         * Interprets a ctype=cvalue pair and adds it to the given match specification.
         *
         */
        void parse_property(std::string const ctype, std::string const cvalue);

        /**
         * Check if a match is empty. This is necessary while parsing commands to see
         * whether the user specified a match at all.
         *
         */
        bool match_is_empty();

        /**
         * Check if a match data structure matches the given window.
         *
         */
        bool match_matches_window(i3Window const *window) const;
    };
}
// assignments.cppm
export {
    enum class workspace_assignment_type {
        WORKSPACE,
        WORKSPACE_NUMBER,
    };
    
    /**
     * An Assignment makes specific windows go to a specific workspace/output or
     * run a command for that window. With this mechanism, the user can -- for
     * example -- assign their browser to workspace "www". Checking if a window is
     * assigned works by comparing the Match data structure with the window (see
     * match_matches_window()).
     *
     */
    class Assignment {
      protected:
        explicit Assignment(Match &&match)
            : match(std::move(match)) {
        }

      public:
        /** the criteria to check if a window matches */
        Match match;

        Assignment() = delete;

        virtual ~Assignment() {
        }
    };

    /**
     * A_COMMAND = run the specified command for the matching window
     */
    class CommandAssignment : public Assignment {
      public:
        std::string command{};
        CommandAssignment(Match &&match, std::string command)
            : Assignment(std::move(match)), command(command) {
        }
    };

    /**
     * A_TO_WORKSPACE = assign the matching window to the specified workspace
     */
    class WorkspaceAssignment : public Assignment {
      public:
        workspace_assignment_type type;
        /** destination workspace/command/output, depending on the type */
        std::string workspace{};
        WorkspaceAssignment(Match &&match)
            : Assignment(std::move(match)) {
        }
    };

    class OutputAssignment : public Assignment {
      public:
        std::string output{};
        OutputAssignment(Match &&match, std::string output)
            : Assignment(std::move(match)), output(output) {
        }
    };

    /**
     * A_NO_FOCUS = don't focus matched window when it is managed
     */
    class NoFocusAssignment : public Assignment {
      public:
        NoFocusAssignment(Match &&match)
            : Assignment(std::move(match)) {
        }
    };

    class AssignmentManager {
      private:
        /* The list of assignments */
        std::deque<std::unique_ptr<Assignment>> assignments{};

      public:
        void add(std::unique_ptr<Assignment> &&assignment) {
            assignments.push_back(std::move(assignment));
        }

        /**
         * Checks the list of assignments for the given window and runs all matching
         * ones (unless they have already been run for this specific window).
         *
         */
        void run_assignments(i3Window *window);

        void clear() {
            assignments.clear();
        }

        /**
         * Returns the first matching assignment for the given window.
         *
         */
        template <typename T>
        std::optional<std::reference_wrapper<T>> assignment_for(i3Window const *window) {
            auto assignment = std::ranges::find_if(assignments, [&](auto &a) {
                return ((dynamic_cast<T *>(a.get())) && (a->match.match_matches_window(window)));
            });

            if (assignment != assignments.end()) {
                DLOG("got a matching assignment\n");
                return std::reference_wrapper<T>(*static_cast<T *>(assignment->get()));
            }
            return std::nullopt;
        }
    };
}

// autostarts.cppm
/**
 * Holds a command specified by either an:
 * - exec-line
 * - exec_always-line
 * in the config (see src/config.c)
 *
 */
export struct Autostart {
    /** Command, like in command mode */
    std::string command;
    /** no_startup_id flag for start_application(). Determines whether a
     * startup notification context/ID should be created. */
    bool no_startup_id;
};

export void autostart(ApplicationLauncher &applicationLauncher);

export void autostart_add(std::unique_ptr<Autostart> autostart);

export void autostart_always_add(std::unique_ptr<Autostart> autostart);

export void autostart_always(ApplicationLauncher &applicationLauncher);
// font.cppm
void freeFontDesc(PangoFontDescription *desc);

export {
    /**
     * Data structure for cached font information:
     * - font id in X11 (load it once)
     * - font height (multiple calls needed to get it)
     *
     */
    class i3Font {
      private:
        xcb_visualtype_t *root_visual_type;
        xcb_connection_t *conn;
        xcb_screen_t *root_screen;

        double pango_font_red;
        double pango_font_green;
        double pango_font_blue;
        double pango_font_alpha;

      public:
        /** The height of the font, built from font_ascent + font_descent */
        int height{};

        /** The pattern/name used to load the font. */
        std::string pattern{};

        /** The pango font description */
        std::unique_ptr<PangoFontDescription, decltype(&freeFontDesc)> pango_desc{nullptr, &freeFontDesc};

        i3Font(xcb_connection_t *conn, xcb_screen_t *root_screen, std::string const pattern);

        /**
         * Predict the text width in pixels for the given text. Text must be
         * specified as an i3String.
         *
         */
        int predict_text_width(std::string const &text) const;

        /**
         * Draws text onto the specified X drawable (normally a pixmap) at the
         * specified coordinates (from the top left corner of the leftmost, uppermost
         * glyph) and using the provided gc.
         *
         * The given cairo surface must refer to the specified X drawable.
         *
         * Text must be specified as an i3String.
         *
         */
        void draw_text(std::string const &text, xcb_drawable_t drawable, xcb_gcontext_t gc,
                       cairo_surface_t *surface, int x, int y, int max_width) const;

        /**
         * Defines the colors to be used for the forthcoming draw_text calls.
         *
         */
        void set_font_colors(xcb_gcontext_t gc, color_t foreground, color_t background);
    };

    /**
     * Loads a font for usage, also getting its height. If fallback is true,
     * the fonts 'fixed' or '-misc-*' will be loaded instead of exiting. If any
     * font was previously loaded, it will be freed.
     *
     */
    std::unique_ptr<i3Font> load_font(xcb_connection_t * conn, xcb_screen_t * root_screen, std::string const pattern);
}
// configuration.cppm
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

        explicit Mode(std::string s)
            : name(std::move(s)) {
        }

        Mode(std::string s, bool const pango_markup)
            : name(std::move(s)), pango_markup(pango_markup) {
        }

        auto operator<=>(Mode const &) const = default;
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
        
        /* display all floating windows */
        PDF_ALL = 3,
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

    enum class title_align_t {
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

        auto operator<=>(config_client const &r) const = default;
    };

    struct config_bar {
        Colortriple focused;
        Colortriple unfocused;
        Colortriple urgent;

        auto operator<=>(config_bar const &r) const = default;
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
        bool disable_workspace_bar;  // TODO: remove

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
        title_align_t title_align{title_align_t::ALIGN_LEFT};

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

        config_bar bar;  // TODO: remove

        /** What should happen when a new popup is opened during fullscreen mode */
        conf_pdf_t popup_during_fullscreen;

        tiling_drag_t tiling_drag{tiling_drag_t::TILING_DRAG_MODIFIER};
        
        /** The modifier which needs to be pressed in combination with the floating
         * modifier and your mouse buttons to swap containers during tiling drag */
        uint32_t swap_modifier{XCB_KEY_BUT_MASK_SHIFT};

        /* Gap sizes */
        gaps_t gaps{};

        /* Disable gaps if there is only one container on the workspace */
        smart_gaps_t smart_gaps{smart_gaps_t::SMART_GAPS_OFF};

        std::filesystem::path current_configpath{};
        std::map<std::string, Mode> modes{};
        std::vector<IncludedFile> included_files{};
        std::string _current_mode{"default"};

        Mode *current_mode() {
            return &modes.at(_current_mode);
        };

        Config();
    };

    class ConfigurationManager {
      private:
        X &x;

        void init_color(Config &config);

      public:
        std::unique_ptr<Config> config{};

        void set_config(std::unique_ptr<Config> _config);

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
        std::unique_ptr<Config> load_configuration(std::optional<std::filesystem::path> const configfile = std::nullopt);

        explicit ConfigurationManager(X &x)
            : x(x) {};
    };
    
    /**
     * Reorders bindings by event_state_mask descendingly so that get_binding()
     * correctly matches more specific bindings before more generic bindings. Take
     * the following binding configuration as an example:
     *
     *   bindsym n nop lower-case n pressed
     *   bindsym Shift+n nop upper-case n pressed
     *
     * Without reordering, the first binding’s event_state_mask of 0x0 would match
     * the actual event_stat_mask of 0x1 and hence trigger instead of the second
     * keybinding.
     *
     */
    void reorder_bindings(Config * config);
    
    /**
     * Checks for duplicate key bindings (the same keycode or keysym is configured
     * more than once). If a duplicate binding is found, a message is printed to
     * stderr and the has_errors variable is set to true, which will start
     * i3-nagbar.
     *
     */
    bool has_duplicate_bindings(Config * config);

    /**
     * Adds a binding from config parameters given as strings and returns a
     * pointer to the binding structure. Returns NULL if the input code could not
     * be parsed.
     *
     */
    void configure_binding(Config * config, std::string const &bindtype, std::string const &modifiers, std::string const &input_code,
                           bool release, bool border, bool whole_window,
                           bool exclude_titlebar, std::string const &command, std::string const &modename,
                           bool pango_markup);

    /**
     * Switches the key bindings to the given mode, if the mode exists
     *
     */
    void switch_mode(std::string const new_mode);

}
// draw.cppm
export {
    /* A wrapper grouping an XCB drawable and both a graphics context
     * and the corresponding cairo objects representing it. */
    class surface_t {
      private:
        /* The connection to the X server. */
        xcb_connection_t *conn;
        
      public:
        /* The drawable which is being represented. */
        xcb_drawable_t id{};
        
        /* A classic XCB graphics context. */
        xcb_gcontext_t gc{};
        bool owns_gc;

        int width;
        int height;

        /* A cairo surface representing the drawable. */
        cairo_surface_t *surface{};

        /* The cairo object representing the drawable. In general,
         * this is what one should use for any drawing operation. */
        cairo_t *cr{};
        surface_t(xcb_connection_t *conn, xcb_drawable_t drawable,
                  xcb_visualtype_t *visual, int width, int height);
        ~surface_t();

        surface_t() = delete;
        surface_t(surface_t const &) = delete;
        void operator=(surface_t const &) = delete;
        surface_t(surface_t &&other) noexcept {
            auto tmp_owns_gc = this->owns_gc;
            auto tmp_cr = this->cr;
            auto tmp_gc = this->gc;
            auto tmp_surface = this->surface;

            this->conn = other.conn;
            this->id = other.id;
            this->gc = other.gc;
            this->owns_gc = other.owns_gc;
            this->width = other.width;
            this->height = other.height;
            this->surface = other.surface;
            this->cr = other.cr;

            other.surface = tmp_surface;
            other.cr = tmp_cr;
            other.owns_gc = tmp_owns_gc;
            other.gc = tmp_gc;
        }
        void operator=(surface_t &&other) noexcept {
            auto tmp_owns_gc = this->owns_gc;
            auto tmp_cr = this->cr;
            auto tmp_gc = this->gc;
            auto tmp_surface = this->surface;

            this->conn = other.conn;
            this->id = other.id;
            this->gc = other.gc;
            this->owns_gc = other.owns_gc;
            this->width = other.width;
            this->height = other.height;
            this->surface = other.surface;
            this->cr = other.cr;

            other.surface = tmp_surface;
            other.cr = tmp_cr;
            other.owns_gc = tmp_owns_gc;
            other.gc = tmp_gc;
        }

        /**
         * Resize the surface to the given size.
         *
         */
        surface_t *draw_util_surface_set_size(int width, int height);

        /**
         * Draw the given text using libi3.
         * This function also marks the surface dirty which is needed if other means of
         * drawing are used. This will be the case when using XCB to draw text.
         *
         */
        surface_t *draw_util_text(i3Font &font, std::string &text, color_t fg_color, color_t bg_color, int x, int y, int max_width);

        /**
         * Clears a surface with the given color.
         *
         */
        surface_t *draw_util_clear_surface(color_t color);

        /**
         * This function is a convenience wrapper and takes care of flushing the
         * surface as well as restoring the cairo state.
         *
         */
        surface_t *draw_util_image(cairo_surface_t *image, int x, int y, int width, int height);

        /**
         * Draws a filled rectangle.
         * This function is a convenience wrapper and takes care of flushing the
         * surface as well as restoring the cairo state.
         *
         */
        surface_t *draw_util_rectangle(color_t color, double x, double y, double w, double h);
    };

    /**
     * Parses the given color in hex format to an internal color representation.
     * Note that the input must begin with a hash sign, e.g., "#3fbc59".
     *
     */
    color_t draw_util_hex_to_color(xcb_connection_t * conn, xcb_screen_t * root_screen, char const *color);

    /**
     * Copies a surface onto another surface.
     *
     */
    void draw_util_copy_surface(surface_t * src, surface_t * dest, double src_x, double src_y,
                                double dest_x, double dest_y, double width, double height);
}
// con.cppm
export {
    /**
     * Stores a width/height pair, used as part of deco_render_params to check
     * whether the rects width/height have changed.
     *
     */
    struct width_height {
        uint32_t w;
        uint32_t h;
        
        auto operator<=>(width_height const &r) const = default;
    };

    /**
     * Stores the parameters for rendering a window decoration. This structure is
     * cached in every Con and no re-rendering will be done if the parameters have
     * not changed (only the pixmaps will be copied).
     *
     */
    struct deco_render_params {
        Colortriple *color;
        border_style_t border_style;
        width_height con_rect;
        width_height con_window_rect;
        Rect con_deco_rect;
        color_t background;
        layout_t parent_layout;
        bool con_is_leaf;

        auto operator<=>(deco_render_params const &r) const = default;
    };

    class WorkspaceCon;
    class FloatingCon;
    class OutputCon;
    class ConCon;

    /**
     * A 'Con' represents everything from the X11 root window down to a single X11 window.
     *
     * The following things are all stored in a 'Con', from very high level (the
     * biggest Cons) to very small (a single window):
     *
     * 1) X11 root window (as big as all your outputs combined)
     * 2) output (like LVDS1)
     * 3) content container, dockarea containers
     * 4) workspaces
     * 5) split containers
     * ... (you can arbitrarily nest split containers)
     * 6) X11 window containers
     *
     */
    class Con {
      private:
        /**
         * Make all parent containers urgent if con is urgent or clear the urgent flag
         * of all parent containers if there are no more urgent children left.
         *
         */
        virtual void con_update_parents_urgency();

        /* timer used for disabling urgency */
        ev_timer *urgency_timer{};
        std::function<void(Con *)> urgency_timer_cb{};

      public:
        bool mapped{};

        /* Should this container be marked urgent? This gets set when the window
         * inside this container (if any) sets the urgency hint, for example. */
        bool urgent{};

        /** This counter contains the number of UnmapNotify events for this
         * container (or, more precisely, for its ->frame) which should be ignored.
         * UnmapNotify events need to be ignored when they are caused by i3 itself,
         * for example when reparenting or when unmapping the window on a workspace
         * change. */
        uint8_t ignore_unmap{};

        /* The surface used for the frame window. */
        std::unique_ptr<surface_t> frame{};
        std::unique_ptr<surface_t> frame_buffer{};
        bool pixmap_recreated{};

        con_type_t type;

        Con *parent{};

        /* The position and size for this con. These coordinates are absolute. Note
         * that the rect of a container does not include the decoration. */
        Rect rect{};
        /* The position and size of the container's decoration. These coordinates
         * are relative to the container's parent's rect. */
        Rect deco_rect{};

        std::string name{};

        /** The format with which the window's name should be displayed. */
        std::string title_format{};

        /* a sticky-group is an identifier which bundles several containers to a
         * group. The contents are shared between all of them, that is they are
         * displayed on whichever of the containers is currently visible */
        std::string sticky_group{};

        double percent{};

        /* the x11 border pixel attribute */
        int border_width{};
        int current_border_width;

        /** Cache for the decoration rendering */
        std::unique_ptr<deco_render_params> deco_render_params{};

        std::deque<Con *> nodes{};
        std::deque<Con *> focused{};

        std::deque<std::unique_ptr<Match>> swallow{};

        fullscreen_mode_t fullscreen_mode;

        /* Whether this window should stick to the glass. This corresponds to
         * the _NET_WM_STATE_STICKY atom and will only be respected if the
         * window is floating. */
        bool sticky{};

        /* layout is the layout of this container: one of split[v|h], stacked or
         * tabbed. Special containers in the tree (above workspaces) have special
         * layouts like dockarea or output.
         *
         * last_split_layout is one of splitv or splith to support the old "layout
         * default" command which by now should be "layout splitv" or "layout
         * splith" explicitly.
         */
        layout_t layout, last_split_layout;

        border_style_t border_style;
        /* When the border style of a con changes because of motif hints, we don't
         * want to set more decoration that the user wants. The user's preference is determined by these:
         * 1. For new tiling windows, as set by `default_border`
         * 2. For new floating windows, as set by `default_floating_border`
         * 3. For all windows that the user runs the `border` command, whatever is
         * the result of that command for that window. */
        border_style_t max_user_border_style;

        /** floating? (= not in tiling layout) This cannot be simply a bool
         * because we want to keep track of whether the status was set by the
         * application (by setting _NET_WM_WINDOW_TYPE appropriately) or by the
         * user. The user’s choice overwrites automatic mode, of course. The
         * order of the values is important because we check with >=
         * FLOATING_AUTO_ON if a client is floating. */
        con_floating_t floating;

        /* The ID of this container before restarting. Necessary to correctly
         * interpret back-references in the JSON (such as the focus stack). */
        int old_id{};

        /* Depth of the container window */
        uint16_t depth;

        /* The colormap for this con if a custom one is used. */
        xcb_colormap_t colormap{};

        void stop_urgency_timer();
        void start_urgency_timer(float after, float repeat, std::function<void(Con *)> cb);

        /** callbacks */
        virtual void on_remove_child();

        /**
         * Sets input focus to the given container. Will be updated in X11 in the next
         * run of x_push_changes().
         *
         */
        void con_focus();

        /**
         * Sets input focus to the given container and raises it to the top.
         *
         */
        void con_activate();

        /**
         * Activates the container like in con_activate but removes fullscreen
         * restrictions and properly warps the pointer if needed.
         *
         */
        void con_activate_unblock();

        /**
         * Returns true when this node is a leaf node (has no children)
         *
         */
        bool con_is_leaf() const;

        /**
         * Returns true when this con is a leaf node with a managed X11 window (e.g.,
         * excluding dock containers)
         */
        virtual bool con_has_managed_window();

        /**
         * Returns true if a container should be considered split.
         *
         */
        bool con_is_split() const;

        /**
         * This will only return true for containers which have some parent with
         * a tabbed / stacked parent of which they are not the currently focused child.
         *
         */
        bool con_is_hidden();

        /**
         * Returns true if the container is maximized in the given orientation.
         *
         * If the container is floating or fullscreen, it is not considered maximized.
         * Otherwise, it is maximized if it doesn't share space with any other
         * container in the given orientation. For example, if a workspace contains
         * a single splitv container with three children, none of them are considered
         * vertically maximized, but they are all considered horizontally maximized.
         *
         * Passing "maximized" hints to the application can help it make the right
         * choices about how to draw its borders. See discussion in
         * https://github.com/i3/i3/pull/2380.
         *
         */
        bool con_is_maximized(orientation_t orientation);

        /**
         * Returns whether the container or any of its children is sticky.
         *
         */
        bool con_is_sticky();

        /**
         * Returns true if this node has regular or floating children.
         *
         */
        virtual bool con_has_children() const;

        /**
         * Returns true if this node accepts a window (if the node swallows windows,
         * it might already have swallowed enough and cannot hold any more).
         *
         */
        virtual bool con_accepts_window();

        /**
         * Gets the output container (first container with CT_OUTPUT in hierarchy) this
         * node is on.
         *
         */
        OutputCon *con_get_output() const;

        /**
         * Gets the workspace container this node is on.
         *
         */
        WorkspaceCon *con_get_workspace() const;

        /**
         * Returns the fullscreen node that covers the given workspace if it exists.
         * This is either a CF_GLOBAL fullscreen container anywhere or a CF_OUTPUT
         * fullscreen container in the workspace.
         *
         */
        Con *con_get_fullscreen_covering_ws();

        /**
         * Returns true if the node is floating.
         *
         */
        bool con_is_floating() const;

        /**
         * Returns true if the container is a docked container.
         *
         */
        bool con_is_docked() const;

        /**
         * Checks if the given container is either floating or inside some floating
         * container. It returns the FLOATING_CON container.
         *
         */
        virtual FloatingCon *con_inside_floating();

        /**
         * Checks if the given container is inside a focused container.
         *
         */
        bool con_inside_focused() const;

        /**
         * Returns the number of children of this container.
         *
         */
        unsigned long con_num_children() const;

        /**
         * Returns the number of visible non-floating children of this container.
         * For example, if the container contains a hsplit which has two children,
         * this will return 2 instead of 1.
         */
        int con_num_visible_children();

        /**
         * Count the number of windows (i.e., leaf containers).
         *
         */
        int con_num_windows();

        /**
         * Iterate over the container's focus stack and return an array with the
         * containers inside it, ordered from higher focus order to lowest.
         *
         */
        std::vector<Con *> get_focus_order();

        /**
         * Clear the container's focus stack and re-add it using the provided container
         * array. The function doesn't check if the provided array contains the same
         * containers with the previous focus stack but will not add floating containers
         * in the new focus stack if container is not a workspace.
         *
         */
        void set_focus_order(std::vector<Con *> focus_order);

        /**
         * Attaches the given container to the given parent. This happens when moving
         * a container or when inserting a new container at a specific place in the
         * tree.
         *
         * ignore_focus is to just insert the Con at the end (useful when creating a
         * new split container *around* some containers, that is, detaching and
         * attaching them in order without wanting to mess with the focus in between).
         *
         */
        virtual void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr);

        /**
         * Detaches the given container from its current parent
         *
         */
        virtual void con_detach();

        /**
         * Closes the given container.
         *
         */
        void con_close(kill_window_t kill_window);

        /**
         * Searches parents of the given 'con' until it reaches one with the specified
         * 'orientation'. Aborts when it comes across a floating_con.
         *
         */
        Con *con_parent_with_orientation(orientation_t orientation);

        /**
         * Returns the first fullscreen node below this node.
         *
         */
        Con *con_get_fullscreen_con(fullscreen_mode_t fullscreen_mode);

        /**
         * Checks if the container has the given parent as an actual parent.
         *
         */
        bool con_has_parent(Con *parent) const;

        void insert_before(Con *con);

        void insert_after(Con *con);

        /**
         * Returns true if the given container (still) exists.
         * This can be used, e.g., to make sure a container hasn't been closed in the meantime.
         *
         */
        bool exists();

        /**
         * Updates the percent attribute of the children of the given container. This
         * function needs to be called when a window is added or removed from a
         * container.
         *
         */
        void con_fix_percent();

        /**
         * This function changes the layout of a given container. Use it to handle
         * special cases like changing a whole workspace to stacked/tabbed (creates a
         * new split container before).
         *
         */
        virtual void con_set_layout(layout_t layout);

        /**
         * Set urgency flag to the container, all the parent containers and the workspace.
         *
         */
        void con_set_urgency(bool urgent);

        virtual i3Window *get_window() {
            return nullptr;
        }

        virtual Rect &get_geometry() {
            static Rect r{};
            return r;
        }

        virtual Rect &get_window_rect() {
            static Rect r{};
            return r;
        }

        virtual int get_window_icon_padding() {
            return -1;
        }

        Con() = delete;

        /**
         * Frees the specified container.
         *
         */
        virtual ~Con();

      protected:
        /**
         * Create a new container (and attach it to the given parent, if not NULL).
         * This function only initializes the data structures.
         *
         */
        explicit Con(bool skeleton);
    };

    class RootCon : public Con {
      public:
        RootCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_ROOT;
            this->name = "root";
        }

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of root");
        }

        void on_remove_child() override;
    };

    class OutputCon : public Con {
      public:
        OutputCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_OUTPUT;
            this->layout = L_OUTPUT;
        }

        /**
         * Returns the output container below the given output container.
         *
         */
        ConCon *output_get_content();

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of output");
        }

        void on_remove_child() override;
    };

    class ConCon : public Con {
      private:
        /** The position and size of the actual client window. These coordinates are
         * relative to the container's rect. */
        Rect window_rect{};
        /** the geometry this window requested when getting mapped */
        Rect geometry{};
        /** Whether the window icon should be displayed, and with what padding. -1
         * means display no window icon (default behavior), 0 means display without
         * any padding, 1 means display with 1 pixel of padding and so on. */
        int window_icon_padding{-1};
        std::unique_ptr<i3Window> window{};

      public:
        bool con_has_managed_window() override;
        bool con_accepts_window() override;
        int get_window_icon_padding() override;
        void set_window_icon_padding(int padding);
        i3Window *get_window() override;
        i3Window *release_window();
        void set_window(i3Window *_window);
        void set_geometry(Rect _geometry);
        Rect &get_geometry() override;
        void set_window_rect(Rect _window_rect);
        Rect &get_window_rect() override;

        ConCon();
        explicit ConCon(i3Window *window);
        ConCon(i3Window *window, bool skeleton);
    };

    /* Wrap a floating ConCon in a FloatingCon. */
    class FloatingCon : public Con {
      public:
        FloatingCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_FLOATING_CON;
        }
        void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr) override;
        void con_detach() override;
        FloatingCon *con_inside_floating() override;
    };

    class DockCon : public Con {
      public:
        DockCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_DOCKAREA;
            this->layout = L_DOCKAREA;
        }

        FloatingCon *con_inside_floating() override;

        void con_set_layout(layout_t layout) override {
            throw std::domain_error("Cannot change layout of dockarea");
        }

        void on_remove_child() override;
    };

    class WorkspaceCon : public Con {
      private:
        void con_update_parents_urgency() override;

      public:
        /* Only workspace-containers can have floating clients */
        std::deque<FloatingCon *> floating_windows{};
        /* workspace_layout is only for type == CT_WORKSPACE cons. When you change
         * the layout of a workspace without any children, i3 cannot just set the
         * layout (because workspaces need to be splitv/splith to allow focus
         * parent and opening new containers). Instead, it stores the requested
         * layout in workspace_layout and creates a new split container with that
         * layout whenever a new container is attached to the workspace. */
        layout_t workspace_layout;

        /** Only applicable for containers of type CT_WORKSPACE. */
        gaps_t gaps;

        /** the workspace number, if this Con is of type CT_WORKSPACE and the
         * workspace is not a named workspace (for named workspaces, num == -1) */
        int num{};

        WorkspaceCon(bool skeleton = false)
            : Con(skeleton) {
            this->type = CT_WORKSPACE;
            this->workspace_layout = L_DEFAULT;
        }

        void con_attach(Con *parent, bool ignore_focus, Con *previous = nullptr) override;

        FloatingCon *con_inside_floating() override;

        void con_set_layout(layout_t layout) override;

        void on_remove_child() override;

        bool con_has_children() const override;
    };

    namespace con {
        Con *first(std::deque<Con *> &queue);

        Con *previous(Con *con, std::deque<Con *> &queue);

        Con *next(Con *con, std::deque<Con *> &queue);

        Con *last(std::deque<Con *> &queue);
    }  // namespace con

    /**
     * Returns the container with the given client window ID or NULL if no such
     * container exists.
     *
     */
    ConCon *con_by_window_id(xcb_window_t window);

    /**
     * Returns the container with the given frame ID or NULL if no such container
     * exists.
     *
     */
    Con *con_by_frame_id(xcb_window_t frame);

    /**
     * Start from a container and traverse the transient_for linked list. Returns
     * true if target window is found in the list. Protects againsts potential
     * cycles.
     *
     */
    bool con_find_transient_for_window(Con * start, xcb_window_t target);

    /**
     * Returns the first container below 'con' which wants to swallow this window
     * TODO: priority
     *
     */
    Con *con_for_window(Con const *con, i3Window const *window, Match **store_match);

    /**
     * Toggles fullscreen mode for the given container. Fullscreen mode will not be
     * entered when there already is a fullscreen container on this workspace.
     *
     */
    void con_toggle_fullscreen(Con * con, int fullscreen_mode);

    /**
     * Enables fullscreen mode for the given container, if necessary.
     *
     */
    void con_enable_fullscreen(Con * con, fullscreen_mode_t fullscreen_mode);

    /**
     * Disables fullscreen mode for the given container, if necessary.
     *
     */
    void con_disable_fullscreen(Con * con);

    /**
     * Moves the given container to the currently focused container on the given
     * workspace.
     *
     * The fix_coordinates flag will translate the current coordinates (offset from
     * the monitor position basically) to appropriate coordinates on the
     * destination workspace.
     * Not enabling this behaviour comes in handy when this function gets called by
     * floating_maybe_reassign_ws, which will only "move" a floating window when it
     * *already* changed its coordinates to a different output.
     *
     * The dont_warp flag disables pointer warping and will be set when this
     * function is called while dragging a floating window.
     *
     * If ignore_focus is set, the container will be moved without modifying focus
     * at all.
     *
     * TODO: is there a better place for this function?
     *
     */
    void con_move_to_workspace(Con * con, WorkspaceCon * workspace, bool fix_coordinates,
                               bool dont_warp, bool ignore_focus);

    /**
     * Moves the given container to the currently focused container on the
     * visible workspace on the given output.
     *
     */
    void con_move_to_output(Con * con, Output * output, bool fix_coordinates);

    bool con_move_to_target(Con * con, Con * target);

    /**
     * Returns the orientation of the given container (for stacked containers,
     * vertical orientation is used regardless of the actual orientation of the
     * container).
     *
     */
    orientation_t con_orientation(Con const *const con);

    /**
     * Returns the container which will be focused next when the given container
     * is not available anymore. Called in tree_close_internal and con_move_to_workspace
     * to properly restore focus.
     *
     */
    Con *con_next_focused(Con * con);

    /**
     * Returns the focused con inside this client, descending the tree as far as
     * possible. This comes in handy when attaching a con to a workspace at the
     * currently focused position, for example.
     *
     */
    Con *con_descend_focused(Con * con);

    /**
     * Returns the focused con inside this client, descending the tree as far as
     * possible. This comes in handy when attaching a con to a workspace at the
     * currently focused position, for example.
     *
     * Works like con_descend_focused but considers only tiling cons.
     *
     */
    Con *con_descend_tiling_focused(Con * con);

    /**
     * Returns the leftmost, rightmost, etc. container in sub-tree. For example, if
     * direction is D_LEFT, then we return the rightmost container and if direction
     * is D_RIGHT, we return the leftmost container.  This is because if we are
     * moving D_LEFT, and thus want the rightmost container.
     */
    Con *con_descend_direction(Con * con, direction_t direction);

    /**
     * Returns whether the window decoration (title bar) should be drawn into the
     * X11 frame window of this container (default) or into the X11 frame window of
     * the parent container (for stacked/tabbed containers).
     *
     */
    bool con_draw_decoration_into_frame(Con const *const con);

    /**
     * Returns a "relative" Rect which contains the amount of pixels that need to
     * be added to the original Rect to get the final position (obviously the
     * amount of pixels for normal, 1pixel and borderless are different).
     *
     */
    Rect con_border_style_rect(Con const *const con);

    /**
     * Returns adjacent borders of the window. We need this if hide_edge_borders is
     * enabled.
     */
    adjacent_t con_adjacent_borders(Con const *const con);

    /**
     * Use this function to get a container’s border style. This is important
     * because when inside a stack, the border style is always BS_NORMAL.
     * For tabbed mode, the same applies, with one exception: when the container is
     * borderless and the only element in the tabbed container, the border is not
     * rendered.
     *
     * For children of a CT_DOCKAREA, the border style is always none.
     *
     */
    border_style_t con_border_style(Con const *const con);

    /**
     * Sets the given border style on con, correctly keeping the position/size of a
     * floating window.
     *
     */
    void con_set_border_style(Con * con, border_style_t border_style, int border_width);

    /**
     * Returns true if changing the focus to con would be allowed considering
     * the fullscreen focus constraints. Specifically, if a fullscreen container or
     * any of its descendants is focused, this function returns true if and only if
     * focusing con would mean that focus would still be visible on screen, i.e.,
     * the newly focused container would not be obscured by a fullscreen container.
     *
     * In the simplest case, if a fullscreen container or any of its descendants is
     * fullscreen, this functions returns true if con is the fullscreen container
     * itself or any of its descendants, as this means focus wouldn't escape the
     * boundaries of the fullscreen container.
     *
     * In case the fullscreen container is of type CF_OUTPUT, this function returns
     * true if con is on a different workspace, as focus wouldn't be obscured by
     * the fullscreen container that is constrained to a different workspace.
     *
     * Note that this same logic can be applied to moving containers. If a
     * container can be focused under the fullscreen focus constraints, it can also
     * become a parent or sibling to the currently focused container.
     *
     */
    bool con_fullscreen_permits_focusing(Con * con);

    /**
     * Create a string representing the subtree under con.
     *
     */
    std::string con_get_tree_representation(Con * con);

    /**
     * force parent split containers to be redrawn
     *
     */
    void con_force_split_parents_redraw(Con * con);

    /**
     * Returns the window title considering the current title format.
     *
     */
    std::string con_parse_title_format(Con * con);

    /**
     * Returns given container's rect size depending on its orientation.
     * i.e. its width when horizontal, its height when vertical.
     *
     */
    uint32_t con_rect_size_in_orientation(Con * con);

    /**
     * Returns true if the container is within any stacked/tabbed split container.
     *
     */
    bool con_inside_stacked_or_tabbed(Con * con);
    
    /**
     * Runs the given binding and handles parse errors. If con is passed, it will
     * execute the command binding with that container selected by criteria.
     * Returns a CommandResult for running the binding's command. Caller should
     * render tree if needs_tree_render is true. Free with command_result_free().
     *
     */
    CommandResult run_binding(Binding * bind, Con * con);
    
}
// util.cppm
/* TODO: move stuff used by more than one module into utils/ */
export {
    std::vector<std::string> add_argument(std::vector<std::string> & original, char const *opt_char, char const *opt_arg, char const *opt_name);
    
    /**
     * Restart i3 in-place
     * appends -a to argument list to disable autostart
     *
     */
    void i3_restart(bool forget_layout);

    /**
     * Convert a direction to its corresponding orientation.
     *
     */
    orientation_t orientation_from_direction(direction_t direction);

    /**
     * Convert a direction to its corresponding position.
     *
     */
    position_t position_from_direction(direction_t direction);

}
// x.cppm
export {
    using x_connection = xpp::connection<xpp::xkb::extension, xpp::shape::extension, xpp::big_requests::extension, xpp::randr::extension>;
    
    /*
     * Describes the X11 state we may modify (map state, position, window stack).
     * There is one entry per container. The state represents the current situation
     * as X11 sees it (with the exception of the order in the state_head CIRCLEQ,
     * which represents the order that will be pushed to X11, while old_state_head
     * represents the current order). It will be updated in x_push_changes().
     *
     */
    struct con_state {
        xcb_window_t id{};
        bool mapped{};
        bool unmap_now{};
        bool child_mapped{};
        bool is_hidden{};
        bool is_maximized_vert;
        bool is_maximized_horz;

        /* The con for which this state is. */
        Con *con{};

        /* For reparenting, we have a flag (need_reparent) and the X ID of the old
         * frame this window was in. The latter is necessary because we need to
         * ignore UnmapNotify events (by changing the window event mask). */
        bool need_reparent{};
        xcb_window_t old_frame{};

        /* The container was child of floating container during the previous call of
         * x_push_node(). This is used to remove the shape when the container is no
         * longer floating. */
        bool was_floating{};

        Rect rect{};
        Rect window_rect{};

        bool initial{};

        std::string name{};
    };

    class X {
      protected:
        xcb_cursor_context_t *ctx;
        std::map<xcursor_cursor_t, xcb_cursor_t> cursors{};

        xcb_visualtype_t *get_visual_type_for_root();

        explicit X(x_connection *conn)
            : conn(conn) {
        }

      public:
        /**
         * The EWMH support window that is used to indicate that an EWMH-compliant
         * window manager is present. This window is created when i3 starts and
         * kept alive until i3 exits.
         * We also use this window as the focused window if no other window is
         * available to be focused on the active workspace in order to prevent
         * keyboard focus issues (see #1378).
         */
        xcb_window_t ewmh_window;

        /* Color depth, visual id and colormap to use when creating windows and
         * pixmaps. Will use 32 bit depth and an appropriate visual, if available,
         * otherwise the root window’s default (usually 24 bit TrueColor). */
        uint8_t root_depth;
        /* The default visual_type to use if none is specified when creating the surface. Must be defined globally. */
        xcb_visualtype_t *visual_type;
        xcb_colormap_t colormap;

        unsigned int xcb_numlock_mask;
        /* Stores coordinates to warp mouse pointer to if set */
        std::optional<Rect> warp_to{};

        std::deque<con_state *> state_head{};
        std::deque<con_state *> old_state_head{};
        std::deque<con_state *> initial_mapping_head{};

        /* Because 'focused_id' might be reset to force input focus, we separately keep
         * track of the X11 window ID to be able to always tell whether the focused
         * window actually changed. */
        xcb_window_t last_focused{XCB_NONE};
        /** Stores the X11 window ID of the currently focused window */
        xcb_window_t focused_id{XCB_NONE};
        x_connection *conn;
        xcb_screen_t *root_screen;
        /* The screen (0 when you are using DISPLAY=:0) of the connection 'conn' */
        int conn_screen;
        xcb_atom_t wm_sn;
        xcb_window_t wm_sn_selection_owner;
        xcb_window_t root;

        int shape_base{-1};
        bool shape_supported{true};

        explicit X();
        virtual ~X() = default;

        operator x_connection *() {
            return conn;
        }
        
        xcb_connection_t* con() {
            return *conn;
        }

        void xcursor_load_cursors();
        xcb_cursor_t xcursor_get_cursor(xcursor_cursor_t c);

        /**
         * Sets the cursor of the root window to the 'pointer' cursor.
         *
         * This function is called when i3 is initialized, because with some login
         * managers, the root window will not have a cursor otherwise.
         *
         * We have a separate xcursor function to use the same X11 connection as the
         * xcursor_load_cursors() function. If we mix the Xlib and the XCB connection,
         * races might occur (even though we flush the Xlib connection).
         *
         */
        void xcursor_set_root_cursor(xcursor_cursor_t cursor_id);

        /**
         * Initializes the X11 part for the given container. Called exactly once for
         * every container from con_new().
         *
         */
        virtual void con_init(Con *con, std::optional<xcb_drawable_t> id = std::nullopt);
    };

    /*
     * Moves a child window from Container src to Container dest.
     *
     */
    void x_move_win(Con * src, Con * dest);

    /*
     * Re-initializes the associated X window state for this container. You have
     * to call this when you assign a client to an empty container to ensure that
     * its state gets updated correctly.
     *
     */
    void x_reinit(Con * con);

    /**
     * Reparents the child window of the given container (necessary for sticky
     * containers). The reparenting happens in the next call of x_push_changes().
     *
     */
    void x_reparent_child(Con * con, Con * old);

    /**
     * Kills the window decoration associated with the given container.
     *
     */
    void x_con_kill(Con * con);

    /*
     * Completely reinitializes the container's frame, without destroying the old window.
     *
     */
    void x_con_reframe(Con * con);

    /**
     * Returns true if the client supports the given protocol atom (like WM_DELETE_WINDOW)
     *
     */
    bool window_supports_protocol(xcb_window_t window, xcb_atom_t atom);

    /**
     * Kills the given X11 window using WM_DELETE_WINDOW (if supported).
     *
     */
    void x_window_kill(xcb_connection_t * c, xcb_window_t window, kill_window_t kill_window);

    /**
     * Draws the decoration of the given container onto its parent.
     *
     */
    void x_draw_decoration(Con * con);

    /**
     * Recursively calls x_draw_decoration. This cannot be done in x_push_node
     * because x_push_node uses focus order to recurse (see the comment above)
     * while drawing the decoration needs to happen in the actual order.
     *
     */
    void x_deco_recurse(Con * con);

    /**
     * This function pushes the properties of each node of the layout tree to
     * X11 if they have changed (like the map state, position of the window, …).
     * It recursively traverses all children of the given node.
     *
     */
    void x_push_node(Con * con);

    /**
     * Pushes all changes (state of each node, see x_push_node() and the window
     * stack) to X11.
     *
     */
    void x_push_changes(Con * con);

    /**
     * Raises the specified container in the internal stack of X windows. The
     * next call to x_push_changes() will make the change visible in X11.
     *
     */
    void x_raise_con(Con * con);

    /**
     * Sets the WM_NAME property (so, no UTF8, but used only for debugging anyways)
     * of the given name. Used for properly tagging the windows for easily spotting
     * i3 windows in xwininfo -root -all.
     *
     */
    void x_set_name(Con * con, std::string const &name);

    /**
     * Sets up i3 specific atoms (I3_SOCKET_PATH and I3_CONFIG_PATH)
     *
     */
    void x_set_i3_atoms();

    /**
     * Set warp_to coordinates.  This will trigger on the next call to
     * x_push_changes().
     *
     */
    void x_set_warp_to(std::optional<Rect> rect);

    /**
     * Applies the given mask to the event mask of every i3 window decoration X11
     * window. This is useful to disable EnterNotify while resizing so that focus
     * is untouched.
     *
     */
    void x_mask_event_mask(uint32_t mask);

    /**
     * Enables or disables nonrectangular shape of the container frame.
     */
    void x_set_shape(ConCon * con, xcb_shape_sk_t kind, bool enable);
    
    /**
     * Ungrabs all keys, to be called before re-grabbing the keys because of a
     * mapping_notify event or a configuration file reload
     *
     */
    void ungrab_all_keys(x_connection * conn);
    
    /**
     * Grab the bound keys (tell X to send us keypress events for those keycodes)
     *
     */
    void grab_all_keys(x_connection * conn);

    /**
     * Release the button grabs on all managed windows and regrab them,
     * reevaluating which buttons need to be grabbed.
     *
     */
    void regrab_all_buttons(x_connection * conn);

}
// workspace.cppm
export {
    /* We use NET_WM_DESKTOP_NONE for cases where we cannot determine the EWMH
     * desktop index for a window. We cannot use a negative value like -1 since we
     * need to use uint32_t as we actually need the full range of it. This is
     * technically dangerous, but it's safe to assume that we will never have more
     * than 4294967279 workspaces open at a time. */
    uint32_t const NET_WM_DESKTOP_NONE = 0xFFFFFFF0;
    uint32_t const NET_WM_DESKTOP_ALL = 0xFFFFFFFF;
    
    /**
     * Stores which workspace (by name or number) goes to which output and its gaps config.
     *
     */
    class WorkspaceConfig {
      public:
        std::string name{};
        std::string output{};
        gaps_t gaps;
        gaps_mask_t gaps_mask;

        auto operator<=>(WorkspaceConfig const &) const = default;
        WorkspaceConfig &operator=(WorkspaceConfig const &) = default;

        explicit WorkspaceConfig(std::string name)
            : name(std::move(name)) {
        }

        WorkspaceConfig(std::string name, std::string output)
            : name(std::move(name)), output(std::move(output)) {
        }

        WorkspaceConfig(WorkspaceConfig const &) = default;
        WorkspaceConfig(WorkspaceConfig &&) = default;
    };

    // TODO: I feel this should reside with the configuration
    class WorkspaceManager {
      private:
        ConfigurationManager &configManager;
        /* The list of workspace assignments (which workspace should end up on which
         * output) */
        std::map<std::string, WorkspaceConfig> ws_assignments{};

      public:
        /**
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
        Output *get_assigned_output(std::string const name, long const parsed_num);
        Output *get_assigned_output(long const parsed_num);
        Output *get_assigned_output(std::string const name);

        /**
         * Stores a copy of the name of the last used workspace for the workspace
         * back-and-forth switching.
         *
         */
        std::string previous_workspace_name{};

        std::optional<WorkspaceConfig> get_workspace_config(std::string const &name);

        std::optional<WorkspaceConfig> get_workspace_config(WorkspaceCon const *ws);

        void add_workspace_config(WorkspaceConfig &&config) {
            auto name = config.name;
            ws_assignments.insert_or_assign(name, std::forward<WorkspaceConfig>(config));
        }

        void clear() {
            ws_assignments.clear();
        }

        std::vector<WorkspaceConfig> all_workspace_configs() {
            std::vector<WorkspaceConfig> config{};
            config.reserve(ws_assignments.size());
            for (auto &c : ws_assignments) {
                config.push_back(c.second);
            }
            return config;
        }

        /**
         * Returns workspace assignments which would be triggered for the output.
         */
        std::vector<WorkspaceConfig> configs_for_output(Output *output);

        WorkspaceCon *workspace_get_or_create(std::string const &num);
        WorkspaceCon *create_workspace_on_output(Output *output, Con *content);

        /**
         * Move the given workspace to the specified output.
         *
         */
        void workspace_move_to_output(WorkspaceCon *ws, Output *output);

        /**
         * Returns the previously focused workspace con, or NULL if unavailable.
         *
         */
        WorkspaceCon *workspace_back_and_forth_get();

        /**
         * Focuses the previously focused workspace.
         *
         */
        void workspace_back_and_forth();

        /**
         * Looks up the workspace by name and switches to it.
         *
         */
        void workspace_show_by_name(std::string const &num);

        /**
         * Switches to the given workspace
         *
         */
        void workspace_show(WorkspaceCon *ws);

        WorkspaceManager(ConfigurationManager &configManager)
            : configManager(configManager) {
        }
    };

    /**
     * Returns the workspace with the given name or NULL if such a workspace does
     * not exist.
     *
     */
    WorkspaceCon *get_existing_workspace_by_name(std::string const &name);

    /**
     * Returns the workspace with the given number or NULL if such a workspace does
     * not exist.
     *
     */
    WorkspaceCon *get_existing_workspace_by_num(int num);

    /**
     * Returns a pointer to the workspace with the given number (starting at 0),
     * creating the workspace if necessary (by allocating the necessary amount of
     * memory and initializing the data structures correctly).
     *
     */
    WorkspaceCon *workspace_get_or_create(std::string const &num);

    /**
     * Returns a pointer to a new workspace in the given output. The workspace
     * is created attached to the tree hierarchy through the given content
     * container.
     *
     */
    WorkspaceCon *create_workspace_on_output(Output * output, Con * content);

    /**
     * Returns true if the workspace is currently visible. Especially important for
     * multi-monitor environments, as they can have multiple currently active
     * workspaces.
     *
     */
    bool workspace_is_visible(Con * ws);

    /**
     * Returns the next workspace.
     *
     */
    WorkspaceCon *workspace_next();

    /**
     * Returns the previous workspace.
     *
     */
    WorkspaceCon *workspace_prev();

    /**
     * Returns the next workspace on the same output
     *
     */
    WorkspaceCon *workspace_next_on_output();

    /**
     * Returns the previous workspace on the same output
     *
     */
    WorkspaceCon *workspace_prev_on_output();

    /**
     * Goes through all clients on the given workspace and updates the workspace’s
     * urgent flag accordingly.
     *
     */
    void workspace_update_urgent_flag(WorkspaceCon * ws);

    /**
     * 'Forces' workspace orientation by moving all cons into a new split-con with
     * the same orientation as the workspace and then changing the workspace
     * orientation.
     *
     */
    void ws_force_orientation(Con * ws, orientation_t orientation);

    /**
     * Called when a new con (with a window, not an empty or split con) should be
     * attached to the workspace (for example when managing a new window or when
     * moving an existing window to the workspace level).
     *
     * Depending on the workspace_layout setting, this function either returns the
     * workspace itself (default layout) or creates a new stacked/tabbed con and
     * returns that.
     *
     */
    Con *workspace_attach_to(WorkspaceCon * ws);

    /**
     * Creates a new container and re-parents all of children from the given
     * workspace into it.
     *
     * The container inherits the layout from the workspace.
     */
    Con *workspace_encapsulate(Con * ws);
}
// event_handler.cppm
export {
    // TODO: zybreak Rename to Event or EventLoop or something
    class EventHandler {
      private:
        PropertyHandlers &handlers;
        X &x;
        /* We keep the xcb_prepare watcher around to be able to enable and disable it
         * temporarily for drag_pointer(). */
        ev_prepare *xcb_prepare;
        ev_io *xcb_watcher;
        void xcb_prepare_cb(EV_P_ ev_prepare *w, int revents);
        
      public:
        EventHandler() = delete;
        EventHandler(EventHandler const &) = delete;
        EventHandler(X &x, PropertyHandlers &handlers);
        ~EventHandler();

        void loop();

        /**
         * Set up handlers for all signals with default action "Term", see signal(7)
         *
         */
        void setup_term_handlers();

        /**
         * Enable or disable the main X11 event handling function.
         * This is used by drag_pointer() which has its own, modal event handler, which
         * takes precedence over the normal event handler.
         *
         */
        void main_set_x11_cb(bool enable);
    };

    /*
     * (One-shot) Handler for all signals with default action "Term", see signal(7)
     *
     * Exits the program gracefully.
     *
     */
    void handle_term_signal(EV_P_ ev_signal * signal, int revents);
}
// xkb.cppm
export class Keysyms {
    friend class Xkb;
    
  private:
    X &x;
    xcb_key_symbols_t *keysyms;

  public:
    Keysyms() = delete;
    explicit Keysyms(X &x);
    Keysyms(Keysyms &&other) noexcept
        : x(other.x) {
        std::swap(keysyms, other.keysyms);
    }

    void operator=(Keysyms const &) = delete;
    void operator=(Keysyms &&other) noexcept {
        std::swap(keysyms, other.keysyms);
    }
    ~Keysyms();
};

export class ResolvedKeymap {
    friend class Keymap;

  private:
    ResolvedKeymap(xkb_keymap *keymap) {
        dummy_state = xkb_state_new(keymap);
        dummy_state_no_shift = xkb_state_new(keymap);
        dummy_state_numlock = xkb_state_new(keymap);
        dummy_state_numlock_no_shift = xkb_state_new(keymap);

        if (dummy_state == nullptr ||
            dummy_state_no_shift == nullptr ||
            dummy_state_numlock == nullptr ||
            dummy_state_numlock_no_shift == nullptr) {
            xkb_state_unref(dummy_state);
            xkb_state_unref(dummy_state_no_shift);
            xkb_state_unref(dummy_state_numlock);
            xkb_state_unref(dummy_state_numlock_no_shift);
            throw std::runtime_error("Could not create XKB state, cannot translate keysyms.");
        }
    }

  public:
    xkb_state *dummy_state{nullptr};
    xkb_state *dummy_state_no_shift{nullptr};
    xkb_state *dummy_state_numlock{nullptr};
    xkb_state *dummy_state_numlock_no_shift{nullptr};

    ResolvedKeymap(ResolvedKeymap const &) = delete;
    ResolvedKeymap &operator=(ResolvedKeymap const &) = delete;
    ResolvedKeymap(ResolvedKeymap &&other) noexcept {
        std::swap(dummy_state, other.dummy_state);
        std::swap(dummy_state_no_shift, other.dummy_state_no_shift);
        std::swap(dummy_state_numlock, other.dummy_state_numlock);
        std::swap(dummy_state_numlock_no_shift, other.dummy_state_numlock_no_shift);
    }

    ResolvedKeymap &operator=(ResolvedKeymap &&other) noexcept {
        std::swap(dummy_state, other.dummy_state);
        std::swap(dummy_state_no_shift, other.dummy_state_no_shift);
        std::swap(dummy_state_numlock, other.dummy_state_numlock);
        std::swap(dummy_state_numlock_no_shift, other.dummy_state_numlock_no_shift);
        return *this;
    }

    ~ResolvedKeymap() {
        xkb_state_unref(dummy_state);
        xkb_state_unref(dummy_state_no_shift);
        xkb_state_unref(dummy_state_numlock);
        xkb_state_unref(dummy_state_numlock_no_shift);
    }
};

export class Keymap {
    friend class Xkb;
    friend void translate_keysyms(Keymap const &keymap);

  private:
    xkb_context *context = nullptr;
    xkb_keymap *keymap = nullptr;

  public:
    std::unique_ptr<ResolvedKeymap> resolve() const {
        return std::unique_ptr<ResolvedKeymap>(new ResolvedKeymap(keymap));
    }
    Keymap();
    Keymap(Keymap &other) = delete;
    void operator=(Keymap &other) = delete;

    Keymap &operator=(Keymap &&other) noexcept {
        std::swap(keymap, other.keymap);
        std::swap(context, other.context);
        return *this;
    }

    Keymap(Keymap &&other) noexcept {
        std::swap(keymap, other.keymap);
        std::swap(context, other.context);
    }

    ~Keymap() {
        if (keymap) {
            xkb_keymap_unref(keymap);
        }
        if (context) {
            xkb_context_unref(context);
        }
    }
};

export class Xkb {
  private:
    X &x;
    std::unique_ptr<Keysyms> _keysyms{};
    std::unique_ptr<Keymap> _keymap{};

  public:
    bool xkb_supported{true};
    int xkb_base{-1};
    int xkb_current_group;

    /**
     * Loads the XKB keymap from the X11 server and feeds it to xkbcommon.
     *
     */
    void load_keymap();
    void key_symbols_alloc();
    Keysyms &keysyms() {
        if (!_keysyms) {
            _keysyms = std::make_unique<Keysyms>(x);
        }

        return *_keysyms;
    }

    Keymap &keymap() {
        if (!_keymap) {
            _keymap = std::make_unique<Keymap>();
        }

        return *_keymap;
    }

    Xkb() = delete;
    explicit Xkb(X &x);

    unsigned int get_numlock_mask();
    xcb_keysym_t key_press_lookup_keysym(xcb_key_press_event_t *event, uint16_t state);
    void refresh_keyboard_mapping(xcb_mapping_notify_event_t *event);
};

/**
 * Translates keysymbols to keycodes for all bindings which use keysyms.
 *
 */
export void translate_keysyms(Keymap const &keymap);

// handlers.cppm
enum click_destination_t {
    CLICK_BORDER = 0,
    CLICK_DECORATION = 1,
    CLICK_INSIDE = 2
};

export {
    struct Ignore_Event {
        int sequence;
        int response_type;
        std::chrono::time_point<std::chrono::system_clock> added{};
    };

    // TODO: zybreak Rename to EventHandlers
    class PropertyHandlers {
      private:
        X &x;
        WorkspaceManager &workspaceManager;
        ConfigurationManager &configManager;
        InputManager &inputManager;
        RandR &randr;
        Xkb &xkb;
        IPCManager &ipcManager;

        /* After mapping/unmapping windows, a notify event is generated. However, we don’t want it,
           since it’d trigger an infinite loop of switching between the different windows when
           changing workspaces */
        std::mutex mtx{};

        std::vector<std::unique_ptr<Ignore_Event>> ignore_events{};

        /**
         * Returns whether there currently are any drop targets.
         * Used to only initiate a drag when there is something to drop onto.
         *
         */
        bool has_drop_targets();

        /**
         * Initiates a mouse drag operation on a tiled window.
         *
         */
        void tiling_drag(Con *con, xcb_button_press_event_t *event, bool use_threshold);

        void route_click(Con *con, xcb_button_press_event_t *event, click_destination_t const dest);

        /**
         * Called when the user clicked on the titlebar of a floating window.
         * Calls the drag_pointer function with the drag_window callback
         *
         */
        void floating_drag_window(Con *con, xcb_button_press_event_t const *event, bool use_threshold);

        void handle_move_resize(xcb_client_message_event_t *event);

        /**
         * Called when the user clicked on a floating window while holding the
         * floating_modifier and the right mouse button.
         * Calls the drag_pointer function with the resize_window callback
         *
         */
        void floating_resize_window(Con *con, bool proportional, xcb_button_press_event_t const *event);

      public:
        // TODO: make private
        void resize_graphical_handler(Con *first, Con *second, orientation_t orientation,
                                      xcb_button_press_event_t const *event,
                                      bool use_threshold);

      private:
        /**
         * The button press X callback. This function determines whether the floating
         * modifier is pressed and where the user clicked (decoration, border, inside
         * the window).
         *
         * Then, route_click is called on the appropriate con.
         *
         */
        void handle_button_press(xcb_button_press_event_t *event);

        /**
         * When the user moves the mouse pointer onto a window, this callback gets called.
         */
        void handle_enter_notify(xcb_enter_notify_event_t *event);

        /**
         * When the user moves the mouse but does not change the active window
         * (e.g. when having no windows opened but moving mouse on the root screen
         * and crossing virtual screen boundaries), this callback gets called.
         */
        void handle_motion_notify(xcb_motion_notify_event_t *event);

        /**
         * When the user moves the mouse but does not change the active window
         * (e.g. when having no windows opened but moving mouse on the root screen
         * and crossing virtual screen boundaries), this callback gets called.
         */
        void handle_mapping_notify(xcb_mapping_notify_event_t *event);

        /**
         * A new window appeared on the screen (=was mapped), so let’s manage it.
         */
        void handle_map_request(xcb_map_request_event_t *event);

        /**
         * Gets triggered upon a RandR screen change event, that is when the user
         * changes the screen configuration in any way (mode, position, …)
         */
        void handle_screen_change(xcb_generic_event_t *e);

        /**
         * Our window decorations were unmapped. That means, the window will be killed
         * now, so we better clean up before.
         */
        void handle_unmap_notify_event(xcb_unmap_notify_event_t *event);

        /**
         * A destroy notify event is sent when the window is not unmapped, but
         * immediately destroyed (for example when starting a window and immediately
         * killing the program which started it).
         *
         * We just pass on the event to the unmap notify handler (by copying the
         * important fields in the event data structure).
         */
        void handle_destroy_notify_event(xcb_destroy_notify_event_t *event);

        /**
         * Expose event means we should redraw our windows (= title bar)
         */
        void handle_expose_event(xcb_expose_event_t *event);

        /**
         * Handle client messages (EWMH)
         */
        void handle_client_message(xcb_client_message_event_t *event);

        /**
         * Handles FocusIn events which are generated by clients (i3’s focus changes
         * don’t generate FocusIn events due to a different EventMask) and updates the
         * decorations accordingly.
         */
        void handle_focus_in(xcb_focus_in_event_t *event);

        /**
         * Log FocusOut events.
         */
        void handle_focus_out(xcb_focus_in_event_t *event);

        /**
         * Handles ConfigureNotify events for the root window, which are generated when
         * the monitor configuration changed.
         *
         */
        void handle_configure_notify(xcb_configure_notify_event_t *event);

        void property_notify(xcb_property_notify_event_t *event);

        /**
         * There was a KeyPress or KeyRelease (both events have the same fields). We
         * compare this key code with our bindings table and pass the bound action to
         * parse_command().
         */
        void handle_key_press(xcb_key_press_event_t *event);

      public:
        PropertyHandlers() = delete;
        PropertyHandlers(PropertyHandlers const &) = delete;
        PropertyHandlers operator=(PropertyHandlers const &) = delete;

        /**
         * Sets the appropriate atoms for the property handlers after the atoms were
         * received from X11
         *
         */
        PropertyHandlers(X &x, WorkspaceManager &workspaceManager, ConfigurationManager &configManager, RandR &randr, Xkb &xkb, InputManager &inputManager, ApplicationLauncher &applicationLauncher, IPCManager &ipcManager);

        /**
         * Configure requests are received when the application wants to resize windows
         * on their own.
         *
         * We generate a synthetic configure notify event to signalize the client its
         * "new" position.
         */
        void handle_configure_request(xcb_configure_request_event_t *event);

        /**
         * Adds the given sequence to the list of events which are ignored.
         * If this ignore should only affect a specific response_type, pass
         * response_type, otherwise, pass -1.
         *
         * Every ignored sequence number gets garbage collected after 5 seconds.
         *
         */
        void add_ignore_event(int sequence, int response_type);

        /**
         * Checks if the given sequence is ignored and returns true if so.
         *
         */
        bool event_is_ignored(int sequence, int response_type);

        /**
         * Takes an xcb_generic_event_t and calls the appropriate handler, based on the
         * event type.
         *
         */
        void handle_event(int type, xcb_generic_event_t *event);
    };
}
// criteria_state.cppm
export class criteria_state_impl : public criteria_state {
  public:
    int criteria_next_state;
    Match current_match{};
    std::deque<Con *> owindows{};
    
    criteria_state_impl(int criteria_next_state)
        : criteria_next_state(criteria_next_state) {
    }

    criteria_state_impl(criteria_state_impl &&src) noexcept {
        std::swap(this->criteria_next_state, src.criteria_next_state);
        std::swap(this->current_match, src.current_match);
        std::swap(this->owindows, src.owindows);
    }

    criteria_state_impl &operator=(criteria_state_impl &&src) noexcept {
        std::swap(this->criteria_next_state, src.criteria_next_state);
        std::swap(this->current_match, src.current_match);
        std::swap(this->owindows, src.owindows);
        return *this;
    }
};
// resize.cppm
export {
    bool resize_find_tiling_participants(Con * *current, Con * *other, direction_t direction, bool both_sides);
    
    /**
     * Resize the two given containers using the given amount of pixels or
     * percentage points. One of the two needs to be 0. A positive amount means
     * growing the first container while a negative means shrinking it.
     * Returns false when the resize would result in one of the two containers
     * having less than 1 pixel of size.
     *
     */
    bool resize_neighboring_cons(Con * first, Con * second, int px, int ppt);

    /**
     * Calculate the minimum percent needed for the given container to be at least 1
     * pixel.
     *
     */
    double percent_for_1px(Con * con);
}
// window.cppm
export {
    /**
     * Stores the reserved pixels on each screen edge read from a
     * _NET_WM_STRUT_PARTIAL.
     *
     */
    struct reservedpx {
        uint32_t left{0};
        uint32_t right{0};
        uint32_t top{0};
        uint32_t bottom{0};
    };

    class i3WindowIcon {
        /** Window icon, as Cairo surface */
        cairo_surface_t *icon;
        uint32_t *icon_data;

      public:
        cairo_surface_t *data() {
            return icon;
        }

        i3WindowIcon() = delete;
        i3WindowIcon(i3WindowIcon const &other) = delete;
        i3WindowIcon &operator=(i3WindowIcon const &other) = delete;
        i3WindowIcon(i3WindowIcon &&other) noexcept {
            std::swap(icon, other.icon);
            std::swap(icon_data, other.icon_data);
        }
        i3WindowIcon(uint32_t width, uint32_t height, uint32_t *data, uint64_t len) {
            this->icon_data = new uint32_t[len * 4];

            for (uint64_t i = 0; i < len; i++) {
                uint8_t r, g, b, a;
                uint32_t const pixel = data[2 + i];
                a = (pixel >> 24) & 0xff;
                r = (pixel >> 16) & 0xff;
                g = (pixel >> 8) & 0xff;
                b = (pixel >> 0) & 0xff;

                /* Cairo uses premultiplied alpha */
                r = (r * a) / 0xff;
                g = (g * a) / 0xff;
                b = (b * a) / 0xff;

                icon_data[i] = (static_cast<uint32_t>(a) << 24) | (r << 16) | (g << 8) | b;
            }

            this->icon = cairo_image_surface_create_for_data(
                reinterpret_cast<unsigned char *>(icon_data),
                CAIRO_FORMAT_ARGB32,
                width,
                height,
                width * 4);
        }

        ~i3WindowIcon() {
            cairo_surface_destroy(this->icon);
            delete[] icon_data;
        }
    };

    enum window_dock_t {
        W_NODOCK = 0,
        W_DOCK_TOP = 1,
        W_DOCK_BOTTOM = 2
    };

    /**
     * A 'Window' is a type which contains an xcb_window_t and all the related
     * information (hints like _NET_WM_NAME for that window).
     *
     */
    class i3Window {
      public:
        xcb_window_t id{XCB_WINDOW_NONE};

        /** Holds the xcb_window_t (just an ID) for the leader window (logical
         * parent for toolwindows and similar floating windows) */
        xcb_window_t leader{XCB_WINDOW_NONE};
        xcb_window_t transient_for{XCB_WINDOW_NONE};

        /** Pointers to the Assignments which were already ran for this Window
         * (assignments run only once) */
        std::deque<Assignment *> ran_assignments{};

        std::string class_class{};
        std::string class_instance{};

        /** The name of the window. */
        std::string name{};

        /** The WM_WINDOW_ROLE of this window (for example, the pidgin buddy window
         * sets "buddy list"). Useful to match specific windows in assignments or
         * for_window. */
        std::string role{};

        /** WM_CLIENT_MACHINE of the window */
        std::string machine{};

        /** Flag to force re-rendering the decoration upon changes */
        bool name_x_changed{false};

        /** Whether the application used _NET_WM_NAME */
        bool uses_net_wm_name{false};

        /** Whether the application needs to receive WM_TAKE_FOCUS */
        bool needs_take_focus{false};

        /** Whether this window accepts focus. We store this inverted so that the
         * default will be 'accepts focus'. */
        bool doesnt_accept_focus{false};

        /** The _NET_WM_WINDOW_TYPE for this window. */
        xcb_atom_t window_type{XCB_ATOM_NONE};

        /** The _NET_WM_DESKTOP for this window. */
        uint32_t wm_desktop{0};

        /** Whether the window says it is a dock window */
        window_dock_t dock{W_NODOCK};

        /** When this window was marked urgent. 0 means not urgent */
        std::optional<std::chrono::time_point<std::chrono::system_clock>> urgent{};

        /** Pixels the window reserves. left/right/top/bottom */
        reservedpx reserved{};

        /** Depth of the window */
        uint16_t depth{0};

        /* the wanted size of the window, used in combination with size
         * increments (see below). */
        int base_width{0};
        int base_height{0};

        /* minimum increment size specified for the window (in pixels) */
        int width_increment{0};
        int height_increment{0};

        /* Minimum size specified for the window. */
        uint32_t min_width{0};
        uint32_t min_height{0};

        /* Maximum size specified for the window. */
        uint32_t max_width{0};
        uint32_t max_height{0};

        /* aspect ratio from WM_NORMAL_HINTS (MPlayer uses this for example) */
        double min_aspect_ratio{0};
        double max_aspect_ratio{0};

        /** Window icon, as Cairo surface */
        std::unique_ptr<i3WindowIcon> icon{};

        /** The window has a nonrectangular shape. */
        bool shaped{false};
        /** The window has a nonrectangular input shape. */
        bool input_shaped{false};

        /* Time when the window became managed. Used to determine whether a window
         * should be swallowed after initial management. */
        std::chrono::time_point<std::chrono::system_clock> managed_since{};

        /* The window has been swallowed. */
        bool swallowed{false};

        explicit i3Window(xcb_window_t id)
            : id(id) {};
        i3Window() = delete;
        i3Window(i3Window const &) = delete;
        i3Window &operator=(i3Window const &) = delete;

        /**
         * Updates the WM_CLASS (consisting of the class and instance) for the
         * given window.
         *
         */
        void window_update_class(std::string &window_class, std::string &window_instance);

        /**
         * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
         * window. Further updates using window_update_name_legacy will be ignored.
         *
         */
        void window_update_name(std::string &name);

        /**
         * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
         * touch what the client sends us but pass it to xcb_image_text_8. To get
         * proper unicode rendering, the application has to use _NET_WM_NAME (see
         * window_update_name()).
         *
         */
        void window_update_name_legacy(std::string &name);

        /**
         * Updates the CLIENT_LEADER (logical parent window).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_leader(xcb_get_property_reply_t *prop);

        /**
         * Updates the TRANSIENT_FOR (logical parent window).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_transient_for(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_strut_partial(xcb_get_property_reply_t *prop);

        /**
         * Updates the WM_WINDOW_ROLE
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_role(xcb_get_property_reply_t *prop);

        /**
         * Updates the _NET_WM_WINDOW_TYPE property.
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_type(xcb_get_property_reply_t *reply);

        /**
         * Updates the WM_NORMAL_HINTS
         *
         */
        bool window_update_normal_hints(xcb_size_hints_t &size_hints, xcb_get_geometry_reply_t *geom);
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_normal_hints(xcb_get_property_reply_t *reply, xcb_get_geometry_reply_t *geom);

        /**
         * Updates the WM_HINTS (we only care about the input focus handling part).
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_hints(xcb_get_property_reply_t *prop, bool *urgency_hint);

        /**
         * Updates the MOTIF_WM_HINTS. The container's border style should be set to
         * `motif_border_style' if border style is not BS_NORMAL.
         *
         * i3 only uses this hint when it specifies a window should have no
         * title bar, or no decorations at all, which is how most window managers
         * handle it.
         *
         * The EWMH spec intended to replace Motif hints with _NET_WM_WINDOW_TYPE, but
         * it is still in use by popular widget toolkits such as GTK+ and Java AWT.
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        bool window_update_motif_hints(xcb_get_property_reply_t *prop, border_style_t *motif_border_style);

        /**
         * Updates the WM_CLIENT_MACHINE
         *
         */
        void window_update_machine(std::string const &machine);

        /**
         * Updates the _NET_WM_ICON
         *
         */
        [[deprecated("Replaced by handle_property method, refactor this to use the returned data from there instead of parsing xcb data")]]
        void window_update_icon(xcb_get_property_reply_t *prop);
    };
}

// startup.cppm
export {
    class ApplicationLauncher {
      public:
        /* Display handle for libstartup-notification */
        SnDisplay *sndisplay;
        
        explicit ApplicationLauncher(X &x);

        /**
         * Starts the given application by passing it through a shell. We use double
         * fork to avoid zombie processes. As the started application’s parent exits
         * (immediately), the application is reparented to init (process-id 1), which
         * correctly handles children, so we don’t have to do it :-).
         *
         * The shell used to start applications is the system's bourne shell (i.e.,
         * /bin/sh).
         *
         * The no_startup_id flag determines whether a startup notification context
         * (and ID) should be created, which is the default and encouraged behavior.
         *
         */
        void start_application(std::string const command, bool no_startup_id);

        /**
         * Called by libstartup-notification when something happens
         *
         */
        void startup_monitor_event(SnMonitorEvent *event);

        /**
         * Renames workspaces that are mentioned in the startup sequences.
         *
         */
        void startup_sequence_rename_workspace(char const *old_name, char const *new_name);

        /**
         * Checks if the given window belongs to a startup notification by checking if
         * the _NET_STARTUP_ID property is set on the window (or on its leader, if it’s
         * unset).
         *
         * If so, returns the workspace on which the startup was initiated.
         * Returns NULL otherwise.
         *
         */
        std::optional<std::string> startup_workspace_for_window(i3Window *cwindow, xcb_get_property_reply_t *startup_id_reply);

        /**
         * Deletes the startup sequence for a window if it exists.
         *
         */
        void startup_sequence_delete_by_window(i3Window *win);
    };

    /**
     * Stores internal information about a startup sequence, like the workspace it
     * was initiated on.
     *
     */
    class Startup_Sequence {
      private:
        ev_timer *timeout;

      public:
        /** startup ID for this sequence, generated by libstartup-notification */
        std::string id{};
        /** workspace on which this startup was initiated */
        std::string workspace{};
        /** libstartup-notification context for this launch */
        struct SnLauncherContext *context{};
        /** time at which this sequence should be deleted (after it was marked as
         * completed) */
        std::optional<std::chrono::time_point<std::chrono::system_clock>> delete_at{};

        void stop_timer();

        Startup_Sequence() = delete;
        Startup_Sequence(int screen, class WorkspaceCon *ws, std::string name, std::string description, std::string launchee, xcb_timestamp_t last_timestamp);

        Startup_Sequence(Startup_Sequence const &) = delete;
        Startup_Sequence &operator=(Startup_Sequence const &) = delete;

        virtual ~Startup_Sequence();
    };
}

// gaps.cppm
export {
    /**
     * Calculates the effective gap sizes for a container.
     */
    gaps_t calculate_effective_gaps(Con const* const con);
    
    /*
     * Decides whether the container should be inset.
     */
    bool gaps_should_inset_con(Con * con, int children);

    /*
     * Returns whether the given container has an adjacent container in the
     * specified direction. In other words, this returns true if and only if
     * the container is not touching the edge of the screen in that direction.
     */
    bool gaps_has_adjacent_container(Con * con, direction_t direction);

    /**
     * Returns the configured gaps for this workspace based on the workspace name,
     * number, and configured workspace gap assignments.
     */
    gaps_t gaps_for_workspace(WorkspaceCon * ws);

    /**
     * Re-applies all workspace gap assignments to existing workspaces after
     * reloading the configuration file.
     *
     */
    void gaps_reapply_workspace_assignments();
}
// sync.cppm
export void sync_respond(X *x, xcb_window_t window, uint32_t rnd) {
    DLOG(fmt::sprintf("[i3 sync protocol] Sending random value %d back to X11 window 0x%08x\n", rnd, window));
    
    xcb_client_message_event_t ev{};

    ev.response_type = XCB_CLIENT_MESSAGE;
    ev.window = window;
    ev.type = i3::atoms[i3::Atom::I3_SYNC];
    ev.format = 32;
    ev.data.data32[0] = window;
    ev.data.data32[1] = rnd;

    xcb_send_event(**x, false, window, XCB_EVENT_MASK_NO_EVENT, (char *)&ev);
    xcb_flush(**x);
}
// output.cppm
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
// render.cppm
export {
    /**
     * "Renders" the given container (and its children), meaning that all rects are
     * updated correctly. Note that this function does not call any xcb_*
     * functions, so the changes are completely done in memory only (and
     * side-effect free). As soon as you call x_push_changes(), the changes will be
     * updated in X11.
     *
     */
    void render_con(Con * con);
    
    /**
     * Returns the height for the decorations
     *
     */
    int render_deco_height();
}
// randr.cppm
export {
    enum output_close_far_t {
        CLOSEST_OUTPUT = 0,
        FARTHEST_OUTPUT = 1
    };
    
    class RandR {
      private:
        X &x;
        ConfigurationManager &configManager;
        WorkspaceManager &workspaceManager;

        /* Pointer to the result of the query for primary output */
        xcb_randr_output_t primary;
        /* This is the output covering the root window */
        Output *root_output;
        bool has_randr_1_5 = false;

        Output *get_output_by_id(xcb_randr_output_t id);
        /*
         * Check whether there are any active outputs (excluding the root output).
         *
         */
        bool any_randr_output_active();
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
        void output_change_mode(xcb_connection_t *conn, Output *output);

        /*
         * randr_query_outputs_15 uses RandR ≥ 1.5 to update outputs.
         *
         */
        bool randr_query_outputs_15();

        /*
         * Gets called by randr_query_outputs_14() for each output. The function adds
         * new outputs to the list of outputs, checks if the mode of existing outputs
         * has been changed or if an existing output has been disabled. It will then
         * change either the "changed" or the "to_be_deleted" flag of the output, if
         * appropriate.
         *
         */
        void handle_output(xcb_connection_t *conn, xcb_randr_output_t id,
                           xcb_randr_get_output_info_reply_t *output,
                           xcb_timestamp_t cts,
                           xcb_randr_get_screen_resources_current_reply_t *res);

        /*
         * randr_query_outputs_14 uses RandR ≤ 1.4 to update outputs.
         *
         */
        void randr_query_outputs_14();

        void fallback_to_root_output();

      public:
        int randr_base{-1};

        /* Stores all outputs available in your current session. */
        std::deque<Output *> outputs{};

        /*
         * Returns the output with the given name or NULL.
         * If require_active is true, only active outputs are considered.
         *
         */
        virtual Output *get_output_by_name(std::string const &name, bool const require_active);

        /*
         * Returns the first output which is active.
         *
         */
        Output *get_first_output();

        /*
         * Returns the active (!) output which contains the coordinates x, y or NULL
         * if there is no output which contains these coordinates.
         *
         */
        Output *get_output_containing(uint32_t x, uint32_t y);

        /*
         * Returns the active output which spans exactly the area specified by
         * rect or NULL if there is no output like this.
         *
         */
        Output *get_output_with_dimensions(Rect rect);

        /*
         * In output_containing_rect, we check if any active output contains part of the container.
         * We do this by checking if the output rect is intersected by the Rect.
         * This is the 2-dimensional counterpart of get_output_containing.
         * Returns the output with the maximum intersecting area.
         *
         */
        Output *output_containing_rect(Rect rect);

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
        Output *get_output_next(direction_t direction, Output *current, output_close_far_t close_far);

        /*
         * (Re-)queries the outputs via RandR and stores them in the list of outputs.
         *
         * If no outputs are found use the root window.
         *
         */
        void randr_query_outputs();

        /**
         * Returns the active output which contains the midpoint of the given rect. If
         * such an output doesn't exist, returns the output which contains most of the
         * rectangle or NULL if there is no output which intersects with it.
         *
         */
        Output *get_output_from_rect(Rect rect);

        /**
         * Disables the output and moves its content.
         *
         */
        void randr_disable_output(Output *output);

        /*
         * We have just established a connection to the X server and need the initial
         * XRandR information to setup workspaces for each screen.
         *
         */
        RandR(X &x, ConfigurationManager &configManager, WorkspaceManager &workspaceManager, TreeManager &treeManager);

        virtual ~RandR() = default;
    };

    /**
     * Initializes a CT_OUTPUT Con (searches existing ones from inplace restart
     * before) to use for the given Output.
     *
     */
    void output_init_con(Output * output);

    /**
     * Initializes at least one workspace for this output, trying the following
     * steps until there is at least one workspace:
     *
     * • Move existing workspaces, which are assigned to be on the given output, to
     *   the output.
     * • Create the first assigned workspace for this output.
     * • Create the first unused workspace.
     *
     */
    void init_ws_for_output(Output * output);

    /**
     * Like get_output_next with close_far == CLOSEST_OUTPUT, but wraps.
     *
     * For example if get_output_next(D_DOWN, x, FARTHEST_OUTPUT) = NULL, then
     * get_output_next_wrap(D_DOWN, x) will return the topmost output.
     *
     * This function always returns a output: if no active outputs can be found,
     * current itself is returned.
     *
     */
    Output *get_output_next_wrap(direction_t direction, Output * current);

    /**
     * Creates an output covering the root window.
     *
     */
    Output *create_root_output(xcb_connection_t * conn);
}

// xcb.cppm
/* from X11/keysymdef.h */
export uint32_t const XCB_NUM_LOCK = 0xff7f;

/* The event masks are defined here because we don’t only set them once but we
   need to set slight variations of them (without XCB_EVENT_MASK_ENTER_WINDOW
   while rendering the layout) */
/** The XCB_CW_EVENT_MASK for the child (= real window) */
export uint32_t const CHILD_EVENT_MASK = (XCB_EVENT_MASK_PROPERTY_CHANGE |
                                          XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                          XCB_EVENT_MASK_FOCUS_CHANGE);

/** The XCB_CW_EVENT_MASK for its frame */
export uint32_t const FRAME_EVENT_MASK = (XCB_EVENT_MASK_BUTTON_PRESS |          /* …mouse is pressed/released */
                                          XCB_EVENT_MASK_BUTTON_RELEASE |
                                          XCB_EVENT_MASK_POINTER_MOTION |        /* …mouse is moved */
                                          XCB_EVENT_MASK_EXPOSURE |              /* …our window needs to be redrawn */
                                          XCB_EVENT_MASK_STRUCTURE_NOTIFY |      /* …the frame gets destroyed */
                                          XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | /* …the application tries to resize itself */
                                          XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |   /* …subwindows get notifies */
                                          XCB_EVENT_MASK_ENTER_WINDOW);          /* …user moves cursor inside our window */

export uint32_t const ROOT_EVENT_MASK = (XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT |
                                         XCB_EVENT_MASK_BUTTON_PRESS |
                                         XCB_EVENT_MASK_STRUCTURE_NOTIFY | /* when the user adds a screen (e.g. video \
                                                                            * projector), the root window gets a      \
                                                                            * ConfigureNotify */
                                         XCB_EVENT_MASK_POINTER_MOTION |
                                         XCB_EVENT_MASK_PROPERTY_CHANGE |
                                         XCB_EVENT_MASK_FOCUS_CHANGE |
                                         XCB_EVENT_MASK_ENTER_WINDOW);

export {
    /**
     * Convenience wrapper around xcb_create_window which takes care of depth,
     * generating an ID and checking for errors.
     *
     */
    xcb_window_t create_window(xcb_connection_t * conn, Rect dims,
                               uint16_t depth, xcb_visualid_t visual, uint16_t window_class,
                               enum xcursor_cursor_t cursor, bool map, uint32_t mask, uint32_t *values);

    /**
     * Generates a configure_notify_event with absolute coordinates (relative to
     * the X root window, not to the client’s frame) for the given client.
     *
     */
    void fake_absolute_configure_notify(Con * con);

    /**
     * Sends the WM_TAKE_FOCUS ClientMessage to the given window
     *
     */
    void send_take_focus(xcb_window_t window, xcb_timestamp_t timestamp);

    /**
     * Configures the given window to have the size/position specified by given rect
     *
     */
    void xcb_set_window_rect(xcb_connection_t * conn, xcb_window_t window, Rect r);

    /**
     * Returns the first supported _NET_WM_WINDOW_TYPE atom.
     *
     */
    xcb_atom_t xcb_get_preferred_window_type(xcb_get_property_reply_t * reply);

    /**
     * Returns true if the given reply contains the given data.
     *
     */
    bool xcb_reply_contains_atom(xcb_get_property_reply_t * prop, xcb_atom_t atom);

    /**
     * Get depth of visual specified by visualid
     *
     */
    uint16_t get_visual_depth(xcb_visualid_t visual_id);

    /**
     * Get visual type specified by visualid
     *
     */
    xcb_visualtype_t *get_visualtype_by_id(xcb_visualid_t visual_id);

    /**
     * Get visualid with specified depth
     *
     */
    xcb_visualid_t get_visualid_by_depth(uint16_t depth);

    /**
     * Add an atom to a list of atoms the given property defines.
     * This is useful, for example, for manipulating _NET_WM_STATE.
     *
     */
    void xcb_add_property_atom(xcb_connection_t * conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom);

    /**
     * Remove an atom from a list of atoms the given property defines without
     * removing any other potentially set atoms.  This is useful, for example, for
     * manipulating _NET_WM_STATE.
     *
     */
    void xcb_remove_property_atom(xcb_connection_t * conn, xcb_window_t window, xcb_atom_t property, xcb_atom_t atom);

    /**
     * Grab the specified buttons on a window when managing it.
     *
     */
    void xcb_grab_buttons(xcb_window_t window, std::set<int> & buttons);
}
// drag.cppm
export {
    /** Callback for dragging */
    using callback_t = std::function<void(Con *, Rect const &, uint32_t, uint32_t, xcb_button_press_event_t const *)>;
    
    /**
     * This is the return value of a drag operation like drag_pointer.
     *
     * DRAGGING will indicate the drag action is still in progress and can be
     * continued or resolved.
     *
     * DRAG_SUCCESS will indicate the intention of the drag action should be
     * carried out.
     *
     * DRAG_REVERT will indicate an attempt should be made to restore the state of
     * the involved windows to their condition before the drag.
     *
     * DRAG_ABORT will indicate that the intention of the drag action cannot be
     * carried out (e.g. because the window has been unmapped).
     *
     */
    enum drag_result_t {
        DRAGGING = 0,
        DRAG_SUCCESS,
        DRAG_REVERT,
        DRAG_ABORT
    };

    class InputManager {
      private:
        X &x;
        bool drain_drag_events(EV_P_ struct drag_x11_cb *dragloop);

      public:
        InputManager(X &x)
            : x(x) {};
        /**
         * This function grabs your pointer and keyboard and lets you drag stuff around
         * (borders). Every time you move your mouse, an XCB_MOTION_NOTIFY event will
         * be received and the given callback will be called with the parameters
         * specified (client, the original event), the original rect of the client,
         * and the new coordinates (x, y).
         *
         * If use_threshold is set, dragging only starts after the user moves the
         * pointer past a certain threshold. That is, the cursor will not be set and the
         * callback will not be called until then.
         *
         */
        drag_result_t drag_pointer(Con *con, xcb_button_press_event_t const *event,
                                   xcb_window_t confine_to, int cursor,
                                   bool use_threshold, callback_t callback);
    };
}
// dpi.cppm
export {
    /**
     * Initialize the DPI setting.
     * This will use the 'Xft.dpi' X resource if available and fall back to
     * guessing the correct value otherwise.
     */
    void init_dpi(xcb_connection_t * conn, xcb_screen_t * root_screen);
    
    /**
     * This function returns the value of the DPI setting.
     *
     */
    double get_dpi_value();

    /**
     * Convert a logical amount of pixels (e.g. 2 pixels on a “standard” 96 DPI
     * screen) to a corresponding amount of physical pixels on a standard or retina
     * screen, e.g. 5 pixels on a 227 DPI MacBook Pro 13" Retina screen.
     *
     */
    long logical_px(xcb_screen_t * root_screen, long logical);
}
// get_visualtype.cppm
/**
 * Returns the visual type associated with the given screen.
 *
 */
export xcb_visualtype_t *get_visualtype(xcb_screen_t *screen) {
    xcb_depth_iterator_t depth_iter;
    for (depth_iter = xcb_screen_allowed_depths_iterator(screen);
         depth_iter.rem;
         xcb_depth_next(&depth_iter)) {
        xcb_visualtype_iterator_t visual_iter;
        for (visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
             visual_iter.rem;
             xcb_visualtype_next(&visual_iter)) {
            if (screen->root_visual == visual_iter.data->visual_id) {
                return visual_iter.data;
            }
        }
    }
    return nullptr;
}
// ewmh.cppm
export {
    /**
     * Updates all the EWMH desktop properties.
     *
     */
    void ewmh_update_desktop_properties();
    
    /**
     * Updates _NET_CURRENT_DESKTOP with the current desktop number.
     *
     * EWMH: The index of the current desktop. This is always an integer between 0
     * and _NET_NUMBER_OF_DESKTOPS - 1.
     *
     */
    void ewmh_update_current_desktop();

    /**
     * Updates _NET_WM_DESKTOP for all windows.
     * A request will only be made if the cached value differs from the calculated value.
     *
     */
    void ewmh_update_wm_desktop();

    /**
     * Updates _NET_ACTIVE_WINDOW with the currently focused window.
     *
     * EWMH: The window ID of the currently active window or None if no window has
     * the focus.
     *
     */
    void ewmh_update_active_window(xcb_window_t window);

    /**
     * Updates _NET_WM_VISIBLE_NAME.
     *
     */
    void ewmh_update_visible_name(xcb_window_t window, char const *name);

    /**
     * Updates the _NET_CLIENT_LIST hint. Used for window listers.
     */
    void ewmh_update_client_list(xcb_window_t * list, int num_windows);

    /**
     * Updates the _NET_CLIENT_LIST_STACKING hint. Necessary to move tabs in
     * Chromium correctly.
     *
     * EWMH: These arrays contain all X Windows managed by the Window Manager.
     * _NET_CLIENT_LIST has initial mapping order, starting with the oldest window.
     * _NET_CLIENT_LIST_STACKING has bottom-to-top stacking order. These properties
     * SHOULD be set and updated by the Window Manager.
     *
     */
    void ewmh_update_client_list_stacking(xcb_window_t * stack, int num_windows);

    /**
     * Set or remove _NET_WM_STATE_STICKY on the window.
     *
     */
    void ewmh_update_sticky(xcb_window_t window, bool sticky);

    /**
     * Set or remove _NEW_WM_STATE_FOCUSED on the window.
     *
     */
    void ewmh_update_focused(xcb_window_t window, bool is_focused);

    /**
     * Set up the EWMH hints on the root window.
     *
     */
    void ewmh_setup_hints();

    /**
     * i3 currently does not support _NET_WORKAREA, because it does not correspond
     * to i3’s concept of workspaces. See also:
     * https://bugs.i3wm.org/539
     * https://bugs.i3wm.org/301
     * https://bugs.i3wm.org/1038
     *
     * We need to actively delete this property because some display managers (e.g.
     * LightDM) set it.
     *
     * EWMH: Contains a geometry for each desktop. These geometries specify an area
     * that is completely contained within the viewport. Work area SHOULD be used by
     * desktop applications to place desktop icons appropriately.
     *
     */
    void ewmh_update_workarea();

    /**
     * Returns the workspace container as enumerated by the EWMH desktop model.
     * Returns NULL if no workspace could be found for the index.
     *
     * This is the reverse of ewmh_get_workspace_index.
     *
     */
    WorkspaceCon *ewmh_get_workspace_by_index(uint32_t idx);

    /**
     * Returns the EWMH desktop index for the workspace the given container is on.
     * Returns NET_WM_DESKTOP_NONE if the desktop index cannot be determined.
     *
     * This is the reverse of ewmh_get_workspace_by_index.
     *
     */
    uint32_t ewmh_get_workspace_index(Con * con);
}
// floating.cppm
export {
    /** On which border was the dragging initiated? */
    enum border_t {
        BORDER_LEFT = (1 << 0),
        BORDER_RIGHT = (1 << 1),
        BORDER_TOP = (1 << 2),
        BORDER_BOTTOM = (1 << 3)
    };
    
    /**
     * Enables floating mode for the given container by detaching it from its
     * parent, creating a new container around it and storing this container in the
     * floating_windows list of the workspace.
     *
     */
    bool floating_enable(ConCon * con, bool automatic);

    /**
     * Disables floating mode for the given container by re-attaching the container
     * to its old parent.
     *
     */
    void floating_disable(ConCon * con);

    /**
     * Calls floating_enable() for tiling containers and floating_disable() for
     * floating containers.
     *
     * If the automatic flag is set to true, this was an automatic update by a
     * change of the window class from the application which can be overwritten by
     * the user.
     *
     */
    void toggle_floating_mode(ConCon * con, bool automatic);

    /**
     * Raises the given container in the list of floating containers
     *
     */
    void floating_raise_con(FloatingCon * con);

    /**
     * Checks if con’s coordinates are within its workspace and re-assigns it to
     * the actual workspace if not.
     *
     */
    bool floating_maybe_reassign_ws(Con * con);

    /**
     * Centers a floating con above the specified rect.
     *
     */
    void floating_center(Con * con, Rect rect);

    /**
     * Moves the given floating con to the current pointer position.
     *
     */
    void floating_move_to_pointer(Con * con);

    /**
     * Called when a floating window is created or resized.  This function resizes
     * the window if its size is higher or lower than the configured maximum/minimum
     * size, respectively or when adjustments are needed to conform to the
     * configured size increments or aspect ratio limits.
     *
     * When prefer_height is true and the window needs to be resized because of the
     * configured aspect ratio, the width is adjusted first, preserving the previous
     * height.
     *
     */
    void floating_check_size(Con * floating_con, bool prefer_height);

    /**
     * Repositions the CT_FLOATING_CON to have the coordinates specified by
     * newrect, but only if the coordinates are not out-of-bounds. Also reassigns
     * the floating con to a different workspace if this move was across different
     * outputs.
     *
     */
    bool floating_reposition(Con * con, Rect newrect);

    /**
     * Sets size of the CT_FLOATING_CON to specified dimensions. Might limit the
     * actual size with regard to size constraints taken from user settings.
     * Additionally, the dimensions may be upscaled until they're divisible by the
     * window's size hints.
     *
     */
    void floating_resize(Con * floating_con, uint32_t x, uint32_t y);

    /**
     * Fixes the coordinates of the floating window whenever the window gets
     * reassigned to a different output (or when the output’s rect changes).
     *
     */
    void floating_fix_coordinates(FloatingCon * con, Rect const &old_rect, Rect const &new_rect);
}
// move.cppm
export {
    /**
     * Moves the given container in the given direction
     *
     */
    void tree_move(Con * con, direction_t direction);
    
    /**
     * This function detaches 'con' from its parent and inserts it either before or
     * after 'target'.
     *
     */
    void insert_con_into(Con * con, Con * target, position_t position);
}
// sighandler.cppm
export {
    /**
     * Configured a signal handler to gracefully handle crashes and allow the user
     * to generate a backtrace and rescue their session.
     *
     */
    void setup_signal_handler();
}
// ipc.cppm
export {
    /**
     * Calls to ipc_shutdown() should provide a reason for the shutdown.
     */
    enum class shutdown_reason_t {
        SHUTDOWN_REASON_RESTART,
        SHUTDOWN_REASON_EXIT
    };
    
    class ipc_client {
      public:
        using callback = void (*)(ev_loop *loop, ev_io *w, int revents);
        int fd;

        /* The events which this client wants to receive */
        std::vector<std::string> events{};

        /* For clients which subscribe to the tick event: whether the first tick
         * event has been sent by i3. */
        bool first_tick_sent{};

        ev_io *read_callback;
        ev_io *write_callback;
        ev_timer *timeout{};
        uint8_t *buffer{};
        size_t buffer_size{};

        ipc_client() = delete;
        ipc_client(ipc_client const &) = delete;
        ipc_client operator=(ipc_client const &) = delete;

        ipc_client(int fd, callback read_callback_t, callback write_callback_t);
        ~ipc_client();

        bool operator==(ipc_client const &o) {
            return this->fd == o.fd;
        }

        bool operator!=(ipc_client const &o) {
            return this->fd != o.fd;
        }
    };

    class IPCManager {
        friend void ipc_receive_message(EV_P_ ev_io *w, int revents);
        friend void ipc_client_timeout(EV_P_ ev_timer *w, int revents);

      private:
        std::vector<std::unique_ptr<ipc_client>> all_clients{};
        ev_io *ipc_io;
        ConfigurationManager &configManager;
        std::tuple<std::string, int> create_socket(std::string filename);
        void ipc_send_shutdown_event(shutdown_reason_t reason);
        void free_ipc_client(ipc_client &client, int exempt_fd = -1);

      public:
        std::optional<std::string> current_socketpath{};
        int ipc_socket{-1};
        explicit IPCManager(ConfigurationManager &configManager);
        ~IPCManager();
        IPCManager(IPCManager const &) = delete;
        void operator=(IPCManager const &) = delete;
        IPCManager(IPCManager &&other) = delete;
        void operator=(IPCManager &&) = delete;

        int create_socket();
        /* Listen to the IPC socket for clients */
        void listen();
        /**
         * ipc_new_client_on_fd() only sets up the event handler
         * for activity on the new connection and inserts the file descriptor into
         * the list of clients.
         *
         * This variant is useful for the inherited IPC connection when restarting.
         *
         */
        ipc_client *ipc_new_client_on_fd(int fd);

        /**
         * Sends the specified event to all IPC clients which are currently connected
         * and subscribed to this kind of event.
         *
         */
        void ipc_send_event(std::string const event, uint32_t message_type, std::string const &payload);

        /**
         * Calls shutdown() on each socket and closes it. This function is to be called
         * when exiting or restarting only!
         *
         * exempt_fd is never closed. Set to -1 to close all fds.
         *
         */
        void ipc_shutdown(shutdown_reason_t reason, int exempt_fd = -1);

        /**
         * For the workspace events we send, along with the usual "change" field, also
         * the workspace container in "current". For focus events, we send the
         * previously focused workspace in "old".
         */
        void ipc_send_workspace_event(char const *change, Con *current, Con *old);

        /**
         * For the window events we send, along the usual "change" field,
         * also the window container, in "container".
         */
        void ipc_send_window_event(char const *property, Con *con);

        /**
         * For the binding events, we send the serialized binding struct.
         */
        void ipc_send_binding_event(char const *event_type, Binding *bind, char const *modename);
    };

    /*
     * Callback type for the different message types.
     *
     * message is the raw packet, as received from the UNIX domain socket. size
     * is the remaining size of bytes for this packet.
     *
     * message_size is the size of the message as the sender specified it.
     * message_type is the type of the message as the sender specified it.
     *
     */
    using handler_t = void (*)(ipc_client *, uint8_t *, int, uint32_t, uint32_t);

    /* Macro to declare a callback */

    /**
     * Generates a json workspace event. Returns a dynamically allocated yajl
     * generator. Free with yajl_gen_free().
     */
    nlohmann::json ipc_marshal_workspace_event(char const *change, Con *current, Con *old);

    /**
     * For the barconfig update events, we send the serialized barconfig.
     */
    //void ipc_send_barconfig_update_event(Barconfig * barconfig);

    /**
     * Set the maximum duration that we allow for a connection with an unwriteable
     * socket.
     */
    void ipc_set_kill_timeout(double newTimeout);

    /**
     * Sends a restart reply to the IPC client on the specified fd.
     */
    void ipc_confirm_restart(ipc_client * client);
}
// load_layout.cppm
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
// restore_layout.cppm
export {
    /**
     * Opens a separate connection to X11 for placeholder windows when restoring
     * layouts. This is done as a safety measure (users can xkill a placeholder
     * window without killing their window manager) and for better isolation, both
     * on the wire to X11 and thus also in the code.
     *
     */
    void restore_connect();
    
    /**
     * Open placeholder windows for all children of parent. The placeholder window
     * will vanish as soon as a real window is swallowed by the container. Until
     * then, it exposes the criteria that must be fulfilled for a window to be
     * swallowed by this container.
     *
     */
    void restore_open_placeholder_windows(Con * con);

    /**
     * Kill the placeholder window, if placeholder refers to a placeholder window.
     * This function is called when manage.c puts a window into an existing
     * container. In order not to leak resources, we need to destroy the window and
     * all associated X11 objects (pixmap/gc).
     *
     */
    bool restore_kill_placeholder(xcb_window_t placeholder);
}
// root_atom_contents.cppm
/**
 * Try to get the contents of the given atom (for example I3_SOCKET_PATH) from
 * the X11 root window and return NULL if it doesn’t work.
 *
 * If the provided XCB connection is NULL, a new connection will be
 * established.
 *
 */
export std::optional<std::string> root_atom_contents(char const *atomname, xcb_connection_t *provided_conn, int screen) {
    xcb_intern_atom_cookie_t atom_cookie;
    xcb_intern_atom_reply_t *atom_reply;
    char *content = nullptr;
    size_t content_max_words = 256;
    xcb_connection_t *conn = provided_conn;
    
    if (provided_conn == nullptr &&
        ((conn = xcb_connect(nullptr, &screen)) == nullptr ||
         xcb_connection_has_error(conn))) {
        return std::nullopt;
    }

    atom_cookie = xcb_intern_atom(conn, 0, std::strlen(atomname), atomname);

    xcb_screen_t *root_screen = xcb_aux_get_screen(conn, screen);
    xcb_window_t root = root_screen->root;

    atom_reply = xcb_intern_atom_reply(conn, atom_cookie, nullptr);
    if (atom_reply == nullptr) {
        goto out_conn;
    }

    xcb_get_property_cookie_t prop_cookie;
    xcb_get_property_reply_t *prop_reply;
    prop_cookie = xcb_get_property_unchecked(conn, false, root, atom_reply->atom,
                                             XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
    prop_reply = xcb_get_property_reply(conn, prop_cookie, nullptr);
    if (prop_reply == nullptr) {
        goto out_atom;
    }
    if (xcb_get_property_value_length(prop_reply) > 0 && prop_reply->bytes_after > 0) {
        /* We received an incomplete value. Ask again but with a properly
         * adjusted size. */
        content_max_words += std::ceil(prop_reply->bytes_after / 4.0);
        /* Repeat the request, with adjusted size */
        std::free(prop_reply);
        prop_cookie = xcb_get_property_unchecked(conn, false, root, atom_reply->atom,
                                                 XCB_GET_PROPERTY_TYPE_ANY, 0, content_max_words);
        prop_reply = xcb_get_property_reply(conn, prop_cookie, nullptr);
        if (prop_reply == nullptr) {
            goto out_atom;
        }
    }
    if (xcb_get_property_value_length(prop_reply) == 0) {
        goto out;
    }
    if (prop_reply->type == XCB_ATOM_CARDINAL) {
        /* We treat a CARDINAL as a >= 32-bit unsigned int. The only CARDINAL
         * we query is I3_PID, which is 32-bit. */
        sasprintf(&content, "%u", *((unsigned int *)xcb_get_property_value(prop_reply)));
    } else {
        sasprintf(&content, "%.*s", xcb_get_property_value_length(prop_reply),
                  (char *)xcb_get_property_value(prop_reply));
    }

out:
    std::free(prop_reply);
out_atom:
    std::free(atom_reply);
out_conn:
    if (provided_conn == nullptr) {
        xcb_disconnect(conn);
    }
    return std::make_optional<std::string>(content);
}
// nagbar.cppm
export {
    enum class bar_type_t {
        TYPE_ERROR,
        TYPE_WARNING
    };
    
    class button_t {
      public:
        std::string label{};
        std::string action{};
        int16_t x{};
        uint16_t width{};
        bool terminal{false};

        button_t() = default;

        button_t(std::string label, std::string action, bool terminal)
            : label(std::move(label)), action(std::move(action)), terminal(terminal) {
        }
    };

    /**
     * Starts an i3-nagbar instance with the given parameters. Takes care of
     * handling SIGCHLD and killing i3-nagbar when i3 exits.
     *
     * The resulting PID will be stored in *nagbar_pid and can be used with
     * kill_nagbar() to kill the bar later on.
     *
     */
    void start_nagbar(pid_t * nagbar_pid,
                      std::vector<button_t> buttons,
                      std::string prompt,
                      std::string pattern,
                      bar_type_t bar_type,
                      bool position_on_primary);

    /**
     * Kills the i3-nagbar process, if nagbar_pid != -1.
     *
     * If wait_for_it is set (restarting i3), this function will waitpid(),
     * otherwise, ev is assumed to handle it (reloading).
     *
     */
    void kill_nagbar(pid_t nagbar_pid, bool wait_for_it);
}
// manage.cppm
export {
    /**
     * Go through all existing windows (if the window manager is restarted) and
     * manage them
     *
     */
    void manage_existing_windows(xcb_window_t root);
    
    /**
     * Restores the geometry of each window by reparenting it to the root window
     * at the position of its frame.
     *
     * This is to be called *only* before exiting/restarting i3 because of evil
     * side-effects which are to be expected when continuing to run i3.
     *
     */
    void restore_geometry();

    /**
     * Do some sanity checks and then reparent the window.
     *
     */
    void manage_window(xcb_window_t window, xcb_visualid_t visual);

    /**
     * Remanages a window: performs a swallow check and runs assignments.
     * Returns con for the window regardless if it updated.
     *
     */
    ConCon *remanage_window(ConCon * con);
}
// tree.cppm
export {
    class TreeManager {
        X &x;
        
      public:
        TreeManager(X &x);
    };

    /**
     * Initializes the Tree by creating the root node, adding all RandR outputs
     * to the Tree (that means randr_init() has to be called before) and
     * assigning a workspace to each RandR output.
     *
     */
    void tree_init(xcb_get_geometry_reply_t const *geometry);

    /**
     * Opens an empty container in the current container
     *
     */
    ConCon *tree_open_con(Con * con, i3Window *window = nullptr);

    /**
     * Splits (horizontally or vertically) the given container by creating a new
     * container which contains the old one and the future ones.
     *
     */
    void tree_split(Con * con, orientation_t orientation);

    /**
     * Moves focus one level up. Returns true if focus changed.
     *
     */
    bool level_up();

    /**
     * Moves focus one level down. Returns true if focus changed.
     *
     */
    bool level_down();

    /**
     * Renders the Tree, that is rendering all outputs using render_con() and
     * pushing the changes to X11 using x_push_changes().
     *
     */
    void tree_render();

    /**
     * Changes focus in the given direction
     *
     */
    void tree_next(Con * con, direction_t direction);

    /**
     * Get the previous / next sibling
     *
     */
    Con *get_tree_next_sibling(Con * con, position_t direction);

    /**
     * Closes the given container including all children.
     * Returns true if the container was killed or false if just WM_DELETE was sent
     * and the window is expected to kill itself.
     *
     * The dont_kill_parent flag is specified when the function calls itself
     * recursively while deleting a containers children.
     *
     */
    bool tree_close_internal(Con * con, kill_window_t kill_window, bool dont_kill_parent);

    /**
     * Loads Tree from ~/.i3/_restart.json (used for in-place restarts).
     *
     */
    bool tree_restore(std::filesystem::path const path, xcb_get_geometry_reply_t const *geometry);

    /**
     * tree_flatten() removes pairs of redundant split containers, e.g.:
     *       [workspace, horizontal]
     *   [v-split]           [child3]
     *   [h-split]
     * [child1] [child2]
     * In this example, the v-split and h-split container are redundant.
     * Such a situation can be created by moving containers in a direction which is
     * not the orientation of their parent container. i3 needs to create a new
     * split container then and if you move containers this way multiple times,
     * redundant chains of split-containers can be the result.
     *
     */
    void tree_flatten(Con * child);
}
// commands.cppm
export {
    /**
     * Parses and executes the given command. If a caller-allocated yajl_gen is
     * passed, a json reply will be generated in the format specified by the ipc
     * protocol. Pass NULL if no json reply is required.
     *
     * Free the returned CommandResult with command_result_free().
     */
    CommandResult parse_command(std::string const &input, BaseCommandsApplier &applier);
}
// is_background_set.cppm
/**
 * Find the region in the given window that is not covered by a mapped child
 * window.
 */
static cairo_region_t *unobscured_region(x_connection *conn, xcb_window_t window,
                                         uint16_t window_width, uint16_t window_height) {
    cairo_rectangle_int_t rectangle;
    cairo_region_t *region;
    
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = window_width;
    rectangle.height = window_height;
    region = cairo_region_create_rectangle(&rectangle);

    xcb_query_tree_reply_t *tree = xcb_query_tree_reply(*conn, xcb_query_tree_unchecked(*conn, window), nullptr);
    if (!tree) {
        return region;
    }

    /* Get information about children */
    uint16_t const n_children = tree->children_len;
    xcb_window_t *children = xcb_query_tree_children(tree);

    std::vector<xcb_get_geometry_cookie_t> geometries{};
    geometries.reserve(n_children);
    std::vector<xcb_get_window_attributes_cookie_t> attributes{};
    attributes.reserve(n_children);

    for (int i = 0; i < n_children; i++) {
        geometries.push_back(xcb_get_geometry_unchecked(*conn, children[i]));
        attributes.push_back(xcb_get_window_attributes_unchecked(*conn, children[i]));
    }

    /* Remove every visible child from the region */
    for (int i = 0; i < n_children; i++) {
        xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(*conn, geometries.at(i), nullptr);
        xcb_get_window_attributes_reply_t *attr = xcb_get_window_attributes_reply(*conn, attributes.at(i), nullptr);

        if (geom && attr && attr->map_state == XCB_MAP_STATE_VIEWABLE) {
            rectangle.x = geom->x;
            rectangle.y = geom->y;
            rectangle.width = geom->width;
            rectangle.height = geom->height;
            cairo_region_subtract_rectangle(region, &rectangle);
        }

        free(geom);
        free(attr);
    }

    free(tree);
    return region;
}

static void find_unobscured_pixel(x_connection *conn, xcb_window_t window,
                                  uint16_t window_width, uint16_t window_height,
                                  uint16_t *x, uint16_t *y) {
    cairo_region_t *region = unobscured_region(conn, window, window_width, window_height);
    if (cairo_region_num_rectangles(region) > 0) {
        /* Return the top left pixel of the first rectangle */
        cairo_rectangle_int_t rect;
        cairo_region_get_rectangle(region, 0, &rect);
        *x = rect.x;
        *y = rect.y;
    } else {
        /* No unobscured area found */
        *x = 0;
        *y = 0;
    }
    cairo_region_destroy(region);
}

static uint32_t flicker_window_at(x_connection *conn, xcb_screen_t *screen, uint16_t x, uint16_t y, xcb_window_t window,
                                  uint32_t pixel) {
    uint32_t value_list[]{pixel, 1};
    conn->create_window(XCB_COPY_FROM_PARENT, window, screen->root, x, y, 10, 10,
                        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, XCB_COPY_FROM_PARENT,
                        XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT, value_list);
    conn->map_window(window);
    conn->clear_area(0, window, 0, 0, 0, 0);
    xcb_aux_sync(*conn);
    conn->destroy_window(window);

    auto img = conn->get_image_unchecked(XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root, x, y, 1, 1, ~0);
    uint32_t result = 0;
    if (img) {
        uint8_t *data = xcb_get_image_data(img.get().get());
        uint8_t depth = img->depth;
        for (int i = 0; i < std::min(depth, (uint8_t)4); i++) {
            result = (result << 8) | data[i];
        }
    }
    return result;
}

export bool is_background_set(x_connection *conn, xcb_screen_t *screen) {
    uint16_t x, y;
    find_unobscured_pixel(conn, screen->root, screen->width_in_pixels, screen->height_in_pixels, &x, &y);

    xcb_window_t window = conn->generate_id();

    uint32_t pixel1 = flicker_window_at(conn, screen, x, y, window, screen->black_pixel);
    uint32_t pixel2 = flicker_window_at(conn, screen, x, y, window, screen->white_pixel);
    return pixel1 == pixel2;
}
// resource_database.cppm
export class ResourceDatabase : public BaseResourceDatabase {
    xcb_xrm_database_t *database = nullptr;
    xcb_connection_t *conn;
    
  public:
    explicit ResourceDatabase(xcb_connection_t *conn)
        : conn(conn) {
        assert(conn != nullptr);
    }

    std::string get_resource(std::string const name, std::string const fallback) override {
        /* Load the resource database lazily. */
        if (database == nullptr) {
            database = xcb_xrm_database_from_default(conn);

            if (database == nullptr) {
                ELOG("Failed to open the resource database.\n");

                /* Load an empty database so we don't keep trying to load the
                 * default database over and over again. */
                database = xcb_xrm_database_from_string("");

                return fallback.data();
            }
        }

        char *resource;
        xcb_xrm_resource_get_string(database, name.c_str(), nullptr, &resource);

        if (resource == nullptr) {
            DLOG(fmt::sprintf("Could not get resource '%s', using fallback '%s'.\n", name, fallback));
            return fallback;
        }

        std::string resource_str = resource;

        free(resource);

        return resource_str;
    }

    ~ResourceDatabase() override {
        if (database != nullptr) {
            xcb_xrm_database_free(database);
        }
    }
};
// config_applier.cppm
/*******************************************************************************
 * Include functions.
 ******************************************************************************/
export class ConfigApplier : public BaseConfigApplier {
  private:
    std::string current_mode{};
    bool current_mode_pango_markup;
    Config *config;
    
  public:
    ConfigApplier(Config *config)
        : config(config) {};
    criteria_state *criteria_create(int _state) override;

    void criteria_init(criteria_state *cs, int _state) override;

    int criteria_pop_state(criteria_state *cs) override;

    void criteria_add(criteria_state *cs, char const *ctype, char const *cvalue) override;

    void font(std::string const &font) override;

    void binding(std::string const &bindtype, std::string const &modifiers,
                 std::string const &key, bool release, bool border, bool whole_window,
                 bool exclude_titlebar, std::string const &command) override;

    void mode_binding(std::string const &bindtype, std::string const &modifiers,
                      std::string const &key, bool release, bool border, bool whole_window,
                      bool exclude_titlebar, std::string const &command) override;

    void enter_mode(bool pango_markup, std::string const &modename) override;

    void exec(std::string const &exectype, bool no_startup_id,
              std::string const &command) override;

    void gaps(std::string const &workspace, std::string const &type, long const value) override;

    void smart_borders(std::string const &enable) override;

    void smart_gaps(std::string const &enable) override;

    void for_window(criteria_state *criteria_state, std::string const &command) override;

    void floating_minimum_size(long width, long height) override;

    void floating_maximum_size(long width, long height) override;

    void floating_modifier(std::string const &modifiers) override;
    
    void tiling_drag_swap_modifier(std::string const &modifiers) override;

    void default_orientation(std::string const &orientation) override;

    void workspace_layout(std::string const &layout) override;

    void default_border(std::string const &windowtype, std::string const &border, long width) override;

    void hide_edge_borders(std::string const &borders) override;

    void focus_follows_mouse(std::string const &value) override;

    void mouse_warping(std::string const &value) override;

    void focus_wrapping(std::string const &value) override;

    void force_focus_wrapping(std::string const &value) override;

    void workspace_back_and_forth(std::string const &value) override;

    void force_display_urgency_hint(long duration_ms) override;

    void focus_on_window_activation(std::string const &mode) override;

    void title_align(std::string const &alignment) override;

    void workspace(std::string const &workspace, std::string const &output) override;

    void ipc_socket(std::string const &path) override;

    void restart_state(std::string const &path) override;

    void popup_during_fullscreen(std::string const &value) override;

    void color_single(std::string const &colorclass, std::string const &color) override;

    void color(std::string const &colorclass, std::string const &border, std::string const &background, std::string const &text, std::string const &indicator, std::string const &child_border) override;

    void assign_output(criteria_state *criteria_state, std::string const &output) override;

    void assign(criteria_state *criteria_state, std::string const &workspace, bool is_number) override;

    void no_focus(criteria_state *criteria_state) override;

    void ipc_kill_timeout(long timeout_ms) override;

    void tiling_drag(std::string const &value) override;

    void bar_height(long const height) override;

    void bar_padding_one(long const all) override;

    void bar_padding_two(long const top_and_bottom, long const right_and_left) override;

    void bar_padding_three(long const top, long const right_and_left, long const bottom) override;

    void bar_padding_four(long const top, long const right, long const bottom, long const left) override;
};
// commands_applier.cppm
export class CommandsApplier : public BaseCommandsApplier {
  private:
    ipc_client *client;
    nlohmann::json *json_gen;

    bool cmd_resize_tiling_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, Con *current, char const *direction, int px, int ppt);
    bool cmd_resize_tiling_width_height(criteria_state *criteria_state, CommandsResultIR &cmd_output, Con *current, char const *direction, int px, double ppt);
    bool resize_set_tiling(criteria_state *criteria_state, CommandsResultIR &cmd_output, Con *target, orientation_t resize_orientation, bool is_ppt, long target_size);
    
  public:
    CommandsApplier() : client(nullptr), json_gen(nullptr) {};
    
    CommandsApplier(ipc_client *client, nlohmann::json *json_gen)
        : client(client), json_gen(json_gen) {};

    criteria_state *create_criteria_state(int _state) override;

    void append_json(nlohmann::json j) override;
    /*
     * Initializes the specified 'Match' data structure and the initial state of
     * commands.c for matching target windows of a command.
     *
     */
    void criteria_init(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * A match specification just finished (the closing square bracket was found),
     * so we filter the list of owindows.
     *
     */
    void criteria_match_windows(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Interprets a ctype=cvalue pair and adds it to the current match
     * specification.
     *
     */
    void criteria_add(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *ctype, char const *cvalue) override;

    /*
     * Implementation of 'move [window|container] [to] workspace
     * next|prev|next_on_output|prev_on_output|current'.
     *
     */
    void move_con_to_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) override;

    /*
     * Implementation of 'move [window|container] [to] workspace back_and_forth'.
     *
     */
    void move_con_to_workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace <name>'.
     *
     */
    void move_con_to_workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *no_auto_back_and_forth) override;

    /*
     * Implementation of 'move [--no-auto-back-and-forth] [window|container] [to] workspace number <name>'.
     *
     */
    void move_con_to_workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *no_auto_back_and_forth) override;

    /*
     * Implementation of 'resize grow|shrink <direction> [<px> px] [or <ppt> ppt]'.
     *
     */
    void resize(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *way, char const *direction, long resize_px, long resize_ppt) override;

    /*
     * Implementation of 'resize set <width> [px | ppt] <height> [px | ppt]'.
     *
     */
    void resize_set(criteria_state *criteria_state, CommandsResultIR &cmd_output, long cwidth, char const *mode_width, long cheight, char const *mode_height) override;

    /*
     * Implementation of 'border normal|pixel [<n>]', 'border none|1pixel|toggle'.
     *
     */
    void border(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *border_style_str, long border_width) override;

    /*
     * Implementation of 'nop <comment>'.
     *
     */
    void nop(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment) override;

    /*
     * Implementation of 'append_layout <path>'.
     *
     */
    void append_layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *cpath) override;

    /*
     * Implementation of 'workspace next|prev|next_on_output|prev_on_output'.
     *
     */
    void workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] number <name>'
     *
     */
    void workspace_number(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *which, char const *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'workspace back_and_forth'.
     *
     */
    void workspace_back_and_forth(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'workspace [--no-auto-back-and-forth] <name>'
     *
     */
    void workspace_name(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, char const *_no_auto_back_and_forth) override;

    /*
     * Implementation of 'mode <string>'.
     *
     */
    void mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *mode) override;

    /*
     * Implementation of 'move [window|container|workspace] [to] output <strings>'.
     *
     */
    void move_con_to_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name, bool move_workspace) override;

    /*
     * Implementation of 'floating enable|disable|toggle'
     *
     */
    void floating(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *floating_mode) override;

    /*
     * Implementation of 'split v|h|t|vertical|horizontal|toggle'.
     *
     */
    void split(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction) override;

    /*
     * Implementation of 'kill [window|client]'.
     *
     */
    void kill(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *kill_mode_str) override;

    /*
     * Implementation of 'exec [--no-startup-id] <command>'.
     *
     */
    void exec(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *nosn, char const *command) override;

    /*
     * Implementation of 'focus left|right|up|down|next|prev'.
     *
     */
    void focus_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) override;

    /*
     * Implementation of 'focus next|prev sibling'
     *
     */
    void focus_sibling(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str) override;

    /*
     * Implementation of 'focus tiling|floating|mode_toggle'.
     *
     */
    void focus_window_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *window_mode) override;

    /*
     * Implementation of 'focus parent|child'.
     *
     */
    void focus_level(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *level) override;

    /*
     * Implementation of 'focus'.
     *
     */
    void focus(criteria_state *criteria_state, CommandsResultIR &cmd_output, bool focus_workspace) override;

    /*
     * Implementation of 'fullscreen enable|toggle [global]' and
     *                   'fullscreen disable'
     *
     */
    void fullscreen(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action, char const *fullscreen_mode) override;

    /*
     * Implementation of 'sticky enable|disable|toggle'.
     *
     */
    void sticky(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *action) override;

    /*
     * Implementation of 'move <direction> [<amount> [px|ppt]]'.
     *
     */
    void move_direction(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *direction_str, long amount, char const *mode) override;

    /*
     * Implementation of 'layout default|stacked|stacking|tabbed|splitv|splith'.
     *
     */
    void layout(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *layout_str) override;

    /*
     * Implementation of 'layout toggle [all|split]'.
     *
     */
    void layout_toggle(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *toggle_mode) override;

    /*
     * Implementation of 'exit'.
     *
     */
    void exit(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'reload'.
     *
     */
    void reload(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'restart'.
     *
     */
    void restart(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'open'.
     *
     */
    void open(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'focus output <output>'.
     *
     */
    void focus_output(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *name) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position [<pos_x> [px|ppt] <pos_y> [px|ppt]]
     *
     */
    void move_window_to_position(criteria_state *criteria_state, CommandsResultIR &cmd_output, long x, char const *mode_x, long y, char const *mode_y) override;

    /*
     * Implementation of 'move [window|container] [to] [absolute] position center
     *
     */
    void move_window_to_center(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *method) override;

    /*
     * Implementation of 'move [window|container] [to] position mouse'
     *
     */
    void move_window_to_mouse(criteria_state *criteria_state, CommandsResultIR &cmd_output) override;

    /*
     * Implementation of 'title_format <format>'
     *
     */
    void title_format(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *format) override;

    /*
     * Implementation of 'title_window_icon <yes|no|toggle>' and 'title_window_icon <padding|toggle> <px>'
     *
     */
    void title_window_icon(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *enable, int padding) override;

    /*
     * Implementation of 'rename workspace [<name>] to <name>'
     *
     */
    void rename_workspace(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *old_name, char const *new_name) override;

    /*
     * Implementation of 'bar mode dock|hide|invisible|toggle [<bar_id>]'
     *
     */
    void bar_mode(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_mode, char const *bar_id) override;

    /*
     * Implementation of 'bar hidden_state hide|show|toggle [<bar_id>]'
     *
     */
    void bar_hidden_state(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *bar_hidden_state, char const *bar_id) override;

    /*
     * Implementation of 'debuglog toggle|on|off'
     *
     */
    void debuglog(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *argument) override;

    /*
     * Implementation of 'nagbar'
     *
     */
    void nagbar(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *message, char const *font, char const *primary, char const *labels, char const *actions, char const *terminals) override;

    /**
     * Implementation of 'gaps inner|outer|top|right|bottom|left|horizontal|vertical current|all set|plus|minus|toggle <px>'
     *
     */
    void gaps(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *type, char const *scope, char const *mode, char const *value) override;
};

// handle_property.cppm
export namespace handle_property {
    
    struct window_class {
        std::string class_name{};
        std::string class_instance{};
    };

    struct window_hints {
        bool doesnt_accept_focus;
        bool urgency_hint;
    };

    /*
     * Updates the WM_CLASS (consisting of the class and instance) for the
     * given window.
     *
     */
    std::optional<window_class> window_update_class(xcb_get_property_reply_t* prop);

    /*
     * Updates the name by using _NET_WM_NAME (encoded in UTF-8) for the given
     * window. Further updates using window_update_name_legacy will be ignored.
     *
     */
    std::optional<std::string> window_update_name(xcb_get_property_reply_t* prop);

    /*
     * Updates the name by using WM_NAME (encoded in COMPOUND_TEXT). We do not
     * touch what the client sends us but pass it to xcb_image_text_8. To get
     * proper unicode rendering, the application has to use _NET_WM_NAME (see
     * window_update_name()).
     *
     */
    std::optional<std::string> window_update_name_legacy(xcb_get_property_reply_t* prop);

    /*
     * Updates the CLIENT_LEADER (logical parent window).
     *
     */
    std::optional<xcb_window_t> window_update_leader(xcb_get_property_reply_t* prop);

    /*
     * Updates the TRANSIENT_FOR (logical parent window).
     *
     */
    std::optional<xcb_window_t> window_update_transient_for(xcb_get_property_reply_t* prop);

    /*
     * Updates the _NET_WM_STRUT_PARTIAL (reserved pixels at the screen edges)
     *
     */
    std::optional<reservedpx> window_update_strut_partial(xcb_get_property_reply_t* prop);

    /*
     * Updates the WM_WINDOW_ROLE
     *
     */
    std::optional<std::string> window_update_role(xcb_get_property_reply_t* prop);

    /*
     * Updates the _NET_WM_WINDOW_TYPE property.
     *
     */
    std::optional<xcb_atom_t> window_update_type(xcb_get_property_reply_t* reply);

    /*
     * Updates the WM_HINTS (we only care about the input focus handling part).
     *
     */
    std::optional<window_hints> window_update_hints(xcb_get_property_reply_t* prop);

    /*
     * Updates the MOTIF_WM_HINTS. The container's border style should be set to
     * `motif_border_style' if border style is not BS_NORMAL.
     *
     * i3 only uses this hint when it specifies a window should have no
     * title bar, or no decorations at all, which is how most window managers
     * handle it.
     *
     * The EWMH spec intended to replace Motif hints with _NET_WM_WINDOW_TYPE, but
     * it is still in use by popular widget toolkits such as GTK+ and Java AWT.
     *
     */
    std::optional<uint32_t> window_update_motif_hints(xcb_get_property_reply_t* prop);

    /*
     * Updates the WM_CLIENT_MACHINE
     *
     */
    std::optional<std::string> window_update_machine(xcb_get_property_reply_t* prop);
}  // namespace handle_property

// dump_node.cppm

export nlohmann::json dump_node(Con* con, bool inplace_restart);

export namespace i3 {

}

export class Global {
  public:
    Global() = default;
    Global(Global const &) = delete;
    Global &operator=(Global const &) = delete;

    std::string current_socketpath{};

    Con *focused;
    RootCon *croot;

    std::deque<Con *> all_cons{};

    std::vector<std::string> start_argv{};

    pid_t config_error_nagbar_pid = -1;
    pid_t command_error_nagbar_pid = -1;

    X *x;
    RandR *randr;
    Xkb *xkb;
    ConfigurationManager *configManager;
    ApplicationLauncher *applicationLauncher;
    IPCManager *ipcManager;

    program_arguments *args;

    /* The original value of RLIMIT_CORE when i3 was started. We need to restore
     * this before starting any other process, since we set RLIMIT_CORE to
     * RLIM_INFINITY for i3 debugging versions. */
    rlimit original_rlimit_core;

    /* The list of assignments */
    AssignmentManager *assignmentManager;

    WorkspaceManager *workspaceManager;

    /* The last timestamp we got from X11 (timestamps are included in some events
     * and are used for some things, like determining a unique ID in startup
     * notification). */
    xcb_timestamp_t last_timestamp = XCB_CURRENT_TIME;

    bool new_parser;
    PropertyHandlers *handlers;
    EventHandler *eventHandler;
    ev_loop *main_loop;

    static Global &instance() {
        static Global inst{};

        return inst;
    }
};

export Global global{};
