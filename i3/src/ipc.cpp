/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * ipc.c: UNIX domain socket IPC (initialization, client handling, protocol).
 *
 */
module;
#include <config.h>

#include <cassert>
#include <cerrno>

#include <cstdint>
#include <cstdio>
#include <vector>

#include <xcb/xcb.h>

#include "i3_ipc/i3-ipc.h"
#include "i3.h"

#include <fcntl.h>
#include <clocale>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <nlohmann/json.hpp>
#include <ranges>
#include "atoms.h"

#include <fmt/printf.h>
module i3;

import log;
import i3ipc;
import utils;
import regex;
import rect;
import i3_commands_base;
import i3_config_base;

#define FREE(pointer)   \
    do {                \
        free(pointer);  \
        pointer = NULL; \
    } while (0)

static std::vector<ipc_client*> all_clients{};

static void ipc_client_timeout(EV_P_ ev_timer *w, int revents);
static void ipc_socket_writeable_cb(ev_io *w, int revents);

static ev_tstamp kill_timeout = 10.0;

void ipc_set_kill_timeout(ev_tstamp new_timeout) {
    kill_timeout = new_timeout;
}

/*
 * Try to write the contents of the pending buffer to the client's subscription
 * socket. Will set, reset or clear the timeout and io write callbacks depending
 * on the result of the write operation.
 *
 */
static void ipc_push_pending(ipc_client *client) {
    const ssize_t result = writeall_nonblock(client->fd, client->buffer, client->buffer_size);
    if (result < 0) {
        return;
    }

    if ((size_t)result == client->buffer_size) {
        /* Everything was written successfully: clear the timer and stop the io
         * callback. */
        do {
            free(client->buffer);
            client->buffer = __null;
        } while (0);
        client->buffer_size = 0;
        if (client->timeout) {
            ev_timer_stop(global.eventHandler->main_loop, client->timeout);
            delete client->timeout;
            client->timeout = nullptr;
        }
        ev_io_stop(global.eventHandler->main_loop, client->write_callback);
        return;
    }

    /* Otherwise, make sure that the io callback is enabled and create a new
     * timer if needed. */
    ev_io_start(global.eventHandler->main_loop, client->write_callback);

    if (!client->timeout) {
        auto *timeout = new ev_timer();
        ev_timer_init(timeout, ipc_client_timeout, kill_timeout, 0.);
        timeout->data = client;
        client->timeout = timeout;
        ev_set_priority(timeout, EV_MINPRI);
        ev_timer_start(global.eventHandler->main_loop, client->timeout);
    } else if (result > 0) {
        /* Keep the old timeout when nothing is written. Otherwise, we would
         * keep a dead connection by continuously renewing its timeouts. */
        ev_timer_stop(global.eventHandler->main_loop, client->timeout);
        ev_timer_set(client->timeout, kill_timeout, 0.0);
        ev_timer_start(global.eventHandler->main_loop, client->timeout);
    }
    if (result == 0) {
        return;
    }

    /* Shift the buffer to the left and reduce the allocated space. */
    client->buffer_size -= (size_t)result;
    memmove(client->buffer, client->buffer + result, client->buffer_size);
    client->buffer = (uint8_t*)srealloc(client->buffer, client->buffer_size);
}

/*
 * Given a message and a message type, create the corresponding header, merge it
 * with the message and append it to the given client's output buffer. Also,
 * send the message if the client's buffer was empty.
 *
 */
static void ipc_send_client_message(ipc_client *client, const uint32_t message_type, const std::string &payload) {
    auto size = payload.length();

    const i3ipc::i3_ipc_header_t header = {
        .magic = {'i', '3', '-', 'i', 'p', 'c'},
        .size = (uint32_t)size,
        .type = message_type};
    const size_t header_size = sizeof(i3ipc::i3_ipc_header_t);
    const size_t message_size = header_size + size;

    const bool push_now = (client->buffer_size == 0);
    client->buffer = (uint8_t*)srealloc(client->buffer, client->buffer_size + message_size);
    memcpy(client->buffer + client->buffer_size, ((void *)&header), header_size);
    memcpy(client->buffer + client->buffer_size + header_size, payload.c_str(), size);
    client->buffer_size += message_size;

    if (push_now) {
        ipc_push_pending(client);
    }
}

static void free_ipc_client(ipc_client *client, int exempt_fd) {
    if (client->fd != exempt_fd) {
        DLOG(fmt::sprintf("Disconnecting client on fd %d\n",  client->fd));
        close(client->fd);
    }

    std::erase(all_clients, client);

    delete client;
}

