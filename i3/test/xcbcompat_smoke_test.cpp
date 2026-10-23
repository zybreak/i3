/*
 * i3 on Xvfb, on xcbpp's libxcb compatibility layer: it manages a window, replays a click to it (GrabButton and
 * AllowEvents), runs a key binding (GrabKey), shows and closes nagbars, exits on request (with a nagbar open), and its
 * save set gives the window back to the root window. At depth 8, i3 allocates its colours on the server (AllocColor).
 *
 * The test client (xcbcompat/client.c) drives i3 through XTEST. It runs on the system's libxcb, as X clients do, in a
 * process of its own: i3's process loads the layer as its libxcb.so.1.
 */
#include <gtest/gtest.h>

import std;
import i3ipc;
import i3_runner;
import xvfb_fixture;

namespace {
    class XcbcompatSmokeTest : public XvfbTest,
                               public ::testing::WithParamInterface<int> {
      protected:
        [[nodiscard]] XvfbOptions xvfb_options() const override {
            auto options = XvfbTest::xvfb_options();
            options.screens = {XvfbScreen{.width = 1024, .height = 768, .depth = GetParam()}};
            return options;
        }

        /* Starts the client with @p arguments; its output goes to @p output. */
        ChildProcess start_client(std::vector<std::string> const &arguments, std::filesystem::path const &output) {
            std::vector<std::string> argv{I3_TEST_XCBCOMPAT_CLIENT};
            argv.append_range(arguments);
            auto client = ChildProcess::spawn({.argv = argv, .output = output, .error_output = work_dir() / "client.err"});
            if (!client) {
                throw std::runtime_error{client.error()};
            }
            return std::move(*client);
        }

        /* Runs the client with @p arguments to its end; returns its standard output without the last newline, as the
         * shell's $(…) does. */
        std::string run_client(std::vector<std::string> const &arguments) {
            auto const output = work_dir() / "client-run.out";
            auto client = start_client(arguments, output);
            if (!client.wait_for(std::chrono::seconds{10})) {
                ADD_FAILURE() << "the client is still running after 10 s";
            }
            auto text = read_file(output);
            if (text.ends_with('\n')) {
                text.pop_back();
            }
            return text;
        }

        /* Waits up to five seconds for the number of dock windows (i3's nagbars) to be @p count. */
        bool wait_docks(int count) {
            return poll_until([&] { return run_client({"docks"}) == std::format("docks {}", count); }, std::chrono::seconds{5});
        }

        /* Shows a nagbar whose button touches nagbar-marker. Each nagbar runs on a thread of its own, with an X
         * connection of its own (at depth 8, its colours are allocated on it). */
        void nagbar(I3Process &i3, std::string_view text) {
            auto const command = std::format(R"(nagbar warning "smoke {}" "pango:monospace 8" false "Touch" "touch {}" false)", text, (work_dir() / "nagbar-marker").native());
            auto const reply = i3.ipc().request(i3ipc::MESSAGE_TYPE::RUN_COMMAND, command);
            ASSERT_TRUE(reply) << reply.error();
        }
    };

    std::string maps_of(pid_t pid) {
        return read_file(std::format("/proc/{}/maps", pid));
    }

    /* The layer's file: lib/xcbpp/libxcb.so.1 in xcbpp's package. */
    bool contains_layer(std::string_view text) {
        static std::regex const layer{R"(/xcbpp/libxcb\.so)"};
        return std::regex_search(text.begin(), text.end(), layer);
    }

    /* Whether a line of @p text satisfies @p predicate. */
    bool any_line(std::string_view text, std::invocable<std::string_view> auto predicate) {
        return std::ranges::any_of(std::views::split(text, '\n'), [&](auto const line) {
            return predicate(std::string_view{line.begin(), line.end()});
        });
    }

    /* Whether a mapping of a libxcb.so.1 other than the layer's is in @p maps. */
    bool other_libxcb_mapped(std::string_view maps) {
        static std::regex const libxcb{R"(libxcb\.so\.1)"};
        return any_line(maps, [](std::string_view line) {
            return std::regex_search(line.begin(), line.end(), libxcb) && !contains_layer(line);
        });
    }

    /* Whether a line of @p text starts with @p prefix (grep '^prefix'). */
    bool has_line_starting_with(std::string_view text, std::string_view prefix) {
        return any_line(text, [prefix](std::string_view line) { return line.starts_with(prefix); });
    }
}  // namespace

TEST_P(XcbcompatSmokeTest, I3RunsOnTheLayer) {
    auto const config = work_dir() / "config";
    auto const ipc_socket = work_dir() / "ipc";
    auto const key_marker = work_dir() / "key-marker";
    std::ofstream{config} << std::format(R"(# i3 config file (v4)
font pango:monospace 8
ipc-socket {}
bindcode 75 exec touch {}
)",
                                         ipc_socket.native(), key_marker.native());

    auto started = I3Process::start({.binary = I3_TEST_I3, .config = config, .ipc_socket = ipc_socket, .log = work_dir() / "i3.log"});
    ASSERT_TRUE(started) << "i3 died at startup: " << started.error() << '\n'
                         << read_file(work_dir() / "i3.log");
    auto &i3 = *started;

    auto const maps = maps_of(i3.process().pid());
    EXPECT_TRUE(contains_layer(maps)) << "i3's libxcb.so.1 is the layer";
    EXPECT_FALSE(other_libxcb_mapped(maps)) << "no other libxcb.so.1 is loaded";

    /* The client maps a window, waits until i3 has reparented it, clicks into it and waits for the click to be
     * replayed to it. It keeps the window until the end of the test, for the save set's check. */
    auto const manage_output = work_dir() / "client.out";
    auto manage = start_client({"manage"}, manage_output);
    static std::regex const done{"click (ok|lost)|not managed"};
    poll_until([&] { return std::regex_search(read_file(manage_output), done); }, std::chrono::seconds{6});
    auto const managed = read_file(manage_output);
    EXPECT_TRUE(has_line_starting_with(managed, "managed")) << "the window is managed: " << managed;
    EXPECT_TRUE(has_line_starting_with(managed, "click ok")) << "the click is replayed to the window: " << managed;

    run_client({"key", "75"});
    EXPECT_TRUE(poll_until([&] { return std::filesystem::exists(key_marker); }, std::chrono::seconds{2}))
        << "the key binding runs";

    /* The first nagbar is closed with its close button, which closes its connection; the second one, on a new
     * connection, is still open when i3 exits. */
    nagbar(i3, "1");
    EXPECT_TRUE(wait_docks(1)) << "a nagbar is shown";
    run_client({"close-dock"});
    EXPECT_TRUE(wait_docks(0)) << "its close button closes it";
    nagbar(i3, "2");
    EXPECT_TRUE(wait_docks(1)) << "a second nagbar is shown";

    auto const sent = i3.ipc().send(i3ipc::MESSAGE_TYPE::RUN_COMMAND, "exit");
    ASSERT_TRUE(sent) << sent.error();
    auto status = i3.process().wait_for(std::chrono::seconds{5});
    EXPECT_TRUE(status) << "i3 exits";
    if (!status) {
        status = i3.process().wait_for(std::chrono::seconds{10});
    }
    EXPECT_EQ(status, ExitStatus::exited(0)) << "i3's exit status is 0";
    EXPECT_EQ(run_client({"orphans"}), "orphans 1") << "the save set gives the window back to the root";
}

INSTANTIATE_TEST_SUITE_P(Depth, XcbcompatSmokeTest, ::testing::Values(24, 8), [](::testing::TestParamInfo<int> const &info) {
    return std::format("depth{}", info.param);
});
