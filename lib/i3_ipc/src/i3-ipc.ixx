/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 * libi3: contains functions which are used by i3 *and* accompanying tools such
 * as i3-config-wizard, …
 *
 */
module;
#include <string_view>

export module i3ipc;

export namespace i3ipc {

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
    int ipc_connect(const std::string_view &socket_path);

    /**
     * Formats a message (payload) of the given size and type and sends it to i3 via
     * the given socket file descriptor.
     *
     * Returns -1 when write() fails, errno will remain.
     * Returns 0 on success.
     *
     */
    int ipc_send_message(int sockfd, const uint32_t message_size,
                                const uint32_t message_type, const uint8_t *payload);

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
}