/*
 * Sends the specified event to all IPC clients which are currently connected
 * and subscribed to this kind of event.
 *
 */
void ipc_send_event(const char *event, uint32_t message_type, const std::string &payload) {
    for (auto &current : all_clients) {
        for (auto &e : current->events) {
            if (e == event) {
                ipc_send_client_message(current, message_type, payload);
                break;
            }
        }
    }
}

/*
 * For shutdown events, we send the reason for the shutdown.
 */
static void ipc_send_shutdown_event(shutdown_reason_t reason) {
    nlohmann::json j;

    if (reason == SHUTDOWN_REASON_RESTART) {
        j["change"] = "restart";
    } else if (reason == SHUTDOWN_REASON_EXIT) {
        j["change"] = "exit";
    }

    auto payload = j.dump();

    ipc_send_event("shutdown", I3_IPC_EVENT_SHUTDOWN, payload);
}

/*
 * Calls shutdown() on each socket and closes it. This function is to be called
 * when exiting or restarting only!
 *
 * exempt_fd is never closed. Set to -1 to close all fds.
 *
 */
void ipc_shutdown(shutdown_reason_t reason, int exempt_fd) {
    ipc_send_shutdown_event(reason);

    while (!all_clients.empty()) {
        auto &current = all_clients.front();
        if (current->fd != exempt_fd) {
            shutdown(current->fd, SHUT_RDWR);
        }
        free_ipc_client(current, exempt_fd);
    }
}

/*
 * Executes the given command.
 *
 */
static void handle_run_command(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    /* To get a properly terminated buffer, we copy
     * message_size bytes out of the buffer */
    std::string command((const char *)message, message_size);
    LOG(fmt::sprintf("IPC: received: *%.4000s*\n",  command));
    nlohmann::json gen;

    auto commandsApplier = CommandsApplier{};
    CommandResult result = parse_command(command, &gen, client, &commandsApplier);

    if (result.needs_tree_render) {
        tree_render();
    }

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_COMMAND, gen.dump());
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

static nlohmann::json dump_event_state_mask(Binding *bind) {
    auto a = nlohmann::json::array();

    for (int i = 0; i < 20; i++) {
        if (bind->event_state_mask & (1 << i)) {
            switch (1 << i) {
                case XCB_KEY_BUT_MASK_SHIFT:
                    a.push_back("shift");
                    break;
                case XCB_KEY_BUT_MASK_LOCK:
                    a.push_back("lock");
                    break;
                case XCB_KEY_BUT_MASK_CONTROL:
                    a.push_back("ctrl");
                    break;
                case XCB_KEY_BUT_MASK_MOD_1:
                    a.push_back("Mod1");
                    break;
                case XCB_KEY_BUT_MASK_MOD_2:
                    a.push_back("Mod2");
                    break;
                case XCB_KEY_BUT_MASK_MOD_3:
                    a.push_back("Mod3");
                    break;
                case XCB_KEY_BUT_MASK_MOD_4:
                    a.push_back("Mod4");
                    break;
                case XCB_KEY_BUT_MASK_MOD_5:
                    a.push_back("Mod5");
                    break;
                case XCB_KEY_BUT_MASK_BUTTON_1:
                    a.push_back("Button1");
                    break;
                case XCB_KEY_BUT_MASK_BUTTON_2:
                    a.push_back("Button2");
                    break;
                case XCB_KEY_BUT_MASK_BUTTON_3:
                    a.push_back("Button3");
                    break;
                case XCB_KEY_BUT_MASK_BUTTON_4:
                    a.push_back("Button4");
                    break;
                case XCB_KEY_BUT_MASK_BUTTON_5:
                    a.push_back("Button5");
                    break;
                case (I3_XKB_GROUP_MASK_1 << 16):
                    a.push_back("Group1");
                    break;
                case (I3_XKB_GROUP_MASK_2 << 16):
                    a.push_back("Group2");
                    break;
                case (I3_XKB_GROUP_MASK_3 << 16):
                    a.push_back("Group3");
                    break;
                case (I3_XKB_GROUP_MASK_4 << 16):
                    a.push_back("Group4");
                    break;
            }
        }
    }

    return a;
}

