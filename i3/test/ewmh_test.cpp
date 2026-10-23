/*
 * The EWMH properties i3 sets (ewmh.cpp), on a fake X server (fake_x_server.cppm).
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import std;
import i3;
import xcb;
import rect;
import mocks;

using testing::ElementsAre;
using testing::IsEmpty;

namespace {
    constexpr std::uint8_t CreateWindow = 1;
    constexpr std::uint8_t MapWindow = 8;
    constexpr std::uint8_t ConfigureWindow = 12;
    constexpr std::uint8_t ChangeProperty = 18;
    constexpr std::uint8_t DeleteProperty = 19;

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }

    /* A ChangeProperty request. */
    struct Property {
        std::uint8_t mode;
        std::uint32_t window;
        std::uint32_t property;
        std::uint32_t type;
        std::uint8_t format;
        std::string data;
        bool operator==(Property const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, Property const &property) {
            return out << std::format("mode {} window {:#x} property {} type {} format {} data of {} bytes", property.mode,
                                      property.window, property.property, property.type, property.format, property.data.size());
        }
    };

    Property property(FakeRequest const &request) {
        std::size_t const n = request.u32(20) * std::size_t{request.u8(16) / 8U};
        return {request.minor(), request.u32(4), request.u32(8), request.u32(12), request.u8(16), request.string(24, n)};
    }

    std::string uint32s(std::initializer_list<std::uint32_t> values) {
        std::string data(values.size() * 4, '\0');
        std::memcpy(data.data(), std::data(values), data.size());
        return data;
    }

    std::vector<std::uint32_t> uint32s_of(std::vector<std::byte> const &bytes) {
        std::vector<std::uint32_t> values(bytes.size() / 4);
        std::memcpy(values.data(), bytes.data(), values.size() * 4);
        return values;
    }

    std::vector<std::byte> bytes_of(std::string_view text) {
        auto const bytes = std::as_bytes(std::span{text});
        return {bytes.begin(), bytes.end()};
    }
}  // namespace

/*
 * i3's X on a fake server, its atoms interned, and a tree of skeleton containers: root, and an output (at x 100,
 * y 20) with its content container. Each test starts with no request recorded.
 */
class EwmhTest : public testing::Test {
  protected:
    FakeXServer server{};
    /* After the server: the server outlives the client's connection. */
    FakeX x{server};
    ConfigurationManager configManager{x};
    std::vector<Con *> created{};
    RootCon *root{};
    OutputCon *output{};
    ConCon *content{};
    RootCon *saved_croot{global.croot};

    EwmhTest() {
        global.x = &x;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        global.focused = nullptr;

        root = add(new RootCon{true}, nullptr);
        global.croot = root;
        output = add(new OutputCon{true}, root);
        output->rect = Rect{100, 20, 1280, 800};
        content = add(new ConCon{nullptr, true}, output);
        setup_atoms(x);
        std::ignore = sent();
    }

    ~EwmhTest() override {
        for (Con *con : created | std::views::reverse) {
            delete con;
        }
        global.croot = saved_croot;
        global.focused = nullptr;
        global.x = nullptr;
        global.configManager = nullptr;
    }

    /* Attaches con as the last child of parent (if any). */
    template <typename T>
    T *add(T *con, Con *parent) {
        created.push_back(con);
        if (parent != nullptr) {
            con->con_attach(parent, true);
        }
        return con;
    }

    WorkspaceCon *workspace(std::string name) {
        auto *ws = new WorkspaceCon{true};
        ws->name = std::move(name);
        return add(ws, content);
    }

    /* A container with an X window, @p id, in @p parent. */
    ConCon *window(xcb_window_t id, Con *parent) {
        return add(new ConCon{new i3Window{id}, true}, parent);
    }

    /* The requests sent since the last call. */
    std::vector<FakeRequest> sent() {
        return server.take_requests(x.connection());
    }

    static xcb_atom_t atom(i3::Atom atom) {
        return i3::atoms[atom];
    }

    [[nodiscard]] std::vector<std::uint32_t> root_property(i3::Atom name) const {
        return uint32s_of(server.property(FakeXServer::root_window, i3::atoms[name]).value_or(std::vector<std::byte>{}));
    }
};

/*
 * ewmh_setup_hints(): the support window, an InputOnly window at (-1, -1), 1x1, override-redirect, with
 * _NET_SUPPORTING_WM_CHECK and _NET_WM_NAME; the same two and _NET_SUPPORTED on the root window; the window mapped and
 * put below its siblings.
 */
