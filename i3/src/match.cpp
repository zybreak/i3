/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * A "match" is a data structure which acts like a mask or expression to match
 * certain windows or not. For example, when using commands, you can specify a
 * command like this: [title="*Firefox*"] kill. The title member of the match
 * data structure will then be filled and i3 will check each window using
 * match_matches_window() to find the windows affected by this command.
 *
 */
module;
#include <xcb/xcb.h>

#include <fmt/printf.h>
module i3;

import std;
import utils;
import regex;
import :match;
import log;
import :con;
import :window;
import :util;

/* From sys/time.h, not sure if it’s available on all systems. */
template<typename Comp = std::ranges::less>
static bool i3_timercmp(std::optional<std::chrono::time_point<std::chrono::system_clock>> a, std::optional<std::chrono::time_point<std::chrono::system_clock>> b, Comp cmp) {
    auto epoch_time_point = std::chrono::system_clock::from_time_t(0);
    return (a && b) ? cmp(a, b) : a ? cmp(a, epoch_time_point) : cmp(epoch_time_point, b);
}

static inline bool is_initialized(Regex const *regex) {
    return regex != nullptr && regex->valid;
}

/*
 * Check if a match is empty. This is necessary while parsing commands to see
 * whether the user specified a match at all.
 *
 */
bool Match::match_is_empty() {
    /* we cannot simply use memcmp() because the structure is part of a
     * TAILQ and I don’t want to start with things like assuming that the
     * last member of a struct really is at the end in memory… */
    return (!is_initialized(this->title.get()) &&
            !is_initialized(this->application.get()) &&
            !is_initialized(this->window_class.get()) &&
            !is_initialized(this->instance.get()) &&
            !is_initialized(this->window_role.get()) &&
            !is_initialized(this->workspace.get()) &&
            !is_initialized(this->machine.get()) &&
            this->urgent == U_DONTCHECK &&
            this->id == XCB_NONE &&
            this->window_type == UINT32_MAX &&
            this->con_id == nullptr &&
            this->dock == M_NODOCK &&
            this->window_mode == WM_ANY &&
            this->match_all_windows == false);
}

static bool checkWindowField(Regex const *match_field, i3Window const *window, std::string i3Window::*window_field) {
    std::string window_field_str = window->*window_field;
    if (match_field->pattern == "__focused__" &&
            global.focused && global.focused->get_window() && !(global.focused->get_window()->*window_field).empty() &&
            window_field_str == global.focused->get_window()->*window_field) {
        LOG("window match_field matches focused window\n");
        return true;
    } else if (match_field->regex_matches(window_field_str)) {
         LOG(std::format("window match_field matches ({})", window_field_str));
         return true;
    }
    
    return false;
}

/*
 * Check if a match data structure matches the given window.
 *
 */
