/*
 * The requests of i3's X helpers in xcb.cpp, on a fake X server (fake_x_server.cppm).
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
    constexpr std::uint8_t ChangeWindowAttributes = 2;
    constexpr std::uint8_t MapWindow = 8;
    constexpr std::uint8_t ChangeProperty = 18;
    constexpr std::uint8_t GetProperty = 20;
    constexpr std::uint8_t SendEvent = 25;
    constexpr std::uint8_t GrabServer = 36;
    constexpr std::uint8_t UngrabServer = 37;
    constexpr std::uint8_t GetInputFocus = 43;

    constexpr std::uint8_t bad_match = 8;
    constexpr std::uint8_t bad_window = 3;

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }

    std::vector<std::byte> atoms_of(std::initializer_list<std::uint32_t> atoms) {
        auto const bytes = std::as_bytes(std::span{atoms.begin(), atoms.size()});
        return {bytes.begin(), bytes.end()};
    }
}  // namespace

/*
 * i3's X on a fake server, its atoms interned, and a configuration. Each test starts with no request recorded.
 */
class XcbTest : public testing::Test {
  protected:
    FakeXServer server{};
    /* After the server: the server outlives the client's connection. */
    FakeX x{server};
    ConfigurationManager configManager{x};

    XcbTest() {
        global.x = &x;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        setup_atoms(x);
        std::ignore = sent();
    }

    ~XcbTest() override {
        global.x = nullptr;
        global.configManager = nullptr;
    }

    /* The requests sent since the last call. */
    std::vector<FakeRequest> sent() {
        return server.take_requests(x.connection());
    }

    static xcb_atom_t atom(i3::Atom atom) {
        return i3::atoms[atom];
    }
};

/*
 * create_window(): CreateWindow, unchecked and followed by a round trip, the cursor, and MapWindow if asked. The values
 * go out in the order of the mask's bits.
 */
TEST_F(XcbTest, CreateWindowCreatesSetsTheCursorAndMaps) {
    auto const values = xcb::xproto::CreateWindowValueList{}
                            .with_background_pixel(0x123456)
                            .with_override_redirect(1)
                            .with_event_mask(xcb::xproto::event_mask_t::exposure);
    xcb_window_t const window = create_window(x.connection(), Rect{10, 20, 300, 200}, 24, FakeXServer::root_visual, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                              XCURSOR_CURSOR_MOVE, true, values);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(CreateWindow, GetInputFocus, ChangeWindowAttributes, MapWindow));
    auto const &create = requests[0];
    EXPECT_EQ(create.minor(), 24);
    EXPECT_EQ(create.u32(4), window);
    EXPECT_EQ(create.u32(8), FakeXServer::root_window);
    EXPECT_EQ(create.i16(12), 10);
    EXPECT_EQ(create.i16(14), 20);
    EXPECT_EQ(create.u16(16), 300);
    EXPECT_EQ(create.u16(18), 200);
    EXPECT_EQ(create.u16(20), 0); /* border width */
    EXPECT_EQ(create.u16(22), XCB_WINDOW_CLASS_INPUT_OUTPUT);
    EXPECT_EQ(create.u32(24), FakeXServer::root_visual);
    EXPECT_EQ(create.u32(28), 0xa02U); /* BackPixel, OverrideRedirect, EventMask */
    EXPECT_THAT(create.u32s(32), ElementsAre(0x123456U, 1U, std::uint32_t{XCB_EVENT_MASK_EXPOSURE}));
    EXPECT_EQ(requests[2].u32(4), window);
    EXPECT_EQ(requests[2].u32(8), std::uint32_t{XCB_CW_CURSOR});
    EXPECT_THAT(requests[2].u32s(12), ElementsAre(FakeX::first_cursor + XCURSOR_CURSOR_MOVE));
    EXPECT_EQ(requests[3].u32(4), window);
}

