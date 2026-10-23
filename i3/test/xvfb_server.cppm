/*
 * An Xvfb for a test, on a display it picks itself (-displayfd), so that tests run in parallel without clashing with
 * each other or with the X servers already running.
 */
module;
#include <cerrno>
#include <poll.h>
#include <unistd.h>
export module xvfb_server;

import std;
import child_process;

export {
    /** One screen of an Xvfb. */
    struct XvfbScreen {
        int width{1024};
        int height{768};
        int depth{24};
    };

    /** How an XvfbServer runs. */
    struct XvfbOptions {
        /** The program, looked up on $PATH unless it contains a slash. */
        std::string program{"Xvfb"};
        /** Its screens, screen 0 first. */
        std::vector<XvfbScreen> screens{XvfbScreen{}};
        /** Whether it resets when its last client disconnects, as Xvfb does by default. A connection being set up
         *  during a reset is closed, so a test's server doesn't (-noreset). */
        bool reset{false};
        /** More arguments, such as -extension NAME or +extension NAME. */
        std::vector<std::string> arguments{};
        /** Where its standard output and error go. */
        std::filesystem::path log{"/dev/null"};
        /** How long it may take to be ready. */
        std::chrono::milliseconds timeout{std::chrono::seconds{10}};
    };

    /**
     * An Xvfb on the local display it picked. It is ready once start() has returned: Xvfb writes the display number to
     * the -displayfd pipe when it accepts connections. It listens on its Unix sockets only (-nolisten tcp), and its
     * display is ":N". The destructor terminates and reaps it.
     */
    class XvfbServer {
      public:
        [[nodiscard]] static std::expected<XvfbServer, std::string> start(XvfbOptions const &options = {});

        /** The display, ":N". */
        [[nodiscard]] std::string const &display() const noexcept {
            return display_;
        }

        [[nodiscard]] int display_number() const noexcept {
            return number_;
        }

        [[nodiscard]] ChildProcess &process() noexcept {
            return process_;
        }

        /** Terminates the server and reaps it; returns how it ended. */
        ExitStatus stop() {
            return process_.stop();
        }

      private:
        XvfbServer(ChildProcess process, int number) : process_(std::move(process)), number_(number), display_(std::format(":{}", number)) {
        }

        ChildProcess process_;
        int number_;
        std::string display_;
    };
}

namespace {
    /*
     * Reads what Xvfb writes to the -displayfd pipe: the display number, and then "\n" in a second write. Xvfb exits
     * with "Cannot write display number to fd" if the pipe is closed before the second write, so this reads up to the
     * newline. Returns the text without the newline, or std::nullopt if the pipe was closed first (Xvfb exited).
     */
    std::expected<std::optional<std::string>, std::string> read_display(int fd, std::chrono::milliseconds timeout) {
        using clock = std::chrono::steady_clock;
        auto const deadline = clock::now() + timeout;
        std::string text;
        while (!text.ends_with('\n')) {
            auto const remaining = std::chrono::ceil<std::chrono::milliseconds>(deadline - clock::now());
            if (remaining <= std::chrono::milliseconds::zero()) {
                return std::unexpected{std::format("it didn't report its display within {}", timeout)};
            }
            pollfd readable{.fd = fd, .events = POLLIN, .revents = 0};
            int const ready = ::poll(&readable, 1, static_cast<int>(std::min<std::chrono::milliseconds::rep>(remaining.count(), std::numeric_limits<int>::max())));
            if (ready == -1 && errno != EINTR) {
                return std::unexpected{std::format("poll: {}", errno_message(errno))};
            }
            if (ready != 1) {
                continue;
            }
            std::array<char, 16> buffer{};
            ssize_t const n = ::read(fd, buffer.data(), buffer.size());
            if (n == -1) {
                if (errno == EINTR) {
                    continue;
                }
                return std::unexpected{std::format("read: {}", errno_message(errno))};
            }
            if (n == 0) {
                return std::nullopt;
            }
            text.append(buffer.data(), static_cast<std::size_t>(n));
        }
        text.pop_back();
        return text;
    }
}  // namespace

std::expected<XvfbServer, std::string> XvfbServer::start(XvfbOptions const &options) {
    auto display_pipe = make_pipe();
    if (!display_pipe) {
        return std::unexpected{display_pipe.error()};
    }
    auto &[read_end, write_end] = *display_pipe;

    std::vector<std::string> argv{options.program, "-displayfd", std::to_string(write_end.get()), "-nolisten", "tcp",
                                  options.reset ? "-reset" : "-noreset"};
    for (auto const &[index, screen] : std::views::enumerate(options.screens)) {
        argv.insert(argv.end(), {"-screen", std::to_string(index), std::format("{}x{}x{}", screen.width, screen.height, screen.depth)});
    }
    argv.append_range(options.arguments);

    auto process = ChildProcess::spawn({.argv = argv, .output = options.log, .inherited_fds = {write_end.get()}});
    if (!process) {
        return std::unexpected{std::format("Xvfb: {}", process.error())};
    }
    /* Only Xvfb holds the write end now, so the read ends if it exits. */
    write_end.reset();

    auto const display = read_display(read_end.get(), options.timeout);
    if (!display) {
        return std::unexpected{std::format("Xvfb: {} (its output: {})", display.error(), options.log.native())};
    }
    if (!*display) {
        auto const status = process->wait_for(std::chrono::seconds{1});
        return std::unexpected{std::format("Xvfb ended ({}) before it reported its display (its output: {})",
                                           status ? status->describe() : "it still runs", options.log.native())};
    }

    std::string const &text = **display;
    int number{};
    auto const parsed = std::from_chars(text.data(), text.data() + text.size(), number);
    if (!parsed || parsed.ptr != text.data() + text.size() || number < 0) {
        return std::unexpected{std::format("Xvfb reported the display {:?}, which isn't a display number", text)};
    }
    return XvfbServer{std::move(*process), number};
}
