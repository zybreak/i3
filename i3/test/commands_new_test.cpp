#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

#include "commands_applier_test.h"

import i3;
import i3_commands_base;
import i3_commands_new;
import i3ipc;

using namespace i3_commands_new;

TEST(CommandsNewTest, Nop) {
    CommandsApplierTest commandsApplier{};
    command_parser_data d{};
    parse_command("nop", std::move(d), commandsApplier);
}
