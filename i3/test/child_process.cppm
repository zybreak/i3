/*
 * Child processes for the tests on Xvfb: Xvfb itself, i3 and the test clients.
 *
 * A ChildProcess is started with fork() and execve(). Between the two, the child calls only async-signal-safe
 * functions, because the test process may have other threads (xcbpp's), whose locks fork() copies in whatever state
 * they are. It dies with the test process (PR_SET_PDEATHSIG), so that a crashed or timed-out test leaves no Xvfb or
 * i3 behind, and its owner terminates and reaps it.
 */
module;
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
/* glibc's <sys/pidfd.h> doesn't declare its functions extern "C". */
extern "C" {
#include <sys/pidfd.h>
}
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
export module child_process;

import std;

export {
    /** The message of an errno value. */
    std::string errno_message(int error) {
        return std::system_category().message(error);
    }

    /** A file descriptor, closed when its owner goes. */
    class UniqueFd {
      public:
        UniqueFd() noexcept = default;

        explicit UniqueFd(int fd) noexcept : fd_(fd) {
        }

        UniqueFd(UniqueFd &&other) noexcept : fd_(std::exchange(other.fd_, -1)) {
        }

        UniqueFd &operator=(UniqueFd &&other) noexcept {
            if (this != &other) {
                reset(std::exchange(other.fd_, -1));
            }
            return *this;
        }

        UniqueFd(UniqueFd const &) = delete;
        UniqueFd &operator=(UniqueFd const &) = delete;

        ~UniqueFd() {
            reset();
        }

        [[nodiscard]] int get() const noexcept {
            return fd_;
        }

        explicit operator bool() const noexcept {
            return fd_ >= 0;
        }

        /** Closes the descriptor held, if any, and holds @p fd. */
        void reset(int fd = -1) noexcept {
            if (fd_ >= 0) {
                ::close(fd_);
            }
            fd_ = fd;
        }

      private:
        int fd_{-1};
    };

    /**
     * Owns @p fd, moved to a number of at least 3 and close-on-exec. A child's standard descriptors are replaced with
     * dup2(); a descriptor it keeps must not be one of them.
     */
    std::expected<UniqueFd, std::string> owned_above_standard_fds(int fd) {
        UniqueFd owned{fd};
        if (fd > STDERR_FILENO) {
            return owned;
        }
        UniqueFd moved{::fcntl(fd, F_DUPFD_CLOEXEC, STDERR_FILENO + 1)};
        if (!moved) {
            return std::unexpected{std::format("fcntl(F_DUPFD_CLOEXEC): {}", errno_message(errno))};
        }
        return moved;
    }

    /** Opens @p path close-on-exec, at a number of at least 3. */
    std::expected<UniqueFd, std::string> open_fd(std::filesystem::path const &path, int flags) {
        /* open()'s flags are an int. */
        int const fd = ::open(path.c_str(), flags | O_CLOEXEC, 0644); // NOLINT(bugprone-signed-bitwise)
        if (fd == -1) {
            return std::unexpected{std::format("{}: {}", path.native(), errno_message(errno))};
        }
        return owned_above_standard_fds(fd);
    }

    /** A pipe (read end, write end), both close-on-exec and at numbers of at least 3. */
    std::expected<std::pair<UniqueFd, UniqueFd>, std::string> make_pipe() {
        int fds[2];
        if (::pipe2(fds, O_CLOEXEC) == -1) {
            return std::unexpected{std::format("pipe2: {}", errno_message(errno))};
        }
        auto read_end = owned_above_standard_fds(fds[0]);
        auto write_end = owned_above_standard_fds(fds[1]);
        if (!read_end) {
            return std::unexpected{read_end.error()};
        }
        if (!write_end) {
            return std::unexpected{write_end.error()};
        }
        return std::pair{std::move(*read_end), std::move(*write_end)};
    }

    /** How a child process ended: its exit code, or the signal that killed it. */
    class ExitStatus {
      public:
        [[nodiscard]] static ExitStatus exited(int code) noexcept {
            return ExitStatus{false, code};
        }

        [[nodiscard]] static ExitStatus killed(int signal) noexcept {
            return ExitStatus{true, signal};
        }

        [[nodiscard]] bool exited_with(int code) const noexcept {
            return !signaled_ && value_ == code;
        }

        [[nodiscard]] bool killed_by(int signal) const noexcept {
            return signaled_ && value_ == signal;
        }

        [[nodiscard]] std::string describe() const {
            if (!signaled_) {
                return std::format("exit status {}", value_);
            }
            char const *name = ::sigabbrev_np(value_);
            return std::format("signal {} (SIG{})", value_, name != nullptr ? name : "?");
        }

        bool operator==(ExitStatus const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, ExitStatus const &status) {
            return out << status.describe();
        }

      private:
        ExitStatus(bool signaled, int value) noexcept : signaled_(signaled), value_(value) {
        }

        bool signaled_;
        int value_;
    };

    /**
     * The file that runs as @p name: @p name itself if it contains a slash, or the first executable file of that name
     * in a directory on $PATH.
     */
    std::expected<std::string, std::string> find_program(std::string_view name) {
        if (name.contains('/')) {
            return std::string{name};
        }
        char const *path = std::getenv("PATH");
        std::string_view const directories{path != nullptr ? path : "/usr/local/bin:/usr/bin:/bin"};
        for (auto const directory : std::views::split(directories, ':')) {
            std::string_view const dir{directory.begin(), directory.end()};
            auto const candidate = std::format("{}/{}", dir.empty() ? "." : dir, name);
            std::error_code error;
            if (std::filesystem::is_regular_file(candidate, error) && ::access(candidate.c_str(), X_OK) == 0) {
                return candidate;
            }
        }
        return std::unexpected{std::format("{} is not on PATH", name)};
    }

    /** What ChildProcess::spawn() runs, and where its output goes. */
    struct SpawnOptions {
        /** The program and its arguments. The program is looked up as find_program() does. */
        std::vector<std::string> argv{};
        /** Standard output, truncated first. Standard input is /dev/null. */
        std::filesystem::path output{"/dev/null"};
        /** Standard error, truncated first; standard output's file if empty. */
        std::filesystem::path error_output{};
        /** Descriptors the program keeps, at the same numbers (none of 0, 1 and 2). It gets no others. */
        std::vector<int> inherited_fds{};
    };

    /**
     * A child process. It gets SIGTERM if the thread that started it ends (gtest runs tests on the main thread), and
     * its owner's destructor terminates and reaps it. A pidfd refers to it, so it can be waited for with a timeout,
     * and no other process that reuses its pid can be signalled by mistake.
     */
    class ChildProcess {
      public:
        /** How long stop() gives a process to exit after SIGTERM, before SIGKILL. */
        static constexpr std::chrono::milliseconds default_grace{std::chrono::seconds{5}};

        /** Starts a process, or says why it couldn't (an exec failure included). */
        [[nodiscard]] static std::expected<ChildProcess, std::string> spawn(SpawnOptions const &options);

        ChildProcess(ChildProcess &&other) noexcept
            : pid_(std::exchange(other.pid_, -1)), pidfd_(std::move(other.pidfd_)), status_(std::exchange(other.status_, std::nullopt)) {
        }

        ChildProcess &operator=(ChildProcess &&other) noexcept {
            if (this != &other) {
                stop_if_running();
                pid_ = std::exchange(other.pid_, -1);
                pidfd_ = std::move(other.pidfd_);
                status_ = std::exchange(other.status_, std::nullopt);
            }
            return *this;
        }

        ChildProcess(ChildProcess const &) = delete;
        ChildProcess &operator=(ChildProcess const &) = delete;

        /** Terminates the process if it still runs (see stop()), and reaps it. */
        ~ChildProcess() {
            stop_if_running();
        }

        [[nodiscard]] pid_t pid() const noexcept {
            return pid_;
        }

        /** Whether the process hasn't exited yet. */
        [[nodiscard]] bool running() const {
            if (status_ || !pidfd_) {
                return false;
            }
            pollfd ready{.fd = pidfd_.get(), .events = POLLIN, .revents = 0};
            int result;
            do {
                result = ::poll(&ready, 1, 0);
            } while (result == -1 && errno == EINTR);
            return result == 0;
        }

        /** How the process ended, once it has been reaped. */
        [[nodiscard]] std::optional<ExitStatus> status() const noexcept {
            return status_;
        }

        /** Waits up to @p timeout for the process to exit, and reaps it. std::nullopt if it still runs. */
        std::optional<ExitStatus> wait_for(std::chrono::milliseconds timeout) {
            if (status_ || !pidfd_) {
                return status_;
            }
            using clock = std::chrono::steady_clock;
            bool const forever = timeout == std::chrono::milliseconds::max();
            auto const deadline = forever ? clock::time_point::max() : clock::now() + timeout;
            for (;;) {
                int wait_ms = -1;
                if (!forever) {
                    auto const remaining = std::chrono::ceil<std::chrono::milliseconds>(deadline - clock::now());
                    wait_ms = static_cast<int>(std::clamp<std::chrono::milliseconds::rep>(remaining.count(), 0, std::numeric_limits<int>::max()));
                }
                pollfd ready{.fd = pidfd_.get(), .events = POLLIN, .revents = 0};
                int const result = ::poll(&ready, 1, wait_ms);
                if (result == 1) {
                    break;
                }
                if (result == -1 && errno != EINTR) {
                    return std::nullopt;
                }
                if (result == 0 && clock::now() >= deadline) {
                    return std::nullopt;
                }
            }
            siginfo_t info{};
            int result;
            do {
                result = ::waitid(P_PIDFD, static_cast<id_t>(pidfd_.get()), &info, WEXITED);
            } while (result == -1 && errno == EINTR);
            if (result == -1) {
                return std::nullopt;
            }
            status_ = info.si_code == CLD_EXITED ? ExitStatus::exited(info.si_status) : ExitStatus::killed(info.si_status);
            pidfd_.reset();
            return status_;
        }

        /** Sends @p signal to the process, unless it has been reaped. */
        void signal(int signal) const noexcept {
            if (!status_ && pidfd_) {
                (void)::pidfd_send_signal(pidfd_.get(), signal, nullptr, 0);
            }
        }

        /**
         * Ends the process: SIGTERM, then SIGKILL if it hasn't exited after @p grace. Returns how it ended (also if it
         * had ended before).
         */
        ExitStatus stop(std::chrono::milliseconds grace = default_grace) {
            if (auto const ended = wait_for(std::chrono::milliseconds::zero())) {
                return *ended;
            }
            signal(SIGTERM);
            if (auto const ended = wait_for(grace)) {
                return *ended;
            }
            signal(SIGKILL);
            if (auto const ended = wait_for(std::chrono::milliseconds::max())) {
                return *ended;
            }
            /* waitid() failed, which it doesn't for a child of ours. */
            return ExitStatus::killed(SIGKILL);
        }

      private:
        ChildProcess(pid_t pid, UniqueFd pidfd) noexcept : pid_(pid), pidfd_(std::move(pidfd)) {
        }

        void stop_if_running() noexcept {
            if (pidfd_ && !status_) {
                (void)stop();
            }
        }

        pid_t pid_{-1};
        UniqueFd pidfd_{};
        std::optional<ExitStatus> status_{};
    };
}

