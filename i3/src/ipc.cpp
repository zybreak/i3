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
#include <xcb/xcb.h>

#include <ev.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

#include <fmt/printf.h>
module i3;

import std;
import log;
import i3ipc;
import utils;
import regex;
import rect;
import i3_commands_base;
import i3_config_base;

void ipc_client_timeout(EV_P_ ev_timer *w, int revents);
static void ipc_socket_writeable_cb(EV_P_ ev_io *w, int revents);

static ev_tstamp kill_timeout = 10.0;

/**
 * Like writeall, but instead of retrying upon EAGAIN (returned when a write
 * would block), the function stops and returns the total number of bytes
 * written so far.
 *
 */
static ssize_t writeall_nonblock(int fd, const void *buf, size_t count) {
    size_t written = 0;

    while (written < count) {
        const ssize_t n = write(fd, ((char *)buf) + written, count - written);
        if (n == -1) {
            if (errno == EAGAIN) {
                return written;
            } else if (errno == EINTR) {
                continue;
            } else {
                return n;
            }
        }
        written += static_cast<size_t>(n);
    }
    return written;
}

/*
 * ipc_new_client_on_fd() only sets up the event handler
 * for activity on the new connection and inserts the file descriptor into
 * the list of clients.
 *
 * This variant is useful for the inherited IPC connection when restarting.
 *
 */
ipc_client::ipc_client(int fd, callback read_callback_t, callback write_callback_t) {
    this->fd = fd;

    read_callback = new ev_io{};
    read_callback->data = this;
    ev_io_init(read_callback, read_callback_t, fd, EV_READ);
    ev_io_start(global.main_loop, read_callback);

    write_callback = new ev_io{};
    write_callback->data = this;
    ev_io_init(write_callback, write_callback_t, fd, EV_WRITE);
}

ipc_client::~ipc_client() {
    ev_io_stop(global.main_loop, read_callback);
    delete read_callback;
    ev_io_stop(global.main_loop, write_callback);
    delete write_callback;
    if (timeout) {
        ev_timer_stop(global.main_loop, timeout);
        delete timeout;
    }

    free(buffer);
}

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

    if (static_cast<size_t>(result) == client->buffer_size) {
        /* Everything was written successfully: clear the timer and stop the io
         * callback. */
        free(client->buffer);
        client->buffer = nullptr;
        client->buffer_size = 0;
        if (client->timeout) {
            ev_timer_stop(global.main_loop, client->timeout);
            delete client->timeout;
            client->timeout = nullptr;
        }
        ev_io_stop(global.main_loop, client->write_callback);
        return;
    }

    /* Otherwise, make sure that the io callback is enabled and create a new
     * timer if needed. */
    ev_io_start(global.main_loop, client->write_callback);

    if (!client->timeout) {
        auto *timeout = new ev_timer();
        ev_timer_init(timeout, ipc_client_timeout, kill_timeout, 0.);
        timeout->data = client;
        client->timeout = timeout;
        ev_set_priority(timeout, EV_MINPRI);
        ev_timer_start(global.main_loop, client->timeout);
    } else if (result > 0) {
        /* Keep the old timeout when nothing is written. Otherwise, we would
         * keep a dead connection by continuously renewing its timeouts. */
        ev_timer_stop(global.main_loop, client->timeout);
        ev_timer_set(client->timeout, kill_timeout, 0.0);
        ev_timer_start(global.main_loop, client->timeout);
    }
    if (result == 0) {
        return;
    }

    /* Shift the buffer to the left and reduce the allocated space. */
    client->buffer_size -= static_cast<size_t>(result);
    memmove(client->buffer, client->buffer + result, client->buffer_size);
    client->buffer = static_cast<uint8_t*>(srealloc(client->buffer, client->buffer_size));
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
        .size = static_cast<uint32_t>(size),
        .type = message_type
    };

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

static void ipc_send_client_message(ipc_client *client, const i3ipc::REPLY_TYPE message_type, const std::string &payload) {
    ipc_send_client_message(client, static_cast<const uint32_t>(message_type), payload);
}

