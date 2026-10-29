/*
 * The requests x.cpp sends, on a fake X server (fake_x_server.cppm).
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unistd.h>
#include <xcb/xcb.h>

import std;
import i3;
import xcb;
import xcb.proto.shape;
import rect;
import mocks;

using testing::ElementsAre;
using testing::ElementsAreArray;
using testing::IsEmpty;

namespace {
    /* Core opcodes. */
    constexpr std::uint8_t CreateWindow = 1;
    constexpr std::uint8_t ChangeWindowAttributes = 2;
    constexpr std::uint8_t DestroyWindow = 4;
    constexpr std::uint8_t ReparentWindow = 7;
    constexpr std::uint8_t MapWindow = 8;
    constexpr std::uint8_t UnmapWindow = 10;
    constexpr std::uint8_t ConfigureWindow = 12;
    constexpr std::uint8_t ChangeProperty = 18;
    constexpr std::uint8_t GetProperty = 20;
    constexpr std::uint8_t SendEvent = 25;
    constexpr std::uint8_t QueryPointer = 38;
    constexpr std::uint8_t WarpPointer = 41;
    constexpr std::uint8_t SetInputFocus = 42;
    constexpr std::uint8_t CreatePixmap = 53;
    constexpr std::uint8_t FreePixmap = 54;
    constexpr std::uint8_t ChangeGC = 56;
    constexpr std::uint8_t CreateColormap = 78;
    constexpr std::uint8_t FreeColormap = 79;
    constexpr std::uint8_t KillClient = 113;

    constexpr std::uint8_t shape_opcode = 129;
    /* SHAPE's minor opcodes. */
    constexpr std::uint8_t ShapeRectangles = 1;
    constexpr std::uint8_t ShapeMask = 2;
    constexpr std::uint8_t ShapeCombine = 3;

    /* The requests with one of the given major opcodes. */
    std::vector<FakeRequest> only(std::vector<FakeRequest> const &requests, std::initializer_list<std::uint8_t> majors) {
        std::vector<FakeRequest> result{};
        std::ranges::copy_if(requests, std::back_inserter(result),
                             [&](FakeRequest const &request) { return std::ranges::contains(majors, request.major()); });
        return result;
    }

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }

    /* A ChangeWindowAttributes request: the window, the value mask and the values. */
    struct Attributes {
        std::uint32_t window;
        std::uint32_t mask;
        std::vector<std::uint32_t> values;
        bool operator==(Attributes const &) const = default;
    };

    Attributes attributes(FakeRequest const &request) {
        return {request.u32(4), request.u32(8), request.u32s(12)};
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

    /* Rectangles (x, y, width and height) from @p offset to the end of a request. */
    std::vector<std::array<int, 4>> rectangles_of(FakeRequest const &request, std::size_t offset) {
        std::vector<std::array<int, 4>> rectangles{};
        for (std::size_t at = offset; at + 8 <= request.bytes.size(); at += 8) {
            rectangles.push_back({request.i16(at), request.i16(at + 2), request.u16(at + 4), request.u16(at + 6)});
        }
        return rectangles;
    }

    /* A server with SHAPE 1.1. */
    FakeXServerOptions with_shape() {
        return {.extensions = {{.name = "SHAPE",
                                .major_opcode = shape_opcode,
                                .first_event = 64,
                                .first_error = 128,
                                .handler = [](FakeRequest const &request) {
                                    if (request.minor() == 0) { /* ShapeQueryVersion */
                                        return FakeXServer::reply(request, 0, ByteWriter{}.u16(1).u16(1).take());
                                    }
                                    return std::vector<std::byte>{};
                                }}}};
    }

    constexpr std::uint32_t frame_mask_without_enter = FRAME_EVENT_MASK & ~std::uint32_t{XCB_EVENT_MASK_ENTER_WINDOW};
}  // namespace

/*
 * i3's X on a fake server with SHAPE 1.1, its atoms interned, and a configuration. Each test starts with no request
 * recorded.
 */
class XTest : public testing::Test {
  protected:
    FakeXServer server{with_shape()};
    /* After the server: the server outlives the client's connection. */
    FakeX x{server};
    ConfigurationManager configManager{x};
    std::vector<std::unique_ptr<Con>> created{};

    XTest() {
        global.x = &x;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        setup_atoms(x);
        std::ignore = sent();
    }

