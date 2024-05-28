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
                Con *ws = ctx.json_node->con_get_workspace();
                ctx.json_node = new ConCon(nullptr, nullptr, true);
                ctx.json_node->name.clear();
                ctx.json_node->parent = ws;
                DLOG(fmt::sprintf("Parent is workspace = %p\n", fmt::ptr(ws)));
            } else {
                Con *parent = ctx.json_node;
                ctx.json_node = new ConCon(nullptr, nullptr, true);
                ctx.json_node->name.clear();
                ctx.json_node->parent = parent;
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
            ctx.json_node->num = utils::ws_name_to_number(ctx.json_node->name);
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
                    DLOG(fmt::sprintf("child geometry: %d x %d\n",  child->geometry.width, child->geometry.height));
                    ctx.json_node->rect.width += child->geometry.width;
                    ctx.json_node->rect.height = std::max(ctx.json_node->rect.height, child->geometry.height);
                }
            }

            floating_check_size(ctx.json_node, false);
        }

        LOG("attaching\n");
        ctx.json_node->con_attach(ctx.json_node->parent, true);
        LOG("Creating window\n");
        global.x->con_init(ctx.json_node);

        /* Fix erroneous JSON input regarding floating containers to avoid
         * crashing, see #3901. */
        const int old_floating_mode = ctx.json_node->floating;
        if (old_floating_mode >= FLOATING_AUTO_ON && ctx.json_node->parent->type != CT_FLOATING_CON) {
            LOG("Fixing floating node without CT_FLOATING_CON parent\n");

            /* Force floating_enable to work */
            ctx.json_node->floating = FLOATING_AUTO_OFF;
            floating_enable(ctx.json_node, false);
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

    if (ctx.last_key == "rect")
        ctx.parsing_rect = true;

    if (strcasecmp(ctx.last_key.c_str(), "actual_deco_rect") == 0)
        ctx.parsing_actual_deco_rect = true;

    if (ctx.last_key == "deco_rect")
        ctx.parsing_deco_rect = true;

    if (ctx.last_key == "window_rect")
        ctx.parsing_window_rect = true;

    if (ctx.last_key == "geometry")
        ctx.parsing_geometry = true;

    if (ctx.last_key == "focus")
        ctx.parsing_focus = true;
}

