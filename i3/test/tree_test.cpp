#include <gtest/gtest.h>
#include <gmock/gmock.h>

import std;
import i3;
import mocks;

using testing::ElementsAre;
using testing::IsEmpty;

/*
 * A tree of skeleton containers (no X windows) like the one i3 builds: root, output, content and one workspace
 * (splith). Containers are created on the heap, because tree_close_internal() deletes the ones it closes; the
 * fixture deletes the rest.
 */
class TreeTest : public testing::Test {
  protected:
    MockX mockX{};
    ConfigurationManager configManager{mockX};
    std::vector<Con *> created{};
    RootCon *root{};
    WorkspaceCon *ws{};

    TreeTest() {
        global.x = &mockX;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        global.focused = nullptr;

        root = add(new RootCon{true}, nullptr);
        root->layout = L_SPLITH;
        auto *output = add(new OutputCon{true}, root);
        auto *content = add(new ConCon{nullptr, true}, output);
        content->layout = L_SPLITH;
        ws = add(new WorkspaceCon{true}, content);
        ws->layout = L_SPLITH;
    }

    ~TreeTest() override {
        /* The containers the code under test closed are no longer in global.all_cons. */
        for (Con *con : created | std::views::reverse) {
            if (std::ranges::contains(global.all_cons, con)) {
                delete con;
            }
        }
        global.focused = nullptr;
    }

    /* Attaches con as the last child of parent (if any), the way a container is opened. */
    template <typename T>
    T *add(T *con, Con *parent) {
        created.push_back(con);
        if (parent != nullptr) {
            con->con_attach(parent, true);
        }
        return con;
    }

    ConCon *split(Con *parent, layout_t layout) {
        auto *con = add(new ConCon{nullptr, true}, parent);
        con->layout = layout;
        return con;
    }

    ConCon *leaf(Con *parent) {
        return add(new ConCon{nullptr, true}, parent);
    }

    static bool exists(Con *con) {
        return std::ranges::contains(global.all_cons, con);
    }
};

/*
 * Test 145's first case: moving a window up and then right leaves a redundant pair of split containers on a splith
 * workspace, next to another window. Flattening moves the pair's children to the workspace, before the pair, while
 * tree_flatten() is iterating over the workspace's children.
 */
TEST_F(TreeTest, FlattenMovesGrandchildrenToTheContainerItIsIteratingOver) {
    auto *x = split(ws, L_SPLITV);
    auto *y = split(x, L_SPLITH);
    auto *a = leaf(y);
    auto *b = leaf(y);
    auto *c = leaf(ws);

    tree_flatten(root);

    EXPECT_THAT(ws->nodes, ElementsAre(a, b, c));
    EXPECT_EQ(a->parent, ws);
    EXPECT_EQ(b->parent, ws);
    /* The pair was focused on the workspace, so its focused child is now. */
    EXPECT_THAT(ws->focused, ElementsAre(a, c, b));
    EXPECT_FALSE(exists(x));
    EXPECT_FALSE(exists(y));
}

/* After flattening a container, the iteration goes on with the container that followed it. */
TEST_F(TreeTest, FlattenVisitsTheContainersAfterAFlattenedOne) {
    auto *x = split(ws, L_SPLITV);
    auto *y = split(x, L_SPLITH);
    auto *a = leaf(y);
    auto *b = leaf(y);
    auto *n = split(ws, L_SPLITV);
    auto *z = split(n, L_SPLITH);
    auto *c = leaf(z);
    auto *d = leaf(z);

    tree_flatten(root);

    EXPECT_THAT(ws->nodes, ElementsAre(a, b, c, d));
    EXPECT_THAT(ws->focused, ElementsAre(a, b, c, d));
    for (Con *con : {x, y, n, z}) {
        EXPECT_FALSE(exists(con));
    }
}

/*
 * Like upstream's TAILQ_FOREACH_SAFE loop, one pass doesn't visit the containers that flattening moved before the
 * flattened one: a redundant pair among them is flattened by the next pass.
 */
TEST_F(TreeTest, FlattenLeavesTheMovedContainersToTheNextPass) {
    auto *x = split(ws, L_SPLITV);
    auto *y = split(x, L_SPLITH);
    auto *p = split(y, L_SPLITV);
    auto *q = split(p, L_SPLITH);
    auto *e = leaf(q);
    auto *f = leaf(q);
    auto *g = leaf(y);

    tree_flatten(root);

    EXPECT_THAT(ws->nodes, ElementsAre(p, g));
    EXPECT_THAT(p->nodes, ElementsAre(q));

    tree_flatten(root);

    EXPECT_THAT(ws->nodes, ElementsAre(e, f, g));
    EXPECT_FALSE(exists(p));
    EXPECT_FALSE(exists(q));
}

/* Closing a container closes each of its children, each of which removes itself from the container's nodes. */
TEST_F(TreeTest, CloseInternalClosesEveryChild) {
    auto *s = split(ws, L_SPLITH);
    std::array children{leaf(s), leaf(s), leaf(s)};

    EXPECT_TRUE(tree_close_internal(s, kill_window_t::DONT_KILL_WINDOW, true));

    EXPECT_THAT(ws->nodes, IsEmpty());
    EXPECT_FALSE(exists(s));
    for (Con *child : children) {
        EXPECT_FALSE(exists(child));
    }
}

/* Like upstream, level_down() refuses only when the focused container's focus stack is empty. */
TEST_F(TreeTest, LevelDownFocusesTheOnlyChild) {
    auto *s = split(ws, L_SPLITV);
    auto *a = leaf(s);
    global.focused = s;

    EXPECT_TRUE(level_down());

    EXPECT_EQ(global.focused, a);
}

TEST_F(TreeTest, LevelDownFocusesTheFirstChildInTheFocusStack) {
    auto *s = split(ws, L_SPLITV);
    leaf(s);
    auto *b = leaf(s);
    leaf(s);
    b->con_focus();
    global.focused = s;

    EXPECT_TRUE(level_down());

    EXPECT_EQ(global.focused, b);
}

TEST_F(TreeTest, LevelDownRefusesWithoutChildren) {
    auto *a = leaf(ws);
    global.focused = a;

    EXPECT_FALSE(level_down());

    EXPECT_EQ(global.focused, a);
}

/* A floating container is skipped: level_down() focuses its child. */
TEST_F(TreeTest, LevelDownFocusesTheOnlyChildOfAFloatingContainer) {
    leaf(ws);
    auto *f = add(new FloatingCon{true}, ws);
    auto *w = leaf(f);
    f->con_focus();
    global.focused = ws;

    EXPECT_TRUE(level_down());

    EXPECT_EQ(global.focused, w);
}

TEST_F(TreeTest, LevelDownRefusesAFloatingContainerWithoutChildren) {
    leaf(ws);
    auto *f = add(new FloatingCon{true}, ws);
    f->con_focus();
    global.focused = ws;

    EXPECT_FALSE(level_down());

    EXPECT_EQ(global.focused, ws);
}
