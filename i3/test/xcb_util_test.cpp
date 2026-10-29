/*
 * i3's versions of the xcb-util (libxcb-util) helpers it used (xcb.cpp), against xcb-util itself, which this test links
 * and runs on the libxcb compatibility layer: get_screen() as xcb_aux_get_screen(), find_visual_by_attrs() as
 * xcb_aux_find_visual_by_attrs(), get_depth_of_visual() as xcb_aux_get_depth_of_visual() and atom_name_by_screen() as
 * xcb_atom_name_by_screen(). The screens are those of a fake X server's setup, and screens built by hand.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_aux.h>

import std;
import i3;
import xcb;
import xcb.xcbcompat;
import mocks;

namespace {
    /* Screen numbers around the setups' screens, negative ones, and ones past 8 bits. */
    constexpr std::array screen_numbers{-256, -2, -1, 0, 1, 2, 3, 4, 255, 256, std::numeric_limits<int>::max()};

    /* Classes and depths, as int8_t: -1 means any, and a value above 127 (200) is negative (-56). */
    constexpr std::array<int, 12> visual_classes{-128, -56, -2, -1, 0, 1, 2, 3, 4, 5, 6, 127};
    constexpr std::array<int, 11> depths{-128, -56, -2, -1, 0, 1, 8, 15, 24, 32, 127};

    struct canned_visual {
        std::uint32_t id;
        std::uint8_t visual_class;
    };

    struct canned_depth {
        std::uint8_t depth;
        std::vector<canned_visual> visuals;
    };

    /* A screen as a setup holds it, followed by its depths, each followed by its visuals. */
    class ScreenBytes {
      public:
        explicit ScreenBytes(std::vector<canned_depth> const &screen_depths) {
            xcb_screen_t screen{};
            screen.root = 0x100;
            screen.root_depth = 24;
            screen.allowed_depths_len = static_cast<std::uint8_t>(screen_depths.size());
            append(screen);
            for (auto const &[depth, visuals] : screen_depths) {
                xcb_depth_t d{};
                d.depth = depth;
                d.visuals_len = static_cast<std::uint16_t>(visuals.size());
                append(d);
                for (auto const &[id, visual_class] : visuals) {
                    xcb_visualtype_t v{};
                    v.visual_id = id;
                    v._class = visual_class;
                    append(v);
                }
            }
        }

        /* libxcb's view of the screen, which xcb-util reads. */
        [[nodiscard]] xcb_screen_t *libxcb() {
            return reinterpret_cast<xcb_screen_t *>(bytes_.data());
        }

        /* xcbpp's view of the same bytes, which i3's versions read. */
        [[nodiscard]] xcb::xproto::screen_t const &xcbpp() const {
            return *reinterpret_cast<xcb::xproto::screen_t const *>(bytes_.data());
        }

      private:
        template <typename T>
        void append(T const &value) {
            ASSERT_LE(size_ + sizeof value, bytes_.size());
            std::memcpy(bytes_.data() + size_, &value, sizeof value);
            size_ += sizeof value;
        }

        alignas(8) std::array<std::byte, 1024> bytes_{};
        std::size_t size_{0};
    };

    /*
     * Depths of 24 (TrueColor, DirectColor, TrueColor), 1 (no visuals), 8 (PseudoColor, StaticGray, GrayScale,
     * StaticColor), 32 (TrueColor, and 0x23 again: the depth of a visual is the first one that has it) and 200 (one
     * visual of class 200), which xcb-util compares as int8_t.
     */
    std::vector<canned_depth> several_depths() {
        return {
            {24, {{0x21, 4}, {0x22, 5}, {0x23, 4}}},
            {1, {}},
            {8, {{0x31, 3}, {0x32, 0}, {0x33, 1}, {0x34, 2}}},
            {32, {{0x41, 4}, {0x23, 4}}},
            {200, {{0x51, 200}}},
        };
    }

    struct free_deleter {
        void operator()(char *name) const noexcept {
            std::free(name);
        }
    };

    /* xcbpp's view of the screen that libxcb's @p screen is. */
    xcb::xproto::screen_t const &view(xcb_screen_t const *screen) {
        return *reinterpret_cast<xcb::xproto::screen_t const *>(screen);
    }

    /* xcbpp's screen_t, depth_t and visualtype_t walk @p screen's bytes as libxcb's iterators do: every depth and
     * visual is at the address libxcb finds it at. */
    void expect_same_walk(xcb_screen_t *screen) {
        auto depth_it = xcb_screen_allowed_depths_iterator(screen);
        for (auto const &depth : view(screen).get_allowed_depths()) {
            ASSERT_GT(depth_it.rem, 0);
            EXPECT_EQ(static_cast<void const *>(&depth), static_cast<void const *>(depth_it.data));
            auto visual_it = xcb_depth_visuals_iterator(depth_it.data);
            for (auto const &visual : depth.get_visuals()) {
                ASSERT_GT(visual_it.rem, 0);
                EXPECT_EQ(static_cast<void const *>(&visual), static_cast<void const *>(visual_it.data));
                xcb_visualtype_next(&visual_it);
            }
            EXPECT_EQ(visual_it.rem, 0);
            xcb_depth_next(&depth_it);
        }
        EXPECT_EQ(depth_it.rem, 0);
    }
}  // namespace

