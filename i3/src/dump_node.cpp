module;
#include <nlohmann/json.hpp>
#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import rect;
import log;

static std::string orientation(Con const * const con) {
    if (!con->con_is_split()) {
        return "none";
    } else {
        if (con_orientation(con) == HORIZ) {
            return "horizontal";
        } else {
            return "vertical";
        }
    }
}

static void to_json(nlohmann::json &j, gaps_t const &gaps) {
   j = {
           { "inner", gaps.inner },
           // TODO: the i3ipc Python modules recognize gaps, but only inner/outer
           // This is currently here to preserve compatibility with that
           { "outer", gaps.top },
           { "top", gaps.top },
           { "right", gaps.right },
           { "bottom", gaps.bottom },
           { "left", gaps.left }
   }; 
}

static void to_json(nlohmann::json &j, Match const *match) {
    if (match->dock != M_DONTCHECK) {
        j["dock"] = match->dock;
        j["insert_where"] = match->insert_where;
    }

    if (match->window_class != nullptr) {
        j["window_class"] = match->window_class->pattern;
    }
    if (match->instance != nullptr) {
        j["instance"] = match->instance->pattern;
    }
    if (match->window_role != nullptr) {
        j["window_role"] = match->window_role->pattern;
    }
    if (match->title != nullptr) {
        j["title"] = match->title->pattern;
    }
    if (match->machine != nullptr) {
        j["machine"] = match->machine->pattern;
    }
}

static std::string window_type(i3Window const * const window) {
    if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL]) {
        return "normal";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DOCK]) {
        return "dock";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG]) {
        return "dialog";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY]) {
        return "utility";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR]) {
        return "toolbar";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH]) {
        return "splash";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_MENU]) {
        return "menu";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU]) {
        return "dropdown_menu";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_POPUP_MENU]) {
        return "popup_menu";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLTIP]) {
        return "tooltip";
    } else if (window->window_type == i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NOTIFICATION]) {
        return "notification";
    } else {
        return "unknown";
    }
}

static void dump_window(nlohmann::json &j, i3Window const * const window, bool const inplace_restart) {
    j["window"] = window->id;
    j["window_type"] = window_type(window);

    if (!inplace_restart) {
        /* Window properties are useless to preserve when restarting because
         * they will be queried again anyway. However, for i3-save-tree(1),
         * they are very useful and save i3-save-tree dealing with X11. */
        auto map = nlohmann::json::object();

        if (!window->class_class.empty()) {
            map["class"] = window->class_class;
        }
        if (!window->class_instance.empty()) {
            map["instance"] = window->class_instance;
        }
        if (!window->role.empty()) {
            map["window_role"] = window->role;
        }
        if (!window->machine.empty()) {
            map["machine"] = window->machine;
        }

        if (!window->name.empty()) {
            map["title"] = window->name;
        }

        if (window->transient_for != XCB_NONE) {
            map["transient_for"] = window->transient_for;
        }

        j["window_properties"] = map;
    }
}

static std::string type(Con const * const con) {
    switch (con->type) {
        case con_type_t::CT_ROOT:
            return "root";
        case con_type_t::CT_OUTPUT:
            return "output";
        case con_type_t::CT_CON:
            return "con";
        case con_type_t::CT_FLOATING_CON:
            return "floating_con";
        case con_type_t::CT_WORKSPACE:
            return "workspace";
        case con_type_t::CT_DOCKAREA:
            return "dockarea";
        default:
            throw std::runtime_error("unknown type");
    }
}

static std::string layout(Con const * const con) {
    switch (con->layout) {
        case layout_t::L_DEFAULT:
            throw std::runtime_error("About to dump layout=default, this is a bug in the code.");
        case layout_t::L_SPLITV:
            return "splitv";
        case layout_t::L_SPLITH:
            return "splith";
        case layout_t::L_STACKED:
            return "stacked";
        case layout_t::L_TABBED:
            return "tabbed";
        case layout_t::L_DOCKAREA:
            return "dockarea";
        case layout_t::L_OUTPUT:
            return "output";
        default:
            throw std::runtime_error("unknown Con layout");
    }
}

static std::string workspace_layout(WorkspaceCon const * const con) {
    switch (con->workspace_layout) {
        case layout_t::L_DEFAULT:
            return "default";
        case layout_t::L_STACKED:
            return "stacked";
        case layout_t::L_TABBED:
            return "tabbed";
        default:
            throw std::runtime_error(std::format("About to dump workspace_layout={} (none of default/stacked/tabbed), this is a bug.",  std::to_underlying(con->workspace_layout)));
    }
}

static std::string border_style(Con const * const con) {
    switch (con->border_style) {
        case border_style_t::BS_NORMAL:
            return "normal";
        case border_style_t::BS_NONE:
            return "none";
        case border_style_t::BS_PIXEL:
            return "pixel";
        default:
            throw std::runtime_error("unknown border_style");
    }
}

