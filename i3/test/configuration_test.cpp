#include <gtest/gtest.h>
#include <gmock/gmock.h>

import i3;

TEST(ConfigurationTest, foo) {
    auto config = std::make_unique<Config>();
    
    config->ipc_socket_path = "/tmp/i3-ipc.sock";

    ASSERT_EQ(config->ipc_socket_path, "/tmp/i3-ipc.sock");
    
    std::optional<std::string> foo;
    
    foo = "hej";
}

TEST(ConfigurationTest, ModeEquality) {
    Binding_Keycode keycode1{0, 0};
    Binding binding1{};
    binding1.command = "command";
    binding1.keycodes.push_back(keycode1);
    
    Mode mode1{"1", false};
    mode1.bindings.push_back(binding1);

    Binding_Keycode keycode2{0, 0};
    Binding binding2{};
    binding2.command = "command";
    binding2.keycodes.push_back(keycode2);

    Mode mode2{"1", false};
    mode2.bindings.push_back(binding2);

    ASSERT_TRUE(mode1 == mode2);
    ASSERT_FALSE(&mode1 == &mode2);
}
