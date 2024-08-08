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

TEST(ConTest, Floating) {
    global.x = new MockX{};
    global.configManager->config = std::make_unique<Config>();
    ConCon x{nullptr, true};
    FloatingCon f{};

    x.con_attach(&f, true, nullptr);
    x.floating = FLOATING_USER_ON;

    auto *floating = x.con_inside_floating();

    ASSERT_EQ(floating, &f);
    
    delete global.x;
}
