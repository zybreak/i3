/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * config_directives.c: all config storing functions (see config_parser.c)
 *
 */
module;
struct criteria_state;

#include <xcb/xcb.h>

#include <fmt/printf.h>

#include <wordexp.h>
module i3;

import std;
import utils;
import log;
import i3_config_base;

using namespace std::literals;

criteria_state* ConfigApplier::criteria_create(int _state) {
    criteria_state *st = new criteria_state(_state);

    DLOG(fmt::sprintf("Creating criteria, current_match = %p, state = %d\n", fmt::ptr(&(st->current_match)), _state));
    st->current_match = Match();

    return st;
}

void ConfigApplier::criteria_init(criteria_state *st, int _state) {
    //criteria_state *st = new criteria_state();
    st->criteria_next_state = _state;

    DLOG(fmt::sprintf("Initializing criteria, current_match = %p, state = %d\n",  fmt::ptr(&(st->current_match)), _state));
    st->current_match = Match();
}

int ConfigApplier::criteria_pop_state(criteria_state *cs) {
    int next_state = cs->criteria_next_state;
    
    return next_state;
}

void ConfigApplier::criteria_add(criteria_state *cs, const char *ctype, const char *cvalue) {
    cs->current_match.parse_property(ctype, cvalue);
}

void ConfigApplier::font(const std::string &font) {
    this->config->font = load_font(**global.x, global.x->root_screen, font, true);
}

void ConfigApplier::binding(const std::string &bindtype, const std::string &modifiers,
                 const std::string &key, bool release, bool border, bool whole_window,
                 bool exclude_titlebar, const std::string &command) {
    configure_binding(config, bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command, DEFAULT_BINDING_MODE, false);
}

/*******************************************************************************
 * Mode handling
 ******************************************************************************/

void ConfigApplier::mode_binding(const std::string &bindtype, const std::string &modifiers,
                      const std::string &key, bool release, bool border, bool whole_window,
                      bool exclude_titlebar, const std::string &command) {
    if (current_mode.empty()) {
        /* When using an invalid mode name, e.g. “default” */
        return;
    }
    configure_binding(config, bindtype, modifiers, key, release, border, whole_window, exclude_titlebar, command, current_mode, current_mode_pango_markup);
}

void ConfigApplier::enter_mode(bool pango_markup, const std::string &modename) {
    if (modename == DEFAULT_BINDING_MODE) {
        ELOG(fmt::sprintf("You cannot use the name %s for your mode\n",  DEFAULT_BINDING_MODE));
        return;
    }

    if (config->modes.contains(modename)) {
         ELOG(fmt::sprintf("The binding mode with name \"%s\" is defined at least twice.\n", modename));
    }

    DLOG(fmt::sprintf("\t now in mode %s\n", modename));
    current_mode = modename;
    current_mode_pango_markup = pango_markup;
}

void ConfigApplier::exec(const std::string &exectype, bool no_startup_id,
              const std::string &command) {
    auto n = std::make_unique<Autostart>();
    n->command = command;
    n->no_startup_id = no_startup_id;
    if (exectype == "exec"s) {
        autostart_add(std::move(n));
    } else {
        autostart_always_add(std::move(n));
    }
}

static void apply_gaps(gaps_t *gaps, gaps_mask_t mask, int value) {
    if (gaps == nullptr) {
        return;
    }
    if (mask & GAPS_INNER) {
        gaps->inner = value;
    }
    if (mask & GAPS_TOP) {
        gaps->top = value;
    }
    if (mask & GAPS_RIGHT) {
        gaps->right = value;
    }
    if (mask & GAPS_BOTTOM) {
        gaps->bottom = value;
    }
    if (mask & GAPS_LEFT) {
        gaps->left = value;
    }
}