static nlohmann::json dump_binding(Binding *bind) {
    nlohmann::json j;

    j["input_code"] = bind->keycode;
    j["input_type"] = bind->input_type == B_KEYBOARD ? "keyboard" : "mouse";

    if (!bind->symbol.empty()) {
        j["symbol"] = bind->symbol;
    }

    j["command"] = bind->command;

    // This key is only provided for compatibility, new programs should use
    // event_state_mask instead.
    j["mods"] = dump_event_state_mask(bind);
    j["event_state_mask"] = dump_event_state_mask(bind);

    return j;
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

static std::string scratchpad_state(Con *con) {
    switch (con->scratchpad_state) {
        case SCRATCHPAD_NONE:
            return "none";
        case SCRATCHPAD_FRESH:
            return "fresh";
        case SCRATCHPAD_CHANGED:
            return "changed";
        default:
            throw std::runtime_error("unknown scratchpad_state");
    }
}

static std::string layout(Con *con) {
    switch (con->layout) {
        case L_DEFAULT:
            DLOG("About to dump layout=default, this is a bug in the code.\n");
            assert(false);
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
            DLOG(fmt::sprintf("About to dump workspace_layout=%d (none of default/stacked/tabbed), this is a bug.\n",  con->workspace_layout));
            assert(false);
            break;
    }
}

static std::string border_style(Con *con) {
    switch (con->border_style) {
        case BS_NORMAL:
            return "normal";
        case BS_NONE:
            return "none";
        case BS_PIXEL:
            return "pixel";
        default:
            throw std::runtime_error("unknown border_style");
    }
}

static std::string window_type(Con *con) {
    if (con->window->window_type == A__NET_WM_WINDOW_TYPE_NORMAL) {
        return "normal";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_DOCK) {
        return "dock";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_DIALOG) {
        return "dialog";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_UTILITY) {
        return "utility";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_TOOLBAR) {
        return "toolbar";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_SPLASH) {
        return "splash";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_MENU) {
        return "menu";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_DROPDOWN_MENU) {
        return "dropdown_menu";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_POPUP_MENU) {
        return "popup_menu";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_TOOLTIP) {
        return "tooltip";
    } else if (con->window->window_type == A__NET_WM_WINDOW_TYPE_NOTIFICATION) {
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

    j["scratchpad_state"] = scratchpad_state(con);

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
    j["window_rect"] = dump_rect(con->window_rect);
    j["geometry"] = dump_rect(con->geometry);

    if (con->window && !con->window->name.empty()) {
        j["name"] = con->window->name;
    } else if (!con->name.empty()) {
        j["name"] = con->name;
    }

    if (!con->title_format.empty()) {
        j["title_format"] = con->title_format;
    }

    j["window_icon_padding"] = con->window_icon_padding;

    if (con->type == CT_WORKSPACE) {
        WorkspaceCon *ws = dynamic_cast<WorkspaceCon*>(con);
        j["num"] = ws->num;
        j["gaps"] = dump_gaps(ws->gaps);
    }

    if (con->window) {
        j["window"] = con->window->id;
        j["window_type"] = window_type(con);
    }


    if (con->window && !inplace_restart) {
        /* Window properties are useless to preserve when restarting because
         * they will be queried again anyway. However, for i3-save-tree(1),
         * they are very useful and save i3-save-tree dealing with X11. */
        auto map = nlohmann::json::object();

        if (con->window->class_class != nullptr) {
            map["class"] = con->window->class_class;
        }
        if (con->window->class_instance != nullptr) {
            map["instance"] = con->window->class_instance;
        }
        if (con->window->role != nullptr) {
            map["window_role"] = con->window->role;
        }
        if (con->window->machine != nullptr) {
            map["machine"] = con->window->machine;
        }

        if (!con->window->name.empty()) {
            map["title"] = con->window->name.c_str();
        }

        if (con->window->transient_for != XCB_NONE) {
            j["transient_for"] = con->window->transient_for;
        }

        j["window_properties"] = map;
    }

    if (con->type != CT_DOCKAREA || !inplace_restart) {
        auto a = nlohmann::json::array();
        std::ranges::transform(con->nodes_head, std::back_inserter(a), [&inplace_restart](Con *node) { return dump_node(node, inplace_restart); });
        j["nodes"] = a;
    }

    if (dynamic_cast<WorkspaceCon*>(con)) {
        auto floating_nodes_array = nlohmann::json::array();
        std::ranges::transform(dynamic_cast<WorkspaceCon*>(con)->floating_windows, std::back_inserter(floating_nodes_array), [&inplace_restart](Con *node) { return dump_node(node, inplace_restart); });
        j["floating_nodes"] = floating_nodes_array;
    }

    auto focus_nodes_array = nlohmann::json::array();
    std::ranges::transform(con->focus_head, std::back_inserter(focus_nodes_array), [](Con *node) { return (uintptr_t)node; });
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
        if (con->window != nullptr) {
            swallows.push_back({
               {"id", con->window->id},
               {"restart_mode", true},
           });
        }
    }
    j["swallows"] = swallows;

    if (inplace_restart && con->window != nullptr) {
        j["depth"] = con->depth;
    }

    if (inplace_restart && con->type == CT_ROOT && !previous_workspace_name.empty()) {
        j["previous_workspace_name"] = previous_workspace_name.c_str();
    }

    return j;
}

static nlohmann::json dump_bar_bindings(Barconfig *config) {
    auto a = nlohmann::json::array();

    if (config->bar_bindings.empty()) {
        return a;
    }

    for (auto &current : config->bar_bindings) {
        a.push_back({
            { "input_code", current->input_code },
            { "command", current->command },
            { "release", current->release == B_UPON_KEYRELEASE }
        });
    }

    return a;
}

static std::string canonicalize_output_name(const std::string &name) {
    /* Do not canonicalize special output names. */
    if (strcasecmp(name.c_str(), "primary") == 0 || strcasecmp(name.c_str(), "nonprimary") == 0) {
        return name;
    }
    Output *output = global.randr->get_output_by_name(name, false);
    return output ? output->output_primary_name() : name;
}

static std::string mode(Barconfig *config) {
    switch (config->mode) {
        case M_HIDE:
            return "hide";
        case M_INVISIBLE:
            return "invisible";
        case M_DOCK:
        default:
            return "dock";
    }
}

static std::string hidden_state(Barconfig *config) {
    switch (config->hidden_state) {
        case S_SHOW:
            return "show";
        case S_HIDE:
        default:
            return "hide";
    }
}

static nlohmann::json dump_bar_config(Barconfig *config) {
    nlohmann::json j;

    j["id"] = config->id;

    if (!config->outputs.empty()) {
        auto a = nlohmann::json::array();
        for (auto &output : config->outputs) {
            /* Convert monitor names (RandR ≥ 1.5) or output names
             * (RandR < 1.5) into monitor names. This way, existing
             * configs which use output names transparently keep
             * working. */
            a.push_back(canonicalize_output_name(output));
        }
        j["outputs"] = a;
    }

    if (!config->tray_outputs.empty()) {
        auto a = nlohmann::json::array();
        std::ranges::transform(config->tray_outputs, std::back_inserter(a), [](auto &output) { return canonicalize_output_name(output); });
        j["tray_outputs"] = a;
    }

    j["tray_padding"] = config->tray_padding;
    if (config->socket_path) {
        j["socket_path"] = config->socket_path;
    }

    j["mode"] = mode(config);

    j["hidden_state"] = hidden_state(config);


    j["modifier"] = config->modifier;

    j["bindings"] = dump_bar_bindings(config);

    j["position"] = (config->position == P_BOTTOM) ? "bottom" : "top";

    if (config->status_command) {
        j["status_command"] = config->status_command;
    }

    if (config->workspace_command) {
        j["workspace_command"] = config->workspace_command;
    }

    if (config->font) {
        j["font"] = config->font;
    }

    if (config->bar_height) {
        j["bar_height"] = config->bar_height;
    }

    j["padding"] = dump_rect(config->padding);

    if (config->separator_symbol) {
        j["separator_symbol"] = config->separator_symbol;
    }

    j["workspace_buttons"] = !config->hide_workspace_buttons;
    j["workspace_min_width"] = config->workspace_min_width;
    j["strip_workspace_numbers"] = config->strip_workspace_numbers;
    j["strip_workspace_name"] = config->strip_workspace_name;
    j["binding_mode_indicator"] = !config->hide_binding_mode_indicator;
    j["verbose"] = config->verbose;

    auto m = nlohmann::json::object();
    if (config->colors.background) { m["background"] = config->colors.background; }
    if (config->colors.statusline) { m["statusline"] = config->colors.statusline; }
    if (config->colors.separator) { m["separator"] = config->colors.separator; }
    if (config->colors.focused_background) { m["focused_background"] = config->colors.focused_background; }
    if (config->colors.focused_statusline) { m["focused_statusline"] = config->colors.focused_statusline; }
    if (config->colors.focused_separator) { m["focused_separator"] = config->colors.focused_separator; }
    if (config->colors.focused_workspace_border) { m["focused_workspace_border"] = config->colors.focused_workspace_border; }
    if (config->colors.focused_workspace_bg) { m["focused_workspace_bg"] = config->colors.focused_workspace_bg; }
    if (config->colors.focused_workspace_text) { m["focused_workspace_text"] = config->colors.focused_workspace_text; }
    if (config->colors.active_workspace_border) { m["active_workspace_border"] = config->colors.active_workspace_border; }
    if (config->colors.active_workspace_bg) { m["active_workspace_bg"] = config->colors.active_workspace_bg; }
    if (config->colors.active_workspace_text) { m["active_workspace_text"] = config->colors.active_workspace_text; }
    if (config->colors.inactive_workspace_border) { m["inactive_workspace_border"] = config->colors.inactive_workspace_border; }
    if (config->colors.inactive_workspace_bg) { m["inactive_workspace_bg"] = config->colors.inactive_workspace_bg; }
    if (config->colors.inactive_workspace_text) { m["inactive_workspace_text"] = config->colors.inactive_workspace_text; }
    if (config->colors.urgent_workspace_border) { m["urgent_workspace_border"] = config->colors.urgent_workspace_border; }
    if (config->colors.urgent_workspace_bg) { m["urgent_workspace_bg"] = config->colors.urgent_workspace_bg; }
    if (config->colors.urgent_workspace_text) { m["urgent_workspace_text"] = config->colors.urgent_workspace_text; }
    if (config->colors.binding_mode_border) { m["binding_mode_border"] = config->colors.binding_mode_border; }
    if (config->colors.binding_mode_bg) { m["binding_mode_bg"] = config->colors.binding_mode_bg; }
    if (config->colors.binding_mode_text) { m["binding_mode_text"] = config->colors.binding_mode_text; }

    j["colors"] = m;

    return j;
}

static void handle_tree(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    setlocale(LC_NUMERIC, "C");
    auto j = dump_node(global.croot, false);
    setlocale(LC_NUMERIC, "");

    auto payload = j.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_TREE, payload);
}

