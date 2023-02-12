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
#include <cassert>

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <xcb/xcb.h>

#include "libi3.h"

#include "util.h"
#include "tree.h"
#include "xcb.h"
#include "configuration.h"
#include "con.h"
#include "match.h"
#include "regex.h"

/* From sys/time.h, not sure if it’s available on all systems. */
#define _i3_timercmp(a, b, CMP) \
    (((a).tv_sec == (b).tv_sec) ? ((a).tv_usec CMP(b).tv_usec) : ((a).tv_sec CMP(b).tv_sec))

static bool is_initialized(const Regex *regex) {
    return regex != nullptr && regex->valid;
}

/*
 * Check if a match is empty. This is necessary while parsing commands to see
 * whether the user specified a match at all.
 *
 */
bool match_is_empty(Match &match) {
    /* we cannot simply use memcmp() because the structure is part of a
     * TAILQ and I don’t want to start with things like assuming that the
     * last member of a struct really is at the end in memory… */
    return (!is_initialized(match.title) &&
            !is_initialized(match.application) &&
            !is_initialized(match.window_class) &&
            !is_initialized(match.instance) &&
            !is_initialized(match.window_role) &&
            !is_initialized(match.workspace) &&
            !is_initialized(match.machine) &&
            match.urgent == U_DONTCHECK &&
            match.id == XCB_NONE &&
            match.window_type == UINT32_MAX &&
            match.con_id == nullptr &&
            match.dock == M_NODOCK &&
            match.window_mode == WM_ANY &&
            match.match_all_windows == false);
}

Match& Match::operator=(Match &&src) noexcept {
    std::swap(this->error, src.error);
    std::swap(this->window_type, src.window_type);
    std::swap(this->urgent, src.urgent);
    std::swap(this->dock, src.dock);
    std::swap(this->id, src.id);
    std::swap(this->window_mode, src.window_mode);
    std::swap(this->con_id, src.con_id);
    std::swap(this->match_all_windows, src.match_all_windows);
    std::swap(this->insert_where, src.insert_where);
    std::swap(this->restart_mode, src.restart_mode);

    std::swap(this->title, src.title);
    std::swap(this->application, src.application);
    std::swap(this->window_class, src.window_class);
    std::swap(this->instance, src.instance);
    std::swap(this->window_role, src.window_role);
    std::swap(this->workspace, src.workspace);

    return *this;
}

/*
 * Copies the data of a match from src to dest.
 *
 */
Match::Match(const Match &src) {

    this->error = src.error;
    this->window_type= src.window_type;
    this->urgent= src.urgent;
    this->dock= src.dock;
    this->id= src.id;
    this->window_mode= src.window_mode;
    this->con_id = src.con_id;
    this->match_all_windows= src.match_all_windows;
    this->insert_where= src.insert_where;
    this->restart_mode= src.restart_mode;

/* The DUPLICATE_REGEX macro creates a new regular expression from the
 * ->pattern of the old one. It therefore does use a little more memory then
 *  with a refcounting system, but it’s easier this way. */

    if (is_initialized(src.title))              this->title = new Regex(*src.title);
    if (is_initialized(src.application))        this->application = new Regex(*src.application);
    if (is_initialized(src.window_class))       this->window_class = new Regex(*src.window_class);
    if (is_initialized(src.instance))           this->instance = new Regex(*src.instance);
    if (is_initialized(src.window_role))        this->window_role = new Regex(*src.window_role);
    if (is_initialized(src.workspace))          this->workspace = new Regex(*src.workspace);
}

/*
 * Check if a match data structure matches the given window.
 *
 */
