#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

#include "commands_applier_test.h"

import i3;
import i3_commands_base;
import i3_commands_old;
import i3ipc;

TEST(CommandsOldTest, Nop) {
    CommandsApplierTest commandsApplier{};
    i3_commands_old::parse_command("nop", commandsApplier);
}

/* Records the replies and the comments of nop commands. */
class RecordingCommandsApplier : public CommandsApplierTest {
  public:
    std::vector<json> replies{};
    std::vector<std::string> nops{};

    void append_json(json j) override {
        replies.push_back(std::move(j));
    }

    void nop(criteria_state *criteria_state, CommandsResultIR &cmd_output, char const *comment) override {
        nops.emplace_back((comment != nullptr) ? comment : "");
    }
};

TEST(CommandsOldTest, ParsingGoesOnAfterACommandFailed) {
    RecordingCommandsApplier applier{};

    /* The test applier's criteria_add() throws for the invalid id. */
    auto result = i3_commands_old::parse_command("[id=foo] nop a; nop b", applier);

    EXPECT_FALSE(result.parse_error);
    ASSERT_EQ(applier.replies.size(), 1u);
    EXPECT_EQ(applier.replies[0]["success"], false);
    EXPECT_EQ(applier.replies[0]["error"], "Invalid match: invalid id");
    EXPECT_EQ(applier.nops, (std::vector<std::string>{"a", "b"}));
}
