#include <gtest/gtest.h>
#include "con.h"
#include "global.h"

TEST(TreeTest, foo) {
    auto t = Tree<std::string>();

    auto i = t.set_head("hej");
    auto c = t.append_child(i, "child");
    auto cc = t.append_child(c, "child-child");
    auto c2 = t.append_child(i, "child2");

    ASSERT_TRUE(c.node == cc.node->parent);

    for (auto &foo : t) {
        std::cout << foo << std::endl;
    }

    auto breath_first_begin = t.begin_breadth_first();
    auto breath_first_end = t.end_breadth_first();

    for (;breath_first_begin != breath_first_end; breath_first_begin++) {
        std::cout << *breath_first_begin << std::endl;
    }

    std::cout << "----" << std::endl;

    for (auto it = i.begin(); it != i.end(); it++) {
        std::cout << *it << std::endl;
    }

    auto found = std::find(t.begin(), t.end(), "hej");

    ASSERT_TRUE(found != t.end());
    ASSERT_TRUE(found == i);
}