TEST_F(EwmhTest, SetupHintsCreatesTheSupportingWindow) {
    ewmh_setup_hints();
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(CreateWindow, ChangeProperty, ChangeProperty, ChangeProperty, ChangeProperty, ChangeProperty,
                                              MapWindow, ConfigureWindow));
    xcb_window_t const window = x.ewmh_window;
    auto const &create = requests[0];
    EXPECT_EQ(create.minor(), XCB_COPY_FROM_PARENT); /* depth */
    EXPECT_EQ(create.u32(4), window);
    EXPECT_EQ(create.u32(8), FakeXServer::root_window);
    EXPECT_EQ(create.i16(12), -1);
    EXPECT_EQ(create.i16(14), -1);
    EXPECT_EQ(create.u16(16), 1);
    EXPECT_EQ(create.u16(18), 1);
    EXPECT_EQ(create.u16(20), 0); /* border width */
    EXPECT_EQ(create.u16(22), XCB_WINDOW_CLASS_INPUT_ONLY);
    EXPECT_EQ(create.u32(24), std::uint32_t{XCB_COPY_FROM_PARENT}); /* visual */
    EXPECT_EQ(create.u32(28), std::uint32_t{XCB_CW_OVERRIDE_REDIRECT});
    EXPECT_THAT(create.u32s(32), ElementsAre(1U));

    std::uint32_t const wm_check = atom(i3::Atom::_NET_SUPPORTING_WM_CHECK);
    std::uint32_t const wm_name = atom(i3::Atom::_NET_WM_NAME);
    std::uint32_t const utf8 = atom(i3::Atom::UTF8_STRING);
    EXPECT_EQ(property(requests[1]), (Property{XCB_PROP_MODE_REPLACE, window, wm_check, XCB_ATOM_WINDOW, 32, uint32s({window})}));
    EXPECT_EQ(property(requests[2]), (Property{XCB_PROP_MODE_REPLACE, window, wm_name, utf8, 8, "i3"}));
    EXPECT_EQ(property(requests[3]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, wm_check, XCB_ATOM_WINDOW, 32, uint32s({window})}));
    EXPECT_EQ(property(requests[4]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, wm_name, utf8, 8, "i3"}));
    auto const supported = property(requests[5]);
    EXPECT_EQ(supported.window, FakeXServer::root_window);
    EXPECT_EQ(supported.property, atom(i3::Atom::_NET_SUPPORTED));
    EXPECT_EQ(supported.type, std::uint32_t{XCB_ATOM_ATOM});
    EXPECT_EQ(supported.format, 32);
    EXPECT_EQ(supported.data.size(), 35U * 4);
    EXPECT_EQ(supported.data.substr(0, 8), uint32s({atom(i3::Atom::_NET_SUPPORTED), wm_check}));
    EXPECT_EQ(supported.data.substr(std::size_t{34} * 4), uint32s({atom(i3::Atom::_NET_MOVERESIZE_WINDOW)}));

    EXPECT_EQ(requests[6].u32(4), window);
    EXPECT_EQ(requests[7].u32(4), window);
    EXPECT_EQ(requests[7].u16(8), XCB_CONFIG_WINDOW_STACK_MODE);
    EXPECT_THAT(requests[7].u32s(12), ElementsAre(std::uint32_t{XCB_STACK_MODE_BELOW}));
    EXPECT_THAT(server.unanswered(), IsEmpty());
}

/*
 * ewmh_update_desktop_properties(): _NET_NUMBER_OF_DESKTOPS, _NET_DESKTOP_VIEWPORT (the output's position for each
 * workspace), _NET_CURRENT_DESKTOP (the focused workspace's index), _NET_DESKTOP_NAMES (null-terminated names) and the
 * windows' _NET_WM_DESKTOP. The number and the current desktop are sent only when they change (a static cache, so the
 * test changes both between its two calls).
 */