static void json_string(tree_append_ctx &ctx, std::string &val) {
    LOG(fmt::sprintf("string: %s for key %s\n",  val, ctx.last_key));
    if (ctx.parsing_swallows) {
        if (ctx.last_key == "class") {
            current_swallow->window_class = new Regex(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "instance") {
            current_swallow->instance = new Regex(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "window_role") {
            current_swallow->window_role = new Regex(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "title") {
            current_swallow->title = new Regex(val.c_str());
            ctx.swallow_is_empty = false;
        } else if (ctx.last_key == "machine") {
            current_swallow->machine = new Regex(val.c_str());
            ctx.swallow_is_empty = false;
        } else {
            ELOG(fmt::sprintf("swallow key %s unknown\n",  ctx.last_key));
        }
    } else {
        if (ctx.last_key == "name") {
            ctx.json_node->name.assign(val);
        } else if (ctx.last_key == "title_format") {
            ctx.json_node->title_format.assign(val);
        } else if (ctx.last_key == "sticky_group") {
            ctx.json_node->sticky_group.assign(val);
            LOG(fmt::sprintf("sticky_group of this container is %s\n",  ctx.json_node->sticky_group));
        } else if (ctx.last_key == "orientation") {
            /* Upgrade path from older versions of i3 (doing an inplace restart
             * to a newer version):
             * "orientation" is dumped before "layout". Therefore, we store
             * whether the orientation was horizontal or vertical in the
             * last_split_layout. When we then encounter layout == "default",
             * we will use the last_split_layout as layout instead. */
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "none") == 0 ||
                strcasecmp(buf, "horizontal") == 0)
                ctx.json_node->last_split_layout = L_SPLITH;
            else if (strcasecmp(buf, "vertical") == 0)
                ctx.json_node->last_split_layout = L_SPLITV;
            else
                LOG(fmt::sprintf("Unhandled orientation: %s\n",  buf));
            free(buf);
        } else if (ctx.last_key == "border") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "none") == 0) {
                ctx.json_node->max_user_border_style = BS_NONE;
                ctx.json_node->border_style = BS_NONE;
            } else if (strcasecmp(buf, "1pixel") == 0) {
                ctx.json_node->max_user_border_style = BS_PIXEL;
                ctx.json_node->border_style = BS_PIXEL;
                ctx.json_node->current_border_width = 1;
            } else if (strcasecmp(buf, "pixel") == 0) {
                ctx.json_node->max_user_border_style = BS_PIXEL;
                ctx.json_node->border_style = BS_PIXEL;
            } else if (strcasecmp(buf, "normal") == 0) {
                ctx.json_node->max_user_border_style = BS_NORMAL;
                ctx.json_node->border_style = BS_NORMAL;
            } else {
                LOG(fmt::sprintf("Unhandled \"border\": %s\n", buf));
            }
            free(buf);
        } else if (ctx.last_key == "type") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "root") == 0)
                ctx.json_node->type = CT_ROOT;
            else if (strcasecmp(buf, "output") == 0)
                ctx.json_node->type = CT_OUTPUT;
            else if (strcasecmp(buf, "con") == 0)
                ctx.json_node->type = CT_CON;
            else if (strcasecmp(buf, "floating_con") == 0)
                ctx.json_node->type = CT_FLOATING_CON;
            else if (strcasecmp(buf, "workspace") == 0)
                ctx.json_node->type = CT_WORKSPACE;
            else if (strcasecmp(buf, "dockarea") == 0)
                ctx.json_node->type = CT_DOCKAREA;
            else
                 LOG(fmt::sprintf("Unhandled \"type\": %s\n", buf));
            free(buf);
        } else if (ctx.last_key == "layout") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "default") == 0)
                /* This set above when we read "orientation". */
                ctx.json_node->layout = ctx.json_node->last_split_layout;
            else if (strcasecmp(buf, "stacked") == 0)
                ctx.json_node->layout = L_STACKED;
            else if (strcasecmp(buf, "tabbed") == 0)
                ctx.json_node->layout = L_TABBED;
            else if (strcasecmp(buf, "dockarea") == 0)
                ctx.json_node->layout = L_DOCKAREA;
            else if (strcasecmp(buf, "output") == 0)
                ctx.json_node->layout = L_OUTPUT;
            else if (strcasecmp(buf, "splith") == 0)
                ctx.json_node->layout = L_SPLITH;
            else if (strcasecmp(buf, "splitv") == 0)
                ctx.json_node->layout = L_SPLITV;
            else
                 LOG(fmt::sprintf("Unhandled \"layout\": %s\n", buf));
            free(buf);
        } else if (ctx.last_key == "workspace_layout") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            // TODO: needed to be commented out
            //if (strcasecmp(buf, "default") == 0)
            //    ctx.json_node->workspace_layout = L_DEFAULT;
            //else if (strcasecmp(buf, "stacked") == 0)
            //   ctx.json_node->workspace_layout = L_STACKED;
            //else if (strcasecmp(buf, "tabbed") == 0)
            //    ctx.json_node->workspace_layout = L_TABBED;
            //else
            //     LOG(fmt::sprintf("Unhandled \"workspace_layout\": %s\n", buf));
            free(buf);
        } else if (ctx.last_key == "last_split_layout") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "splith") == 0)
                ctx.json_node->last_split_layout = L_SPLITH;
            else if (strcasecmp(buf, "splitv") == 0)
                ctx.json_node->last_split_layout = L_SPLITV;
            else
                 LOG(fmt::sprintf("Unhandled \"last_splitlayout\": %s\n", buf));
            free(buf);
        } else if (ctx.last_key == "floating") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "auto_off") == 0)
                ctx.json_node->floating = FLOATING_AUTO_OFF;
            else if (strcasecmp(buf, "auto_on") == 0)
                ctx.json_node->floating = FLOATING_AUTO_ON;
            else if (strcasecmp(buf, "user_off") == 0)
                ctx.json_node->floating = FLOATING_USER_OFF;
            else if (strcasecmp(buf, "user_on") == 0)
                ctx.json_node->floating = FLOATING_USER_ON;
            free(buf);
        } else if (ctx.last_key == "scratchpad_state") {
            char *buf = nullptr;
            sasprintf(&buf, "%.*s", static_cast<int>(val.length()), val.c_str());
            if (strcasecmp(buf, "none") == 0)
                ctx.json_node->scratchpad_state = SCRATCHPAD_NONE;
            else if (strcasecmp(buf, "fresh") == 0)
                ctx.json_node->scratchpad_state = SCRATCHPAD_FRESH;
            else if (strcasecmp(buf, "changed") == 0)
                ctx.json_node->scratchpad_state = SCRATCHPAD_CHANGED;
            free(buf);
        } else if (ctx.last_key == "previous_workspace_name") {
            previous_workspace_name.assign(val);
        }
    }
}