/*
 * Formats the reply message for a GET_WORKSPACES request and sends it to the
 * client
 *
 */
static void handle_get_workspaces(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();

    Con *focused_ws = global.focused->con_get_workspace();

    for (auto &output : global.croot->nodes_head) {
        for (auto &ws : output->output_get_content()->nodes_head) {
            assert(ws->type == CT_WORKSPACE);

            a.push_back({
                { "id", (uintptr_t)ws },
                { "num", ws->num },
                { "name", ws->name },
                { "visible", workspace_is_visible(ws) },
                { "focused", ws == focused_ws },
                { "rect", dump_rect(ws->rect) },
                { "output", output->name },
                {"urgent", ws->urgent }
            });
        }
    }

    auto payload = a.dump();
    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_WORKSPACES, payload);
}

/*
 * Formats the reply message for a GET_OUTPUTS request and sends it to the
 * client
 *
 */
static void handle_get_outputs(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {

    auto a = nlohmann::json::array();

    for (Output *output : global.randr->outputs) {

        auto o = nlohmann::json::object();

        o["name"] = output->output_primary_name();
        o["active"] = output->active;
        o["primary"] = output->primary;
        o["rect"] = dump_rect(output->rect);
        Con *ws = nullptr;
        if (output->con && (ws = output->con->con_get_fullscreen_con(CF_OUTPUT))) {
            o["current_workspace"] = ws->name;
        }

        a.push_back(o);
    }

    auto payload = a.dump();
    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_OUTPUTS, payload);
}