void IPCManager::free_ipc_client(ipc_client &client, int exempt_fd) {
    if (client.fd != exempt_fd) {
        DLOG(fmt::sprintf("Disconnecting client on fd %d\n",  client.fd));
        close(client.fd);
    }

    std::erase_if(all_clients, [&client](auto &c) { return *c == client; });
}

/*
 * Sends the specified event to all IPC clients which are currently connected
 * and subscribed to this kind of event.
 *
 */
void IPCManager::ipc_send_event(std::string const event, uint32_t message_type, const std::string &payload) {
    for (auto &current : all_clients) {
        for (auto &e : current->events) {
            if (e == event) {
                ipc_send_client_message(current.get(), message_type, payload);
                break;
            }
        }
    }
}

/*
 * For shutdown events, we send the reason for the shutdown.
 */
void IPCManager::ipc_send_shutdown_event(shutdown_reason_t reason) {
    nlohmann::json j;

    if (reason == shutdown_reason_t::SHUTDOWN_REASON_RESTART) {
        j["change"] = "restart";
    } else if (reason == shutdown_reason_t::SHUTDOWN_REASON_EXIT) {
        j["change"] = "exit";
    }

    auto payload = j.dump();

    ipc_send_event("shutdown", i3ipc::EVENT_SHUTDOWN, payload);
}

/*
 * Calls shutdown() on each socket and closes it. This function is to be called
 * when exiting or restarting only!
 *
 * exempt_fd is never closed. Set to -1 to close all fds.
 *
 */
void IPCManager::ipc_shutdown(shutdown_reason_t reason, int exempt_fd) {
    ipc_send_shutdown_event(reason);

    while (!all_clients.empty()) {
        auto &current = all_clients.front();
        if (current->fd != exempt_fd) {
            shutdown(current->fd, SHUT_RDWR);
        }
        free_ipc_client(*current, exempt_fd);
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
    CommandResult result = parse_command(command, {.gen = &gen, .client = client}, &commandsApplier);

    if (result.needs_tree_render) {
        tree_render();
    }

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::COMMAND, gen.dump());
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

static void handle_tree(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto j = dump_node(global.croot, false);

    auto payload = j.dump();

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::TREE, payload);
}

/*
 * Formats the reply message for a GET_WORKSPACES request and sends it to the
 * client
 *
 */
static void handle_get_workspaces(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();

    WorkspaceCon *focused_ws = global.focused->con_get_workspace();

    for (auto &output : global.croot->nodes) {
        for (auto &ws_con : dynamic_cast<OutputCon*>(output)->output_get_content()->nodes) {
            assert(ws_con->type == CT_WORKSPACE);
            auto ws = dynamic_cast<WorkspaceCon*>(ws_con);

            a.push_back({
                { "id", (uintptr_t)ws },
                { "num", ws->num },
                { "name", ws->name },
                { "visible", workspace_is_visible(ws) },
                { "focused", ws == focused_ws },
                { "rect", ws->rect },
                { "output", output->name },
                {"urgent", ws->urgent }
            });
        }
    }

    auto payload = a.dump();
    ipc_send_client_message(client, i3ipc::REPLY_TYPE::WORKSPACES, payload);
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
        o["rect"] = output->rect;
        Con *ws = nullptr;
        if (output->con && (ws = output->con->con_get_fullscreen_con(CF_OUTPUT))) {
            o["current_workspace"] = ws->name;
        }

        a.push_back(o);
    }

    auto payload = a.dump();
    ipc_send_client_message(client, i3ipc::REPLY_TYPE::OUTPUTS, payload);
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
    j["loaded_config_file_name"] = global.configManager->config->current_configpath.native();

    auto a = nlohmann::json::array();
    for (auto &included_file : std::ranges::drop_view{global.configManager->config->included_files,1}) {
        a.push_back(included_file.path);
    }
    j["included_config_file_names"] = a;

    auto payload = j.dump();

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::VERSION, payload);
}

/*
 * Formats the reply message for a GET_BAR_CONFIG request and sends it to the
 * client.
 *
 */
static void handle_get_bar_config(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
}

/*
 * Returns a list of configured binding modes
 *
 */