static void create_gaps_assignment(const std::string &workspace, const gaps_mask_t mask, const int pixels) {
    if (mask == 0) {
        return;
    }

    DLOG(fmt::sprintf("Setting gaps for workspace %s", workspace));

    auto found = global.workspaceManager->get_workspace_config(workspace);
    WorkspaceConfig config{workspace};

    if (found) {
        config = found.value();
    }

    config.gaps_mask = static_cast<gaps_mask_t>(std::to_underlying(config.gaps_mask) | std::to_underlying(mask));
    apply_gaps(&config.gaps, mask, pixels);

    global.workspaceManager->add_workspace_config(std::move(config));
}

static gaps_mask_t gaps_scope_to_mask(const std::string &scope) {
    if (!strcmp(scope.c_str(), "inner")) {
        return GAPS_INNER;
    } else if (!strcmp(scope.c_str(), "outer")) {
        return GAPS_OUTER;
    } else if (!strcmp(scope.c_str(), "vertical")) {
        return GAPS_VERTICAL;
    } else if (!strcmp(scope.c_str(), "horizontal")) {
        return GAPS_HORIZONTAL;
    } else if (!strcmp(scope.c_str(), "top")) {
        return GAPS_TOP;
    } else if (!strcmp(scope.c_str(), "right")) {
        return GAPS_RIGHT;
    } else if (!strcmp(scope.c_str(), "bottom")) {
        return GAPS_BOTTOM;
    } else if (!strcmp(scope.c_str(), "left")) {
        return GAPS_LEFT;
    }
    ELOG(fmt::sprintf("Invalid command, cannot process scope %s", scope));
    return static_cast<gaps_mask_t>(0);
}

void ConfigApplier::gaps(const std::string &workspace, const std::string &scope, const long value) {
    int pixels = logical_px(global.x->root_screen, value);
    gaps_mask_t mask = gaps_scope_to_mask(scope);

    if (workspace.empty()) {
        apply_gaps(&this->config->gaps, mask, pixels);
    } else {
        create_gaps_assignment(workspace, mask, pixels);
    }
}

void ConfigApplier::smart_borders(const std::string &enable) {
    if (!strcmp(enable.c_str(), "no_gaps"))
        this->config->smart_borders = SMART_BORDERS_NO_GAPS;
    else
        this->config->smart_borders = boolstr(enable.c_str()) ? SMART_BORDERS_ON : SMART_BORDERS_OFF;
}

void ConfigApplier::smart_gaps(const std::string &enable) {
    if (!strcmp(enable.c_str(), "inverse_outer"))
        this->config->smart_gaps = SMART_GAPS_INVERSE_OUTER;
    else
        this->config->smart_gaps = boolstr(enable.c_str()) ? SMART_GAPS_ON : SMART_GAPS_OFF;
}

void ConfigApplier::for_window(criteria_state *criteria_state, const std::string &command) {
    if (criteria_state->current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this for_window statement\n");
        return;
    }
    DLOG(fmt::sprintf("\t should execute command %s for the criteria mentioned above\n",  command));
    auto assignment = std::make_unique<CommandAssignment>(Match(criteria_state->current_match));
    assignment->command = command;
    global.assignmentManager->add(std::move(assignment));
}

void ConfigApplier::floating_minimum_size(const long width, const long height) {
    this->config->floating_minimum_width = width;
    this->config->floating_minimum_height = height;
}

void ConfigApplier::floating_maximum_size(const long width, const long height) {
    this->config->floating_maximum_width = width;
    this->config->floating_maximum_height = height;
}

void ConfigApplier::floating_modifier(const std::string &modifiers) {
    this->config->floating_modifier = event_state_from_str(modifiers.c_str());
}

void ConfigApplier::default_orientation(const std::string &orientation) {
    if (orientation == "horizontal"s)
        this->config->default_orientation = HORIZ;
    else if (orientation == "vertical"s)
        this->config->default_orientation = VERT;
    else
        this->config->default_orientation = NO_ORIENTATION;
}

void ConfigApplier::workspace_layout(const std::string &layout) {
    if (layout == "default"s)
        this->config->default_layout = L_DEFAULT;
    else if (layout == "stacking"s || layout == "stacked"s)
        this->config->default_layout = L_STACKED;
    else
        this->config->default_layout = L_TABBED;
}

