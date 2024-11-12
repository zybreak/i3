#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import i3;
import mocks;

TEST(ConTest, Floating) {
    MockX mockX{};
    global.x = &mockX;
    ConfigurationManager configManager{mockX};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    ConCon x{nullptr, true};
    FloatingCon f{true};

    x.con_attach(&f, true, nullptr);
    x.floating = FLOATING_USER_ON;

    auto *floating = x.con_inside_floating();

    ASSERT_EQ(floating, &f);
}