/* An InputOnly window takes its depth and visual from its parent, and isn't mapped unless asked. */
TEST_F(XcbTest, CreateWindowInputOnly) {
    std::ignore = create_window(x.connection(), Rect{0, 0, 1280, 800}, 24, FakeXServer::root_visual, XCB_WINDOW_CLASS_INPUT_ONLY,
                                XCURSOR_CURSOR_POINTER, false, xcb::xproto::CreateWindowValueList{}.with_override_redirect(1));
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(CreateWindow, GetInputFocus, ChangeWindowAttributes));
    EXPECT_EQ(requests[0].minor(), XCB_COPY_FROM_PARENT);
    EXPECT_EQ(requests[0].u16(22), XCB_WINDOW_CLASS_INPUT_ONLY);
    EXPECT_EQ(requests[0].u32(24), std::uint32_t{XCB_COPY_FROM_PARENT});
    EXPECT_EQ(requests[0].u32(28), std::uint32_t{XCB_CW_OVERRIDE_REDIRECT});
    EXPECT_THAT(requests[0].u32s(32), ElementsAre(1U));
}

/* CreateWindow is unchecked: its error goes to the event loop, and create_window() goes on. */
TEST_F(XcbTest, CreateWindowErrorGoesToTheEventLoop) {
    server.on(CreateWindow, [](FakeRequest const &request) { return FakeXServer::error(request, bad_match); });

    std::ignore = create_window(x.connection(), Rect{0, 0, 10, 10}, 24, FakeXServer::root_visual, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                XCURSOR_CURSOR_POINTER, true, xcb::xproto::CreateWindowValueList{});
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(CreateWindow, GetInputFocus, ChangeWindowAttributes, MapWindow));
    auto const error = x.connection().poll_for_queued_event();
    ASSERT_NE(error, nullptr);
    EXPECT_EQ(error->response_type, 0);
    auto const *generic = reinterpret_cast<xcb::generic_error_t const *>(error.get());
    EXPECT_EQ(generic->error_code, bad_match);
    EXPECT_EQ(generic->major_code, CreateWindow);
    EXPECT_EQ(generic->sequence, static_cast<std::uint16_t>(requests[0].sequence));
    EXPECT_EQ(x.connection().poll_for_queued_event(), nullptr);
}

/*
 * On another connection (restore_layout.cpp's placeholder connection), the requests go there; the parent and the
 * cursor are still i3's.
 */
TEST_F(XcbTest, CreateWindowOnAnotherConnection) {
    FakeXServer other_server{};
    auto other = other_server.connect();

    std::ignore = create_window(*other, Rect{0, 0, 10, 10}, 24, FakeXServer::root_visual, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                XCURSOR_CURSOR_WATCH, true, xcb::xproto::CreateWindowValueList{});
    auto const requests = other_server.take_requests(*other);

    EXPECT_THAT(sent(), IsEmpty());
    ASSERT_THAT(majors(requests), ElementsAre(CreateWindow, GetInputFocus, ChangeWindowAttributes, MapWindow));
    EXPECT_EQ(requests[0].u32(8), x.root);
    EXPECT_THAT(requests[2].u32s(12), ElementsAre(FakeX::first_cursor + XCURSOR_CURSOR_WATCH));
}

