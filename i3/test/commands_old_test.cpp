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
    command_parser_data d{};
    i3_commands_old::parse_command("nop", std::move(d), &commandsApplier);
}