/* A fake X server's setup with a vendor string and a number of screens. */
struct ScreenSetup {
    std::string vendor;
    std::uint8_t screens;
};

class GetScreenTest : public testing::TestWithParam<ScreenSetup> {};

/*
 * get_screen() finds the screen xcb_aux_get_screen() finds, and nullptr for a number the setup has no screen of. The
 * vendor strings' lengths aren't all multiples of four: the screens come after the vendor's padding.
 */
TEST_P(GetScreenTest, FindsTheScreenXcbAuxGetScreenFinds) {
    FakeXServer server{{.vendor = GetParam().vendor, .screens = GetParam().screens}};
    /* After the server: the server outlives the client's connection. */
    auto const handle = xcb::xcbcompat::adopt(server.connect());

    for (int const screen : screen_numbers) {
        SCOPED_TRACE(std::format("screen {}", screen));
        xcb_screen_t *const found = get_screen(handle.get(), screen);
        EXPECT_EQ(found, xcb_aux_get_screen(handle.get(), screen));
        if (screen >= 0 && screen < GetParam().screens) {
            ASSERT_NE(found, nullptr);
            EXPECT_EQ(found->root, FakeXServer::root_window + static_cast<std::uint32_t>(screen));
        } else {
            EXPECT_EQ(found, nullptr);
        }
    }
    /* xcb_aux_get_screen() decrements the number once per screen, which overflows here. */
    EXPECT_EQ(get_screen(handle.get(), std::numeric_limits<int>::min()), nullptr);
}

INSTANTIATE_TEST_SUITE_P(Vendors, GetScreenTest,
                         testing::Values(ScreenSetup{"i3 fake X server", 1}, ScreenSetup{"i3 fake X server", 3},
                                         ScreenSetup{"i3 fake X server!", 3}, ScreenSetup{"i3 fake X server!!", 2},
                                         ScreenSetup{"i3!", 3}, ScreenSetup{"", 2}),
                         [](testing::TestParamInfo<ScreenSetup> const &info) {
                             return std::format("Vendor{}Screens{}", info.param.vendor.size(), info.param.screens);
                         });

/* find_visual_by_attrs() finds the visual xcb_aux_find_visual_by_attrs() finds, for every class and depth. */
TEST(FindVisualByAttrsTest, FindsTheVisualXcbAuxFindVisualByAttrsFinds) {
    ScreenBytes screen{several_depths()};
    for (int const visual_class : visual_classes) {
        for (int const depth : depths) {
            auto const c = static_cast<std::int8_t>(visual_class);
            auto const d = static_cast<std::int8_t>(depth);
            EXPECT_EQ(static_cast<void const *>(find_visual_by_attrs(screen.xcbpp(), c, d)),
                      static_cast<void const *>(xcb_aux_find_visual_by_attrs(screen.libxcb(), c, d)))
                << "class " << visual_class << ", depth " << depth;
        }
    }
}

/* The first visual in the setup's order that matches, -1 matching any class or depth. */
TEST(FindVisualByAttrsTest, FindsTheFirstMatchingVisual) {
    ScreenBytes screen{several_depths()};
    auto const id = [&screen](int visual_class, int depth) -> std::optional<std::uint32_t> {
        auto const *visual = find_visual_by_attrs(screen.xcbpp(), static_cast<std::int8_t>(visual_class), static_cast<std::int8_t>(depth));
        return visual ? std::optional{visual->visual_id} : std::nullopt;
    };
    EXPECT_EQ(id(-1, -1), 0x21U);
    EXPECT_EQ(id(5, -1), 0x22U);
    EXPECT_EQ(id(4, 24), 0x21U);
    EXPECT_EQ(id(-1, 8), 0x31U);
    EXPECT_EQ(id(0, -1), 0x32U);
    EXPECT_EQ(id(2, 8), 0x34U);
    EXPECT_EQ(id(4, 32), 0x41U);
    EXPECT_EQ(id(-1, 1), std::nullopt) << "depth 1 has no visual";
    EXPECT_EQ(id(5, 8), std::nullopt);
    EXPECT_EQ(id(-1, 16), std::nullopt);
    /* 200 is -56 as an int8_t, and no class or depth is negative. */
    EXPECT_EQ(id(200, -1), std::nullopt);
    EXPECT_EQ(id(-1, 200), std::nullopt);
}

