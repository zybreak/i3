/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * libi3: contains functions which are used by i3 *and* accompanying tools such
 * as i3-config-wizard, …
 *
 */
module;
#include <cstdint>
export module i3ipc;

import std;

export namespace i3ipc {

    /** Never change this, only on major IPC breakage (don’t do that) */
    constexpr std::string MAGIC{"i3-ipc"};

    /**
     * Messages from clients to i3
     */
    enum class MESSAGE_TYPE : uint32_t {
        /** The payload of the message will be interpreted as a command */
        RUN_COMMAND = 0,
        /** Requests the current workspaces from i3 */
        GET_WORKSPACES = 1,
        /** Subscribe to the specified events */
        SUBSCRIBE = 2,
        /** Requests the current outputs from i3 */
        GET_OUTPUTS = 3,
        /** Requests the tree layout from i3 */
        GET_TREE = 4,
        /** Request the configuration for a specific 'bar' */
        GET_BAR_CONFIG = 6,
        /** Request the i3 version */
        GET_VERSION = 7,
        /** Request a list of configured binding modes. */
        GET_BINDING_MODES = 8,
        /** Request the raw last loaded i3 config. */
        GET_CONFIG = 9,
        /** Send a tick event to all subscribers. */
        SEND_TICK = 10,
        /** Trigger an i3 sync protocol message via IPC. */
        SYNC = 11,
        /** Request the current binding state. */
        GET_BINDING_STATE = 12
    };

    /**
     * Messages from i3 to clients
     */
    enum class REPLY_TYPE : uint32_t {
        COMMAND = 0,
        WORKSPACES = 1,
        SUBSCRIBE = 2,
        OUTPUTS = 3,
        TREE = 4,
        MARKS = 5,
        BAR_CONFIG = 6,
        VERSION = 7,
        BINDING_MODES = 8,
        CONFIG = 9,
        TICK = 10,
        SYNC = 11,
        GET_BINDING_STATE = 12
    };

    /*
     * Events from i3 to clients. Events have the first bit set high.
     *
     */
    uint32_t const EVENT_MASK = (1UL << 31);

    /* The workspace event will be triggered upon changes in the workspace list */
    uint32_t const EVENT_WORKSPACE = (EVENT_MASK | 0);

    /* The output event will be triggered upon changes in the output list */
    uint32_t const EVENT_OUTPUT = (EVENT_MASK | 1);

    /* The output event will be triggered upon mode changes */
    uint32_t const EVENT_MODE = (EVENT_MASK | 2);

    /* The window event will be triggered upon window changes */
    uint32_t const EVENT_WINDOW = (EVENT_MASK | 3);

    /** Bar config update will be triggered to update the bar config */
    uint32_t const EVENT_BARCONFIG_UPDATE = (EVENT_MASK | 4);

    /** The binding event will be triggered when bindings run */
    uint32_t const EVENT_BINDING = (EVENT_MASK | 5);

    /** The shutdown event will be triggered when the ipc shuts down */
    uint32_t const EVENT_SHUTDOWN = (EVENT_MASK | 6);

    /** The tick event will be sent upon a tick IPC message */
    uint32_t const EVENT_TICK = (EVENT_MASK | 7);

    struct i3_ipc_header_t {
        /* 6 = strlen(I3_IPC_MAGIC) */
        char magic[6];
        uint32_t size;
        uint32_t type;
    } __attribute__((packed));

    /**
     * Connects to the i3 IPC socket and returns the file descriptor for the
     * socket. die()s if anything goes wrong.
     *
     */
    // std::tuple<std::string, int> ipc_connect(const std::string_view &socket_path);

    /**
     * Connects to the socket at the given path with no fallback paths. Returns
     * -1 if connect() fails and die()s for other errors.
     *
     */
    int ipc_connect_impl(std::string_view const &socket_path);

    /**
     * Formats a message (payload) of the given size and type and sends it to i3 via
     * the given socket file descriptor.
     *
     * Returns -1 when write() fails, errno will remain.
     * Returns 0 on success.
     *
     */
    int ipc_send_message(int sockfd, uint32_t message_size,
                         MESSAGE_TYPE message_type, uint8_t const *payload);

    /**
     * Reads a message from the given socket file descriptor and stores its length
     * (reply_length) as well as a pointer to its contents (reply).
     *
     * Returns -1 when read() fails, errno will remain.
     * Returns -2 when the IPC protocol is violated (invalid magic, unexpected
     * message type, EOF instead of a message). Additionally, the error will be
     * printed to stderr.
     * Returns 0 on success.
     *
     */
    int ipc_recv_message(int sockfd, uint32_t *message_type,
                         uint32_t *reply_length, uint8_t **reply);
}  // namespace i3ipc