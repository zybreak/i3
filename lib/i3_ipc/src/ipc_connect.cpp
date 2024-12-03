/*
 * vim:ts=4:sw=4:expandtab
 *
 * i3 - an improved tiling window manager
 * © 2009 Michael Stapelberg and contributors (see also: LICENSE)
 *
 */
module;
#include <err.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
module i3ipc;

import std;

namespace i3ipc {
    /**
     * Connects to the i3 IPC socket and returns the file descriptor for the
     * socket. die()s if anything goes wrong.
     *
     */
#if 0
    std::tuple<std::string, int> ipc_connect(const std::string_view &socket_path) {
        std::string path{};
        if (!socket_path.empty()) {
            path = socket_path;
        }

        if (path.empty()) {
            if ((char *env_path = getenv("I3SOCK")) != nullptr) {
                path = env_path;
            }
        }

        if (path.empty()) {
            path = root_atom_contents("I3_SOCKET_PATH", nullptr, 0);
        }

        if (path.empty()) {
            err(EXIT_FAILURE, "Could not determine i3 socket path");
        }

        int sockfd = ipc_connect_impl(path);
        if (sockfd < 0) {
            err(EXIT_FAILURE, "Could not connect to i3 on socket %s", path);
        }
        return std::make_tuple(path, sockfd);
    }
#endif

    /**
     * Connects to the socket at the given path with no fallback paths. Returns
     * -1 if connect() fails and die()s for other errors.
     *
     */
    int ipc_connect_impl(const std::string_view &socket_path) {
        int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if (sockfd == -1) {
            err(EXIT_FAILURE, "Could not create socket");
        }

        (void)fcntl(sockfd, F_SETFD, FD_CLOEXEC);

        struct sockaddr_un addr{};
        addr.sun_family = AF_LOCAL;
        strncpy(addr.sun_path, socket_path.data(), sizeof(addr.sun_path) - 1);
        if (connect(sockfd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
            close(sockfd);
            return -1;
        }
        return sockfd;
    }
}