bool match_matches_window(Match &match, i3Window *window) {
    LOG(fmt::sprintf("Checking window 0x%08x (class %s)\n",  window->id, window->class_class));

#define GET_FIELD_str(field) (field)
#define GET_FIELD_i3string(field) (i3string_as_utf8(field))
#define CHECK_WINDOW_FIELD(match_field, window_field, type)                                       \
    do {                                                                                          \
        if (is_initialized(match.match_field)) {                                                 \
            const char *window_field_str = window->window_field == NULL                           \
                                               ? ""                                               \
                                               : GET_FIELD_##type(window->window_field);          \
            if (strcmp(match.match_field->pattern, "__focused__") == 0 &&                        \
                focused && focused->window && focused->window->window_field &&                    \
                strcmp(window_field_str, GET_FIELD_##type(focused->window->window_field)) == 0) { \
                LOG("window " #match_field " matches focused window\n");                          \
            } else if (match.match_field->regex_matches(window_field_str)) {                     \
                 LOG(fmt::sprintf("window " #match_field " matches (%s)\n", window_field_str));   \
            } else {                                                                              \
                return false;                                                                     \
            }                                                                                     \
        }                                                                                         \
    } while (0)

    CHECK_WINDOW_FIELD(window_class, class_class, str);
    CHECK_WINDOW_FIELD(instance, class_instance, str);

    if (match.id != XCB_NONE) {
        if (window->id == match.id) {
            LOG(fmt::sprintf("match made by window id (%d)\n",  window->id));
        } else {
            LOG("window id does not match\n");
            return false;
        }
    }

    CHECK_WINDOW_FIELD(title, name, i3string);
    CHECK_WINDOW_FIELD(window_role, role, str);

    if (match.window_type != UINT32_MAX) {
        if (window->window_type == match.window_type) {
            LOG(fmt::sprintf("window_type matches (%i)\n",  match.window_type));
        } else {
            return false;
        }
    }

    CHECK_WINDOW_FIELD(machine, machine, str);

    Con *con = nullptr;
    if (match.urgent == U_LATEST) {
        /* if the window isn't urgent, no sense in searching */
        if (window->urgent.tv_sec == 0) {
            return false;
        }
        /* if we find a window that is newer than this one, bail */
        for (const auto &c : all_cons) {
            con = c;
            if ((con->window != nullptr) &&
                _i3_timercmp(con->window->urgent, window->urgent, >)) {
                return false;
            }
        }
        LOG("urgent matches latest\n");
    }

    if (match.urgent == U_OLDEST) {
        /* if the window isn't urgent, no sense in searching */
        if (window->urgent.tv_sec == 0) {
            return false;
        }
        /* if we find a window that is older than this one (and not 0), bail */
        for (const auto &c : all_cons) {
            con = c;
            if ((con->window != nullptr) &&
                (con->window->urgent.tv_sec != 0) &&
                _i3_timercmp(con->window->urgent, window->urgent, <)) {
                return false;
            }
        }
        LOG("urgent matches oldest\n");
    }

    if (match.workspace != nullptr) {
        if ((con = con_by_window_id(window->id)) == nullptr)
            return false;

        Con *ws = con->con_get_workspace();
        if (ws == nullptr)
            return false;

        if (strcmp(match.workspace->pattern, "__focused__") == 0 &&
            strcmp(ws->name.c_str(), focused->con_get_workspace()->name.c_str()) == 0) {
            LOG("workspace matches focused workspace\n");
        } else if (match.workspace->regex_matches(ws->name.c_str())) {
            LOG(fmt::sprintf("workspace matches (%s)\n",  ws->name));
        } else {
            return false;
        }
    }

    if (match.dock != M_DONTCHECK) {
        if ((window->dock == i3Window::W_DOCK_TOP && match.dock == M_DOCK_TOP) ||
            (window->dock == i3Window::W_DOCK_BOTTOM && match.dock == M_DOCK_BOTTOM) ||
            ((window->dock == i3Window::W_DOCK_TOP || window->dock == i3Window::W_DOCK_BOTTOM) &&
             match.dock == M_DOCK_ANY) ||
            (window->dock == i3Window::W_NODOCK && match.dock == M_NODOCK)) {
            LOG("dock status matches\n");
        } else {
            LOG("dock status does not match\n");
            return false;
        }
    }

    if (match.window_mode != WM_ANY) {
        if ((con = con_by_window_id(window->id)) == nullptr) {
            return false;
        }

        switch (match.window_mode) {
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
                assert(false);
        }

        LOG("window_mode matches\n");
    }

    /* NOTE: See the comment regarding 'all' in match_parse_property()
     * for an explanation of why match_all_windows isn't explicitly
     * checked. */

    return true;
}

/*
 * Frees the given match. It must not be used afterwards!
 *
 */
Match::~Match() {
    FREE(this->error);
    delete this->title;
    delete this->application;
    delete this->window_class;
    delete this->instance;
    delete this->window_role;
    delete this->workspace;
    delete this->machine;
}

/*
 * Interprets a ctype=cvalue pair and adds it to the given match specification.
 *
 */
void Match::parse_property(const char *ctype, const char *cvalue) {
    DLOG(fmt::sprintf("ctype=*%s*, cvalue=*%s*\n",  ctype, cvalue));

    if (strcmp(ctype, "class") == 0) {
        delete this->window_class;
        this->window_class = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "instance") == 0) {
        delete this->instance;
        this->instance = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "window_role") == 0) {
        delete this->window_role;
        this->window_role = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "con_id") == 0) {
        if (strcmp(cvalue, "__focused__") == 0) {
            this->con_id = focused;
            return;
        }

        long parsed;
        if (!parse_long(cvalue, &parsed, 0)) {
             ELOG(fmt::sprintf("Could not parse con id \"%s\"\n", cvalue));
            this->error = sstrdup("invalid con_id");
        } else {
            this->con_id = (Con *)parsed;
            DLOG(fmt::sprintf("id as int = %p\n",  (void*)this->con_id));
        }
        return;
    }

    if (strcmp(ctype, "id") == 0) {
        long parsed;
        if (!parse_long(cvalue, &parsed, 0)) {
             ELOG(fmt::sprintf("Could not parse window id \"%s\"\n", cvalue));
            this->error = sstrdup("invalid id");
        } else {
            this->id = parsed;
            DLOG(fmt::sprintf("window id as int = %d\n",  this->id));
        }
        return;
    }

    if (strcmp(ctype, "window_type") == 0) {
        if (strcasecmp(cvalue, "normal") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_NORMAL;
        } else if (strcasecmp(cvalue, "dialog") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_DIALOG;
        } else if (strcasecmp(cvalue, "utility") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_UTILITY;
        } else if (strcasecmp(cvalue, "toolbar") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_TOOLBAR;
        } else if (strcasecmp(cvalue, "splash") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_SPLASH;
        } else if (strcasecmp(cvalue, "menu") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_MENU;
        } else if (strcasecmp(cvalue, "dropdown_menu") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_DROPDOWN_MENU;
        } else if (strcasecmp(cvalue, "popup_menu") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_POPUP_MENU;
        } else if (strcasecmp(cvalue, "tooltip") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_TOOLTIP;
        } else if (strcasecmp(cvalue, "notification") == 0) {
            this->window_type = A__NET_WM_WINDOW_TYPE_NOTIFICATION;
        } else {
             ELOG(fmt::sprintf("unknown window_type value \"%s\"\n", cvalue));
            this->error = sstrdup("unknown window_type value");
        }

        return;
    }

    if (strcmp(ctype, "title") == 0) {
        delete this->title;
        this->title = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "urgent") == 0) {
        if (strcasecmp(cvalue, "latest") == 0 ||
            strcasecmp(cvalue, "newest") == 0 ||
            strcasecmp(cvalue, "recent") == 0 ||
            strcasecmp(cvalue, "last") == 0) {
            this->urgent = U_LATEST;
        } else if (strcasecmp(cvalue, "oldest") == 0 ||
                   strcasecmp(cvalue, "first") == 0) {
            this->urgent = U_OLDEST;
        }
        return;
    }

    if (strcmp(ctype, "workspace") == 0) {
        delete this->workspace;
        this->workspace = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "machine") == 0) {
        delete this->machine;
        this->machine = new Regex(cvalue);
        return;
    }

    if (strcmp(ctype, "tiling") == 0) {
        this->window_mode = WM_TILING;
        return;
    }

    if (strcmp(ctype, "tiling_from") == 0 &&
        cvalue != nullptr &&
        strcmp(cvalue, "auto") == 0) {
        this->window_mode = WM_TILING_AUTO;
        return;
    }

    if (strcmp(ctype, "tiling_from") == 0 &&
        cvalue != nullptr &&
        strcmp(cvalue, "user") == 0) {
        this->window_mode = WM_TILING_USER;
        return;
    }

    if (strcmp(ctype, "floating") == 0) {
        this->window_mode = WM_FLOATING;
        return;
    }

    if (strcmp(ctype, "floating_from") == 0 &&
        cvalue != nullptr &&
        strcmp(cvalue, "auto") == 0) {
        this->window_mode = WM_FLOATING_AUTO;
        return;
    }

    if (strcmp(ctype, "floating_from") == 0 &&
        cvalue != nullptr &&
        strcmp(cvalue, "user") == 0) {
        this->window_mode = WM_FLOATING_USER;
        return;
    }

    /* match_matches_window() only checks negatively, so match_all_windows
     * won't actually be used there, but that's OK because if no negative
     * match is found (e.g. because of a more restrictive criterion) the
     * return value of match_matches_window() is true.
     * Setting it here only serves to cause match_is_empty() to return false,
     * otherwise empty criteria rules apply, and that's not what we want. */
    if (strcmp(ctype, "all") == 0) {
        this->match_all_windows = true;
        return;
    }

    ELOG(fmt::sprintf("Unknown criterion: %s\n",  ctype));
}