    ~XTest() override {
        /* The containers' frames and the X state of the containers, before the connection. */
        for (auto &con : created) {
            if (con->frame) {
                x_con_kill(con.get());
            }
        }
        created.clear();
        global.x = nullptr;
        global.configManager = nullptr;
    }

    /* The requests sent since the last call. */
    std::vector<FakeRequest> sent() {
        return server.take_requests(x.connection());
    }

    template <typename T, typename... Args>
    T *make(Args &&...args) {
        auto con = std::make_unique<T>(std::forward<Args>(args)...);
        T *raw = con.get();
        created.push_back(std::move(con));
        return raw;
    }

    static xcb_atom_t atom(i3::Atom atom) {
        return i3::atoms[atom];
    }
};

TEST_F(XTest, ConInitCreatesTheFrame) {
    auto *con = make<ConCon>(nullptr, false);
    ASSERT_TRUE(con->frame);
    auto const requests = sent();

    auto const created_windows = only(requests, {CreateWindow});
    ASSERT_EQ(created_windows.size(), 1U);
    auto const &create = created_windows[0];
    EXPECT_EQ(create.minor(), 24); /* depth */
    EXPECT_EQ(create.u32(4), con->frame->id);
    EXPECT_EQ(create.u32(8), FakeXServer::root_window);
    EXPECT_EQ(create.i16(12), -15);
    EXPECT_EQ(create.i16(14), -15);
    EXPECT_EQ(create.u16(16), 10);
    EXPECT_EQ(create.u16(18), 10);
    EXPECT_EQ(create.u16(22), XCB_WINDOW_CLASS_INPUT_OUTPUT);
    EXPECT_EQ(create.u32(24), FakeXServer::root_visual);
    /* BackPixel, BorderPixel, OverrideRedirect, EventMask and Colormap. */
    EXPECT_EQ(create.u32(28), 0x2a0aU);
    EXPECT_THAT(create.u32s(32), ElementsAre(FakeXServer::black_pixel, FakeXServer::black_pixel, 1U, frame_mask_without_enter,
                                             FakeXServer::default_colormap));
    EXPECT_THAT(only(requests, {CreateColormap}), IsEmpty());

    auto const properties = only(requests, {ChangeProperty});
    ASSERT_EQ(properties.size(), 1U);
    EXPECT_EQ(property(properties[0]), (Property{XCB_PROP_MODE_REPLACE, con->frame->id, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8,
                                                 std::string{"i3-frame\0i3-frame\0", 18}}));
    EXPECT_THAT(server.unanswered(), IsEmpty());
}

TEST_F(XTest, ConInitCreatesAColormapForAnotherDepth) {
    auto *con = make<ConCon>(nullptr, true);
    con->depth = 32;
    x.con_init(con);
    auto const requests = sent();

    auto const colormaps = only(requests, {CreateColormap});
    ASSERT_EQ(colormaps.size(), 1U);
    EXPECT_EQ(colormaps[0].minor(), XCB_COLORMAP_ALLOC_NONE);
    EXPECT_EQ(colormaps[0].u32(4), con->colormap);
    EXPECT_EQ(colormaps[0].u32(8), FakeXServer::root_window);
    EXPECT_NE(con->colormap, XCB_NONE);
    /* The server has no 32-bit visual: get_visualid_by_depth() finds none. */
    EXPECT_EQ(colormaps[0].u32(12), 0U);
    auto const created_windows = only(requests, {CreateWindow});
    ASSERT_EQ(created_windows.size(), 1U);
    EXPECT_EQ(created_windows[0].u32s(32).back(), con->colormap);
}

TEST_F(XTest, ConKillFreesTheColormapThePixmapAndTheFrame) {
    auto *con = make<ConCon>(nullptr, true);
    con->depth = 32;
    x.con_init(con);
    xcb_window_t const frame = con->frame->id;
    xcb_colormap_t const colormap = con->colormap;
    xcb_pixmap_t const pixmap = x.connection().generate_id();
    con->frame_buffer = std::make_unique<surface_t>(x.drawing(), pixmap, nullptr, 10, 10);
    std::ignore = sent();

    x_con_kill(con);
    auto const requests = only(sent(), {FreeColormap, FreePixmap, DestroyWindow});

    ASSERT_THAT(majors(requests), ElementsAre(FreeColormap, FreePixmap, DestroyWindow));
    EXPECT_EQ(requests[0].u32(4), colormap);
    EXPECT_EQ(requests[1].u32(4), pixmap);
    EXPECT_EQ(requests[2].u32(4), frame);
    EXPECT_FALSE(con->frame);
    EXPECT_FALSE(con->frame_buffer);
}

