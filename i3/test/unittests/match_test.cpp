#include <gtest/gtest.h>
#include "match.h"

extern std::deque<Con*> all_cons;

TEST(MatchTest, IsNotInitialized){
    Match m{};

    ASSERT_TRUE(match_is_empty(m));
}

TEST(MatchTest, IsInitialized){
    Match m{};

    m.parse_property("class", "foo");

    ASSERT_FALSE(match_is_empty(m));
}

TEST(MatchTest, MatchesWindowClass){
    Match m{};

    m.dock = M_DONTCHECK;
    m.parse_property("class", "foo");

    EXPECT_FALSE(match_is_empty(m));

    i3Window w{};
    w.class_class = sstrdup("foo");

    ASSERT_TRUE(match_matches_window(m, &w));
}

TEST(MatchTest, MatchesUrgentLatest){
    Match m{};

    m.urgent = U_LATEST;

    EXPECT_FALSE(match_is_empty(m));

    i3Window w{};
    w.class_class = sstrdup("foo");
    w.urgent = {
        .tv_sec = 1,
        .tv_usec = 7500000
    };

    i3Window w2{};
    w2.urgent = {
        .tv_sec = 2,
        .tv_usec = 7500000
    };

    auto c = new Con(nullptr, &w2, true);

    ASSERT_FALSE(match_matches_window(m, &w));
}

TEST(MatchTest, MatchesUrgentOldest){
    Match m{};

    m.urgent = U_OLDEST;

    EXPECT_FALSE(match_is_empty(m));

    i3Window w{};
    w.class_class = sstrdup("foo");
    w.urgent = {
        .tv_sec = 1,
        .tv_usec = 7500000
    };

    i3Window w2{};
    w2.urgent = {
        .tv_sec = 2,
        .tv_usec = 7500000
    };

    auto c = new Con(nullptr, &w2, true);

    ASSERT_TRUE(match_matches_window(m, &w));
}