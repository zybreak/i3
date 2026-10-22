#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import std;
import i3;
import mocks;

using testing::ElementsAre;

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

/*
 * Workspaces attached to an output's content container, which keeps them in the order of upstream's _con_attach():
 * numbered workspaces sorted by number (a new one after those with the same number), then named ones (num == -1) in
 * the order they were attached.
 */
class WorkspaceAttachTest : public testing::Test {
  protected:
    MockX mockX{};
    ConfigurationManager configManager{mockX};
    std::vector<std::unique_ptr<Con>> created{};
    ConCon *content{};

    WorkspaceAttachTest() {
        global.x = &mockX;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        content = add(std::make_unique<ConCon>(nullptr, true));
    }

    template <typename T>
    T *add(std::unique_ptr<T> con) {
        T *raw = con.get();
        created.push_back(std::move(con));
        return raw;
    }

    /* Attaches a new workspace with the given number (-1 for a named one) to the content container. */
    WorkspaceCon *attach(int num) {
        auto *ws = add(std::make_unique<WorkspaceCon>(true));
        ws->num = num;
        ws->con_attach(content, false);
        return ws;
    }
};

TEST_F(WorkspaceAttachTest, InsertsBetweenTheFirstAndTheLast) {
    auto *one = attach(1);
    auto *five = attach(5);
    auto *three = attach(3);

    EXPECT_THAT(content->nodes, ElementsAre(one, three, five));
}

TEST_F(WorkspaceAttachTest, InsertsBeforeTheFirst) {
    auto *three = attach(3);
    auto *five = attach(5);
    auto *one = attach(1);

    EXPECT_THAT(content->nodes, ElementsAre(one, three, five));
}

TEST_F(WorkspaceAttachTest, AppendsAfterTheLast) {
    auto *one = attach(1);
    auto *three = attach(3);
    auto *five = attach(5);
    auto *seven = attach(7);

    EXPECT_THAT(content->nodes, ElementsAre(one, three, five, seven));
}

/* A workspace goes after the ones with the same number (upstream 6a530de2), but before larger numbers. */
TEST_F(WorkspaceAttachTest, InsertsAfterTheSameNumber) {
    auto *one = attach(1);
    auto *three = attach(3);
    auto *five = attach(5);
    auto *three_b = attach(3);
    auto *one_b = attach(1);
    auto *five_b = attach(5);

    EXPECT_THAT(content->nodes, ElementsAre(one, one_b, three, three_b, five, five_b));
}

/* Named workspaces are appended; numbered ones go before the first named one. */
TEST_F(WorkspaceAttachTest, KeepsNamedWorkspacesAfterNumberedOnes) {
    auto *one = attach(1);
    auto *foo = attach(-1);
    auto *five = attach(5);
    auto *bar = attach(-1);
    auto *three = attach(3);

    EXPECT_THAT(content->nodes, ElementsAre(one, three, five, foo, bar));
}

TEST_F(WorkspaceAttachTest, InsertsNumberedWorkspacesBeforeAFirstNamedOne) {
    auto *foo = attach(-1);
    auto *two = attach(2);
    auto *one = attach(1);
    auto *three = attach(3);

    EXPECT_THAT(content->nodes, ElementsAre(one, two, three, foo));
}