/*
 * Returns the version of i3
 *
 */
static void handle_get_version(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    nlohmann::json j;

    j["major"] = MAJOR_VERSION;
    j["minor"] = MINOR_VERSION;
    j["patch"] = PATCH_VERSION;
    j["human_readable"] = I3_VERSION;
    j["loaded_config_file_name"] = current_configpath;

    auto a = nlohmann::json::array();
    for (auto &included_file : std::ranges::drop_view{included_files,1}) {
        a.push_back(included_file->path);
    }
    j["included_config_file_names"] = a;

    auto payload = j.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_VERSION, payload);
}

/*
 * Formats the reply message for a GET_BAR_CONFIG request and sends it to the
 * client.
 *
 */
static void handle_get_bar_config(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    /* If no ID was passed, we return a JSON array with all IDs */
    if (message_size == 0) {
        auto a = nlohmann::json::array();
        std::ranges::transform(barconfigs, std::back_inserter(a), [](auto &current) { return current->id; });
        auto payload = a.dump();

        ipc_send_client_message(client, I3_IPC_REPLY_TYPE_BAR_CONFIG, payload);
        return;
    }

    /* To get a properly terminated buffer, we copy
     * message_size bytes out of the buffer */
    char *bar_id = nullptr;
    sasprintf(&bar_id, "%.*s", message_size, message);
     LOG(fmt::sprintf("IPC: looking for config for bar ID \"%s\"\n", bar_id));
    auto config_ptr = std::ranges::find_if(barconfigs, [&bar_id](auto &current) {
        return (strcmp(current->id, bar_id) == 0);
    });
    free(bar_id);

    nlohmann::json j;

    if (config_ptr == barconfigs.end()) {
        /* If we did not find a config for the given ID, the reply will contain
         * a null 'id' field. */
        j["id"] = nullptr;
    } else {
        j = dump_bar_config((*config_ptr).get());
    }

    auto payload = j.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_BAR_CONFIG, payload);
}