/* get_depth_of_visual() gives the depth xcb_aux_get_depth_of_visual() gives, 0 for a visual the screen hasn't. */
TEST(GetDepthOfVisualTest, IsXcbAuxGetDepthOfVisual) {
    ScreenBytes screen{several_depths()};
    for (std::uint32_t const id : {0x0U, 0x21U, 0x22U, 0x23U, 0x31U, 0x32U, 0x33U, 0x34U, 0x41U, 0x51U, 0x99U, 0xffffffffU}) {
        EXPECT_EQ(get_depth_of_visual(screen.xcbpp(), id), xcb_aux_get_depth_of_visual(screen.libxcb(), id)) << std::format("{:#x}", id);
    }
    EXPECT_EQ(get_depth_of_visual(screen.xcbpp(), 0x23), 24) << "the first depth that has the visual";
    EXPECT_EQ(get_depth_of_visual(screen.xcbpp(), 0x51), 200);
    EXPECT_EQ(get_depth_of_visual(screen.xcbpp(), 0x99), 0);
}

/* A screen without depths has neither visuals nor depths. */
TEST(GetDepthOfVisualTest, AScreenWithoutDepths) {
    ScreenBytes screen{{}};
    EXPECT_EQ(find_visual_by_attrs(screen.xcbpp(), -1, -1), nullptr);
    EXPECT_EQ(xcb_aux_find_visual_by_attrs(screen.libxcb(), -1, -1), nullptr);
    EXPECT_EQ(get_depth_of_visual(screen.xcbpp(), 0x21), 0);
    EXPECT_EQ(xcb_aux_get_depth_of_visual(screen.libxcb(), 0x21), 0);
}

/* xcbpp walks the depths and visuals at libxcb's addresses: those of the screens built here, and those of the fake X
 * server's screens, after vendor strings of every length modulo four. */
TEST(SetupViewTest, XcbppFindsTheDepthsAndVisualsWhereLibxcbFindsThem) {
    ScreenBytes several{several_depths()};
    expect_same_walk(several.libxcb());
    ScreenBytes none{{}};
    expect_same_walk(none.libxcb());

    for (std::string const vendor : {"i3 fake X server", "i3 fake X server!", "i3 fake X server!!", "i3!"}) {
        SCOPED_TRACE(vendor);
        FakeXServer server{{.argb_visual = true, .vendor = vendor, .screens = 2}};
        auto const handle = xcb::xcbcompat::adopt(server.connect());
        for (int const screen : {0, 1}) {
            xcb_screen_t *const found = get_screen(handle.get(), screen);
            ASSERT_NE(found, nullptr);
            expect_same_walk(found);
        }
    }
}

/* X takes the 32-bit visual xcb-util finds, and its depth, as it took them from xcb-util before. */
TEST(SetupViewTest, XUsesTheVisualXcbUtilFinds) {
    FakeXServer server{{.argb_visual = true}};
    FakeX x{server};

    auto *const visual = xcb_aux_find_visual_by_attrs(x.root_screen, -1, 32);
    ASSERT_NE(visual, nullptr);
    EXPECT_EQ(x.visual_type, visual);
    EXPECT_EQ(x.root_depth, xcb_aux_get_depth_of_visual(x.root_screen, visual->visual_id));
    EXPECT_EQ(x.root_screen, xcb_aux_get_screen(x.c_handle(), x.conn_screen));
}

/* Without a 32-bit visual, xcb-util finds none, and X takes the root visual. */
TEST(SetupViewTest, WithoutA32BitVisualXUsesTheRootVisual) {
    FakeXServer server{};
    FakeX x{server};

    EXPECT_EQ(xcb_aux_find_visual_by_attrs(x.root_screen, -1, 32), nullptr);
    EXPECT_EQ(find_visual_by_attrs(view(x.root_screen), -1, 32), nullptr);
    EXPECT_EQ(x.visual_type, get_visualtype(x.root_screen));
    EXPECT_EQ(x.root_depth, x.root_screen->root_depth);
}

/* atom_name_by_screen() makes the name xcb_atom_name_by_screen() makes, for every screen number. */
TEST(AtomNameByScreenTest, IsXcbAtomNameByScreen) {
    for (int screen = 0; screen <= std::numeric_limits<std::uint8_t>::max(); ++screen) {
        auto const number = static_cast<std::uint8_t>(screen);
        for (char const *base : {"WM", "_NET_SYSTEM_TRAY", ""}) {
            std::unique_ptr<char, free_deleter> const name{xcb_atom_name_by_screen(base, number)};
            ASSERT_NE(name, nullptr);
            EXPECT_EQ(atom_name_by_screen(base, number), name.get());
        }
    }
    EXPECT_EQ(atom_name_by_screen("WM", 0), "WM_S0");
    EXPECT_EQ(atom_name_by_screen("WM", 12), "WM_S12");
    EXPECT_EQ(atom_name_by_screen("WM", 255), "WM_S255");
}
