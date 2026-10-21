#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import i3;
import mocks;

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

    i3Window w{XCB_WINDOW_NONE};
    w.class_class = "foo";

    ASSERT_TRUE(m.match_matches_window(&w));
}

TEST(MatchTest, MatchesUrgentLatest){
    MockX mockX{};
    ConfigurationManager configManager{mockX};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = &mockX;
    
    Match m{};

    m.urgent = U_LATEST;

    EXPECT_FALSE(m.match_is_empty());

    i3Window w{XCB_WINDOW_NONE};
    w.class_class = "foo";
    w.urgent = std::chrono::system_clock::now();

    i3Window w2{XCB_WINDOW_NONE};
    w2.urgent = std::chrono::system_clock::now() + std::chrono::seconds(1);

    auto c = new ConCon(&w2, true);

    ASSERT_FALSE(m.match_matches_window(&w));
}

TEST(MatchTest, MatchesUrgentOldest){
    MockX mockX{};
    ConfigurationManager configManager{mockX};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = &mockX;
    
    Match m{};

    m.urgent = U_OLDEST;

    EXPECT_FALSE(m.match_is_empty());

    i3Window w{XCB_WINDOW_NONE};
    w.class_class = "foo";
    w.urgent = std::chrono::system_clock::now();

    i3Window w2{XCB_WINDOW_NONE};
    w2.urgent = std::chrono::system_clock::now() + std::chrono::seconds(1);

    auto c = new ConCon(&w2, true);

    ASSERT_TRUE(m.match_matches_window(&w));
}


TEST(MatchTest, ParseProperty){
    MockX mockX{};
    ConfigurationManager configManager{mockX};
    global.configManager = &configManager;
    global.configManager->config = std::make_unique<Config>();
    global.x = &mockX;

    Match m{};
   
    EXPECT_ANY_THROW({
        m.parse_property("con_id", "foo");
    });
}

TEST(MatchTest, ParsePropertyIds) {
    Match m{};

    m.parse_property("id", "94371851");
    EXPECT_EQ(m.id, 94371851u);
    m.parse_property("id", "0x5a0000b");
    EXPECT_EQ(m.id, 0x5a0000bu);

    /* A con_id is the address of the container, as the IPC tree shows it. */
    m.parse_property("con_id", "94371851");
    EXPECT_EQ(reinterpret_cast<std::uintptr_t>(m.con_id), 94371851u);

    EXPECT_THROW(m.parse_property("id", "foo"), std::runtime_error);
    EXPECT_THROW(m.parse_property("con_id", "foobar"), std::runtime_error);
}

TEST(MatchTest, InvalidCriterionFailsTheUsersOfTheCriteria) {
    criteria_state_impl cs{0};

    cs.add_criterion("id", "foo");
    cs.add_criterion("con_id", "bar");
    cs.add_criterion("tiling", nullptr);

    EXPECT_EQ(cs.current_match.window_mode, WM_TILING);
    try {
        cs.check_match();
        FAIL() << "check_match() should throw the first invalid criterion";
    } catch (std::runtime_error const &e) {
        EXPECT_STREQ(e.what(), "Invalid match: invalid id");
    }

    Config config{};
    ConfigApplier applier{&config};
    EXPECT_THROW(applier.for_window(&cs, "kill"), std::runtime_error);
    EXPECT_THROW(applier.no_focus(&cs), std::runtime_error);
}
