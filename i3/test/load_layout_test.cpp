#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include <xcb/xcb.h>

import i3;

class MockX : public X {
public:
    MockX() : X(nullptr) {
        root_depth = 32;
    };
    
    MOCK_METHOD(void, con_init, (Con *con, std::optional<xcb_drawable_t> id), (override));
};

TEST(LoadLayoutTest, ValidateSuccessful) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
        {
            "foo": "bar"
        }
    )"s;
    bool validated = json_validate(input);
    
    ASSERT_TRUE(validated);
}

TEST(LoadLayoutTest, ValidateFailure) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
            "foo" bar
        }
    )"s;
    bool validated = json_validate(input);

    ASSERT_FALSE(validated);
}

TEST(LoadLayoutTest, DetermineContentNoType) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
        {
            "id": 123
        }
    )"s;
    json_content_t content = json_determine_content(input);

    ASSERT_EQ(content, json_content_t::JSON_CONTENT_CON);
}

TEST(LoadLayoutTest, DetermineContentCon) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
        {
            "type": "con",
            "id": 123
        }
    )"s;
    json_content_t content = json_determine_content(input);

    ASSERT_EQ(content, json_content_t::JSON_CONTENT_CON);
}

TEST(LoadLayoutTest, DetermineContentWorkspace) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
        {
            "type": "workspace",
            "id": 123
        }
    )"s;
    json_content_t content = json_determine_content(input);

    ASSERT_EQ(content, json_content_t::JSON_CONTENT_WORKSPACE);
}

TEST(LoadLayoutTest, DetermineContentFailure) {
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
        GARBAGE
    )"s;
    json_content_t content = json_determine_content(input);

    ASSERT_EQ(content, json_content_t::JSON_CONTENT_UNKNOWN);
}