void ConfigApplier::default_border(const std::string &windowtype, const std::string &border, const long width) {
    int border_style;
    int border_width;

    if (border == "1pixel"s) {
        border_style = BS_PIXEL;
        border_width = 1;
    } else if (border == "none"s) {
        border_style = BS_NONE;
        border_width = 0;
    } else if (border == "pixel"s) {
        border_style = BS_PIXEL;
        border_width = width;
    } else {
        border_style = BS_NORMAL;
        border_width = width;
    }

    if (windowtype ==  "default_border"s || windowtype == "new_window"s) {
        DLOG(fmt::sprintf("default tiled border style = %d and border width = %d (%ld physical px)\n",
             border_style, border_width, logical_px(global.x->root_screen, border_width)));
        this->config->default_border = static_cast<border_style_t>(border_style);
        this->config->default_border_width = logical_px(global.x->root_screen, border_width);
    } else {
        DLOG(fmt::sprintf("default floating border style = %d and border width = %d (%ld physical px)\n",
             border_style, border_width, logical_px(global.x->root_screen, border_width)));
        this->config->default_floating_border = static_cast<border_style_t>(border_style);
        this->config->default_floating_border_width = logical_px(global.x->root_screen, border_width);
    }
}

void ConfigApplier::hide_edge_borders(const std::string &borders) {
    if (strcmp(borders.c_str(), "smart_no_gaps") == 0)
        this->config->hide_edge_borders = HEBM_SMART_NO_GAPS;
    else if (borders.c_str() == "smart"s)
        this->config->hide_edge_borders = HEBM_SMART;
    else if (borders.c_str() == "vertical"s)
        this->config->hide_edge_borders = HEBM_VERTICAL;
    else if (borders.c_str() == "horizontal"s)
        this->config->hide_edge_borders = HEBM_HORIZONTAL;
    else if (borders.c_str() == "both"s)
        this->config->hide_edge_borders = HEBM_BOTH;
    else if (borders.c_str() == "none"s)
        this->config->hide_edge_borders = HEBM_NONE;
    else if (boolstr(borders.c_str()))
        this->config->hide_edge_borders = HEBM_VERTICAL;
    else
        this->config->hide_edge_borders = HEBM_NONE;
}

void ConfigApplier::focus_follows_mouse(const std::string &value) {
    this->config->disable_focus_follows_mouse = !boolstr(value.c_str());
}

void ConfigApplier::mouse_warping(const std::string &value) {
    if (value == "none"s)
        this->config->mouse_warping = POINTER_WARPING_NONE;
    else if (value == "output"s)
        this->config->mouse_warping = POINTER_WARPING_OUTPUT;
}

void ConfigApplier::focus_wrapping(const std::string &value) {
    if (value == "force"s) {
        this->config->focus_wrapping = FOCUS_WRAPPING_FORCE;
    } else if (value == "workspace"s) {
        this->config->focus_wrapping = FOCUS_WRAPPING_WORKSPACE;
    } else if (boolstr(value.c_str())) {
        this->config->focus_wrapping = FOCUS_WRAPPING_ON;
    } else {
        this->config->focus_wrapping = FOCUS_WRAPPING_OFF;
    }
}

void ConfigApplier::force_focus_wrapping(const std::string &value) {
    /* Legacy syntax. */
    if (boolstr(value.c_str())) {
        this->config->focus_wrapping = FOCUS_WRAPPING_FORCE;
    } else {
        /* For "force_focus_wrapping off", don't enable or disable
         * focus wrapping, just ensure it's not forced. */
        if (this->config->focus_wrapping == FOCUS_WRAPPING_FORCE) {
            this->config->focus_wrapping = FOCUS_WRAPPING_ON;
        }
    }
}

void ConfigApplier::workspace_back_and_forth(const std::string &value) {
    this->config->workspace_auto_back_and_forth = boolstr(value.c_str());
}

