/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * load_layout.c: Restore (parts of) the layout, for example after an inplace
 *                restart.
 *
 */
module;
#include <fmt/core.h>
#include <fmt/printf.h>
#include <nlohmann/json.hpp>
#include <xcb/xproto.h>
module i3;

import std;
import utils;
import log;
import regex;
import rect;

struct parser_context {
    Con *to_focus{nullptr};
};

static void from_json(nlohmann::json const &j, gaps_t &p) {
    if (j.contains("inner")) {
        j.at("inner").get_to(p.inner);
    }
    if (j.contains("top")) {
        j.at("top").get_to(p.top);
    }
    if (j.contains("right")) {
        j.at("right").get_to(p.right);
    }
    if (j.contains("bottom")) {
        j.at("bottom").get_to(p.bottom);
    }
    if (j.contains("left")) {
        j.at("left").get_to(p.left);
    }
}

/*
 * Returns true if the provided JSON could be parsed.
 *
 */
bool json_validate(std::istream &&fb) {
    setlocale(LC_NUMERIC, "C");
    bool valid = nlohmann::json::accept(fb, true);
    setlocale(LC_NUMERIC, "");

    return valid;
}

/* Parses the given JSON file until it encounters the first “type” property to
 * determine whether the file contains workspaces or regular containers, which
 * is important to know when deciding where (and how) to append the contents.
 * */
json_content_t json_determine_content(std::istream &&fb) {
    // We default to JSON_CONTENT_CON because it is legal to not include
    // “"type": "con"” in the JSON files for better readability.
    try {
        auto j = nlohmann::json::parse(fb, nullptr, true, true);
        if (!j.contains("type")) {
            return json_content_t::JSON_CONTENT_CON;
        } else if (!j["type"].is_string()) {
            return json_content_t::JSON_CONTENT_UNKNOWN;
        }
        std::string type = j.at("type");
        if (type == "workspace") {
            return json_content_t::JSON_CONTENT_WORKSPACE;
        } else {
            return json_content_t::JSON_CONTENT_CON;
        }
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("JSON parsing error: %s\n", e.what()));
        return json_content_t::JSON_CONTENT_UNKNOWN;
    }
}

static Con *extract_con(nlohmann::json &j, Con *parent, parser_context &ctx);
static void con_massage(Con *con);