static void handle_get_binding_modes(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();
    std::ranges::transform(global.configManager->config->modes, std::back_inserter(a), [](auto &mode) { return mode.first; });

    auto payload = a.dump();

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::BINDING_MODES, payload);
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
        ipc_send_client_message(client, i3ipc::REPLY_TYPE::SUBSCRIBE, reply);
        return;
    }

    std::string reply = R"({"success":true})";
    ipc_send_client_message(client, i3ipc::REPLY_TYPE::SUBSCRIBE, reply);

    if (client->first_tick_sent) {
        return;
    }

    bool is_tick = std::ranges::any_of(client->events, [](auto &event) { return event == "tick"; });

    if (!is_tick) {
        return;
    }

    client->first_tick_sent = true;
    std::string payload = R"({"first":true,"payload":""})";
    ipc_send_client_message(client, i3ipc::EVENT_TICK, payload);
}

/*
 * Returns the raw last loaded i3 configuration file contents.
 */
static void handle_get_config(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    auto a = nlohmann::json::array();

    for (auto &included_file : std::ranges::drop_view{global.configManager->config->included_files,1}) {
        a.emplace_back(included_file.raw_contents);
    }

    nlohmann::json j = {
        { "config", std::string(global.configManager->config->included_files[0].raw_contents) },
        { "included_configs", a }
    };

    auto payload = j.dump();

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::CONFIG, payload);
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

    global.ipcManager->ipc_send_event("tick", i3ipc::EVENT_TICK, payload);

    std::string reply = R"({"success":true})";
    ipc_send_client_message(client, i3ipc::REPLY_TYPE::TICK, reply);
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
        state.window = static_cast<xcb_window_t>(val);
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
        ipc_send_client_message(client, i3ipc::REPLY_TYPE::SYNC, reply);
        return;
    }

    DLOG(fmt::sprintf("received IPC sync request (rnd = %d, window = 0x%08x)\n",  state.rnd, state.window));
    sync_respond(global.x,state.window, state.rnd);
    std::string reply = "{\"success\":true}";
    ipc_send_client_message(client, i3ipc::REPLY_TYPE::SYNC, reply);
}

static void handle_get_binding_state(ipc_client *client, uint8_t *message, int size, uint32_t message_size, uint32_t message_type) {
    nlohmann::json j = {
        { "name",  global.configManager->config->current_mode()->name}
    };

    auto payload = j.dump();

    ipc_send_client_message(client, i3ipc::REPLY_TYPE::GET_BINDING_STATE, payload);
}

/* The index of each callback function corresponds to the numeric
 * value of the message type (see include/i3/ipc.h) */
static std::array<handler_t, 13> handlers = {
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
void ipc_receive_message(EV_P_ ev_io *w, int revents) {
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
            free(message);
            return;
        }

        /* If not, there was some kind of error. We don’t bother and close the
         * connection. Delete the client from the list of clients. */
        global.ipcManager->free_ipc_client(*client);
        free(message);
        return;
    }

    try {
        handler_t h = handlers.at(message_type);
        h(client, message, 0, message_length, message_type);
    } catch (std::out_of_range const& e) {
        DLOG(fmt::sprintf("Unhandled message type: %d\n",  message_type));
    }

    free(message);
}

void ipc_client_timeout(EV_P_ ev_timer *w, int revents) {
    /* No need to be polite and check for writeability, the other callback would
     * have been called by now. */
    auto *client = (ipc_client *)w->data;

    char *cmdline = nullptr;
#if defined(__linux__) && defined(SO_PEERCRED)
    struct ucred peercred{};
    socklen_t so_len = sizeof(peercred);
    if (getsockopt(client->fd, SOL_SOCKET, SO_PEERCRED, &peercred, &so_len) != 0) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n", fmt::ptr(client), client->fd));
        }

        global.ipcManager->free_ipc_client(*client);
        return;
    }
    std::string exepath = std::format("/proc/{}/cmdline", peercred.pid);

    int fd = open(exepath.c_str(), O_RDONLY);
    if (fd == -1) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n", fmt::ptr(client), client->fd));
        }

        global.ipcManager->free_ipc_client(*client);
        return;
    }
    char buf[512] = {'\0'}; /* cut off cmdline for the error message. */
    const ssize_t n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n < 0) {
        if (!cmdline) {
            ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n", fmt::ptr(client), client->fd));
        }

        global.ipcManager->free_ipc_client(*client);
        return;
    }
    for (char *walk = buf; walk < buf + n - 1; walk++) {
        if (*walk == '\0') {
            *walk = ' ';
        }
    }
    cmdline = buf;

    if (cmdline) {
        ELOG(fmt::sprintf("client %p with pid %d and cmdline '%s' on fd %d timed out, killing\n", fmt::ptr(client), peercred.pid, cmdline, client->fd));
    }