TEST_F(XTest, WindowKillDestroysAWindowWithoutDeleteWindow) {
    xcb_window_t const window = 0x500000;
    x_window_kill(*global.x, window, kill_window_t::KILL_WINDOW);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(GetProperty, DestroyWindow));
    EXPECT_EQ(requests[0].u32(4), window);
    EXPECT_EQ(requests[0].u32(8), atom(i3::Atom::WM_PROTOCOLS));
    EXPECT_EQ(requests[1].u32(4), window);
}

TEST_F(XTest, WindowKillKillsTheClient) {
    xcb_window_t const window = 0x500000;
    x_window_kill(*global.x, window, kill_window_t::KILL_CLIENT);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(GetProperty, KillClient));
    EXPECT_EQ(requests[1].u32(4), window);
}

TEST_F(XTest, WindowKillSendsDeleteWindow) {
    xcb_window_t const window = 0x500000;
    std::array<std::uint32_t, 2> const protocols{atom(i3::Atom::WM_TAKE_FOCUS), atom(i3::Atom::WM_DELETE_WINDOW)};
    server.set_property(window, atom(i3::Atom::WM_PROTOCOLS), XCB_ATOM_ATOM, 32, std::as_bytes(std::span{protocols}));
    x_window_kill(*global.x, window, kill_window_t::KILL_WINDOW);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(GetProperty, SendEvent));
    auto const &send = requests[1];
    EXPECT_EQ(send.minor(), 0); /* propagate */
    EXPECT_EQ(send.u32(4), window);
    EXPECT_EQ(send.u32(8), XCB_EVENT_MASK_NO_EVENT);
    EXPECT_EQ(send.u8(12), XCB_CLIENT_MESSAGE);
    EXPECT_EQ(send.u8(13), 32); /* format */
    EXPECT_EQ(send.u32(16), window);
    EXPECT_EQ(send.u32(20), atom(i3::Atom::WM_PROTOCOLS));
    EXPECT_THAT(send.u32s(24), ElementsAre(atom(i3::Atom::WM_DELETE_WINDOW), XCB_CURRENT_TIME, 0U, 0U, 0U));
}

TEST_F(XTest, SetI3AtomsSetsTheRootWindowsProperties) {
    std::string const saved_socketpath = global.current_socketpath;
    global.current_socketpath = "/run/i3/ipc-socket.1234";
    global.configManager->config->current_configpath = "/etc/i3/config";
    x_set_i3_atoms();
    auto const requests = sent();
    global.current_socketpath = saved_socketpath;

    ASSERT_THAT(majors(requests), ElementsAre(ChangeProperty, ChangeProperty, ChangeProperty));
    EXPECT_EQ(property(requests[0]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::I3_SOCKET_PATH),
                                               atom(i3::Atom::UTF8_STRING), 8, "/run/i3/ipc-socket.1234"}));
    EXPECT_EQ(property(requests[1]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::I3_PID),
                                               XCB_ATOM_CARDINAL, 32, uint32s({static_cast<std::uint32_t>(getpid())})}));
    EXPECT_EQ(property(requests[2]), (Property{XCB_PROP_MODE_REPLACE, FakeXServer::root_window, atom(i3::Atom::I3_CONFIG_PATH),
                                               atom(i3::Atom::UTF8_STRING), 8, "/etc/i3/config"}));
}

TEST_F(XTest, SetRootCursor) {
    x.xcursor_set_root_cursor(XCURSOR_CURSOR_WATCH);
    auto const requests = sent();

    ASSERT_EQ(requests.size(), 1U);
    EXPECT_EQ(requests[0].major(), ChangeWindowAttributes);
    EXPECT_EQ(attributes(requests[0]), (Attributes{FakeXServer::root_window, XCB_CW_CURSOR, {FakeX::first_cursor + XCURSOR_CURSOR_WATCH}}));
}

