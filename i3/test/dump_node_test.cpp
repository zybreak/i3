#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

import i3;

using json = nlohmann::json;

class MockX : public X {
public:
    MockX() : X(nullptr) {
        root_depth = 32;
    };
};

TEST(DumpNodeTest, Dump) {
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = new MockX{};
    
    RootCon root{true};
    root.layout = layout_t::L_SPLITV;
    
    OutputCon output{true};
    output.name = "eDP-1";
    output.layout = layout_t::L_OUTPUT;
    
    output.con_attach(&root, true, nullptr);
    
    json j = dump_node(&root, false);
    
    ASSERT_TRUE(j.is_object());
}
