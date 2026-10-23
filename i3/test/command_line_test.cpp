/*
 * i3 run with arguments besides its options sends them to the running i3 as a command (handle_extra_args()): it reads
 * the running i3's IPC socket path from the root window's I3_SOCKET_PATH property (root_atom_contents(), on a
 * connection of its own), and exits.
 */
#include <gtest/gtest.h>

import std;
import i3ipc;
import i3.json;
import i3_runner;
import xvfb_fixture;

namespace {
    class CommandLineTest : public XvfbTest {
      protected:
        void start_i3() {
            auto const config = work_dir() / "config";
            std::ofstream{config} << std::format("# i3 config file (v4)\nfont pango:monospace 8\nipc-socket {}\n", ipc_socket().native());
            auto process = I3Process::start({.binary = I3_TEST_I3, .config = config, .ipc_socket = ipc_socket(), .log = work_dir() / "i3.log"});
            ASSERT_TRUE(process) << process.error();
            i3_.emplace(std::move(*process));
        }

        I3Process &i3() {
            if (!i3_) {
                throw std::logic_error{"i3 isn't running: start_i3() failed or wasn't called"};
            }
            return *i3_;
        }

        [[nodiscard]] std::filesystem::path ipc_socket() const {
            return work_dir() / "ipc";
        }

        /* Runs i3 with @p arguments; returns how it ended. */
        std::optional<ExitStatus> run_i3(std::vector<std::string> const &arguments) {
            std::vector<std::string> argv{I3_TEST_I3};
            argv.append_range(arguments);
            auto process = ChildProcess::spawn({.argv = argv, .output = work_dir() / "i3-command.log"});
            if (!process) {
                ADD_FAILURE() << process.error();
                return std::nullopt;
            }
            return process->wait_for(std::chrono::seconds{30});
        }

        [[nodiscard]] std::string focused_workspace() {
            auto const reply = i3().ipc().request_json(i3ipc::MESSAGE_TYPE::GET_WORKSPACES);
            if (!reply) {
                throw std::runtime_error{reply.error()};
            }
            for (auto const &workspace : *reply) {
                if (workspace.at("focused").get<bool>()) {
                    return workspace.at("name").get<std::string>();
                }
            }
            return {};
        }

      private:
        std::optional<I3Process> i3_{};
    };
}  // namespace

TEST_F(CommandLineTest, ArgumentsAreSentToTheRunningI3AsACommand) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_TRUE(i3().ipc().request_json(i3ipc::MESSAGE_TYPE::RUN_COMMAND, "workspace 3"));
    ASSERT_EQ(focused_workspace(), "3");

    EXPECT_EQ(run_i3({"workspace", "back_and_forth"}), ExitStatus::exited(0)) << read_file(work_dir() / "i3-command.log");
    EXPECT_EQ(focused_workspace(), "1");
}

/*
 * Defect: handle_extra_args() appends a space after every argument, so the command is "workspace 7 " and the workspace
 * is named "7 ". Upstream joins the arguments with spaces.
 */
TEST_F(CommandLineTest, DISABLED_TheCommandIsTheArgumentsJoinedWithSpaces) {
    ASSERT_NO_FATAL_FAILURE(start_i3());

    EXPECT_EQ(run_i3({"workspace", "7"}), ExitStatus::exited(0)) << read_file(work_dir() / "i3-command.log");
    EXPECT_EQ(focused_workspace(), "7");
}

/*
 * Defect: without a running i3 the root window has no I3_SOCKET_PATH, and root_atom_contents() returns an optional
 * holding a std::string made from a null pointer instead of std::nullopt (SIGSEGV), where upstream returns NULL and
 * i3 exits with "Could not get i3 IPC socket path".
 */
TEST_F(CommandLineTest, DISABLED_WithoutARunningI3ItFails) {
    auto const status = run_i3({"workspace", "7"});
    ASSERT_TRUE(status);
    EXPECT_EQ(*status, ExitStatus::exited(1)) << status->describe() << ": " << read_file(work_dir() / "i3-command.log");
}
