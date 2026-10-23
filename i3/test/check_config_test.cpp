/*
 * i3 -C (check the configuration file and exit) on Xvfb. i3 -C runs without a WorkspaceManager or an
 * AssignmentManager, and workspace assignments, per-workspace gaps and for_window, assign and no_focus directives once
 * crashed it. It needs an X server: the configuration loads fonts and colours on i3's X connection.
 */
#include <gtest/gtest.h>

import std;
import xvfb_fixture;

namespace {
    /* i3's error log lines, or upstream's ERROR. */
    bool has_error(std::string const &output) {
        static std::regex const error{R"( error \[thread |ERROR)"};
        return std::regex_search(output, error);
    }

    class CheckConfigTest : public XvfbTest {
      protected:
        /* Runs i3 -C on a configuration file holding @p config; returns how it ended and what it printed. */
        std::pair<std::optional<ExitStatus>, std::string> check_config(std::string_view config) {
            auto const file = work_dir() / "config";
            std::ofstream{file} << config;
            auto const output = work_dir() / "i3.out";
            auto i3 = ChildProcess::spawn({.argv = {I3_TEST_I3, "-C", "-c", file.native()}, .output = output});
            if (!i3) {
                ADD_FAILURE() << i3.error();
                return {};
            }
            auto status = i3->wait_for(std::chrono::seconds{30});
            if (!status) {
                ADD_FAILURE() << "i3 -C is still running after 30 s";
            }
            return {status, read_file(output)};
        }
    };
}  // namespace

TEST_F(CheckConfigTest, AValidConfigurationExitsZeroWithoutErrors) {
    auto const [status, output] = check_config(R"(# i3 config file (v4)
font pango:monospace 8
workspace 1 output DP-1
workspace 2 output HDMI-1 DP-1
workspace 1 gaps inner 5
workspace 3 gaps outer 2
for_window [class="XTerm"] border pixel 1
assign [class="Firefox"] 2
assign [class="mpv"] output DP-1
no_focus [class="Dialog"]
)");

    EXPECT_EQ(status, ExitStatus::exited(0)) << output;
    EXPECT_FALSE(has_error(output)) << output;
}

TEST_F(CheckConfigTest, AnInvalidConfigurationExitsOneWithAnError) {
    auto const [status, output] = check_config(R"(# i3 config file (v4)
font pango:monospace 8
workspace 1 output DP-1
invalid
)");

    EXPECT_EQ(status, ExitStatus::exited(1)) << output;
    EXPECT_TRUE(has_error(output)) << output;
}
