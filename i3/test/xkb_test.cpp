/*
 * The X requests of xkb.cpp that don't go through libxcb-xkb, on a fake X server (fake_x_server.cppm), which has no
 * XKEYBOARD: i3's Xkb then sends no XKB request.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

import std;
import i3;
import xcb;
import mocks;

using testing::ElementsAre;

namespace {
    constexpr std::uint8_t GetKeyboardMapping = 101;
    constexpr std::uint8_t GetModifierMapping = 119;

    constexpr std::uint32_t num_lock = 0xff7f;
    constexpr std::uint8_t num_lock_keycode = 77;

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }
}  // namespace

class XkbTest : public testing::Test {
  protected:
    FakeXServer server{};
    FakeX x{server};

    XkbTest() {
        /* One keysym per keycode: Num_Lock on keycode 77, NoSymbol on the others. */
        server.on(GetKeyboardMapping, [](FakeRequest const &request) {
            ByteWriter body{};
            body.zeros(24);
            for (std::uint8_t i = 0; i < request.u8(5); ++i) {
                body.u32(request.u8(4) + i == num_lock_keycode ? num_lock : 0);
            }
            return FakeXServer::reply(request, 1, body.take());
        });
        std::ignore = server.take_requests(x.connection());
    }

    /* A modifier mapping of two keycodes per modifier (Shift, Lock, Control, Mod1 … Mod5), with @p mod_keycodes. */
    void modifier_mapping(std::array<std::uint8_t, 16> mod_keycodes) {
        server.on(GetModifierMapping, [mod_keycodes](FakeRequest const &request) {
            ByteWriter body{};
            body.zeros(24);
            for (auto const keycode : mod_keycodes) {
                body.u8(keycode);
            }
            return FakeXServer::reply(request, 2, body.take());
        });
    }
};

/* get_numlock_mask(): the mask of the modifier that has a keycode of Num_Lock (GetModifierMapping, one round trip). */
TEST_F(XkbTest, NumlockMaskIsTheModifierOfNumLock) {
    /* Mod2 (bit 4) has keycodes 76 and 77. */
    modifier_mapping({50, 62, 66, 0, 37, 105, 64, 108, 76, num_lock_keycode, 0, 0, 0, 0, 0, 0});
    Xkb xkb{x};
    std::ignore = server.take_requests(x.connection());

    EXPECT_EQ(xkb.get_numlock_mask(), 0x10U /* Mod2 */);
    /* xcb-keysyms asks for the keyboard mapping when its table is made, before the modifier mapping is. */
    EXPECT_THAT(majors(server.take_requests(x.connection())), ElementsAre(GetKeyboardMapping, GetModifierMapping));
}

TEST_F(XkbTest, NumlockMaskIsZeroWithoutAModifier) {
    modifier_mapping({50, 62, 66, 0, 37, 105, 64, 108, 0, 0, 0, 0, 0, 0, 0, 0});
    Xkb xkb{x};

    EXPECT_EQ(xkb.get_numlock_mask(), 0U);
}

/* A failed GetModifierMapping gives no mask. */
TEST_F(XkbTest, NumlockMaskIsZeroIfTheRequestFails) {
    server.on(GetModifierMapping, [](FakeRequest const &request) { return FakeXServer::error(request, 17 /* Implementation */); });
    Xkb xkb{x};

    EXPECT_EQ(xkb.get_numlock_mask(), 0U);
}