/* A synthetic ConfigureNotify with the window's absolute position: the event's 32 bytes, the last 4 zeros. */
TEST_F(XcbTest, FakeAbsoluteConfigureNotify) {
    auto *window = new i3Window{0x500000};
    ConCon con{window, true};
    con.rect = Rect{100, 50, 400, 300};
    con.get_window_rect() = Rect{2, 20, 396, 278};
    con.border_width = 2;
    std::ignore = sent();

    fake_absolute_configure_notify(&con);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(SendEvent));
    auto const &send = requests[0];
    ASSERT_EQ(send.bytes.size(), 44U);
    EXPECT_EQ(send.minor(), 0); /* propagate */
    EXPECT_EQ(send.u32(4), window->id);
    EXPECT_EQ(send.u32(8), std::uint32_t{XCB_EVENT_MASK_STRUCTURE_NOTIFY});
    EXPECT_EQ(send.u8(12), XCB_CONFIGURE_NOTIFY);
    EXPECT_EQ(send.u32(16), window->id); /* event */
    EXPECT_EQ(send.u32(20), window->id); /* window */
    EXPECT_EQ(send.u32(24), std::uint32_t{XCB_NONE}); /* above sibling */
    EXPECT_EQ(send.i16(28), 102);
    EXPECT_EQ(send.i16(30), 70);
    EXPECT_EQ(send.u16(32), 396);
    EXPECT_EQ(send.u16(34), 278);
    EXPECT_EQ(send.u16(36), 2);
    EXPECT_EQ(send.u8(38), 0); /* override redirect */
    EXPECT_EQ(send.u8(39), 0);
    EXPECT_EQ(send.u32(40), 0U);
}

/* A container without a window gets none. */
TEST_F(XcbTest, FakeAbsoluteConfigureNotifyWithoutAWindow) {
    ConCon con{nullptr, true};
    std::ignore = sent();

    fake_absolute_configure_notify(&con);

    EXPECT_THAT(sent(), IsEmpty());
}

TEST_F(XcbTest, SendTakeFocus) {
    send_take_focus(0x500000, 1234);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(SendEvent));
    auto const &send = requests[0];
    EXPECT_EQ(send.minor(), 0); /* propagate */
    EXPECT_EQ(send.u32(4), 0x500000U);
    EXPECT_EQ(send.u32(8), std::uint32_t{XCB_EVENT_MASK_NO_EVENT});
    EXPECT_EQ(send.u8(12), XCB_CLIENT_MESSAGE);
    EXPECT_EQ(send.u8(13), 32);
    EXPECT_EQ(send.u32(16), 0x500000U);
    EXPECT_EQ(send.u32(20), atom(i3::Atom::WM_PROTOCOLS));
    EXPECT_THAT(send.u32s(24), ElementsAre(atom(i3::Atom::WM_TAKE_FOCUS), 1234U, 0U, 0U, 0U));
}

TEST_F(XcbTest, AddPropertyAtomAppends) {
    xcb_add_property_atom(x.connection(), 0x500000, atom(i3::Atom::_NET_WM_STATE), atom(i3::Atom::_NET_WM_STATE_STICKY));
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(ChangeProperty));
    EXPECT_EQ(requests[0].minor(), XCB_PROP_MODE_APPEND);
    EXPECT_EQ(requests[0].u32(4), 0x500000U);
    EXPECT_EQ(requests[0].u32(8), atom(i3::Atom::_NET_WM_STATE));
    EXPECT_EQ(requests[0].u32(12), std::uint32_t{XCB_ATOM_ATOM});
    EXPECT_EQ(requests[0].u8(16), 32);
    EXPECT_EQ(requests[0].u32(20), 1U); /* one element */
    EXPECT_THAT(requests[0].u32s(24), ElementsAre(atom(i3::Atom::_NET_WM_STATE_STICKY)));
}