static void extract_basecon(nlohmann::json &j, Con *con, Con *parent, parser_context &ctx) {
    if (j.contains("name")) {
        j["name"].get_to(con->name);
    }
    if (j.contains("title_format")) {
        j["title_format"].get_to(con->title_format);
    }
    if (j.contains("sticky_group")) {
        j["sticky_group"].get_to(con->sticky_group);
    }

    if (j.contains("orientation")) {
        /* Upgrade path from older versions of i3 (doing an inplace restart
         * to a newer version):
         * "orientation" is dumped before "layout". Therefore, we store
         * whether the orientation was horizontal or vertical in the
         * last_split_layout. When we then encounter layout == "default",
         * we will use the last_split_layout as layout instead. */
        std::string orientation = j["orientation"].template get<std::string>();
        if (orientation == "none" ||
            orientation == "horizontal") {
            con->last_split_layout = L_SPLITH;
        } else if (orientation == "vertical") {
            con->last_split_layout = L_SPLITV;
        } else {
            LOG(std::format("Unhandled orientation: {}", orientation));
        }
    }

    if (j.contains("border")) {
        std::string buf = j["border"].get<std::string>();
        if (buf == "none") {
            con->max_user_border_style = border_style_t::BS_NONE;
            con->border_style = border_style_t::BS_NONE;
        } else if (buf == "1pixel") {
            con->max_user_border_style = border_style_t::BS_PIXEL;
            con->border_style = border_style_t::BS_PIXEL;
            con->current_border_width = 1;
        } else if (buf == "pixel") {
            con->max_user_border_style = border_style_t::BS_PIXEL;
            con->border_style = border_style_t::BS_PIXEL;
        } else if (buf == "normal") {
            con->max_user_border_style = border_style_t::BS_NORMAL;
            con->border_style = border_style_t::BS_NORMAL;
        } else {
            LOG(std::format("Unhandled \"border\": {}", buf));
        }
    }

    if (j.contains("layout")) {
        std::string buf = j["layout"].get<std::string>();
        if (buf == "default") {
            /* This set above when we read "orientation". */
            con->layout = con->last_split_layout;
        } else if (buf == "stacked") {
            con->layout = L_STACKED;
        } else if (buf == "tabbed") {
            con->layout = L_TABBED;
        } else if (buf == "dockarea") {
            con->layout = L_DOCKAREA;
        } else if (buf == "output") {
            con->layout = L_OUTPUT;
        } else if (buf == "splith") {
            con->layout = L_SPLITH;
        } else if (buf == "splitv") {
            con->layout = L_SPLITV;
        } else {
            LOG(std::format("Unhandled \"layout\": {}", buf));
        }
    }

    if (j.contains("last_split_layout")) {
        std::string buf = j["last_split_layout"].get<std::string>();
        if (buf == "splith") {
            con->last_split_layout = L_SPLITH;
        } else if (buf == "splitv") {
            con->last_split_layout = L_SPLITV;
        } else {
            LOG(std::format("Unhandled \"last_splitlayout\": {}", buf));
        }
    }

    if (j.contains("floating")) {
        std::string buf = j["floating"].get<std::string>();
        if (buf == "auto_off") {
            con->floating = FLOATING_AUTO_OFF;
        } else if (buf == "auto_on") {
            con->floating = FLOATING_AUTO_ON;
        } else if (buf == "user_off") {
            con->floating = FLOATING_USER_OFF;
        } else if (buf == "user_on") {
            con->floating = FLOATING_USER_ON;
        }
    }

    if (j.contains("percent")) {
        con->percent = j["percent"];
    }

    if (j.contains("swallows")) {
        if (j["swallows"].is_array()) {
            for (auto &match_json : j["swallows"]) {
                LOG("creating new swallow\n");
                auto match = std::make_unique<Match>();
                match->dock = M_DONTCHECK;
                if (match_json.contains("class")) {
                    match->window_class = std::make_unique<Regex>(match_json["class"].get<std::string>());
                }
                if (match_json.contains("instance")) {
                    match->instance = std::make_unique<Regex>(match_json["instance"].get<std::string>());
                }
                if (match_json.contains("window_role")) {
                    match->window_role = std::make_unique<Regex>(match_json["window_role"].get<std::string>());
                }
                if (match_json.contains("title")) {
                    match->title = std::make_unique<Regex>(match_json["title"].get<std::string>());
                }
                if (match_json.contains("machine")) {
                    match->machine = std::make_unique<Regex>(match_json["machine"].get<std::string>());
                }
                if (match_json.contains("id")) {
                    match->id = match_json["id"].get<xcb_window_t>();
                }
                if (match_json.contains("dock")) {
                    match->dock = match_json["dock"].get<match_dock_t>();
                }
                if (match_json.contains("insert_where")) {
                    match->insert_where = match_json["insert_where"].get<match_insert_t>();
                }
                if (match_json.contains("restart_mode")) {
                    match->restart_mode = match_json["restart_mode"].get<bool>();
                }
                con->swallow.push_back(std::move(match));
            }
        }
    }

    if (j.contains("focused")) {
        bool focused = j["focused"];
        if (focused) {
            ctx.to_focus = con;
        }
    }

    if (j.contains("sticky")) {
        con->sticky = j["sticky"];
    }

    if (j.contains("fullscreen_mode")) {
        con->fullscreen_mode = j["fullscreen_mode"];
    }

    if (j.contains("current_border_width")) {
        con->current_border_width = j["current_border_width"];
    }

    if (j.contains("depth")) {
        con->depth = j["depth"];
    }

    if (j.contains("id")) {
        con->old_id = j["id"];
    }

    if (j.contains("focus")) {
        std::vector<uint32_t> focus_mappings{};
        for (auto &focus_json : j["focus"]) {
            focus_mappings.push_back(focus_json.get<int>());
        }
        /* Clear the list of focus mappings */
        for (auto mapping = std::rbegin(focus_mappings); mapping != std::rend(focus_mappings); mapping++) {
            LOG(fmt::sprintf("focus (reverse) %d\n", (*mapping)));
            auto focus_it = std::ranges::find_if(con->focused, [&mapping](Con *c) { return c->old_id == *mapping; });
            if (focus_it != con->focused.end()) {
                Con *c = *focus_it;
                LOG(fmt::sprintf("got it! %p\n", fmt::ptr(c)));
                /* Move this entry to the top of the focus list. */
                std::rotate(con->focused.begin(), focus_it, con->focused.end());
            }
        }
    }

    if (j.contains("rect")) {
        j.at("rect").get_to(con->rect);
    }

    if (j.contains("deco_rect")) {
        j.at("deco_rect").get_to(con->deco_rect);
    }

    if (j.contains("nodes")) {
        for (auto &child_json : j["nodes"]) {
            Con *child = extract_con(child_json, con, ctx);

            LOG("attaching");
            child->con_attach(con, true);
            LOG("Creating window\n");
            global.x->con_init(child);
        }
    }
}

