/*
 * i3 as a child process of a test, and its IPC.
 */
module;
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
export module i3_runner;

import std;
import i3ipc;
import i3.json;
import child_process;

export {
    /** A connection to i3's IPC socket. Reads and writes time out after ten seconds. */
    class IpcClient {
      public:
        /** A message from i3: a reply, or an event if type has i3ipc::EVENT_MASK set. */
        struct Message {
            std::uint32_t type;
            std::string payload;
        };

        [[nodiscard]] static std::expected<IpcClient, std::string> connect(std::filesystem::path const &socket) {
            if (socket.native().size() >= sizeof(sockaddr_un::sun_path)) {
                return std::unexpected{std::format("the socket path {} is too long", socket.native())};
            }
            UniqueFd fd{i3ipc::ipc_connect_impl(socket.native())};
            if (!fd) {
                return std::unexpected{std::format("can't connect to {}: {}", socket.native(), errno_message(errno))};
            }
            timeval const timeout{.tv_sec = 10, .tv_usec = 0};
            if (::setsockopt(fd.get(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) == -1 ||
                ::setsockopt(fd.get(), SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout) == -1) {
                return std::unexpected{std::format("setsockopt: {}", errno_message(errno))};
            }
            return IpcClient{std::move(fd)};
        }

        /**
         * Sends a message without waiting for a reply (for exit, which i3 doesn't answer). It is sent with
         * MSG_NOSIGNAL, unlike i3ipc::ipc_send_message(): writing to an i3 that is gone fails instead of raising
         * SIGPIPE in the test process.
         */
        std::expected<void, std::string> send(i3ipc::MESSAGE_TYPE type, std::string_view payload = {}) {
            i3ipc::i3_ipc_header_t header{.magic = {'i', '3', '-', 'i', 'p', 'c'},
                                          .size = static_cast<std::uint32_t>(payload.size()),
                                          .type = std::to_underlying(type)};
            std::string message(sizeof header, '\0');
            std::memcpy(message.data(), &header, sizeof header);
            message.append(payload);
            for (std::string_view rest{message}; !rest.empty();) {
                ssize_t const n = ::send(fd_.get(), rest.data(), rest.size(), MSG_NOSIGNAL);
                if (n == -1) {
                    if (errno == EINTR) {
                        continue;
                    }
                    return std::unexpected{std::format("sending to i3: {}", errno_message(errno))};
                }
                rest.remove_prefix(static_cast<std::size_t>(n));
            }
            return {};
        }

        /** The next message from i3. */
        std::expected<Message, std::string> receive() {
            std::uint32_t type{};
            std::uint32_t length{};
            std::uint8_t *reply{};
            int const result = i3ipc::ipc_recv_message(fd_.get(), &type, &length, &reply);
            std::unique_ptr<std::uint8_t, decltype([](std::uint8_t *p) { std::free(p); })> const owned{reply};
            switch (result) {
                case 0:
                    return Message{type, std::string{reinterpret_cast<char const *>(reply), length}};
                case -1:
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        return std::unexpected{std::string{"i3 sent nothing within 10 s"}};
                    }
                    return std::unexpected{std::format("reading from i3: {}", errno_message(errno))};
                case -2:
                    return std::unexpected{std::string{"i3 closed the IPC connection"}};
                default:
                    return std::unexpected{std::string{"i3 sent something that isn't an IPC message"}};
            }
        }

        /** Sends a message and returns i3's reply (events that arrive before it are skipped). */
        std::expected<std::string, std::string> request(i3ipc::MESSAGE_TYPE type, std::string_view payload = {}) {
            if (auto const sent = send(type, payload); !sent) {
                return std::unexpected{sent.error()};
            }
            for (;;) {
                auto message = receive();
                if (!message) {
                    return std::unexpected{message.error()};
                }
                if ((message->type & i3ipc::EVENT_MASK) != 0) {
                    continue;
                }
                if (message->type != std::to_underlying(type)) {
                    return std::unexpected{std::format("i3 answered message type {} with type {}", std::to_underlying(type), message->type)};
                }
                return std::move(message->payload);
            }
        }

        /** request(), with the reply parsed. */
        std::expected<json, std::string> request_json(i3ipc::MESSAGE_TYPE type, std::string_view payload = {}) {
            auto reply = request(type, payload);
            if (!reply) {
                return std::unexpected{reply.error()};
            }
            auto parsed = json::parse(*reply, nullptr, false);
            if (parsed.is_discarded()) {
                return std::unexpected{std::format("i3's reply isn't JSON: {}", *reply)};
            }
            return parsed;
        }

        /** Waits for the next event of type @p event (i3ipc::EVENT_OUTPUT, …); returns its payload. Earlier events
         *  of other types are skipped. */
        std::expected<json, std::string> wait_for_event(std::uint32_t event) {
            for (;;) {
                auto message = receive();
                if (!message) {
                    return std::unexpected{message.error()};
                }
                if (message->type == event) {
                    auto parsed = json::parse(message->payload, nullptr, false);
                    if (parsed.is_discarded()) {
                        return std::unexpected{std::format("i3's event isn't JSON: {}", message->payload)};
                    }
                    return parsed;
                }
            }
        }

      private:
        explicit IpcClient(UniqueFd fd) : fd_(std::move(fd)) {
        }

        UniqueFd fd_;
    };

    /** How an I3Process runs. */
    struct I3Options {
        /** The i3 binary. */
        std::filesystem::path binary;
        /** Its configuration file, which must set ipc-socket to @c ipc_socket. */
        std::filesystem::path config;
        /** Its IPC socket. */
        std::filesystem::path ipc_socket;
        /** Where its standard output and error go. */
        std::filesystem::path log;
        /** More arguments. */
        std::vector<std::string> arguments{"-V"};
        /** How long it may take to answer on its IPC socket. */
        std::chrono::milliseconds timeout{std::chrono::seconds{10}};
    };

    /**
     * i3 running on $DISPLAY. start() returns once i3 has answered a GET_VERSION on its IPC socket, which it does from
     * its event loop, so it has set up its outputs, workspaces and grabs by then. The destructor terminates and reaps
     * it.
     */
    class I3Process {
      public:
        [[nodiscard]] static std::expected<I3Process, std::string> start(I3Options const &options) {
            std::vector<std::string> argv{options.binary.native(), "-c", options.config.native()};
            argv.append_range(options.arguments);
            auto process = ChildProcess::spawn({.argv = argv, .output = options.log});
            if (!process) {
                return std::unexpected{std::format("i3: {}", process.error())};
            }

            using clock = std::chrono::steady_clock;
            auto const deadline = clock::now() + options.timeout;
            std::string last_error{"its IPC socket doesn't exist yet"};
            for (;;) {
                if (!process->running()) {
                    auto const status = process->wait_for(std::chrono::milliseconds::zero());
                    return std::unexpected{std::format("i3 ended at startup ({}); its output is in {}",
                                                       status ? status->describe() : "?", options.log.native())};
                }
                if (std::filesystem::exists(options.ipc_socket)) {
                    auto ipc = IpcClient::connect(options.ipc_socket);
                    if (ipc) {
                        auto const version = ipc->request(i3ipc::MESSAGE_TYPE::GET_VERSION);
                        if (version) {
                            return I3Process{std::move(*process), std::move(*ipc)};
                        }
                        last_error = version.error();
                    } else {
                        last_error = ipc.error();
                    }
                }
                if (clock::now() >= deadline) {
                    return std::unexpected{std::format("i3 didn't answer on its IPC socket within {} ({}); its output is in {}",
                                                       options.timeout, last_error, options.log.native())};
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{50});
            }
        }

        [[nodiscard]] ChildProcess &process() noexcept {
            return process_;
        }

        /** The IPC connection that start() made. */
        [[nodiscard]] IpcClient &ipc() noexcept {
            return ipc_;
        }

      private:
        I3Process(ChildProcess process, IpcClient ipc) : process_(std::move(process)), ipc_(std::move(ipc)) {
        }

        ChildProcess process_;
        IpcClient ipc_;
    };
}
