#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import i3;
import mocks;

void foo(bool &out) {
    out = !out;
}

TEST(manage_test, OutputParam) {
   bool bar = true;
   foo(bar);
   
   ASSERT_FALSE(bar);
}
