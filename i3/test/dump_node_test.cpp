#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

import i3;
import mocks;

using json = nlohmann::json;

TEST(DumpNodeTest, Dump) {
    MockX mockX{};
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = &mockX;
    
    RootCon root{true};
    root.layout = layout_t::L_SPLITV;
    
    OutputCon output{true};
    output.name = "eDP-1";
    output.layout = layout_t::L_OUTPUT;
    
    output.con_attach(&root, true, nullptr);
    
    json j = dump_node(&root, false);
   
    json nodes = json::array();
    nodes.push_back({
        {"actual_deco_rect",{{"height",0},{"width",0},{"x",0},{"y",0}}},
        {"border","normal"},
        {"current_border_width",-1},
        {"deco_rect",{{"height",0},{"width",0},{"x",0},{"y",0}}},
        {"floating","auto_off"},
        {"floating_nodes",json::array()},
        {"focus",json::array()},
        {"focused",false},
        {"fullscreen_mode",0},
        {"geometry",{{"height",0},{"width",0},{"x",0},{"y",0}}},
        {"id",(uintptr_t)&output},
        {"last_split_layout","splith"},
        {"layout","output"},
        {"name","eDP-1"},
        {"nodes",json::array()},
        {"orientation","none"},
        {"rect",{{"height",0},{"width",0},{"x",0},{"y",0}}},
        {"sticky",false},
        {"swallows",json::array()},
        {"type","output"},
        {"urgent",false},
        {"window_icon_padding",-1},
        {"window_rect",{{ "height",0 },{ "width",0 },{ "x",0 },{ "y",0 }}}
    });
    
    json j2 = {
        {"border","normal"},
        {"current_border_width",-1},
        {"deco_rect", {{ "height",0 }, { "width",0 }, { "x",0 }, { "y",0 }}},
        {"floating","auto_off"},
        {"floating_nodes", json::array()},
        {"focus",std::vector{(uintptr_t)&output}},
        {"focused",false},
        {"fullscreen_mode",0},
        {"geometry",{{"height",0}, {"width",0}, {"x",0}, {"y",0}}},
        {"id",(uintptr_t)&root},
        {"last_split_layout","splitv"},
        {"layout","splitv"},
        {"name","root"},
        {"nodes", nodes},
        {"orientation","vertical"},
        {"rect",{{"height",0},{"width",0},{"x",0},{"y",0}}},
        {"sticky",false},
        {"swallows",json::array()},
        {"type","root"},
        {"urgent",false},
        {"window_icon_padding",-1},
        {"window_rect",{{"height",0},{"width",0},{"x",0},{"y",0}}}
    };

    ASSERT_EQ(j, j2);
}