/*
 * Returns a list of configured binding modes
 *
 */
static void handle_get_binding_modes(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();
    std::ranges::transform(modes, std::back_inserter(a), [](auto &mode) { return mode->name; });

    auto payload = a.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_BINDING_MODES, payload);
}

/*
 * Callback for the YAJL parser (will be called when a string is parsed).
 *
 */
static void add_subscription(ipc_client *client, std::string &s) {

    client->events.emplace_back(s.c_str());

    DLOG("client is now subscribed to:\n");
    for (auto &event : client->events) {
        DLOG(fmt::sprintf("event %s\n",  event));
    }
    DLOG("(done)\n");
}

/*
 * Subscribes this connection to the event types which were given as a JSON
 * serialized array in the payload field of the message.
 *
 */
static void handle_subscribe(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    try {
        auto j = nlohmann::json::parse(std::string((const char*)message, message_size));
        if (j.is_array()) {
            for (auto type : j) {
                if (type.is_string()) {
                    auto &s = type.get_ref<nlohmann::json::string_t &>();
                    add_subscription(client, s);
                }
            }
        } else {
            throw std::invalid_argument("Invalid JSON structure");
        }
    } catch (std::exception &e) {
        auto err = e.what();
        ELOG(fmt::sprintf("Parse error: %s\n",  err));

        std::string reply = R"({"success":false})";
        ipc_send_client_message(client, I3_IPC_REPLY_TYPE_SUBSCRIBE, reply);
        return;
    }

    std::string reply = R"({"success":true})";
    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_SUBSCRIBE, reply);

    if (client->first_tick_sent) {
        return;
    }

    bool is_tick = std::ranges::any_of(client->events, [](auto &event) { return event == "tick"; });

    if (!is_tick) {
        return;
    }

    client->first_tick_sent = true;
    std::string payload = R"({"first":true,"payload":""})";
    ipc_send_client_message(client, I3_IPC_EVENT_TICK, payload);
}

/*
 * Returns the raw last loaded i3 configuration file contents.
 */
static void handle_get_config(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();

    for (auto &included_file : std::ranges::drop_view{included_files,1}) {
        a.push_back(std::string(included_file->raw_contents));
    }

    nlohmann::json j = {
        { "config", std::string(included_files[0]->raw_contents) },
        { "included_configs", a }
    };

    auto payload = j.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_CONFIG, payload);
}

/*
 * Sends the tick event from the message payload to subscribers. Establishes a
 * synchronization point in event-related tests.
 */
static void handle_send_tick(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {

    nlohmann::json j;

    j["first"] = false;
    j["payload"] = std::string((char *)message, message_size);

    auto payload = j.dump();

    ipc_send_event("tick", I3_IPC_EVENT_TICK, payload);

    std::string reply = R"({"success":true})";
    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_TICK, reply);
    DLOG("Sent tick event\n");
}

struct sync_state {
    std::string last_key{};
    uint32_t rnd;
    xcb_window_t window;
};

static void _sync_json_int(sync_state &state, long long val) {
    if (state.last_key == "rnd") {
        state.rnd = val;
    } else if (state.last_key == "window") {
        state.window = (xcb_window_t)val;
    }
}

static void handle_sync(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    sync_state state{};

    nlohmann::json::parser_callback_t cb = [&state](int depth, nlohmann::json::parse_event_t event, nlohmann::json & parsed) {
        if (event == nlohmann::json::parse_event_t::key) {
            auto key = parsed.get<std::string>();
            state.last_key = key;
        } else if (event == nlohmann::json::parse_event_t::value) {
            if (parsed.is_number_integer()) {
                auto key = parsed.get<long long>();
                _sync_json_int(state, key);
            }
        }

        return true;
    };

    try {
        auto json = nlohmann::json::parse(std::string((const char *) message, message_size), cb, true, true);
    } catch (std::exception &e) {
        ELOG(fmt::sprintf("Parse error: %s\n",  e.what()));

        std::string reply = "{\"success\":false}";
        ipc_send_client_message(client, I3_IPC_REPLY_TYPE_SYNC, reply);
        return;
    }

    DLOG(fmt::sprintf("received IPC sync request (rnd = %d, window = 0x%08x)\n",  state.rnd, state.window));
    sync_respond(global.x,state.window, state.rnd);
    std::string reply = "{\"success\":true}";
    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_SYNC, reply);
}