void ConfigApplier::force_display_urgency_hint(const long duration_ms) {
    this->config->workspace_urgency_timer = duration_ms / 1000.0;
}

void ConfigApplier::focus_on_window_activation(const std::string &mode) {
    if (mode == "smart"s)
        this->config->focus_on_window_activation = FOWA_SMART;
    else if (mode == "urgent"s)
        this->config->focus_on_window_activation = FOWA_URGENT;
    else if (mode == "focus"s)
        this->config->focus_on_window_activation = FOWA_FOCUS;
    else if (mode == "none"s)
        this->config->focus_on_window_activation = FOWA_NONE;
    else {
         ELOG(fmt::sprintf("Unknown focus_on_window_activation mode \"%s\", ignoring it.\n", mode));
        return;
    }

    DLOG(fmt::sprintf("Set new focus_on_window_activation mode = %d.\n",  std::to_underlying(this->config->focus_on_window_activation)));
}

void ConfigApplier::title_align(const std::string &alignment) {
    if (alignment == "left"s) {
        this->config->title_align = title_align_t::ALIGN_LEFT;
    } else if (alignment == "center"s) {
        this->config->title_align = title_align_t::ALIGN_CENTER;
    } else if (alignment == "right"s) {
        this->config->title_align = title_align_t::ALIGN_RIGHT;
    } else {
        std::terminate();
    }
}

void ConfigApplier::workspace(const std::string &workspace, const std::string &output) {
    auto found = global.workspaceManager->get_workspace_config(workspace);
    if (found && found->name == workspace && !found->output.empty()) {
        ELOG(fmt::sprintf("You have a duplicate workspace assignment for workspace \"%s\"\n", workspace));
        return;
    }

    DLOG(fmt::sprintf("Assigning workspace \"%s\" to output \"%s\"\n", workspace, output));

    global.workspaceManager->add_workspace_config(WorkspaceConfig{workspace, output});
}

void ConfigApplier::ipc_socket(const std::string &path) {
    this->config->ipc_socket_path = path;
}

void ConfigApplier::restart_state(const std::string &path) {
    this->config->restart_state_path = path;
}

void ConfigApplier::popup_during_fullscreen(const std::string &value) {
    if (value == "ignore"s) {
        this->config->popup_during_fullscreen = PDF_IGNORE;
    } else if (value == "leave_fullscreen"s) {
        this->config->popup_during_fullscreen = PDF_LEAVE_FULLSCREEN;
    } else {
        this->config->popup_during_fullscreen = PDF_SMART;
    }
}

void ConfigApplier::color_single(const std::string &colorclass, const std::string &color) {
    /* used for client.background only currently */
    this->config->client.background = draw_util_hex_to_color(**global.x, global.x->root_screen, color.c_str());
}