static std::string floating(Con const * const con) {
    switch (con->floating) {
        case con_floating_t::FLOATING_AUTO_OFF:
            return "auto_off";
        case con_floating_t::FLOATING_AUTO_ON:
            return "auto_on";
        case con_floating_t::FLOATING_USER_OFF:
            return "user_off";
        case con_floating_t::FLOATING_USER_ON:
            return "user_on";
        default:
            throw std::runtime_error("unknown floating");
    }
}

static nlohmann::json get_swallows(Con * con, bool inplace_restart) {
    auto swallows = nlohmann::json::array();
    for (auto &match : con->swallow) {
        /* We will generate a new restart_mode match specification after this
         * loop, so skip this one. */
        if (match->restart_mode) {
            continue;
        }

        swallows.push_back(match.get());
    }

    if (inplace_restart) {
        if (con->get_window() != nullptr) {
            swallows.push_back({
                    {"id", con->get_window()->id},
                    {"restart_mode", true},
            });
        }
    }
    
    return swallows;
}

static nlohmann::json get_floating_nodes(Con * con, bool inplace_restart) {

    if (auto ws = dynamic_cast<WorkspaceCon*>(con); ws != nullptr) {
        nlohmann::json j = ws->floating_windows | std::views::transform([&inplace_restart](FloatingCon *node) { return dump_node(node, inplace_restart); }) | std::ranges::to<std::vector>();
        return j;
    } else {
        return nlohmann::json::array();
    }
}

static void handle_deco_rect(nlohmann::json &j, Con * con) {
    if (con_draw_decoration_into_frame(con)) {
        Rect simulated_deco_rect = con->deco_rect;
        simulated_deco_rect.x = con->rect.x - con->parent->rect.x;
        simulated_deco_rect.y = con->rect.y - con->parent->rect.y;
        j["deco_rect"] = simulated_deco_rect;
        j["actual_deco_rect"] = con->deco_rect;
    } else {
        j["deco_rect"] = con->deco_rect;
    }
}

nlohmann::json dump_node(Con * con, bool inplace_restart) {
    nlohmann::json j;

    j["id"] = (uintptr_t)con;
    j["type"] = type(con);

    /* provided for backwards compatibility only. */
    j["orientation"] = orientation(con);

    if (con->percent > 0.0) {
        j["percent"] = con->percent;
    }

    j["urgent"] = con->urgent;
    j["focused"] = (con == global.focused);

    if (con->type != con_type_t::CT_ROOT && con->type != con_type_t::CT_OUTPUT) {
        j["output"] = con->con_get_output()->name;
    }

    j["layout"] = layout(con);
    j["last_split_layout"] = (con->layout == layout_t::L_SPLITV) ? "splitv" : "splith";
    j["border"] = border_style(con);
    j["current_border_width"] = con->current_border_width;

    j["rect"] = con->rect;
    handle_deco_rect(j, con);
    
    j["window_rect"] = con->get_window_rect();
    j["geometry"] = con->get_geometry();

    if (con->get_window() && !con->get_window()->name.empty()) {
        j["name"] = con->get_window()->name;
    } else if (!con->name.empty()) {
        j["name"] = con->name;
    }

    if (!con->title_format.empty()) {
        j["title_format"] = con->title_format;
    }

    j["window_icon_padding"] = con->get_window_icon_padding();

    if (auto *window = con->get_window(); window != nullptr) {
        dump_window(j, window, inplace_restart);
    }

    if (con->type != con_type_t::CT_DOCKAREA || !inplace_restart) {
        j["nodes"] = con->nodes | std::views::transform([&inplace_restart](Con *node) { return dump_node(node, inplace_restart); }) | std::ranges::to<std::vector>();
    }
    
    if (auto ws = dynamic_cast<WorkspaceCon*>(con); ws != nullptr) {
        j["workspace_layout"] = workspace_layout(ws);
        j["num"] = ws->num;
        j["gaps"] = ws->gaps;
    }
    
    j["floating_nodes"] = get_floating_nodes(con, inplace_restart);

    j["focus"] = con->focused | std::views::transform([](Con *node) { return (uintptr_t)node; }) | std::ranges::to<std::vector>();

    j["fullscreen_mode"] = con->fullscreen_mode;
    j["sticky"] = con->sticky;
    j["floating"] = floating(con);

    j["swallows"] = get_swallows(con, inplace_restart);

    if (inplace_restart && con->get_window() != nullptr) {
        j["depth"] = con->depth;
    }

    if (inplace_restart && con->type == con_type_t::CT_ROOT && !global.workspaceManager->previous_workspace_name.empty()) {
        j["previous_workspace_name"] = global.workspaceManager->previous_workspace_name;
    }

    return j;
}