#endif
    if (!cmdline) {
        ELOG(fmt::sprintf("client %p on fd %d timed out, killing\n", fmt::ptr(client), client->fd));
    }

    global.ipcManager->free_ipc_client(*client);
}

static void ipc_socket_writeable_cb(EV_P_ ev_io *w, int revents) {
    DLOG(fmt::sprintf("fd %d writeable\n",  w->fd));
    auto *client = (ipc_client *)w->data;

    /* If this callback is called then there should be a corresponding active
     * timer. */
    if (client->timeout == nullptr) {
        std::terminate();
    }
    ipc_push_pending(client);
}

/*
 * Handler for activity on the listening socket, meaning that a new client
 * has just connected and we should accept() him. Sets up the event handler
 * for activity on the new connection and inserts the file descriptor into
 * the list of clients.
 *
 */
static void ipc_new_client(EV_P_ ev_io *w, int revents) {
    
    IPCManager *ipcManager = (IPCManager*)w->data;
    
    struct sockaddr_un peer{};
    socklen_t len = sizeof(struct sockaddr_un);
    int fd = accept(w->fd, (struct sockaddr *)&peer, &len);
    if (fd < 0) {
        if (errno != EINTR) {
            ELOG(std::format("accept(): {}", std::strerror(errno)));
        }
        return;
    }

    /* Close this file descriptor on exec() */
    (void)fcntl(fd, F_SETFD, FD_CLOEXEC);

    ipcManager->ipc_new_client_on_fd(fd);
}

ipc_client* IPCManager::ipc_new_client_on_fd(int fd) {
    set_nonblock(fd);

    DLOG(fmt::format("IPC: new client connected on fd {}", fd));
    return all_clients.emplace_back(std::make_unique<ipc_client>(fd, ipc_receive_message, ipc_socket_writeable_cb)).get();
}

/*
 * Generates a json workspace event. Returns a dynamically allocated yajl
 * generator. Free with yajl_gen_free().
 */
nlohmann::json ipc_marshal_workspace_event(const char *change, Con *current, Con *old) {
    nlohmann::json j;

    j["change"] = change;

    if (current != nullptr) {
        j["current"] = dump_node(current, false);
    }

    if (old != nullptr) {
        j["old"] = dump_node(old, false);
    }

    return j;
}

/*
 * For the workspace events we send, along with the usual "change" field, also
 * the workspace container in "current". For focus events, we send the
 * previously focused workspace in "old".
 */
void IPCManager::ipc_send_workspace_event(const char *change, Con *current, Con *old) {
    auto gen = ipc_marshal_workspace_event(change, current, old);

    auto payload = gen.dump();

    ipc_send_event("workspace", i3ipc::EVENT_WORKSPACE, payload);
}

/*
 * For the window events we send, along the usual "change" field,
 * also the window container, in "container".
 */
void IPCManager::ipc_send_window_event(const char *property, Con *con) {
    DLOG(fmt::sprintf("Issue IPC window %s event (con = %p, window = 0x%08x)\n",
         property, fmt::ptr(con), (con->get_window() ? con->get_window()->id : XCB_WINDOW_NONE)));

    nlohmann::json j;

    j["change"] = property;
    j["container"] = dump_node(con, false);

    auto payload = j.dump();

    ipc_send_event("window", i3ipc::EVENT_WINDOW, payload);
}

/*
 * For the barconfig update events, we send the serialized barconfig.
 */
void ipc_send_barconfig_update_event(Barconfig *barconfig) {
    // NOOP
}