/* xcb_remove_property_atom() reads the property and writes it back without the atom, with the server grabbed. */
TEST_F(XcbTest, RemovePropertyAtomRewritesTheProperty) {
    xcb_window_t const window = 0x500000;
    server.set_property(window, atom(i3::Atom::_NET_WM_STATE), XCB_ATOM_ATOM, 32,
                        atoms_of({atom(i3::Atom::_NET_WM_STATE_STICKY), atom(i3::Atom::_NET_WM_STATE_FOCUSED), atom(i3::Atom::_NET_WM_STATE_HIDDEN)}));

    xcb_remove_property_atom(x.connection(), window, atom(i3::Atom::_NET_WM_STATE), atom(i3::Atom::_NET_WM_STATE_FOCUSED));
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(GrabServer, GetProperty, ChangeProperty, UngrabServer));
    EXPECT_EQ(requests[1].minor(), 0); /* delete */
    EXPECT_EQ(requests[1].u32(4), window);
    EXPECT_EQ(requests[1].u32(8), atom(i3::Atom::_NET_WM_STATE));
    EXPECT_EQ(requests[1].u32(12), std::uint32_t{XCB_GET_PROPERTY_TYPE_ANY});
    EXPECT_EQ(requests[1].u32(16), 0U);
    EXPECT_EQ(requests[1].u32(20), 4096U);
    EXPECT_EQ(requests[2].minor(), XCB_PROP_MODE_REPLACE);
    EXPECT_EQ(requests[2].u32(4), window);
    EXPECT_EQ(requests[2].u32(8), atom(i3::Atom::_NET_WM_STATE));
    EXPECT_EQ(requests[2].u32(12), std::uint32_t{XCB_ATOM_ATOM});
    EXPECT_EQ(requests[2].u8(16), 32);
    EXPECT_EQ(server.property(window, atom(i3::Atom::_NET_WM_STATE)),
              atoms_of({atom(i3::Atom::_NET_WM_STATE_STICKY), atom(i3::Atom::_NET_WM_STATE_HIDDEN)}));
}

/* Removing the last atom leaves the property, empty. */
TEST_F(XcbTest, RemovePropertyAtomLeavesAnEmptyProperty) {
    xcb_window_t const window = 0x500000;
    server.set_property(window, atom(i3::Atom::_NET_WM_STATE), XCB_ATOM_ATOM, 32, atoms_of({atom(i3::Atom::_NET_WM_STATE_FOCUSED)}));

    xcb_remove_property_atom(x.connection(), window, atom(i3::Atom::_NET_WM_STATE), atom(i3::Atom::_NET_WM_STATE_FOCUSED));
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(GrabServer, GetProperty, ChangeProperty, UngrabServer));
    EXPECT_EQ(requests[2].u32(20), 0U);
}

TEST_F(XcbTest, RemovePropertyAtomFromAMissingProperty) {
    xcb_remove_property_atom(x.connection(), 0x500000, atom(i3::Atom::_NET_WM_STATE), atom(i3::Atom::_NET_WM_STATE_FOCUSED));

    EXPECT_THAT(majors(sent()), ElementsAre(GrabServer, GetProperty, UngrabServer));
    EXPECT_EQ(server.property(0x500000, atom(i3::Atom::_NET_WM_STATE)), std::nullopt);
}

/* GetProperty is checked: its error (a window that is gone) is the caller's, which drops it; the event loop doesn't
 * see it. */
TEST_F(XcbTest, RemovePropertyAtomOfAWindowThatIsGone) {
    server.on(GetProperty, [](FakeRequest const &request) { return FakeXServer::error(request, bad_window, request.u32(4)); });

    xcb_remove_property_atom(x.connection(), 0x500000, atom(i3::Atom::_NET_WM_STATE), atom(i3::Atom::_NET_WM_STATE_FOCUSED));

    EXPECT_THAT(majors(sent()), ElementsAre(GrabServer, GetProperty, UngrabServer));
    EXPECT_EQ(x.connection().poll_for_queued_event(), nullptr);
}

/* The first visual of the first depth with visuals that matches; 0 if none. */
TEST(XcbVisualTest, GetVisualidByDepth) {
    FakeXServer server{{.argb_visual = true}};
    FakeX x{server};
    global.x = &x;

    EXPECT_EQ(get_visualid_by_depth(24), FakeXServer::root_visual);
    EXPECT_EQ(get_visualid_by_depth(32), FakeXServer::argb_visual);
    EXPECT_EQ(get_visualid_by_depth(1), 0U); /* a depth without visuals */
    EXPECT_EQ(get_visualid_by_depth(8), 0U);
    global.x = nullptr;
}