void ConfigApplier::color(const std::string &colorclass, const std::string &border, const std::string &background, const std::string &text, const std::string &indicator, const std::string &child_border) {
#define APPLY_COLORS(classname)                                                              \
    do {                                                                                     \
        if (strcmp(colorclass.c_str(), "client." #classname) == 0) {                         \
            if (strcmp("focused_tab_title", #classname) == 0) {                                              \
                this->config->client.got_focused_tab_title = true;                                                  \
                if (!indicator.empty() || !child_border.empty()) {                                                             \
                    ELOG("indicator and child_border colors have no effect for client.focused_tab_title\n"); \
                }                                                                                            \
            }                                                                                 \
            this->config->client.classname.border = draw_util_hex_to_color(**global.x, global.x->root_screen, border.c_str());                 \
            this->config->client.classname.background = draw_util_hex_to_color(**global.x, global.x->root_screen, background.c_str());         \
            this->config->client.classname.text = draw_util_hex_to_color(**global.x, global.x->root_screen, text.c_str());                     \
            if (!indicator.empty()) {                                                         \
                this->config->client.classname.indicator = draw_util_hex_to_color(**global.x, global.x->root_screen, indicator.c_str());       \
            }                                                                                \
            if (!child_border.empty()) {                                                      \
                this->config->client.classname.child_border = draw_util_hex_to_color(**global.x, global.x->root_screen, child_border.c_str()); \
            } else {                                                                         \
                this->config->client.classname.child_border = this->config->client.classname.background;   \
            }                                                                                \
        }                                                                                    \
    } while (0)

    APPLY_COLORS(focused_inactive);
    APPLY_COLORS(focused_tab_title);
    APPLY_COLORS(focused);
    APPLY_COLORS(unfocused);
    APPLY_COLORS(urgent);
    APPLY_COLORS(placeholder);

#undef APPLY_COLORS
}

void ConfigApplier::assign_output(criteria_state *criteria_state, const std::string &output) {
    if (criteria_state->current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    if (criteria_state->current_match.window_mode != WM_ANY) {
        ELOG("Assignments using window mode (floating/tiling) is not supported\n");
        return;
    }

     DLOG(fmt::sprintf("New assignment, using above criteria, to output \"%s\".\n", output));
    auto assignment = std::make_unique<OutputAssignment>(Match(criteria_state->current_match));
    assignment->output = output;
    global.assignmentManager->add(std::move(assignment));
}

void ConfigApplier::assign(criteria_state *criteria_state, const std::string &workspace, bool is_number) {
    if (criteria_state->current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    if (criteria_state->current_match.window_mode != WM_ANY) {
        ELOG("Assignments using window mode (floating/tiling) is not supported\n");
        return;
    }

    if (is_number && utils::ws_name_to_number(workspace) == -1) {
        ELOG(fmt::sprintf("Could not parse initial part of \"%s\" as a number.\n", workspace));
        return;
    }

    DLOG(fmt::sprintf("New assignment, using above criteria, to workspace \"%s\".\n", workspace));
    auto assignment = std::make_unique<WorkspaceAssignment>(Match(criteria_state->current_match));
    assignment->type = is_number ? workspace_assignment_type::WORKSPACE_NUMBER : workspace_assignment_type::WORKSPACE;
    assignment->workspace = workspace;
    global.assignmentManager->add(std::move(assignment));
}

void ConfigApplier::no_focus(criteria_state *criteria_state) {
    if (criteria_state->current_match.match_is_empty()) {
        ELOG("Match is empty, ignoring this assignment\n");
        return;
    }

    DLOG("New assignment, using above criteria, to ignore focus on manage.\n");
    auto assignment = std::make_unique<NoFocusAssignment>(Match(criteria_state->current_match));
    global.assignmentManager->add(std::move(assignment));
}

void ConfigApplier::ipc_kill_timeout(const long timeout_ms) {
    ipc_set_kill_timeout(timeout_ms / 1000.0);
}


void ConfigApplier::tiling_drag(const std::string &value) {
    if (strcmp(value.c_str(), "modifier") == 0) {
        this->config->tiling_drag = TILING_DRAG_MODIFIER;
    } else if (strcmp(value.c_str(), "titlebar") == 0) {
        this->config->tiling_drag = TILING_DRAG_TITLEBAR;
    } else if (strcmp(value.c_str(), "modifier,titlebar") == 0 ||
               strcmp(value.c_str(), "titlebar,modifier") == 0) {
        /* Switch the above to strtok() or similar if we ever grow more options */
        this->config->tiling_drag = TILING_DRAG_MODIFIER_OR_TITLEBAR;
    } else {
        this->config->tiling_drag = TILING_DRAG_OFF;
    }
}

void ConfigApplier::bar_height(const long height) {
}

void ConfigApplier::bar_padding_one(const long all) {
}

void ConfigApplier::bar_padding_two(const long top_and_bottom, const long right_and_left) {
}

void ConfigApplier::bar_padding_three(const long top, const long right_and_left, const long bottom) {
}

void ConfigApplier::bar_padding_four(const long top, const long right, const long bottom, const long left) {
}
