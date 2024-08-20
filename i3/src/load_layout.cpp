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
module i3;

import std;
import utils;
import log;
import regex;
import rect;

/* TODO: refactor the whole parsing thing */

class Match *current_swallow;

struct tree_append_ctx {
    std::string last_key{};
    Con *json_node{nullptr};
    Con *to_focus{nullptr};
    bool parsing_gaps{false};
    int incomplete{0};
    bool parsing_swallows{false};
    bool parsing_rect{false};
    bool parsing_actual_deco_rect{false};
    bool parsing_deco_rect{false};
    bool parsing_window_rect{false};
    bool parsing_geometry{false};
    bool parsing_focus{false};
    bool swallow_is_empty;
    std::deque<int> focus_mappings{};
};

static void json_start_map(tree_append_ctx &ctx) {
    LOG(fmt::sprintf("start of map, last_key = %s\n",  ctx.last_key));
    if (ctx.parsing_swallows) {
        LOG("creating new swallow\n");
        auto match = std::make_unique<Match>();
        current_swallow = match.get();
        match->dock = M_DONTCHECK;
        ctx.json_node->swallow.push_back(std::move(match));
        ctx.swallow_is_empty = true;
    } else {
        if (!ctx.parsing_rect &&
            !ctx.parsing_actual_deco_rect &&
            !ctx.parsing_deco_rect &&
            !ctx.parsing_window_rect &&
            !ctx.parsing_geometry &&
            !ctx.parsing_gaps) {
            if (ctx.last_key == "floating_nodes") {
                DLOG("New floating_node\n");
                WorkspaceCon *ws = ctx.json_node->con_get_workspace();
                ctx.json_node = new ConCon(nullptr, true);
                ctx.json_node->name.clear();
                ctx.json_node->parent = ws;
                DLOG(fmt::sprintf("Parent is workspace = %p\n", fmt::ptr(ws)));
            }
            /* json_node is incomplete and should be removed if parsing fails */
            ctx.incomplete++;
            DLOG(fmt::sprintf("incomplete = %d\n",  ctx.incomplete));
        }
    }
}