static void json_int(tree_append_ctx &ctx, long long val) {
    LOG(fmt::sprintf("int %lld for key %s\n",  val, ctx.last_key));
    /* For backwards compatibility with i3 < 4.8 */
    if (ctx.last_key == "type")
        ctx.json_node->type = static_cast<con_type_t>(val);

    if (ctx.last_key == "fullscreen_mode")
        ctx.json_node->fullscreen_mode = static_cast<fullscreen_mode_t>(val);

    if (ctx.last_key == "num")
        ctx.json_node->num = val;

    if (ctx.last_key == "current_border_width")
        ctx.json_node->current_border_width = val;

    if (ctx.last_key == "window_icon_padding") {
        ctx.json_node->window_icon_padding = val;
    }

    if (ctx.last_key == "depth")
        ctx.json_node->depth = val;

    if (!ctx.parsing_swallows && ctx.last_key == "id")
        ctx.json_node->old_id = val;

    if (ctx.parsing_focus) {
        ctx.focus_mappings.push_back(val);
    }

    if (ctx.parsing_rect || ctx.parsing_window_rect || ctx.parsing_geometry) {
        Rect *r;
        if (ctx.parsing_rect)
            r = &(ctx.json_node->rect);
        else if (ctx.parsing_window_rect)
            r = &(ctx.json_node->window_rect);
        else
            r = &(ctx.json_node->geometry);
        if (ctx.last_key == "x")
            r->x = val;
        else if (ctx.last_key == "y")
            r->y = val;
        else if (ctx.last_key == "width")
            r->width = val;
        else if (ctx.last_key == "height")
            r->height = val;
        else
            ELOG(fmt::sprintf("WARNING: unknown key %s in rect\n",  ctx.last_key));
        DLOG(fmt::sprintf("rect now: (%d, %d, %d, %d)\n",
             r->x, r->y, r->width, r->height));
    }
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
    if (ctx.parsing_gaps) {
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
}

static void json_bool(tree_append_ctx &ctx, int val) {
    LOG(fmt::sprintf("bool %d for key %s\n",  val, ctx.last_key));
    if (ctx.last_key == "focused" && val) {
        ctx.to_focus = ctx.json_node;
    }

    if (ctx.last_key == "sticky")
        ctx.json_node->sticky = val;

    if (ctx.parsing_swallows) {
        if (ctx.last_key == "restart_mode") {
            current_swallow->restart_mode = val;
            ctx.swallow_is_empty = false;
        }
    }
}

static void json_double(tree_append_ctx &ctx, double val) {
    LOG(fmt::sprintf("double %f for key %s\n",  val, ctx.last_key));
    if (ctx.last_key == "percent") {
        ctx.json_node->percent = val;
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
    json_content_t content_result = JSON_CONTENT_CON;
    std::string last_key;

    nlohmann::json::parser_callback_t cb = [&last_key, &content_result](int depth, nlohmann::json::parse_event_t event, nlohmann::json & parsed) {

        if (event == nlohmann::json::parse_event_t::key) {
            last_key = parsed.get<std::string>();
        } else if (event == nlohmann::json::parse_event_t::value) {
            if (last_key == "type" or depth > 1 or !parsed.is_string())
                return true;

            auto key = parsed.get<std::string>();

            if (key == "workspace")
                content_result = JSON_CONTENT_WORKSPACE;
        }

        return true;
    };

    setlocale(LC_NUMERIC, "C");
    try {
        auto j = nlohmann::json::parse(fb, cb, true, true);
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("JSON parsing error: %s\n",  e.what()));
    }

    setlocale(LC_NUMERIC, "");

    return content_result;
}

void tree_append_json(Con *con, std::string &fb, char **errormsg) {
    tree_append_ctx ctx = {
        .json_node = con
    };

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

    setlocale(LC_NUMERIC, "C");

    try {
        auto j = nlohmann::json::parse(fb, cb, true, true);
    } catch (std::exception &e) {
        if (e.what() != nullptr) {
            *errormsg = sstrdup(e.what());
        }
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
    }

    /* In case not all containers were restored, we need to fix the
     * percentages, otherwise i3 will crash immediately when rendering the
     * next time. */
    con->con_fix_percent();

    setlocale(LC_NUMERIC, "");

    if (ctx.to_focus) {
        ctx.to_focus->con_activate();
    }
}
