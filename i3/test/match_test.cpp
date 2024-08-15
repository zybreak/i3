#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import i3;

class MockX : public X {
public:
    MockX() : X(nullptr) {
        root_depth = 32;
    };

    MOCK_METHOD(void, con_init, (Con *con, std::optional<xcb_drawable_t> id), (override));
};

TEST(MatchTest, IsNotInitialized){
    Match m{};

    ASSERT_TRUE(m.match_is_empty());
}

TEST(MatchTest, IsInitialized){
    Match m{};

    m.parse_property("class", "foo");

    ASSERT_FALSE(m.match_is_empty());
}

TEST(MatchTest, MatchesWindowClass){
    Match m{};

    m.dock = M_DONTCHECK;
    m.parse_property("class", "foo");

    EXPECT_FALSE(m.match_is_empty());

    i3Window w{};
    w.class_class = "foo";

    ASSERT_TRUE(m.match_matches_window(&w));
}

TEST(MatchTest, MatchesUrgentLatest){
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = new MockX{};
    
    Match m{};

    m.urgent = U_LATEST;

    EXPECT_FALSE(m.match_is_empty());

    i3Window w{};
    w.class_class = "foo";
    w.urgent = std::chrono::system_clock::now();

    i3Window w2{};
    w2.urgent = std::chrono::system_clock::now() + std::chrono::seconds(1);

    auto c = new ConCon(&w2, true);

    ASSERT_FALSE(m.match_matches_window(&w));
}

TEST(MatchTest, MatchesUrgentOldest){
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = new MockX{};
    
    Match m{};

    m.urgent = U_OLDEST;

    EXPECT_FALSE(m.match_is_empty());

    i3Window w{};
    w.class_class = "foo";
    w.urgent = std::chrono::system_clock::now();

    i3Window w2{};
    w2.urgent = std::chrono::system_clock::now() + std::chrono::seconds(1);

    auto c = new ConCon(&w2, true);

    ASSERT_TRUE(m.match_matches_window(&w));
}


TEST(MatchTest, ParseProperty){
    ConfigurationManager configManager{};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = new MockX{};

    Match m{};
   
    EXPECT_ANY_THROW({
        m.parse_property("con_id", "foo");
    });
}