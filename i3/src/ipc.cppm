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
#include <ev.h>
#include <nlohmann/json.hpp>
export module i3:ipc;

import i3ipc;

struct Binding;
struct Barconfig;
class Con;
class ConfigurationManager;

export {
    /**
     * Calls to ipc_shutdown() should provide a reason for the shutdown.
     */
    enum class shutdown_reason_t {
        SHUTDOWN_REASON_RESTART,
        SHUTDOWN_REASON_EXIT
    };

    class ipc_client {
      public:
        using callback = void (*)(ev_loop *loop, ev_io *w, int revents);
        int fd;

        /* The events which this client wants to receive */
        std::vector<std::string> events{};

        /* For clients which subscribe to the tick event: whether the first tick
         * event has been sent by i3. */
        bool first_tick_sent{};

        ev_io *read_callback;
        ev_io *write_callback;
        ev_timer *timeout{};
        uint8_t *buffer{};
        size_t buffer_size{};

        ipc_client() = delete;
        ipc_client(ipc_client const &) = delete;
        ipc_client operator=(ipc_client const &) = delete;

        ipc_client(int fd, callback read_callback_t, callback write_callback_t);
        ~ipc_client();

        bool operator==(ipc_client const &o) {
            return this->fd == o.fd;
        }

        bool operator!=(ipc_client const &o) {
            return this->fd != o.fd;
        }
    };

    class IPCManager {
        friend void ipc_receive_message(EV_P_ ev_io *w, int revents);
        friend void ipc_client_timeout(EV_P_ ev_timer *w, int revents);

      private:
        std::vector<std::unique_ptr<ipc_client>> all_clients{};
        ev_io *ipc_io;
        ConfigurationManager &configManager;
        std::tuple<std::string, int> create_socket(std::string filename);
        void ipc_send_shutdown_event(shutdown_reason_t reason);
        void free_ipc_client(ipc_client &client, int exempt_fd = -1);

      public:
        std::optional<std::string> current_socketpath{};
        int ipc_socket{-1};
        explicit IPCManager(ConfigurationManager &configManager);
        ~IPCManager();
        IPCManager(IPCManager const &) = delete;
        void operator=(IPCManager const &) = delete;
        IPCManager(IPCManager &&other) = delete;
        void operator=(IPCManager &&) = delete;

        int create_socket();
        /* Listen to the IPC socket for clients */
        void listen();
        /**
         * ipc_new_client_on_fd() only sets up the event handler
         * for activity on the new connection and inserts the file descriptor into
         * the list of clients.
         *
         * This variant is useful for the inherited IPC connection when restarting.
         *
         */
        ipc_client *ipc_new_client_on_fd(int fd);

        /**
         * Sends the specified event to all IPC clients which are currently connected
         * and subscribed to this kind of event.
         *
         */
        void ipc_send_event(std::string const event, uint32_t message_type, std::string const &payload);

        /**
         * Calls shutdown() on each socket and closes it. This function is to be called
         * when exiting or restarting only!
         *
         * exempt_fd is never closed. Set to -1 to close all fds.
         *
         */
        void ipc_shutdown(shutdown_reason_t reason, int exempt_fd = -1);

        /**
         * For the workspace events we send, along with the usual "change" field, also
         * the workspace container in "current". For focus events, we send the
         * previously focused workspace in "old".
         */
        void ipc_send_workspace_event(char const *change, Con *current, Con *old);

        /**
         * For the window events we send, along the usual "change" field,
         * also the window container, in "container".
         */
        void ipc_send_window_event(char const *property, Con *con);

        /**
         * For the binding events, we send the serialized binding struct.
         */
        void ipc_send_binding_event(char const *event_type, Binding *bind, char const *modename);
    };

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
    using handler_t = void (*)(ipc_client *, uint8_t *, int, uint32_t, uint32_t);

    /* Macro to declare a callback */

    /**
     * Generates a json workspace event. Returns a dynamically allocated yajl
     * generator. Free with yajl_gen_free().
     */
    nlohmann::json ipc_marshal_workspace_event(char const *change, Con *current, Con *old);

    /**
     * For the barconfig update events, we send the serialized barconfig.
     */
    void ipc_send_barconfig_update_event(Barconfig * barconfig);

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
