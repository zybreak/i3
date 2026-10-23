/*
 * The gtest fixture of the tests on Xvfb, and what they share: a scoped environment variable, a temporary directory and
 * polling for a condition.
 */
module;
#include <gtest/gtest.h>
#include <cerrno>
#include <cstdlib>
export module xvfb_fixture;

import std;
export import child_process;
export import xvfb_server;

export {
    /**
     * Sets or unsets an environment variable for a scope, and puts back what it was when the scope ends. gtest runs
     * the tests of a process one after another on one thread, and the children the test starts get a copy of the
     * environment taken when they are started, so nothing races the change.
     */
    class ScopedEnv {
      public:
        /** Saves $@p name, then sets it to @p value, or unsets it if @p value is std::nullopt. */
        ScopedEnv(std::string name, std::optional<std::string> const &value) : name_(std::move(name)) {
            if (char const *saved = std::getenv(name_.c_str())) {
                saved_ = saved;
            }
            set(value);
        }

        ~ScopedEnv() {
            set(saved_);
        }

        ScopedEnv(ScopedEnv const &) = delete;
        ScopedEnv &operator=(ScopedEnv const &) = delete;

      private:
        void set(std::optional<std::string> const &value) const {
            if (value) {
                ::setenv(name_.c_str(), value->c_str(), 1);
            } else {
                ::unsetenv(name_.c_str());
            }
        }

        std::string name_;
        std::optional<std::string> saved_{};
    };

    /** A new directory under $TMPDIR (or /tmp), removed with its contents when its owner goes, unless kept. */
    class TempDir {
      public:
        [[nodiscard]] static std::expected<TempDir, std::string> create(std::string_view prefix) {
            std::error_code error;
            auto const base = std::filesystem::temp_directory_path(error);
            if (error) {
                return std::unexpected{std::format("temporary directory: {}", error.message())};
            }
            std::string name = (base / std::format("{}-XXXXXX", prefix)).native();
            if (::mkdtemp(name.data()) == nullptr) {
                return std::unexpected{std::format("mkdtemp {}: {}", name, errno_message(errno))};
            }
            return TempDir{std::filesystem::path{name}};
        }

        TempDir(TempDir &&other) noexcept : path_(std::exchange(other.path_, {})), kept_(other.kept_) {
        }

        TempDir &operator=(TempDir &&) = delete;
        TempDir(TempDir const &) = delete;
        TempDir &operator=(TempDir const &) = delete;

        ~TempDir() {
            if (!path_.empty() && !kept_) {
                std::error_code ignored;
                std::filesystem::remove_all(path_, ignored);
            }
        }

        [[nodiscard]] std::filesystem::path const &path() const noexcept {
            return path_;
        }

        /** Leaves the directory in place, for a look at what a failed test left there. */
        void keep() noexcept {
            kept_ = true;
        }

      private:
        explicit TempDir(std::filesystem::path path) : path_(std::move(path)) {
        }

        std::filesystem::path path_;
        bool kept_{false};
    };

    /** The contents of @p path; empty if it can't be read. */
    std::string read_file(std::filesystem::path const &path) {
        std::ifstream in{path, std::ios::binary};
        return std::string{std::istreambuf_iterator<char>{in}, std::istreambuf_iterator<char>{}};
    }

    /** Calls @p condition every 50 ms until it returns true (then true) or @p timeout has passed (then false). */
    template <std::invocable Condition>
    bool poll_until(Condition &&condition, std::chrono::milliseconds timeout) {
        auto const deadline = std::chrono::steady_clock::now() + timeout;
        for (;;) {
            if (std::invoke(condition)) {
                return true;
            }
            if (std::chrono::steady_clock::now() >= deadline) {
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{50});
        }
    }

    /**
     * Starts an Xvfb for each test and points $DISPLAY at it for the test's duration, so tests never share a server,
     * whether ctest runs each in its own process or the executable runs them all. Each test also gets a temporary
     * directory, work_dir(), which holds the server's output (xvfb.log); it is kept, and its path printed, if the test
     * fails.
     *
     * The server, $DISPLAY and the directory belong to this base class, so they outlive what a derived fixture holds:
     * an i3 that runs on the server ends before it.
     */
    class XvfbTest : public ::testing::Test {
      protected:
        /** The test's server's options; a fixture overrides this for other screens or arguments. */
        [[nodiscard]] virtual XvfbOptions xvfb_options() const {
            XvfbOptions options{};
#ifdef I3_TEST_XVFB
            options.program = I3_TEST_XVFB;
#endif
            return options;
        }

        void SetUp() override {
            auto dir = TempDir::create("i3-test");
            ASSERT_TRUE(dir) << dir.error();
            work_dir_.emplace(std::move(*dir));

            auto options = xvfb_options();
            options.log = work_dir() / "xvfb.log";
            auto server = XvfbServer::start(options);
            ASSERT_TRUE(server) << server.error();
            server_.emplace(std::move(*server));
            display_.emplace("DISPLAY", server_->display());
        }

        void TearDown() override {
            if (HasFailure() && work_dir_) {
                work_dir_->keep();
                std::cout << "The test's files are kept in " << work_dir().native() << '\n';
            }
        }

        /** The test's server; there is one once SetUp() has succeeded. */
        [[nodiscard]] XvfbServer &server() {
            if (!server_) {
                throw std::logic_error{"the test has no Xvfb: XvfbTest::SetUp() failed"};
            }
            return *server_;
        }

        [[nodiscard]] std::filesystem::path const &work_dir() const {
            if (!work_dir_) {
                throw std::logic_error{"the test has no directory: XvfbTest::SetUp() failed"};
            }
            return work_dir_->path();
        }

      private:
        /* Destroyed in reverse order: $DISPLAY is put back, the server ends, then the directory goes. */
        std::optional<TempDir> work_dir_{};
        std::optional<XvfbServer> server_{};
        std::optional<ScopedEnv> display_{};
    };
}