static RootCon *extract_rootcon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new RootCon{true};
    extract_basecon(j, con, parent, ctx);

    if (j.contains("previous_workspace_name")) {
        global.workspaceManager->previous_workspace_name = j["previous_workspace_name"].get<std::string>();
    }

    return con;
}

static OutputCon *extract_outputcon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new OutputCon{true};
    extract_basecon(j, con, parent, ctx);

    return con;
}

static ConCon *extract_concon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new ConCon{nullptr, true};
    extract_basecon(j, con, parent, ctx);

    if (j.contains("window_icon_padding")) {
        con->set_window_icon_padding(j["window_icon_padding"].get<int>());
    }

    if (j.contains("window_rect")) {
        con->set_window_rect(j.at("window_rect").get<Rect>());
    }

    if (j.contains("geometry")) {
        con->set_geometry(j.at("geometry").get<Rect>());
    }

    return con;
}

static FloatingCon *extract_floatingcon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new FloatingCon{true};
    extract_basecon(j, con, parent, ctx);

    return con;
}

static DockCon *extract_dockcon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new DockCon{true};
    extract_basecon(j, con, parent, ctx);

    return con;
}

static WorkspaceCon *extract_workspacecon(nlohmann::json &j, Con *parent, parser_context &ctx) {
    auto *con = new WorkspaceCon{true};
    extract_basecon(j, con, parent, ctx);

    if (j.contains("workspace_layout")) {
        std::string buf = j["workspace_layout"].get<std::string>();
        if (buf == "default") {
            con->workspace_layout = L_DEFAULT;
        } else if (buf == "stacked") {
            con->workspace_layout = L_STACKED;
        } else if (buf == "tabbed") {
            con->workspace_layout = L_TABBED;
        } else {
            LOG(std::format("Unhandled \"workspace_layout\": {}", buf));
        }
    }

    if (j.contains("num")) {
        con->num = j["num"];
    }

    if (j.contains("floating_nodes")) {
        for (auto &child_json : j["floating_nodes"]) {
            DLOG("New floating_node\n");
            Con *child_con = extract_con(child_json, con, ctx);

            if (auto *child = dynamic_cast<FloatingCon *>(child_con); child != nullptr) {
                child->name.clear();
                DLOG(fmt::sprintf("Parent is workspace = %p\n", fmt::ptr(con)));

                LOG("attaching");
                child->con_attach(con, true);
                LOG("Creating window\n");
                global.x->con_init(child);

                con->floating_windows.push_back(child);
            } else {
                DLOG("Wrong con type in floating_nodes");
            }
        }
    }

    if (j.contains("gaps")) {
        j.at("gaps").get_to(con->gaps);
    }

    return con;
}

static Con *extract_con(nlohmann::json &j, Con *parent, parser_context &ctx) {
    std::string type = j.value("type", "con");
    Con *con{};

    if (type == "root") {
        con = extract_rootcon(j, parent, ctx);
    } else if (type == "output") {
        con = extract_outputcon(j, parent, ctx);
    } else if (type == "con") {
        con = extract_concon(j, parent, ctx);
    } else if (type == "floating_con") {
        con = extract_floatingcon(j, parent, ctx);
    } else if (type == "dockarea") {
        con = extract_dockcon(j, parent, ctx);
    } else if (type == "workspace") {
        con = extract_workspacecon(j, parent, ctx);
    } else {
        LOG(std::format("Unhandled \"type\": {}", type));
        con = extract_concon(j, parent, ctx);
    }

    return con;
}