static void handle_get_binding_state(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    nlohmann::json j = {
        { "name",  current_mode->name}
    };

    auto payload = j.dump();

    ipc_send_client_message(client, I3_IPC_REPLY_TYPE_GET_BINDING_STATE, payload);
}

/* The index of each callback function corresponds to the numeric
 * value of the message type (see include/i3/ipc.h) */
handler_t handlers[13] = {
    handle_run_command,
    handle_get_workspaces,
    handle_subscribe,
    handle_get_outputs,
    handle_tree,
    nullptr,
    handle_get_bar_config,
    handle_get_version,
    handle_get_binding_modes,
    handle_get_config,
    handle_send_tick,
    handle_sync,
    handle_get_binding_state,
};

/*
 * Handler for activity on a client connection, receives a message from a
 * client.
 *
 * For now, the maximum message size is 2048. I’m not sure for what the
 * IPC interface will be used in the future, thus I’m not implementing a
 * mechanism for arbitrarily long messages, as it seems like overkill
 * at the moment.
 *
 */
static void ipc_receive_message(EV_P_ ev_io *w, int revents) {
    uint32_t message_type;
    uint32_t message_length;
    uint8_t *message = nullptr;
    auto *client = (ipc_client *)w->data;
    assert(client->fd == w->fd);

    int ret = i3ipc::ipc_recv_message(w->fd, &message_type, &message_length, &message);
    /* EOF or other error */
    if (ret < 0) {
        /* Was this a spurious read? See ev(3) */
        if (ret == -1 && errno == EAGAIN) {
            do {
                free(message);
                message = __null;
            } while (0);
            return;
        }

        /* If not, there was some kind of error. We don’t bother and close the
         * connection. Delete the client from the list of clients. */
        free_ipc_client(client, -1);
        do {
            free(message);
            message = __null;
        } while (0);
        return;
    }

    if (message_type >= (sizeof(handlers) / sizeof(handler_t))) {
        DLOG(fmt::sprintf("Unhandled message type: %d\n",  message_type));
    } else {
        handler_t h = handlers[message_type];
        h(client, message, 0, message_length, message_type);
    }

    do {
        free(message);
        message = __null;
    } while (0);
}

static void ipc_client_timeout(EV_P_ ev_timer *w, int revents) {
    /* No need to be polite and check for writeability, the other callback would
     * have been called by now. */
    auto *client = (ipc_client *)w->data;

    char *cmdline = nullptr;
#if defined(__linux__) && defined(SO_PEERCRED)
    struct ucred peercred{};
    socklen_t so_len = sizeof(peercred);
    if (getsockopt(client->fd, SOL_SOCKET, SO_PEERCRED, &peercred, &so_len) != 0) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n",  (void*)client, client->fd));
        }

        free_ipc_client(client, -1);
        return;
    }
    char *exepath;
    sasprintf(&exepath, "/proc/%d/cmdline", peercred.pid);

    int fd = open(exepath, O_RDONLY);
    free(exepath);
    if (fd == -1) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n",  (void*)client, client->fd));
        }

        free_ipc_client(client, -1);
        return;
    }
    char buf[512] = {'\0'}; /* cut off cmdline for the error message. */
    const ssize_t n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n < 0) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n",  (void*)client, client->fd));
        }

        free_ipc_client(client, -1);
        return;
    }
    for (char *walk = buf; walk < buf + n - 1; walk++) {
        if (*walk == '\0') {
            *walk = ' ';
        }
    }
    cmdline = buf;

    if (cmdline) {
        ELOG(fmt::sprintf("client %p with pid %d and cmdline '%s' on fd %d timed out, killing\n",  (void*)client, peercred.pid, cmdline, client->fd));
    }

#endif
    if (!cmdline) {
        ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n",  (void*)client, client->fd));
    }

    free_ipc_client(client, -1);
}

static void ipc_socket_writeable_cb(EV_P_ ev_io *w, int revents) {
    DLOG(fmt::sprintf("fd %d writeable\n",  w->fd));
    auto *client = (ipc_client *)w->data;

    /* If this callback is called then there should be a corresponding active
     * timer. */
    assert(client->timeout != nullptr);
    ipc_push_pending(client);
}