namespace {
    /* What the child needs between fork() and execve(), all of it prepared before fork(). */
    struct ChildSetup {
        char const *program;
        char *const *argv;
        char *const *envp;
        int stdin_fd;
        int stdout_fd;
        int stderr_fd;
        /* The write end of a close-on-exec pipe: the child writes errno to it if it can't run the program. */
        int error_pipe;
        int const *inherited_fds;
        std::size_t inherited_count;
        pid_t parent;
    };

    /* Tells the parent why the child can't run the program (errno), and exits. */
    [[noreturn]] void fail_child(int error_pipe) noexcept {
        int const error = errno;
        (void)!::write(error_pipe, &error, sizeof error);
        ::_exit(127);
    }

    /* Runs in the child, after fork(): only async-signal-safe calls, no allocation, no exceptions. */
    [[noreturn]] void exec_child(ChildSetup const &setup) noexcept {
        /* Ends with the test process, whether it exits, crashes or is killed by ctest's timeout. */
        if (::prctl(PR_SET_PDEATHSIG, SIGTERM) == -1) {
            fail_child(setup.error_pipe);
        }
        if (::getppid() != setup.parent) {
            /* The test process ended before prctl(). */
            ::_exit(127);
        }

        /* Default dispositions and no blocked signals. exec keeps ignored signals ignored: an ignored SIGUSR1, for
         * one, makes Xvfb signal its parent when it is ready. sigaction() fails for SIGKILL, SIGSTOP and the C
         * library's own signals, which is harmless. */
        struct sigaction default_action{};
        default_action.sa_handler = SIG_DFL;
        for (int signal = 1; signal < NSIG; ++signal) {
            (void)::sigaction(signal, &default_action, nullptr);
        }
        sigset_t no_signals;
        ::sigemptyset(&no_signals);
        (void)::sigprocmask(SIG_SETMASK, &no_signals, nullptr);

        if (::dup2(setup.stdin_fd, STDIN_FILENO) == -1 || ::dup2(setup.stdout_fd, STDOUT_FILENO) == -1 ||
            ::dup2(setup.stderr_fd, STDERR_FILENO) == -1) {
            fail_child(setup.error_pipe);
        }
        /* No other descriptor of the test process reaches the program, but the ones it is to keep. */
        if (::close_range(STDERR_FILENO + 1, ~0U, CLOSE_RANGE_CLOEXEC) == -1) {
            fail_child(setup.error_pipe);
        }
        for (std::size_t i = 0; i < setup.inherited_count; ++i) {
            if (::fcntl(setup.inherited_fds[i], F_SETFD, 0) == -1) {
                fail_child(setup.error_pipe);
            }
        }

        ::execve(setup.program, setup.argv, setup.envp);
        fail_child(setup.error_pipe);
    }
}  // namespace