bool Match::match_matches_window(const i3Window *window) const {
    LOG(fmt::sprintf("Checking window 0x%08x (class %s)\n",  window->id, window->class_class));

    // TODO: pointer to member
    if (is_initialized(this->window_class.get()) &&
            !checkWindowField(this->window_class.get(), window, &i3Window::class_class)) {
        return false;
    }
    if (is_initialized(this->instance.get()) &&
            !checkWindowField(this->instance.get(), window, &i3Window::class_instance)) {
        return false;
    }

    if (this->id != XCB_NONE) {
        if (window->id == this->id) {
            LOG(fmt::sprintf("match made by window id (%d)\n", window->id));
        } else {
            LOG("window id does not match\n");
            return false;
        }
    }

    if (is_initialized(this->title.get()) && 
            !checkWindowField(this->title.get(), window, &i3Window::name)) {
        return false;
    }
    if (is_initialized(this->window_role.get()) &&
            !checkWindowField(this->window_role.get(), window, &i3Window::role)) {
        return false;
    }

    if (this->window_type != UINT32_MAX) {
        if (window->window_type == this->window_type) {
            LOG(fmt::sprintf("window_type matches (%i)\n", this->window_type));
        } else {
            return false;
        }
    }

    if (is_initialized(this->machine.get()) &&
            !checkWindowField(this->machine.get(), window, &i3Window::machine)) {
        return false;
    }

    Con *con = nullptr;
    if (this->urgent == U_LATEST) {
        /* if the window isn't urgent, no sense in searching */
        if (!window->urgent) {
            return false;
        }
        /* if we find a window that is newer than this one, bail */
        for (const auto &c : global.all_cons) {
            con = c;
            if ((con->get_window() != nullptr) && i3_timercmp(con->get_window()->urgent, window->urgent, std::ranges::greater())) {
                return false;
            }
        }
        LOG("urgent matches latest\n");
    }

    if (this->urgent == U_OLDEST) {
        /* if the window isn't urgent, no sense in searching */
        if (!window->urgent) {
            return false;
        }
        /* if we find a window that is older than this one (and not 0), bail */
        for (const auto &c : global.all_cons) {
            con = c;
            if ((con->get_window() != nullptr) &&
                con->get_window()->urgent &&
                i3_timercmp(con->get_window()->urgent, window->urgent, std::ranges::less())) {
                return false;
            }
        }
        LOG("urgent matches oldest\n");
    }

    if (this->workspace != nullptr) {
        if ((con = con_by_window_id(window->id)) == nullptr)
            return false;

        WorkspaceCon *ws = con->con_get_workspace();
        if (ws == nullptr)
            return false;

        if (this->workspace->pattern == "__focused__" &&
                ws->name == global.focused->con_get_workspace()->name) {
            LOG("workspace matches focused workspace\n");
        } else if (this->workspace->regex_matches(ws->name)) {
            LOG(fmt::sprintf("workspace matches (%s)\n",  ws->name));
        } else {
            return false;
        }
    }

    if (this->dock != M_DONTCHECK) {
        if ((window->dock == i3Window::W_DOCK_TOP && this->dock == M_DOCK_TOP) ||
            (window->dock == i3Window::W_DOCK_BOTTOM && this->dock == M_DOCK_BOTTOM) ||
            ((window->dock == i3Window::W_DOCK_TOP || window->dock == i3Window::W_DOCK_BOTTOM) &&
             this->dock == M_DOCK_ANY) ||
            (window->dock == i3Window::W_NODOCK && this->dock == M_NODOCK)) {
            LOG("dock status matches\n");
        } else {
            LOG("dock status does not match\n");
            return false;
        }
    }

    if (this->window_mode != WM_ANY) {
        if ((con = con_by_window_id(window->id)) == nullptr) {
            return false;
        }

        switch (this->window_mode) {
            case WM_TILING_AUTO:
                if (con->floating != FLOATING_AUTO_OFF) {
                    return false;
                }
                break;
            case WM_TILING_USER:
                if (con->floating != FLOATING_USER_OFF) {
                    return false;
                }
                break;
            case WM_TILING:
                if (con->con_inside_floating() != nullptr) {
                    return false;
                }
                break;
            case WM_FLOATING_AUTO:
                if (con->floating != FLOATING_AUTO_ON) {
                    return false;
                }
                break;
            case WM_FLOATING_USER:
                if (con->floating != FLOATING_USER_ON) {
                    return false;
                }
                break;
            case WM_FLOATING:
                if (con->con_inside_floating() == nullptr) {
                    return false;
                }
                break;
            case WM_ANY:
                std::terminate();
        }

        LOG("window_mode matches\n");
    }

    /* NOTE: See the comment regarding 'all' in match_parse_property()
     * for an explanation of why match_all_windows isn't explicitly
     * checked. */

    return true;
}

static Con* get_con_id(const char *cvalue) {
    if (strcmp(cvalue, "__focused__") == 0) {
        return global.focused;
    }

    long parsed;
    if (!utils::parse_long(cvalue, &parsed, 0)) {
        ELOG(fmt::sprintf("Could not parse con id \"%s\"\n", cvalue));
        throw std::runtime_error("Invalid match: invalid con_id");
    }
    
    DLOG(fmt::format("id as int = {}\n",  parsed));
    return (Con*)parsed;
}

static int get_id(const char *cvalue) {
    long parsed;
    if (!utils::parse_long(cvalue, &parsed, 0)) {
        ELOG(fmt::sprintf("Could not parse window id \"%s\"\n", cvalue));
        throw std::runtime_error("Invalid match: invalid id");
    }
    
    DLOG(fmt::sprintf("window id as int = %d\n",  parsed));
    return parsed;
}