TEST_F(EwmhTest, UpdateDesktopProperties) {
    auto *one = workspace("1");
    auto *two = workspace("2");
    global.focused = one;
    ewmh_update_desktop_properties();
    std::ignore = sent();

    EXPECT_THAT(root_property(i3::Atom::_NET_DESKTOP_VIEWPORT), ElementsAre(100U, 20U, 100U, 20U));
    EXPECT_EQ(server.property(FakeXServer::root_window, atom(i3::Atom::_NET_DESKTOP_NAMES)), bytes_of(std::string_view{"1\0" "2\0", 4}));

    auto *three = workspace("three");
    auto *first = window(0x500000, two);
    auto *second = window(0x600000, three);
    global.focused = two;
    ewmh_update_desktop_properties();
    auto const requests = sent();

    EXPECT_THAT(root_property(i3::Atom::_NET_NUMBER_OF_DESKTOPS), ElementsAre(3U));
    EXPECT_THAT(root_property(i3::Atom::_NET_CURRENT_DESKTOP), ElementsAre(1U));
    EXPECT_THAT(root_property(i3::Atom::_NET_DESKTOP_VIEWPORT), ElementsAre(100U, 20U, 100U, 20U, 100U, 20U));
    EXPECT_EQ(server.property(FakeXServer::root_window, atom(i3::Atom::_NET_DESKTOP_NAMES)),
              bytes_of(std::string_view{"1\0" "2\0" "three\0", 10}));
    EXPECT_THAT(uint32s_of(server.property(0x500000, atom(i3::Atom::_NET_WM_DESKTOP)).value_or(std::vector<std::byte>{})), ElementsAre(1U));
    EXPECT_THAT(uint32s_of(server.property(0x600000, atom(i3::Atom::_NET_WM_DESKTOP)).value_or(std::vector<std::byte>{})), ElementsAre(2U));
    EXPECT_EQ(first->get_window()->wm_desktop, 1U);
    EXPECT_EQ(second->get_window()->wm_desktop, 2U);
    for (auto const &request : requests) {
        ASSERT_EQ(request.major(), ChangeProperty);
        EXPECT_EQ(request.minor(), XCB_PROP_MODE_REPLACE);
    }

    /* The windows' _NET_WM_DESKTOP is cached on the window: unchanged, it isn't sent again. */
    ewmh_update_wm_desktop();
    EXPECT_THAT(sent(), IsEmpty());
}

TEST_F(EwmhTest, UpdateActiveWindow) {
    ewmh_update_active_window(0x500000);

    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(ChangeProperty));
    EXPECT_EQ(property(requests[0]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::_NET_ACTIVE_WINDOW),
                                               XCB_ATOM_WINDOW, 32, uint32s({0x500000})}));
}

TEST_F(EwmhTest, UpdateVisibleNameSetsOrDeletesIt) {
    ewmh_update_visible_name(0x500000, "visible name");
    ewmh_update_visible_name(0x500000, nullptr);

    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(ChangeProperty, DeleteProperty));
    EXPECT_EQ(property(requests[0]), (Property{XCB_PROP_MODE_REPLACE, 0x500000, atom(i3::Atom::_NET_WM_VISIBLE_NAME),
                                               atom(i3::Atom::UTF8_STRING), 8, "visible name"}));
    EXPECT_EQ(requests[1].u32(4), 0x500000U);
    EXPECT_EQ(requests[1].u32(8), atom(i3::Atom::_NET_WM_VISIBLE_NAME));
}

TEST_F(EwmhTest, UpdateWorkareaDeletesIt) {
    ewmh_update_workarea();

    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(DeleteProperty));
    EXPECT_EQ(requests[0].u32(4), FakeXServer::root_window);
    EXPECT_EQ(requests[0].u32(8), atom(i3::Atom::_NET_WORKAREA));
}

TEST_F(EwmhTest, UpdateClientLists) {
    std::array<xcb_window_t, 3> windows{0x500000, 0x600000, 0x700000};
    ewmh_update_client_list(windows.data(), 3);
    ewmh_update_client_list_stacking(windows.data(), 2);
    ewmh_update_client_list(nullptr, 0);

    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(ChangeProperty, ChangeProperty, ChangeProperty));
    EXPECT_EQ(property(requests[0]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::_NET_CLIENT_LIST),
                                               XCB_ATOM_WINDOW, 32, uint32s({0x500000, 0x600000, 0x700000})}));
    EXPECT_EQ(property(requests[1]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::_NET_CLIENT_LIST_STACKING),
                                               XCB_ATOM_WINDOW, 32, uint32s({0x500000, 0x600000})}));
    EXPECT_EQ(property(requests[2]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::_NET_CLIENT_LIST),
                                               XCB_ATOM_WINDOW, 32, ""}));
}