/*
 * Handler for activity on the listening socket, meaning that a new client
 * has just connected and we should accept() him. Sets up the event handler
 * for activity on the new connection and inserts the file descriptor into
 * the list of clients.
 *
 */
void ipc_new_client(EV_P_ ev_io *w, int revents) {
    struct sockaddr_un peer{};
    socklen_t len = sizeof(struct sockaddr_un);
    int fd;
    if ((fd = accept(w->fd, (struct sockaddr *)&peer, &len)) < 0) {
        if (errno != EINTR) {
            perror("accept()");
        }
        return;
    }

    /* Close this file descriptor on exec() */
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);

    ipc_new_client_on_fd(EV_A_ fd);
}

ipc_client *ipc_new_client_on_fd(EV_P_ int fd) {
    set_nonblock(fd);

    auto *client = new ipc_client(EV_A_ fd, ipc_receive_message, ipc_socket_writeable_cb);

    DLOG(fmt::sprintf("IPC: new client connected on fd %d\n",  fd));
    all_clients.push_back(client);
    return client;
}

/*
 * Generates a json workspace event. Returns a dynamically allocated yajl
 * generator. Free with yajl_gen_free().
 */
nlohmann::json ipc_marshal_workspace_event(const char *change, Con *current, Con *old) {
    setlocale(LC_NUMERIC, "C");
    nlohmann::json j;

    j["change"] = change;

    if (current != nullptr) {
        j["current"] = dump_node(current, false);
    }

    if (old != nullptr) {
        j["old"] = dump_node(old, false);
    }

    setlocale(LC_NUMERIC, "");

    return j;
}

/*
 * For the workspace events we send, along with the usual "change" field, also
 * the workspace container in "current". For focus events, we send the
 * previously focused workspace in "old".
 */
void ipc_send_workspace_event(const char *change, Con *current, Con *old) {
    auto gen = ipc_marshal_workspace_event(change, current, old);

    auto payload = gen.dump();

    ipc_send_event("workspace", I3_IPC_EVENT_WORKSPACE, payload);
}

/*
 * For the window events we send, along the usual "change" field,
 * also the window container, in "container".
 */
void ipc_send_window_event(const char *property, Con *con) {
    DLOG(fmt::sprintf("Issue IPC window %s event (con = %p, window = 0x%08x)\n",
         property, (void*)con, (con->window ? con->window->id : XCB_WINDOW_NONE)));

    setlocale(LC_NUMERIC, "C");
    nlohmann::json j;

    j["change"] = property;
    j["container"] = dump_node(con, false);

    auto payload = j.dump();

    ipc_send_event("window", I3_IPC_EVENT_WINDOW, payload);
    setlocale(LC_NUMERIC, "");
}

/*
 * For the barconfig update events, we send the serialized barconfig.
 */
void ipc_send_barconfig_update_event(Barconfig *barconfig) {
    DLOG(fmt::sprintf("Issue barconfig_update event for id = %s\n",  barconfig->id));
    setlocale(LC_NUMERIC, "C");

    auto j = dump_bar_config(barconfig);

    auto payload = j.dump();

    ipc_send_event("barconfig_update", I3_IPC_EVENT_BARCONFIG_UPDATE, payload);
    setlocale(LC_NUMERIC, "");
}

/*
 * For the binding events, we send the serialized binding struct.
 */
void ipc_send_binding_event(const char *event_type, Binding *bind, const char *modename) {
    DLOG(fmt::sprintf("Issue IPC binding %s event (sym = %s, code = %d)\n",  event_type, bind->symbol, bind->keycode));

    setlocale(LC_NUMERIC, "C");

    nlohmann::json j;

    j["change"] = event_type;

    if (modename == nullptr) {
        j["mode"] = "default";
    } else {
        j["mode"] = modename;
    }

    j["binding"] = dump_binding(bind);

    auto payload = j.dump();

    ipc_send_event("binding", I3_IPC_EVENT_BINDING, payload);
    setlocale(LC_NUMERIC, "");
}

/*
 * Sends a restart reply to the IPC client on the specified fd.
 */
void ipc_confirm_restart(ipc_client *client) {
    DLOG(fmt::sprintf("ipc_confirm_restart(fd %d)\n",  client->fd));
    std::string reply = "[{\"success\":true}]";
    ipc_send_client_message(
        client, I3_IPC_REPLY_TYPE_COMMAND,
        reply);
    ipc_push_pending(client);
}