static void json_end_map(tree_append_ctx &ctx) {
    LOG("end of map\n");
    if (!ctx.parsing_swallows &&
        !ctx.parsing_rect &&
        !ctx.parsing_actual_deco_rect &&
        !ctx.parsing_deco_rect &&
        !ctx.parsing_window_rect &&
        !ctx.parsing_geometry &&
        !ctx.parsing_gaps) {
        /* Set a few default values to simplify manually crafted layout files. */
        if (ctx.json_node->layout == L_DEFAULT) {
            DLOG("Setting layout = L_SPLITH\n");
            ctx.json_node->layout = L_SPLITH;
        }

        /* Sanity check: swallow criteria don’t make any sense on a split
         * container. */
        if (ctx.json_node->con_is_split() > 0 && !ctx.json_node->swallow.empty()) {
            DLOG("sanity check: removing swallows specification from split container\n");
            ctx.json_node->swallow.clear();
        }

        if (ctx.json_node->type == CT_WORKSPACE) {
            /* Ensure the workspace has a name. */
            DLOG(fmt::sprintf("Attaching workspace. name = %s\n",  ctx.json_node->name));
            if (ctx.json_node->name.empty()) {
                ctx.json_node->name = "unnamed";
            }

            /* Prevent name clashes when appending a workspace, e.g. when the
             * user tries to restore a workspace called “1” but already has a
             * workspace called “1”. */
            int cnt = 1;
            while (get_existing_workspace_by_name(ctx.json_node->name) != nullptr) {
                ctx.json_node->name = fmt::format("{}_{}", ctx.json_node->name, cnt++);
            }

            /* Set num accordingly so that i3bar will properly sort it. */
            auto ws = dynamic_cast<WorkspaceCon*>(ctx.json_node);
            ws->num = utils::ws_name_to_number(ctx.json_node->name);
        }

        // When appending JSON layout files that only contain the workspace
        // _contents_, we might not have an upfront signal that the
        // container we’re currently parsing is a floating container (like
        // the “floating_nodes” key of the workspace container itself).
        // That’s why we make sure the con is attached at the right place
        // in the hierarchy in case it’s floating.
        if (ctx.json_node->type == CT_FLOATING_CON) {
            DLOG(fmt::sprintf("fixing parent which currently is %p / %s\n", fmt::ptr(ctx.json_node->parent), ctx.json_node->parent->name));
            ctx.json_node->parent = ctx.json_node->parent->con_get_workspace();

            // Also set a size if none was supplied, otherwise the placeholder
            // window cannot be created as X11 requests with width=0 or
            // height=0 are invalid.
            if (ctx.json_node->rect == (Rect){0, 0, 0, 0}) {
                DLOG("Geometry not set, combining children\n");
                for (auto &child : ctx.json_node->nodes) {
                    DLOG(fmt::sprintf("child geometry: %d x %d\n",  child->get_geometry().width, child->get_geometry().height));
                    ctx.json_node->rect.width += child->get_geometry().width;
                    ctx.json_node->rect.height = std::max(ctx.json_node->rect.height, child->get_geometry().height);
                }
            }

            floating_check_size(ctx.json_node, false);
        }

        /* Fix erroneous JSON input regarding floating containers to avoid
         * crashing, see #3901. */
        const int old_floating_mode = ctx.json_node->floating;
        if (old_floating_mode >= FLOATING_AUTO_ON && ctx.json_node->parent->type != CT_FLOATING_CON) {
            LOG("Fixing floating node without CT_FLOATING_CON parent\n");

            /* Force floating_enable to work */
            ctx.json_node->floating = FLOATING_AUTO_OFF;
            floating_enable(dynamic_cast<ConCon*>(ctx.json_node), false);
            ctx.json_node->floating = static_cast<con_floating_t>(old_floating_mode);
        }

        ctx.json_node = ctx.json_node->parent;
        ctx.incomplete--;
        DLOG(fmt::sprintf("incomplete = %d\n",  ctx.incomplete));
    }

    if (ctx.parsing_swallows && ctx.swallow_is_empty) {
        /* We parsed an empty swallow definition. This is an invalid layout
         * definition, hence we reject it. */
        ELOG("Layout file is invalid: found an empty swallow definition.\n");
        return;
    }

    ctx.parsing_gaps = false;
    ctx.parsing_rect = false;
    ctx.parsing_actual_deco_rect = false;
    ctx.parsing_deco_rect = false;
    ctx.parsing_window_rect = false;
    ctx.parsing_geometry = false;
}

static void json_end_array(tree_append_ctx &ctx) {
    LOG("end of array\n");
    if (!ctx.parsing_swallows && !ctx.parsing_focus) {
        ctx.json_node->con_fix_percent();
    }
    if (ctx.parsing_swallows) {
        ctx.parsing_swallows = false;
    }

    if (ctx.parsing_focus) {
        /* Clear the list of focus mappings */
        for (auto mapping = ctx.focus_mappings.end(); mapping != ctx.focus_mappings.begin();--mapping) {
            LOG(fmt::sprintf("focus (reverse) %d\n",  (*mapping)));
            for (auto &con : ctx.json_node->focused) {
                if (con->old_id != *mapping)
                    continue;
                LOG(fmt::sprintf("got it! %p\n", fmt::ptr(con)));
                /* Move this entry to the top of the focus list. */
                std::erase(ctx.json_node->focused, con);
                ctx.json_node->focused.push_front(con);
                break;
            }
        }
        ctx.focus_mappings.clear();
        ctx.parsing_focus = false;
    }
}

static void json_key(tree_append_ctx &ctx, const std::string &key) {
    LOG(fmt::sprintf("key: %s\n",  key));
    ctx.last_key = key;
    if (ctx.last_key == "swallows")
        ctx.parsing_swallows = true;

    if (strcasecmp(ctx.last_key.c_str(), "gaps") == 0)
        ctx.parsing_gaps = true;

    if (strcasecmp(ctx.last_key.c_str(), "actual_deco_rect") == 0)
        ctx.parsing_actual_deco_rect = true;

    if (ctx.last_key == "focus")
        ctx.parsing_focus = true;
}

