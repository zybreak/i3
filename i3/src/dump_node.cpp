module;
#include <nlohmann/json.hpp>
#include <xcb/xcb.h>
#include <fmt/printf.h>
module i3;

import rect;
import log;

static std::string orientation(Con *con) {
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

static nlohmann::json dump_rect(Rect &r) {
    return {
            { "x", r.x },
            { "y", r.y },
            { "width", r.width },
            { "height", r.height },
    };
}

static nlohmann::json dump_gaps(gaps_t gaps) {
    return {
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

static std::string type(Con *con) {
    switch (con->type) {
        case CT_ROOT:
            return "root";
        case CT_OUTPUT:
            return "output";
        case CT_CON:
            return "con";
        case CT_FLOATING_CON:
            return "floating_con";
        case CT_WORKSPACE:
            return "workspace";
        case CT_DOCKAREA:
            return "dockarea";
        default:
            throw std::runtime_error("unknown type");
    }
}

static std::string layout(Con *con) {
    switch (con->layout) {
        case L_DEFAULT:
            DLOG("About to dump layout=default, this is a bug in the code.\n");
            std::terminate();
            break;
        case L_SPLITV:
            return "splitv";
        case L_SPLITH:
            return "splith";
        case L_STACKED:
            return "stacked";
        case L_TABBED:
            return "tabbed";
        case L_DOCKAREA:
            return "dockarea";
        case L_OUTPUT:
            return "output";
        default:
            throw std::runtime_error("unknown layout");
    }
}

static std::string workspace_layout(WorkspaceCon *con) {
    switch (con->workspace_layout) {
        case L_DEFAULT:
            return "default";
        case L_STACKED:
            return "stacked";
        case L_TABBED:
            return "tabbed";
        default:
            DLOG(fmt::sprintf("About to dump workspace_layout=%d (none of default/stacked/tabbed), this is a bug.\n",  std::to_underlying(con->workspace_layout)));
            std::terminate();
            break;
    }
}

static std::string border_style(Con *con) {
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

static std::string window_type(i3Window *window) {
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

static std::string floating(Con *con) {
    switch (con->floating) {
        case FLOATING_AUTO_OFF:
            return "auto_off";
        case FLOATING_AUTO_ON:
            return "auto_on";
        case FLOATING_USER_OFF:
            return "user_off";
        case FLOATING_USER_ON:
            return "user_on";
        default:
            throw std::runtime_error("unknown floating");
    }
}

nlohmann::json dump_node(Con *con, bool inplace_restart) {
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

    if (con->type != CT_ROOT && con->type != CT_OUTPUT) {
        j["output"] = con->con_get_output()->name;
    }

    j["layout"] = layout(con);
    if (con->type == CT_WORKSPACE) {
        j["workspace_layout"] = workspace_layout(dynamic_cast<WorkspaceCon*>(con));
    }
    j["last_split_layout"] = (con->layout == L_SPLITV) ? "splitv" : "splith";
    j["border"] = border_style(con);
    j["current_border_width"] = con->current_border_width;

    j["rect"] = dump_rect(con->rect);
    if (con_draw_decoration_into_frame(con)) {
        Rect simulated_deco_rect = con->deco_rect;
        simulated_deco_rect.x = con->rect.x - con->parent->rect.x;
        simulated_deco_rect.y = con->rect.y - con->parent->rect.y;
        j["deco_rect"] = dump_rect(simulated_deco_rect);
        j["actual_deco_rect"] = dump_rect(con->deco_rect);
    } else {
        j["deco_rect"] = dump_rect(con->deco_rect);
    }
    j["window_rect"] = dump_rect(con->get_window_rect());
    j["geometry"] = dump_rect(con->get_geometry());

    if (con->get_window() && !con->get_window()->name.empty()) {
        j["name"] = con->get_window()->name;
    } else if (!con->name.empty()) {
        j["name"] = con->name;
    }

    if (!con->title_format.empty()) {
        j["title_format"] = con->title_format;
    }

    j["window_icon_padding"] = con->get_window_icon_padding();

    if (con->type == CT_WORKSPACE) {
        WorkspaceCon *ws = dynamic_cast<WorkspaceCon*>(con);
        j["num"] = ws->num;
        j["gaps"] = dump_gaps(ws->gaps);
    }

    if (auto window = con->get_window(); window != nullptr) {
        j["window"] = window->id;
        j["window_type"] = window_type(window);
    }


    if (con->get_window() && !inplace_restart) {
        /* Window properties are useless to preserve when restarting because
         * they will be queried again anyway. However, for i3-save-tree(1),
         * they are very useful and save i3-save-tree dealing with X11. */
        auto map = nlohmann::json::object();

        if (!con->get_window()->class_class.empty()) {
            map["class"] = con->get_window()->class_class;
        }
        if (!con->get_window()->class_instance.empty()) {
            map["instance"] = con->get_window()->class_instance;
        }
        if (!con->get_window()->role.empty()) {
            map["window_role"] = con->get_window()->role;
        }
        if (!con->get_window()->machine.empty()) {
            map["machine"] = con->get_window()->machine;
        }

        if (!con->get_window()->name.empty()) {
            map["title"] = con->get_window()->name.c_str();
        }

        if (con->get_window()->transient_for != XCB_NONE) {
            j["transient_for"] = con->get_window()->transient_for;
        }

        j["window_properties"] = map;
    }

    if (con->type != CT_DOCKAREA || !inplace_restart) {
        auto a = nlohmann::json::array();
        std::ranges::transform(con->nodes, std::back_inserter(a), [&inplace_restart](Con *node) { return dump_node(node, inplace_restart); });
        j["nodes"] = a;
    }

    if (dynamic_cast<WorkspaceCon*>(con)) {
        auto floating_nodes_array = nlohmann::json::array();
        std::ranges::transform(dynamic_cast<WorkspaceCon*>(con)->floating_windows, std::back_inserter(floating_nodes_array), [&inplace_restart](FloatingCon *node) { return dump_node(node, inplace_restart); });
        j["floating_nodes"] = floating_nodes_array;
    }

    auto focus_nodes_array = nlohmann::json::array();
    std::ranges::transform(con->focused, std::back_inserter(focus_nodes_array), [](Con *node) { return (uintptr_t)node; });
    j["focus"] = focus_nodes_array;

    j["fullscreen_mode"] = con->fullscreen_mode;
    j["sticky"] = con->sticky;
    j["floating"] = floating(con);

    auto swallows = nlohmann::json::array();
    for (auto &match : con->swallow) {
        /* We will generate a new restart_mode match specification after this
         * loop, so skip this one. */
        if (match->restart_mode) {
            continue;
        }

        auto map = nlohmann::json::object();

        if (match->dock != M_DONTCHECK) {
            map["dock"] = match->dock;
            map["insert_where"] = match->insert_where;
        }

        if (match->window_class != nullptr) {
            map["window_class"] = match->window_class->pattern;
        }
        if (match->instance != nullptr) {
            map["instance"] = match->instance->pattern;
        }
        if (match->window_role != nullptr) {
            map["window_role"] = match->window_role->pattern;
        }
        if (match->title != nullptr) {
            map["title"] = match->title->pattern;
        }
        if (match->machine != nullptr) {
            map["machine"] = match->machine->pattern;
        }

        swallows.push_back(map);
    }

    if (inplace_restart) {
        if (con->get_window() != nullptr) {
            swallows.push_back({
                    {"id", con->get_window()->id},
                    {"restart_mode", true},
            });
        }
    }
    j["swallows"] = swallows;

    if (inplace_restart && con->get_window() != nullptr) {
        j["depth"] = con->depth;
    }

    if (inplace_restart && con->type == CT_ROOT && !global.workspaceManager->previous_workspace_name.empty()) {
        j["previous_workspace_name"] = global.workspaceManager->previous_workspace_name.c_str();
    }

    return j;
}