/* x_mask_event_mask() changes the event mask of the mapped frames, from the bottom of the stack to the top. */
TEST_F(XTest, MaskEventMaskChangesTheMappedFrames) {
    auto *a = make<ConCon>(nullptr, false);
    auto *b = make<ConCon>(nullptr, false);
    auto *c = make<ConCon>(nullptr, false);
    for (con_state *state : x.state_head) {
        state->mapped = state->id != b->frame->id;
    }
    std::ignore = sent();

    x_mask_event_mask(~std::uint32_t{XCB_EVENT_MASK_ENTER_WINDOW});
    auto const requests = sent();

    /* con_init() puts a new frame on top of the stack (the front of state_head). */
    ASSERT_EQ(requests.size(), 2U);
    EXPECT_EQ(attributes(requests[0]), (Attributes{a->frame->id, XCB_CW_EVENT_MASK, {frame_mask_without_enter}}));
    EXPECT_EQ(attributes(requests[1]), (Attributes{c->frame->id, XCB_CW_EVENT_MASK, {frame_mask_without_enter}}));
}

/*
 * A floating container's frame takes the shape of its window and of its borders (x_set_shape()), and loses it when the
 * window's shape goes.
 */
TEST_F(XTest, ShapesAFloatingFrame) {
    auto *window = new i3Window{0x500000};
    window->depth = 24;
    auto *con = make<ConCon>(window, false);
    con->floating = FLOATING_USER_ON;
    con->border_style = border_style_t::BS_PIXEL;
    con->border_width = 2;
    con->rect = Rect{10, 20, 100, 80};
    con->get_window_rect() = Rect{2, 2, 96, 76};
    std::ignore = sent();

    x_set_shape(con, xcb::shape::shape_sk_t::SHAPE_SK_BOUNDING, true);
    auto const shaped = only(sent(), {shape_opcode});
    EXPECT_TRUE(window->shaped);
    ASSERT_THAT(majors(shaped), ElementsAre(shape_opcode, shape_opcode));
    auto const &combine = shaped[0];
    EXPECT_EQ(combine.minor(), ShapeCombine);
    EXPECT_EQ(combine.u8(4), std::to_underlying(xcb::shape::shape_so_t::SHAPE_SO_SET));
    EXPECT_EQ(combine.u8(5), std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_BOUNDING));
    EXPECT_EQ(combine.u8(6), std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_BOUNDING));
    EXPECT_EQ(combine.u32(8), con->frame->id);
    EXPECT_EQ(combine.i16(12), 4);
    EXPECT_EQ(combine.i16(14), 4);
    EXPECT_EQ(combine.u32(16), window->id);
    auto const &rectangles = shaped[1];
    EXPECT_EQ(rectangles.minor(), ShapeRectangles);
    EXPECT_EQ(rectangles.u8(4), std::to_underlying(xcb::shape::shape_so_t::SHAPE_SO_UNION));
    EXPECT_EQ(rectangles.u8(5), std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_BOUNDING));
    EXPECT_EQ(rectangles.u8(6), XCB_CLIP_ORDERING_UNSORTED);
    EXPECT_EQ(rectangles.u32(8), con->frame->id);
    EXPECT_EQ(rectangles.i16(12), 0);
    EXPECT_EQ(rectangles.i16(14), 0);
    /* The left, right, bottom and top borders, two pixels wide: x, y, width and height. */
    EXPECT_THAT(rectangles_of(rectangles, 16), ElementsAre(std::array{0, 0, 2, 80}, std::array{98, 0, 2, 80},
                                                           std::array{2, 78, 96, 2}, std::array{2, 0, 96, 2}));

    x_set_shape(con, xcb::shape::shape_sk_t::SHAPE_SK_INPUT, false);
    auto const unshaped = only(sent(), {shape_opcode});
    EXPECT_FALSE(window->input_shaped);
    ASSERT_THAT(majors(unshaped), ElementsAre(shape_opcode));
    EXPECT_EQ(unshaped[0].minor(), ShapeMask);
    EXPECT_EQ(unshaped[0].u8(4), std::to_underlying(xcb::shape::shape_so_t::SHAPE_SO_SET));
    EXPECT_EQ(unshaped[0].u8(5), std::to_underlying(xcb::shape::shape_sk_t::SHAPE_SK_INPUT));
    EXPECT_EQ(unshaped[0].u32(8), con->frame->id);
    EXPECT_EQ(unshaped[0].u32(16), XCB_PIXMAP_NONE);
}