std::expected<ChildProcess, std::string> ChildProcess::spawn(SpawnOptions const &options) {
    if (options.argv.empty()) {
        return std::unexpected{std::string{"no program to run"}};
    }
    if (std::ranges::any_of(options.inherited_fds, [](int fd) { return fd <= STDERR_FILENO; })) {
        return std::unexpected{std::string{"a standard descriptor can't be inherited at its number"}};
    }
    auto const program = find_program(options.argv.front());
    if (!program) {
        return std::unexpected{program.error()};
    }

    auto const stdin_fd = open_fd("/dev/null", O_RDONLY);
    if (!stdin_fd) {
        return std::unexpected{stdin_fd.error()};
    }
    auto const stdout_fd = open_fd(options.output, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND);
    if (!stdout_fd) {
        return std::unexpected{stdout_fd.error()};
    }
    std::optional<UniqueFd> stderr_fd{};
    if (!options.error_output.empty()) {
        auto opened = open_fd(options.error_output, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND);
        if (!opened) {
            return std::unexpected{opened.error()};
        }
        stderr_fd = std::move(*opened);
    }
    auto errors = make_pipe();
    if (!errors) {
        return std::unexpected{errors.error()};
    }

    /* Everything the child uses is built here: it may not allocate. */
    std::vector<std::string> arguments{options.argv};
    std::vector<char *> argv;
    argv.reserve(arguments.size() + 1);
    for (auto &argument : arguments) {
        argv.push_back(argument.data());
    }
    argv.push_back(nullptr);
    std::vector<std::string> environment;
    for (char **variable = environ; *variable != nullptr; ++variable) {
        environment.emplace_back(*variable);
    }
    std::vector<char *> envp;
    envp.reserve(environment.size() + 1);
    for (auto &variable : environment) {
        envp.push_back(variable.data());
    }
    envp.push_back(nullptr);

    ChildSetup const setup{
        .program = program->c_str(),
        .argv = argv.data(),
        .envp = envp.data(),
        .stdin_fd = stdin_fd->get(),
        .stdout_fd = stdout_fd->get(),
        .stderr_fd = stderr_fd ? stderr_fd->get() : stdout_fd->get(),
        .error_pipe = errors->second.get(),
        .inherited_fds = options.inherited_fds.data(),
        .inherited_count = options.inherited_fds.size(),
        .parent = ::getpid(),
    };

    pid_t const pid = ::fork();
    if (pid == -1) {
        return std::unexpected{std::format("fork: {}", errno_message(errno))};
    }
    if (pid == 0) {
        exec_child(setup);
    }

    /* Only the child holds the write end now: the read below ends at its execve() or its exit. */
    errors->second.reset();
    /* The child can't be reaped before this, so its pid refers to it. */
    UniqueFd pidfd{::pidfd_open(pid, 0)};
    if (!pidfd) {
        int const error = errno;
        ::kill(pid, SIGKILL);
        ::waitpid(pid, nullptr, 0);
        return std::unexpected{std::format("pidfd_open: {}", errno_message(error))};
    }
    ChildProcess child{pid, std::move(pidfd)};

    int child_errno = 0;
    ssize_t n;
    do {
        n = ::read(errors->first.get(), &child_errno, sizeof child_errno);
    } while (n == -1 && errno == EINTR);
    if (n == -1) {
        return std::unexpected{std::format("reading {}'s exec status: {}", *program, errno_message(errno))};
    }
    if (n != 0) {
        (void)child.wait_for(std::chrono::milliseconds::max());
        return std::unexpected{std::format("can't run {}: {}", *program, errno_message(child_errno))};
    }
    return child;
}