static void json_string(tree_append_ctx &ctx, std::string &val) {
    LOG(fmt::sprintf("string: %s for key %s\n",  val, ctx.last_key));
    if (ctx.parsing_swallows) {
        if (ctx.last_key == "class") {
            current_swallow->window_class = std::make_unique<Regex>(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "instance") {
            current_swallow->instance = std::make_unique<Regex>(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "window_role") {
            current_swallow->window_role = std::make_unique<Regex>(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "title") {
            current_swallow->title = std::make_unique<Regex>(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "machine") {
            current_swallow->machine = std::make_unique<Regex>(val.c_str());
            ctx.swallow_is_empty = false;
        } else {
            ELOG(fmt::sprintf("swallow key %s unknown\n",  ctx.last_key));
        }
    }
}

/*
 * Returns true if the provided JSON could be parsed.
 *
 */
bool json_validate(std::string &fb) {

    setlocale(LC_NUMERIC, "C");
    bool valid = nlohmann::json::accept(fb, true);
    setlocale(LC_NUMERIC, "");

    return valid;
}

/* Parses the given JSON file until it encounters the first “type” property to
 * determine whether the file contains workspaces or regular containers, which
 * is important to know when deciding where (and how) to append the contents.
 * */
json_content_t json_determine_content(std::string &fb) {
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

static Con* extract_con(nlohmann::json &j, Con *parent);

static void extract_basecon(nlohmann::json &j, Con *con, Con *parent) {
    if (j.contains("name")) j["name"].get_to(con->name);
    if (j.contains("title_format")) j["title_format"].get_to(con->title_format);
    if (j.contains("sticky_group")) j["sticky_group"].get_to(con->sticky_group);
    
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

    //if (ctx.last_key == "focused" && val) {
    //    ctx.to_focus = ctx.json_node;
    //}

    if (j.contains("sticky")) {
        con->sticky = j["sticky"];
    }

#if 0
    if (ctx.parsing_swallows) {
        if (ctx.last_key == "restart_mode") {
            current_swallow->restart_mode = val;
            ctx.swallow_is_empty = false;
        }
    }
#endif

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

    //if (ctx.parsing_focus) {
    //    ctx.focus_mappings.push_back(val);
    //}

    if (j.contains("rect")) {
        j.at("rect").get_to(con->rect);
    }

    if (j.contains("deco_rect")) {
        j.at("deco_rect").get_to(con->deco_rect);
    }


#if 0
    if (ctx.parsing_swallows) {
        if (ctx.last_key == "id") {
            current_swallow->id = val;
            ctx.swallow_is_empty = false;
        }
        if (ctx.last_key == "dock") {
            current_swallow->dock = (enum match_dock_t)val;
            ctx.swallow_is_empty = false;
        }
        if (ctx.last_key == "insert_where") {
            current_swallow->insert_where = static_cast<match_insert_t>(val);
            ctx.swallow_is_empty = false;
        }
    }
#endif
    if (j.contains("gaps")) {
#if 0
        if (strcasecmp(ctx.last_key.c_str(), "inner") == 0)
            ctx.json_node->gaps.inner = val;
        else if (strcasecmp(ctx.last_key.c_str(), "top") == 0)
            ctx.json_node->gaps.top = val;
        else if (strcasecmp(ctx.last_key.c_str(), "right") == 0)
            ctx.json_node->gaps.right = val;
        else if (strcasecmp(ctx.last_key.c_str(), "bottom") == 0)
            ctx.json_node->gaps.bottom = val;
        else if (strcasecmp(ctx.last_key.c_str(), "left") == 0)
            ctx.json_node->gaps.left = val;
#endif
    }
    
    if (j.contains("nodes")) {
        for (auto &child_json : j["nodes"]) {
            Con *child = extract_con(child_json, con);
            con->nodes.push_back(child);
        }
    }

#if 0
    if (j.contains("floating_nodes")) {
        Con *child = extract_con(j, con);
        FloatingCon
        j.floating_windows.push_back(child);
    }
#endif
}

static RootCon* extract_rootcon(nlohmann::json &j, Con *parent) {
    auto *con = new RootCon{true};
    extract_basecon(j, con, parent);

    return con;
}

static OutputCon* extract_outputcon(nlohmann::json &j, Con *parent) {
    auto *con = new OutputCon{true};
    extract_basecon(j, con, parent);

    return con;
}

static ConCon* extract_concon(nlohmann::json &j, Con *parent) {
    auto *con = new ConCon{nullptr, true};
    extract_basecon(j, con, parent);

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

static FloatingCon* extract_floatingcon(nlohmann::json &j, Con *parent) {
    auto *con = new FloatingCon{true};
    extract_basecon(j, con, parent);

    return con;
}

static DockCon* extract_dockcon(nlohmann::json &j, Con *parent) {
    auto *con = new DockCon{true};
    extract_basecon(j, con, parent);

    return con;
}

static WorkspaceCon* extract_workspacecon(nlohmann::json &j, Con *parent) {
    auto *con = new WorkspaceCon{true};
    extract_basecon(j, con, parent);

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

    if (j.contains("num")) con->num = j["num"];

    //if (j.contains("previous_workspace_name")) j["previous_workspace_name"].get_to(con->previous_workspace_name);
    return con;
}

static Con* extract_con(nlohmann::json &j, Con *parent) {
   std::string type = j.value("type", "con");
   Con *con{};
   
   if (type == "root") {
       con = extract_rootcon(j, parent);
   } else if (type == "output") {
       con = extract_outputcon(j, parent);
   } else if (type == "con") {
       con = extract_concon(j, parent);
   } else if (type == "floating_con") {
       con = extract_floatingcon(j, parent);
   } else if (type == "dockarea") {
       con = extract_dockcon(j, parent);
   } else if (type == "workspace") {
       con = extract_workspacecon(j, parent);
   } else {
       LOG(std::format("Unhandled \"type\": {}", type));
       con = extract_concon(j, parent);
   }

    LOG("attaching");
    con->con_attach(parent, true);
    LOG("Creating window\n");
    global.x->con_init(con);

    return con;
}

void tree_append_json(Con *parent, std::string &fb) {
#if 0
    nlohmann::json::parser_callback_t cb = [&ctx](int depth, nlohmann::json::parse_event_t event, nlohmann::json & parsed) {
        if (event == nlohmann::json::parse_event_t::array_start) {
        } else if (event == nlohmann::json::parse_event_t::array_end) {
            json_end_array(ctx);
        } else if (event == nlohmann::json::parse_event_t::object_start) {
            json_start_map(ctx);
        } else if (event == nlohmann::json::parse_event_t::object_end) {
            json_end_map(ctx);
        } else if (event == nlohmann::json::parse_event_t::key) {
            auto key = parsed.get<std::string>();
            json_key(ctx, key);
        } else if (event == nlohmann::json::parse_event_t::value) {
            if (parsed.is_boolean()) {
                auto key = parsed.get<bool>();
                json_bool(ctx, key);
            } else if (parsed.is_number_integer()) {
                auto key = parsed.get<long long>();
                json_int(ctx, key);
            } else if (parsed.is_number_float()) {
                auto key = parsed.get<double>();
                json_double(ctx, key);
            } else if (parsed.is_string()) {
                auto key = parsed.get<std::string>();
                json_string(ctx, key);
            }
        }

        return true;
    };
#endif

    try {
        auto j = nlohmann::json::parse(fb, nullptr, true, true);
        Con *con = extract_con(j, parent);
        std::cout << j << std::endl;
    } catch (std::exception &e) {
#if 0
        ELOG(fmt::sprintf("JSON parsing error: %s\n",  e.what()));
        while (ctx.incomplete-- > 0) {
            Con *parent = ctx.json_node->parent;
            DLOG(fmt::sprintf("freeing incomplete container %p\n", fmt::ptr(ctx.json_node)));
            if (ctx.json_node == ctx.to_focus) {
                ctx.to_focus = nullptr;
            }
            delete ctx.json_node;
            ctx.json_node = parent;
        }
        throw e;
#endif
    }

    /* In case not all containers were restored, we need to fix the
     * percentages, otherwise i3 will crash immediately when rendering the
     * next time. */
    //con->con_fix_percent();

    //if (ctx.to_focus) {
    //    ctx.to_focus->con_activate();
    //}
}