TEST(LoadLayoutTest, AppendJson) {
    global.x = new MockX{};
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    
    using namespace nlohmann::literals;
    using namespace std::literals::string_literals;
    auto input = R"(
{
  "border": "normal",
  "current_border_width": -1,
  "deco_rect": {
    "height": 0,
    "width": 0,
    "x": 0,
    "y": 0
  },
  "floating": "auto_off",
  "focus": [
    107150438687216
  ],
  "focused": false,
  "fullscreen_mode": 0,
  "geometry": {
    "height": 0,
    "width": 0,
    "x": 0,
    "y": 0
  },
  "id": 107150438392128,
  "last_split_layout": "splith",
  "layout": "splith",
  "name": "root",
  "nodes": [
    {
      "border": "normal",
      "current_border_width": -1,
      "deco_rect": {
        "height": 0,
        "width": 0,
        "x": 0,
        "y": 0
      },
      "floating": "auto_off",
      "focus": [
        107150438714080,
        107150438698592,
        107150438717232
      ],
      "focused": false,
      "fullscreen_mode": 0,
      "geometry": {
        "height": 0,
        "width": 0,
        "x": 0,
        "y": 0
      },
      "id": 107150438687216,
      "last_split_layout": "splith",
      "layout": "output",
      "name": "HDMI-1",
      "nodes": [
        {
          "border": "normal",
          "current_border_width": -1,
          "deco_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "floating": "auto_off",
          "focus": [
            107150438751712
          ],
          "focused": false,
          "fullscreen_mode": 0,
          "geometry": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "id": 107150438698592,
          "last_split_layout": "splith",
          "layout": "dockarea",
          "name": "topdock",
          "nodes": [
            {
              "border": "normal",
              "current_border_width": 2,
              "deco_rect": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "floating": "auto_off",
              "focus": [],
              "focused": false,
              "fullscreen_mode": 0,
              "geometry": {
                "height": 24,
                "width": 2560,
                "x": 0,
                "y": 0
              },
              "id": 107150438751712,
              "last_split_layout": "splith",
              "layout": "splith",
              "name": "polybar-example_HDMI-1",
              "nodes": [],
              "orientation": "none",
              "output": "HDMI-1",
              "percent": 1.0,
              "rect": {
                "height": 24,
                "width": 2560,
                "x": 0,
                "y": 0
              },
              "scratchpad_state": "none",
              "sticky": true,
              "swallows": [],
              "type": "con",
              "urgent": false,
              "window": 10485762,
              "window_icon_padding": -1,
              "window_properties": {
                "class": "Polybar",
                "instance": "polybar\u0000Polybar",
                "title": "polybar-example_HDMI-1"
              },
              "window_rect": {
                "height": 24,
                "width": 2560,
                "x": 0,
                "y": 0
              },
              "window_type": "unknown"
            }
          ],
          "orientation": "none",
          "output": "HDMI-1",
          "rect": {
            "height": 24,
            "width": 2560,
            "x": 0,
            "y": 0
          },
          "scratchpad_state": "none",
          "sticky": false,
          "swallows": [
            {
              "dock": 2,
              "insert_where": 2
            }
          ],
          "type": "dockarea",
          "urgent": false,
          "window_icon_padding": -1,
          "window_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          }
        },
        {
          "border": "normal",
          "current_border_width": -1,
          "deco_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "floating": "auto_off",
          "focus": [
            107150438351216,
            107150438173840
          ],
          "focused": false,
          "fullscreen_mode": 0,
          "geometry": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "id": 107150438714080,
          "last_split_layout": "splith",
          "layout": "splith",
          "name": "content",
          "nodes": [
            {
              "border": "normal",
              "current_border_width": -1,
              "deco_rect": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "floating": "auto_off",
              "floating_nodes": [],
              "focus": [
                107150438817504
              ],
              "focused": false,
              "fullscreen_mode": 0,
              "gaps": {
                "bottom": 0,
                "inner": 0,
                "left": 0,
                "outer": 0,
                "right": 0,
                "top": 0
              },
              "geometry": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "id": 107150438173840,
              "last_split_layout": "splith",
              "layout": "splith",
              "name": "1",
              "nodes": [
                {
                  "border": "normal",
                  "current_border_width": -1,
                  "deco_rect": {
                    "height": 0,
                    "width": 0,
                    "x": 0,
                    "y": 0
                  },
                  "floating": "auto_off",
                  "focus": [
                    107150438381376,
                    107150439062112,
                    107150438371440
                  ],
                  "focused": false,
                  "fullscreen_mode": 0,
                  "geometry": {
                    "height": 0,
                    "width": 0,
                    "x": 0,
                    "y": 0
                  },
                  "id": 107150438817504,
                  "last_split_layout": "splith",
                  "layout": "tabbed",
                  "nodes": [
                    {
                      "border": "normal",
                      "current_border_width": 2,
                      "deco_rect": {
                        "height": 24,
                        "width": 853,
                        "x": 0,
                        "y": 0
                      },
                      "floating": "auto_off",
                      "focus": [],
                      "focused": false,
                      "fullscreen_mode": 0,
                      "geometry": {
                        "height": 1359,
                        "width": 2556,
                        "x": 2,
                        "y": 79
                      },
                      "id": 107150438371440,
                      "last_split_layout": "splith",
                      "layout": "splith",
                      "name": "Autobahn - 2009 Remaster • Kraftwerk - Chromium",
                      "nodes": [],
                      "orientation": "none",
                      "output": "HDMI-1",
                      "percent": 0.3333333333333333,
                      "rect": {
                        "height": 1392,
                        "width": 2560,
                        "x": 0,
                        "y": 48
                      },
                      "scratchpad_state": "none",
                      "sticky": false,
                      "swallows": [],
                      "type": "con",
                      "urgent": false,
                      "window": 25165827,
                      "window_icon_padding": -1,
                      "window_properties": {
                        "class": "Chromium\u0000",
                        "instance": "chromium\u0000Chromium\u0000",
                        "machine": "FUCKUP",
                        "title": "Autobahn - 2009 Remaster • Kraftwerk - Chromium",
                        "window_role": "browser"
                      },
                      "window_rect": {
                        "height": 1390,
                        "width": 2556,
                        "x": 2,
                        "y": 0
                      },
                      "window_type": "normal"
                    },
                    {
                      "border": "normal",
                      "current_border_width": 2,
                      "deco_rect": {
                        "height": 24,
                        "width": 853,
                        "x": 853,
                        "y": 0
                      },
                      "floating": "auto_off",
                      "focus": [],
                      "focused": false,
                      "fullscreen_mode": 0,
                      "geometry": {
                        "height": 1359,
                        "width": 2556,
                        "x": 2,
                        "y": 79
                      },
                      "id": 107150438381376,
                      "last_split_layout": "splith",
                      "layout": "splith",
                      "name": "command line - How do I make `less` output colors? - Ask Ubuntu - Chromium",
                      "nodes": [],
                      "orientation": "none",
                      "output": "HDMI-1",
                      "percent": 0.3333333333333333,
                      "rect": {
                        "height": 1392,
                        "width": 2560,
                        "x": 0,
                        "y": 48
                      },
                      "scratchpad_state": "none",
                      "sticky": false,
                      "swallows": [],
                      "type": "con",
                      "urgent": false,
                      "window": 25165829,
                      "window_icon_padding": -1,
                      "window_properties": {
                        "class": "Chromium\u0000",
                        "instance": "chromium\u0000Chromium\u0000",
                        "machine": "FUCKUP",
                        "title": "command line - How do I make `less` output colors? - Ask Ubuntu - Chromium",
                        "window_role": "browser"
                      },
                      "window_rect": {
                        "height": 1390,
                        "width": 2556,
                        "x": 2,
                        "y": 0
                      },
                      "window_type": "normal"
                    },
                    {
                      "border": "normal",
                      "current_border_width": 2,
                      "deco_rect": {
                        "height": 24,
                        "width": 854,
                        "x": 1706,
                        "y": 0
                      },
                      "floating": "auto_off",
                      "focus": [],
                      "focused": false,
                      "fullscreen_mode": 0,
                      "geometry": {
                        "height": 1359,
                        "width": 2556,
                        "x": 2,
                        "y": 79
                      },
                      "id": 107150439062112,
                      "last_split_layout": "splith",
                      "layout": "splith",
                      "name": "objective c - lldb equivalent of gdb's \"follow-fork-mode\" or \"detach-on-fork\" - Stack Overflow - Chromium",
                      "nodes": [],
                      "orientation": "none",
                      "output": "HDMI-1",
                      "percent": 0.3333333333333333,
                      "rect": {
                        "height": 1392,
                        "width": 2560,
                        "x": 0,
                        "y": 48
                      },
                      "scratchpad_state": "none",
                      "sticky": false,
                      "swallows": [],
                      "type": "con",
                      "urgent": false,
                      "window": 25165830,
                      "window_icon_padding": -1,
                      "window_properties": {
                        "class": "Chromium\u0000",
                        "instance": "chromium\u0000Chromium\u0000",
                        "machine": "FUCKUP",
                        "title": "objective c - lldb equivalent of gdb's \"follow-fork-mode\" or \"detach-on-fork\" - Stack Overflow - Chromium",
                        "window_role": "browser"
                      },
                      "window_rect": {
                        "height": 1390,
                        "width": 2556,
                        "x": 2,
                        "y": 0
                      },
                      "window_type": "normal"
                    }
                  ],
                  "orientation": "horizontal",
                  "output": "HDMI-1",
                  "rect": {
                    "height": 1416,
                    "width": 2560,
                    "x": 0,
                    "y": 24
                  },
                  "scratchpad_state": "none",
                  "sticky": false,
                  "swallows": [],
                  "type": "con",
                  "urgent": false,
                  "window_icon_padding": -1,
                  "window_rect": {
                    "height": 0,
                    "width": 0,
                    "x": 0,
                    "y": 0
                  }
                }
              ],
              "num": 1,
              "orientation": "horizontal",
              "output": "HDMI-1",
              "rect": {
                "height": 1416,
                "width": 2560,
                "x": 0,
                "y": 24
              },
              "scratchpad_state": "none",
              "sticky": false,
              "swallows": [],
              "type": "workspace",
              "urgent": false,
              "window_icon_padding": -1,
              "window_rect": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "workspace_layout": "default"
            },
            {
              "border": "normal",
              "current_border_width": -1,
              "deco_rect": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "floating": "auto_off",
              "floating_nodes": [],
              "focus": [
                107150438821104,
                107150438822240
              ],
              "focused": false,
              "fullscreen_mode": 1,
              "gaps": {
                "bottom": 0,
                "inner": 0,
                "left": 0,
                "outer": 0,
                "right": 0,
                "top": 0
              },
              "geometry": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "id": 107150438351216,
              "last_split_layout": "splith",
              "layout": "splith",
              "name": "2",
              "nodes": [
                {
                  "actual_deco_rect": {
                    "height": 24,
                    "width": 1280,
                    "x": 0,
                    "y": 0
                  },
                  "border": "normal",
                  "current_border_width": 2,
                  "deco_rect": {
                    "height": 24,
                    "width": 1280,
                    "x": 0,
                    "y": 0
                  },
                  "floating": "auto_off",
                  "focus": [],
                  "focused": false,
                  "fullscreen_mode": 0,
                  "geometry": {
                    "height": 1386,
                    "width": 2550,
                    "x": 0,
                    "y": 24
                  },
                  "id": 107150438822240,
                  "last_split_layout": "splith",
                  "layout": "splith",
                  "name": "i3 – i3-lib.cppm",
                  "nodes": [],
                  "orientation": "none",
                  "output": "HDMI-1",
                  "percent": 0.5,
                  "rect": {
                    "height": 1416,
                    "width": 1280,
                    "x": 0,
                    "y": 24
                  },
                  "scratchpad_state": "none",
                  "sticky": false,
                  "swallows": [],
                  "type": "con",
                  "urgent": false,
                  "window": 35651651,
                  "window_icon_padding": -1,
                  "window_properties": {
                    "class": "jetbrains-clion\u0000",
                    "instance": "jetbrains-clion\u0000jetbrains-clion\u0000",
                    "machine": "FUCKUP",
                    "title": "i3 – i3-lib.cppm"
                  },
                  "window_rect": {
                    "height": 1390,
                    "width": 1276,
                    "x": 2,
                    "y": 24
                  },
                  "window_type": "normal"
                },
                {
                  "actual_deco_rect": {
                    "height": 24,
                    "width": 1280,
                    "x": 0,
                    "y": 0
                  },
                  "border": "normal",
                  "current_border_width": 2,
                  "deco_rect": {
                    "height": 24,
                    "width": 1280,
                    "x": 1280,
                    "y": 0
                  },
                  "floating": "auto_off",
                  "focus": [],
                  "focused": true,
                  "fullscreen_mode": 0,
                  "geometry": {
                    "height": 600,
                    "width": 800,
                    "x": 0,
                    "y": 0
                  },
                  "id": 107150438821104,
                  "last_split_layout": "splith",
                  "layout": "splith",
                  "name": "zybreak@FUCKUP:~/src/i3",
                  "nodes": [],
                  "orientation": "none",
                  "output": "HDMI-1",
                  "percent": 0.5,
                  "rect": {
                    "height": 1416,
                    "width": 1280,
                    "x": 1280,
                    "y": 24
                  },
                  "scratchpad_state": "none",
                  "sticky": false,
                  "swallows": [],
                  "type": "con",
                  "urgent": false,
                  "window": 39845891,
                  "window_icon_padding": -1,
                  "window_properties": {
                    "class": "Alacritty\u0000",
                    "instance": "Alacritty\u0000Alacritty\u0000",
                    "machine": "FUCKUP",
                    "title": "zybreak@FUCKUP:~/src/i3"
                  },
                  "window_rect": {
                    "height": 1390,
                    "width": 1276,
                    "x": 2,
                    "y": 24
                  },
                  "window_type": "normal"
                }
              ],
              "num": 2,
              "orientation": "horizontal",
              "output": "HDMI-1",
              "rect": {
                "height": 1416,
                "width": 2560,
                "x": 0,
                "y": 24
              },
              "scratchpad_state": "none",
              "sticky": false,
              "swallows": [],
              "type": "workspace",
              "urgent": false,
              "window_icon_padding": -1,
              "window_rect": {
                "height": 0,
                "width": 0,
                "x": 0,
                "y": 0
              },
              "workspace_layout": "default"
            }
          ],
          "orientation": "horizontal",
          "output": "HDMI-1",
          "rect": {
            "height": 1416,
            "width": 2560,
            "x": 0,
            "y": 24
          },
          "scratchpad_state": "none",
          "sticky": false,
          "swallows": [],
          "type": "con",
          "urgent": false,
          "window_icon_padding": -1,
          "window_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          }
        },
        {
          "actual_deco_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "border": "normal",
          "current_border_width": -1,
          "deco_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 1440
          },
          "floating": "auto_off",
          "focus": [],
          "focused": false,
          "fullscreen_mode": 0,
          "geometry": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          },
          "id": 107150438717232,
          "last_split_layout": "splith",
          "layout": "dockarea",
          "name": "bottomdock",
          "nodes": [],
          "orientation": "none",
          "output": "HDMI-1",
          "rect": {
            "height": 0,
            "width": 2560,
            "x": 0,
            "y": 1440
          },
          "scratchpad_state": "none",
          "sticky": false,
          "swallows": [
            {
              "dock": 3,
              "insert_where": 2
            }
          ],
          "type": "dockarea",
          "urgent": false,
          "window_icon_padding": -1,
          "window_rect": {
            "height": 0,
            "width": 0,
            "x": 0,
            "y": 0
          }
        }
      ],
      "orientation": "none",
      "percent": 1.0,
      "rect": {
        "height": 1440,
        "width": 2560,
        "x": 0,
        "y": 0
      },
      "scratchpad_state": "none",
      "sticky": false,
      "swallows": [],
      "type": "output",
      "urgent": false,
      "window_icon_padding": -1,
      "window_rect": {
        "height": 0,
        "width": 0,
        "x": 0,
        "y": 0
      }
    }
  ],
  "orientation": "horizontal",
  "rect": {
    "height": 1440,
    "width": 2560,
    "x": 0,
    "y": 0
  },
  "scratchpad_state": "none",
  "sticky": false,
  "swallows": [],
  "type": "root",
  "urgent": false,
  "window_icon_padding": -1,
  "window_rect": {
    "height": 0,
    "width": 0,
    "x": 0,
    "y": 0
  }
}

    )"s;
    
    RootCon rootCon{true};
    
    tree_append_json(&rootCon, input);

    ASSERT_EQ(1, rootCon.nodes.size());
}
