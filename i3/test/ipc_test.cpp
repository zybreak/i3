#include <gtest/gtest.h>
#include <ev.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <optional>
#include <string>
#include <utility>
#include <vector>

import i3;
import mocks;

namespace {
/* An event loop that is global.main_loop while it exists. */
class TestLoop {
  public:
    ev_loop *const loop{ev_loop_new(EVFLAG_AUTO)};

  private:
    ev_loop *const saved{std::exchange(global.main_loop, loop)};

  public:
    TestLoop() = default;

    TestLoop(TestLoop const &) = delete;
    TestLoop &operator=(TestLoop const &) = delete;

    ~TestLoop() {
        global.main_loop = saved;
        ev_loop_destroy(loop);
    }
};

/* An IPCManager with IPC clients connected through socketpairs, on an event
 * loop of its own. global.ipcManager (ipc_receive_message()) points at it
 * while a test runs. */
class IPCManagerTest : public ::testing::Test {
  protected:
    /* First: the clients stop their watchers on it when ipcManager destroys
     * them. */
    TestLoop test_loop{};
    ev_loop *loop{test_loop.loop};
    IPCManager *saved_ipc_manager{global.ipcManager};
    MockX x{};
    ConfigurationManager configManager{x};
    IPCManager ipcManager{configManager};

    /* The client's ends of the connections, and the clients. */
    std::vector<int> peers{};
    std::vector<ipc_client *> clients{};

    IPCManagerTest() {
        global.ipcManager = &ipcManager;
    }

    ~IPCManagerTest() override {
        for (int const peer : peers) {
            if (peer != -1) {
                close(peer);
            }
        }
        global.ipcManager = saved_ipc_manager;
    }

    /* Connects n clients that subscribe to "workspace" events. */
    void connect_clients(int n) {
        for (int i = 0; i < n; i++) {
            std::array<int, 2> fds{};
            ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, fds.data()), 0) << std::strerror(errno);
            ipc_client *client = ipcManager.ipc_new_client_on_fd(fds[0]);
            client->events.emplace_back("workspace");
            clients.push_back(client);
            peers.push_back(fds[1]);
        }
    }

    /* What the client's end has to read without blocking: its bytes, or
     * std::nullopt at EOF (the connection is closed). */
    static std::optional<std::string> read_peer(int peer) {
        std::array<char, 256> buffer{};
        ssize_t const n = recv(peer, buffer.data(), buffer.size(), MSG_DONTWAIT);
        if (n == 0) {
            return std::nullopt;
        }
        if (n < 0) {
            return std::string{};
        }
        return std::string{buffer.data(), static_cast<size_t>(n)};
    }
};
}  // namespace

/* i3_exit() and the restart command free every client with ipc_shutdown(),
 * the first one first. */
TEST_F(IPCManagerTest, ShutdownDisconnectsEveryClient) {
    connect_clients(3);

    ipcManager.ipc_shutdown(shutdown_reason_t::SHUTDOWN_REASON_EXIT);

    for (int const peer : peers) {
        EXPECT_EQ(read_peer(peer), std::nullopt);
    }
}

/* A client that disconnects is freed by ipc_receive_message(); the clients
 * after it in the list stay connected. */
TEST_F(IPCManagerTest, ClientThatDisconnectsIsFreedAndTheOthersStay) {
    connect_clients(3);
    int const first_fd = clients.front()->fd;

    close(std::exchange(peers.front(), -1));
    ev_run(loop, EVRUN_NOWAIT);

    /* i3 closed its end of the first connection. */
    errno = 0;
    EXPECT_EQ(fcntl(first_fd, F_GETFD), -1);
    EXPECT_EQ(errno, EBADF);

    ipcManager.ipc_send_event("workspace", 0, "{}");
    for (size_t i = 1; i < peers.size(); i++) {
        auto const message = read_peer(peers[i]);
        ASSERT_TRUE(message.has_value()) << "client " << i << " was disconnected";
        EXPECT_TRUE(message->starts_with("i3-ipc")) << "client " << i << " got no event";
    }
}