static void con_massage(Con *con) {
    /* Set a few default values to simplify manually crafted layout files. */
    if (con->layout == layout_t::L_DEFAULT) {
        DLOG("Setting layout = L_SPLITH\n");
        con->layout = layout_t::L_SPLITH;
    }

    /* Sanity check: swallow criteria don’t make any sense on a split
     * container. */
    if (con->con_is_split() > 0 && !con->swallow.empty()) {
        DLOG("sanity check: removing swallows specification from split container\n");
        con->swallow.clear();
    }

    if (con->type == con_type_t::CT_WORKSPACE) {
        /* Ensure the workspace has a name. */
        DLOG(fmt::sprintf("Attaching workspace. name = %s\n", con->name));
        if (con->name.empty()) {
            con->name = "unnamed";
        }

        /* Prevent name clashes when appending a workspace, e.g. when the
         * user tries to restore a workspace called “1” but already has a
         * workspace called “1”. */
        int cnt = 1;
        while (get_existing_workspace_by_name(con->name) != nullptr) {
            con->name = fmt::format("{}_{}", con->name, cnt++);
        }

        /* Set num accordingly so that i3bar will properly sort it. */
        auto ws = dynamic_cast<WorkspaceCon *>(con);
        ws->num = utils::ws_name_to_number(con->name);
    }

    // When appending JSON layout files that only contain the workspace
    // _contents_, we might not have an upfront signal that the
    // container we’re currently parsing is a floating container (like
    // the “floating_nodes” key of the workspace container itself).
    // That’s why we make sure the con is attached at the right place
    // in the hierarchy in case it’s floating.
    if (con->type == con_type_t::CT_FLOATING_CON) {
        DLOG(fmt::sprintf("fixing parent which currently is %p / %s\n", fmt::ptr(con->parent), con->parent->name));
        con->parent = con->parent->con_get_workspace();

        // Also set a size if none was supplied, otherwise the placeholder
        // window cannot be created as X11 requests with width=0 or
        // height=0 are invalid.
        if (con->rect == Rect{}) {
            DLOG("Geometry not set, combining children\n");
            for (auto &child : con->nodes) {
                DLOG(fmt::sprintf("child geometry: %d x %d\n", child->get_geometry().width, child->get_geometry().height));
                con->rect.width += child->get_geometry().width;
                con->rect.height = std::max(con->rect.height, child->get_geometry().height);
            }
        }

        floating_check_size(con, false);
    }

    /* Fix erroneous JSON input regarding floating containers to avoid
     * crashing, see #3901. */
    con_floating_t const old_floating_mode = con->floating;
    if (std::to_underlying(old_floating_mode) >= std::to_underlying(con_floating_t::FLOATING_AUTO_ON) && con->parent->type != con_type_t::CT_FLOATING_CON) {
        LOG("Fixing floating node without CT_FLOATING_CON parent\n");

        /* Force floating_enable to work */
        con->floating = con_floating_t::FLOATING_AUTO_OFF;
        floating_enable(dynamic_cast<ConCon *>(con), false);
        con->floating = old_floating_mode;
    }
}

void tree_append_json(Con *parent, std::istream &&fb) {
    try {
        parser_context ctx{};
        auto j = nlohmann::json::parse(fb, nullptr, true, true);
        Con *con = extract_con(j, parent, ctx);

        LOG("attaching");
        con->con_attach(parent, true);

        con_massage(con);

        /* In case not all containers were restored, we need to fix the
         * percentages, otherwise i3 will crash immediately when rendering the
         * next time. */
        parent->con_fix_percent();

        if (ctx.to_focus != nullptr) {
            ctx.to_focus->con_activate();
        }

        LOG("Creating window\n");
        global.x->con_init(con);
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("JSON parsing error: %s\n", e.what()));
    }
}