static xcb_atom_t get_window_type(const char *cvalue) {
    if (strcasecmp(cvalue, "normal") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL];
    } else if (strcasecmp(cvalue, "dialog") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG];
    } else if (strcasecmp(cvalue, "utility") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_UTILITY];
    } else if (strcasecmp(cvalue, "toolbar") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLBAR];
    } else if (strcasecmp(cvalue, "splash") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_SPLASH];
    } else if (strcasecmp(cvalue, "menu") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_MENU];
    } else if (strcasecmp(cvalue, "dropdown_menu") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_DROPDOWN_MENU];
    } else if (strcasecmp(cvalue, "popup_menu") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_POPUP_MENU];
    } else if (strcasecmp(cvalue, "tooltip") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_TOOLTIP];
    } else if (strcasecmp(cvalue, "notification") == 0) {
        return i3::atoms[i3::Atom::_NET_WM_WINDOW_TYPE_NOTIFICATION];
    } else {
        ELOG(fmt::sprintf("unknown window_type value \"%s\"\n", cvalue));
        throw std::runtime_error("Invalid match: unknown window_type value");
    }
}

static match_urgent_t get_urgency(const char *cvalue) {
    if (strcasecmp(cvalue, "latest") == 0 ||
        strcasecmp(cvalue, "newest") == 0 ||
        strcasecmp(cvalue, "recent") == 0 ||
        strcasecmp(cvalue, "last") == 0) {
        return match_urgent_t::U_LATEST;
    } else if (strcasecmp(cvalue, "oldest") == 0 ||
               strcasecmp(cvalue, "first") == 0) {
        return match_urgent_t::U_OLDEST;
    }
    throw std::runtime_error("Invalid match: unknown urgency value");
}

/*
 * Interprets a ctype=cvalue pair and adds it to the given match specification.
 *
 */
void Match::parse_property(const char *ctype, const char *cvalue) {
    DLOG(fmt::sprintf("ctype=*%s*, cvalue=*%s*\n",  ctype, cvalue));

    std::string type = ctype;
    
    // REGEXES
    if (type == "class") {
        this->window_class = std::make_unique<Regex>(cvalue);
        return;
    }

    if (type == "instance") {
        this->instance = std::make_unique<Regex>(cvalue);
        return;
    }

    if (type == "window_role") {
        this->window_role = std::make_unique<Regex>(cvalue);
        return;
    }
        
    if (type == "title") {
        this->title = std::make_unique<Regex>(cvalue);
        return;
    }

    if (type == "workspace") {
        this->workspace = std::make_unique<Regex>(cvalue);
        return;
    }

    if (type == "machine") {
        this->machine = std::make_unique<Regex>(cvalue);
        return;
    }
    // END REGEXES
    
    if (type == "con_id") {
        this->con_id = get_con_id(cvalue);
        return;
    }
        
    if (type == "id") {
        this->id = get_id(cvalue);
        return;
    }
        
    if (type == "window_type") {
        this->window_type = get_window_type(cvalue);
        return;
    }
        
    if (type == "urgent") {
        this->urgent = get_urgency(cvalue);
        return;
    }
        
    // WINDOW MODE

    if (type == "tiling") {
        this->window_mode = WM_TILING;
        return;
    }

    if (type == "tiling_from") {
        if (cvalue != nullptr) {
            if (strcmp(cvalue, "auto") == 0) {
                this->window_mode = WM_TILING_AUTO;
            } else if (strcmp(cvalue, "user") == 0) {
                this->window_mode = WM_TILING_USER;
            }
        }
        return;
    }

    if (type == "floating") {
        this->window_mode = WM_FLOATING;
        return;
    }

    if (type == "floating_from") {
        if (cvalue != nullptr) {
            if (strcmp(cvalue, "auto") == 0) {
                this->window_mode = WM_FLOATING_AUTO;
            } else if (strcmp(cvalue, "user") == 0) {
                this->window_mode = WM_FLOATING_USER;
            }
        }
        return;
    }
        
    // END WINDOW MODE
    
    if (type == "all") {
        /* match_matches_window() only checks negatively, so match_all_windows
        * won't actually be used there, but that's OK because if no negative
        * match is found (e.g. because of a more restrictive criterion) the
        * return value of match_matches_window() is true.
        * Setting it here only serves to cause match_is_empty() to return false,
        * otherwise empty criteria rules apply, and that's not what we want. */
        this->match_all_windows = true;
        return;
    }

    ELOG(fmt::sprintf("Unknown criterion: %s\n",  ctype));
}
