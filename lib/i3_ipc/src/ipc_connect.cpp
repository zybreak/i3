/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved dynamic tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;

#include <err.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <string_view>

module i3ipc;

namespace i3ipc {
    /*
     * Connects to the i3 IPC socket and returns the file descriptor for the
     * socket. die()s if anything goes wrong.
     *
     */
    int ipc_connect(const std::string_view &socket_path) {
        int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (sockfd == -1) {
            err(EXIT_FAILURE, "Could not create socket");
        }

        (void)fcntl(sockfd, F_SETFD, FD_CLOEXEC);

        struct sockaddr_un addr{};
        addr.sun_family = AF_LOCAL;
        strncpy(addr.sun_path, socket_path.data(), sizeof(addr.sun_path) - 1);
        if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
            err(EXIT_FAILURE, "Could not connect to i3 on socket %s", socket_path.data());
        }
        return sockfd;
    }
}