/*
 * For the binding events, we send the serialized binding struct.
 */
void IPCManager::ipc_send_binding_event(const char *event_type, Binding *bind, const char *modename) {
    DLOG(fmt::sprintf("Issue IPC binding %s event (sym = %s, code = %d)\n",  event_type, bind->symbol, bind->keycode));

    nlohmann::json j;

    j["change"] = event_type;

    if (modename == nullptr) {
        j["mode"] = "default";
    } else {
        j["mode"] = modename;
    }

    j["binding"] = dump_binding(bind);

    auto payload = j.dump();

    ipc_send_event("binding", i3ipc::EVENT_BINDING, payload);
}

/*
 * Sends a restart reply to the IPC client on the specified fd.
 */
void ipc_confirm_restart(ipc_client *client) {
    DLOG(fmt::sprintf("ipc_confirm_restart(fd %d)\n",  client->fd));
    std::string reply = "[{\"success\":true}]";
    ipc_send_client_message(
        client, i3ipc::REPLY_TYPE::COMMAND,
        reply);
    ipc_push_pending(client);
}

IPCManager::IPCManager(ConfigurationManager &configManager) : configManager(configManager) {
    ipc_io = new ev_io();
}

IPCManager::~IPCManager() {
    ev_io_stop(global.main_loop, ipc_io);
    delete ipc_io;
}
/*
 * Creates the UNIX domain socket at the given path, sets it to non-blocking
 * mode, bind()s and listen()s on it.
 *
 * The full path to the socket is stored in the char* that out_socketpath points
 * to.
 *
 */
std::tuple<std::string, int> IPCManager::create_socket(std::string filename) {
    auto resolved = utils::resolve_tilde(filename);
    DLOG(fmt::sprintf("Creating UNIX socket at %s\n", resolved));
    const std::filesystem::path p(resolved);
    const std::filesystem::path &parent = p.parent_path();
    
    if (!std::filesystem::exists(parent)) {
        try {
            std::filesystem::create_directories(parent);
        } catch (std::exception &e) {
            ELOG(fmt::sprintf("mkdir(%s) failed: %s\n", parent.string(), e.what()));
        }
    }

    /* Check if the socket is in use by another process (this call does not
     * succeed if the socket is stale / the owner already exited) */
    int sockfd = i3ipc::ipc_connect_impl(resolved);
    if (sockfd != -1) {
        ELOG(fmt::sprintf("Refusing to create UNIX socket at %s: Socket is already in use\n", resolved));
        close(sockfd);
        throw std::exception();
    }

    /* Unlink the unix domain socket before */
    std::filesystem::remove(resolved);

    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd < 0) {
        throw std::runtime_error(std::format("socket(): {}", std::strerror(errno)));
    }

    (void)fcntl(sockfd, F_SETFD, FD_CLOEXEC);

    struct sockaddr_un addr {};
    addr.sun_family = AF_LOCAL;
    strncpy(addr.sun_path, resolved.c_str(), sizeof(addr.sun_path) - 1);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        throw std::runtime_error(std::format("bind(): {}", std::strerror(errno)));
    }

    set_nonblock(sockfd);

    if (::listen(sockfd, 5) < 0) {
        throw std::runtime_error(std::format("listen(): {}", std::strerror(errno)));
    }

    return std::make_tuple(resolved, sockfd);
}

int IPCManager::create_socket() {
    if (!configManager.config->ipc_socket_path) {
        throw std::runtime_error("Could not create the IPC socket since socket path wasnt specified");
    }
    
   std::tie(current_socketpath, ipc_socket) = create_socket(*configManager.config->ipc_socket_path);
    if (ipc_socket == -1) {
        throw std::runtime_error(std::format("Could not create the IPC socket: {}", *configManager.config->ipc_socket_path));
    }
    global.current_socketpath = *current_socketpath;
    
    return ipc_socket;
}

void IPCManager::listen() {
    if (ipc_socket == -1) {
        throw std::runtime_error("No socket created");
    }
   
    ipc_io->data = this;
    ev_io_init(ipc_io, ipc_new_client, ipc_socket, EV_READ);
    ev_io_start(global.main_loop, ipc_io);
}