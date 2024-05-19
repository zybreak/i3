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
#include "i3.h"

#include <nlohmann/json.hpp>
export module i3:ipc;

import i3ipc;

struct Binding;
struct Barconfig;
class Con;

export {
    /*
     * Callback type for the different message types.
     *
     * message is the raw packet, as received from the UNIX domain socket. size
     * is the remaining size of bytes for this packet.
     *
     * message_size is the size of the message as the sender specified it.
     * message_type is the type of the message as the sender specified it.
     *
     */
    typedef void (*handler_t)(ipc_client *, uint8_t *, int, uint32_t, uint32_t);

    /* Macro to declare a callback */

    /**
     * Handler for activity on the listening socket, meaning that a new client
     * has just connected and we should accept() him. Sets up the event handler
     * for activity on the new connection and inserts the file descriptor into
     * the list of clients.
     *
     */
    void ipc_new_client(EV_P_ ev_io * w, int revents);

    /**
     * ipc_new_client_on_fd() only sets up the event handler
     * for activity on the new connection and inserts the file descriptor into
     * the list of clients.
     *
     * This variant is useful for the inherited IPC connection when restarting.
     *
     */
    ipc_client *ipc_new_client_on_fd(EV_P_ int fd);

    /**
     * Sends the specified event to all IPC clients which are currently connected
     * and subscribed to this kind of event.
     *
     */
    void ipc_send_event(const char *event, uint32_t message_type, const std::string &payload);

    /**
     * Calls to ipc_shutdown() should provide a reason for the shutdown.
     */
    enum shutdown_reason_t {
        SHUTDOWN_REASON_RESTART,
        SHUTDOWN_REASON_EXIT
    };

    /**
     * Calls shutdown() on each socket and closes it. This function is to be called
     * when exiting or restarting only!
     *
     * exempt_fd is never closed. Set to -1 to close all fds.
     *
     */
    void ipc_shutdown(shutdown_reason_t reason, int exempt_fd);

    nlohmann::json dump_node(Con * con, bool inplace_restart);

    /**
     * Generates a json workspace event. Returns a dynamically allocated yajl
     * generator. Free with yajl_gen_free().
     */
    nlohmann::json ipc_marshal_workspace_event(const char *change, Con *current, Con *old);

    /**
     * For the workspace events we send, along with the usual "change" field, also
     * the workspace container in "current". For focus events, we send the
     * previously focused workspace in "old".
     */
    void ipc_send_workspace_event(const char *change, Con *current, Con *old);

    /**
     * For the window events we send, along the usual "change" field,
     * also the window container, in "container".
     */
    void ipc_send_window_event(const char *property, Con *con);

    /**
     * For the barconfig update events, we send the serialized barconfig.
     */
    void ipc_send_barconfig_update_event(Barconfig * barconfig);

    /**
     * For the binding events, we send the serialized binding struct.
     */
    void ipc_send_binding_event(const char *event_type, Binding *bind, const char *modename);

    /**
     * Set the maximum duration that we allow for a connection with an unwriteable
     * socket.
     */
    void ipc_set_kill_timeout(double newTimeout);

    /**
     * Sends a restart reply to the IPC client on the specified fd.
     */
    void ipc_confirm_restart(ipc_client * client);
}