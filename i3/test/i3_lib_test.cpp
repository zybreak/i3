/*
 * The requests of the X code in i3-lib.cppm (sync_respond(), root_atom_contents(), is_background_set()), on a fake X
 * server (fake_x_server.cppm), and the ignore list's sequence comparison (sequence_matches()).
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import std;
import i3;
import xcb;
import mocks;

using testing::ElementsAre;
using testing::IsEmpty;

namespace {
    constexpr std::uint8_t CreateWindow = 1;
    constexpr std::uint8_t GetWindowAttributes = 3;
    constexpr std::uint8_t DestroyWindow = 4;
    constexpr std::uint8_t MapWindow = 8;
    constexpr std::uint8_t GetGeometry = 14;
    constexpr std::uint8_t QueryTree = 15;
    constexpr std::uint8_t InternAtom = 16;
    constexpr std::uint8_t GetProperty = 20;
    constexpr std::uint8_t SendEvent = 25;
    constexpr std::uint8_t GetInputFocus = 43;
    constexpr std::uint8_t ClearArea = 61;
    constexpr std::uint8_t GetImage = 73;

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }

    std::span<std::byte const> bytes_of(std::string_view text) {
        return std::as_bytes(std::span{text});
    }
}  // namespace

class I3LibTest : public testing::Test {
  protected:
    FakeXServer server{};
    FakeX x{server};

    I3LibTest() {
        global.x = &x;
        setup_atoms(x);
        std::ignore = sent();
    }

    ~I3LibTest() override {
        global.x = nullptr;
    }

    std::vector<FakeRequest> sent() {
        return server.take_requests(x.connection());
    }

    /* What root_atom_contents() takes as the connection to use. */
    xcb::Connection *provided_connection() {
        return &x.connection();
    }
};

/* The i3 sync protocol: a ClientMessage I3_SYNC with the window and the random value, sent to the window. */
TEST_F(I3LibTest, SyncRespondSendsTheValueBack) {
    xcb_window_t const window = 0x500000;
    sync_respond(&x, window, 0x12345678);
    auto const requests = sent();

    ASSERT_THAT(majors(requests), ElementsAre(SendEvent));
    auto const &send = requests[0];
    EXPECT_EQ(send.minor(), 0); /* propagate */
    EXPECT_EQ(send.u32(4), window);
    EXPECT_EQ(send.u32(8), XCB_EVENT_MASK_NO_EVENT);
    EXPECT_EQ(send.u8(12), XCB_CLIENT_MESSAGE);
    EXPECT_EQ(send.u8(13), 32);
    EXPECT_EQ(send.u32(16), window);
    EXPECT_EQ(send.u32(20), i3::atoms[i3::Atom::I3_SYNC]);
    EXPECT_THAT(send.u32s(24), ElementsAre(window, 0x12345678U, 0U, 0U, 0U));
}

/* root_atom_contents() on a connection it is given: InternAtom, then GetProperty of 256 words on the root window. */
TEST_F(I3LibTest, RootAtomContentsReadsAString) {
    server.set_property(FakeXServer::root_window, server.atom("I3_SOCKET_PATH"), server.atom("UTF8_STRING"), 8, bytes_of("/run/user/1000/i3/ipc"));

    auto const contents = root_atom_contents("I3_SOCKET_PATH", provided_connection(), 0);

    EXPECT_EQ(contents, "/run/user/1000/i3/ipc");
    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(InternAtom, GetProperty));
    EXPECT_EQ(requests[0].minor(), 0); /* only_if_exists */
    EXPECT_EQ(requests[0].string(8, requests[0].u16(4)), "I3_SOCKET_PATH");
    EXPECT_EQ(requests[1].u32(4), FakeXServer::root_window);
    EXPECT_EQ(requests[1].u32(8), server.atom("I3_SOCKET_PATH"));
    EXPECT_EQ(requests[1].u32(12), XCB_GET_PROPERTY_TYPE_ANY);
    EXPECT_EQ(requests[1].u32(16), 0U);
    EXPECT_EQ(requests[1].u32(20), 256U);
}

/* A value longer than 256 words is read again, whole. */
TEST_F(I3LibTest, RootAtomContentsReadsALongStringAgain) {
    std::string const path(1500, 'p');
    server.set_property(FakeXServer::root_window, server.atom("I3_CONFIG_PATH"), server.atom("UTF8_STRING"), 8, bytes_of(path));

    auto const contents = root_atom_contents("I3_CONFIG_PATH", provided_connection(), 0);

    EXPECT_EQ(contents, path);
    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(InternAtom, GetProperty, GetProperty));
    /* 1500 bytes: 1024 read, 476 after, which is 119 words more. */
    EXPECT_EQ(requests[2].u32(20), 256U + 119U);
}

TEST_F(I3LibTest, RootAtomContentsReadsACardinal) {
    std::uint32_t const pid = 4242;
    server.set_property(FakeXServer::root_window, server.atom("I3_PID"), XCB_ATOM_CARDINAL, 32, std::as_bytes(std::span{&pid, 1}));

    EXPECT_EQ(root_atom_contents("I3_PID", provided_connection(), 0), "4242");
}

/*
 * is_background_set() looks for a pixel of the root window that no viewable child covers (QueryTree, then the
 * geometry and attributes of each child), and maps a black and then a white window there, reading the pixel back
 * after each: the same pixel both times means that something draws the root window's background.
 */
TEST_F(I3LibTest, IsBackgroundSetFlickersAnUnobscuredPixel) {
    std::uint32_t const covering = 0x200001;
    std::uint32_t const unmapped = 0x200002;
    server.on(QueryTree, [&](FakeRequest const &request) {
        return FakeXServer::reply(request, 0, ByteWriter{}.u32(FakeXServer::root_window).u32(0).u16(2).zeros(14).u32(covering).u32(unmapped).take());
    });
    server.on(GetGeometry, [&](FakeRequest const &request) {
        /* The covering child takes the left 100 pixels. */
        return FakeXServer::reply(request, 24, ByteWriter{}.u32(FakeXServer::root_window).u16(0).u16(0).u16(100).u16(800).u16(0).take());
    });
    server.on(GetWindowAttributes, [&](FakeRequest const &request) {
        std::uint8_t const map_state = request.u32(4) == covering ? XCB_MAP_STATE_VIEWABLE : XCB_MAP_STATE_UNMAPPED;
        return FakeXServer::reply(request, 0, ByteWriter{}.u32(FakeXServer::root_visual).u16(1).u8(0).u8(1).u32(0).u32(0).u8(0).u8(1).u8(map_state).u8(0).u32(FakeXServer::default_colormap).zeros(12).take());
    });

    EXPECT_TRUE(is_background_set(x, x.root_screen));

    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(QueryTree, GetGeometry, GetWindowAttributes, GetGeometry, GetWindowAttributes,
                                              CreateWindow, MapWindow, ClearArea, GetInputFocus, DestroyWindow, GetImage,
                                              CreateWindow, MapWindow, ClearArea, GetInputFocus, DestroyWindow, GetImage));
    EXPECT_EQ(requests[0].u32(4), FakeXServer::root_window);
    EXPECT_EQ(requests[1].u32(4), covering);
    EXPECT_EQ(requests[2].u32(4), covering);
    EXPECT_EQ(requests[3].u32(4), unmapped);
    EXPECT_EQ(requests[4].u32(4), unmapped);
    for (std::size_t const create : {5U, 11U}) {
        EXPECT_EQ(requests[create].i16(12), 100) << create;
        EXPECT_EQ(requests[create].i16(14), 0) << create;
    }
    EXPECT_THAT(requests[5].u32s(32), ElementsAre(FakeXServer::black_pixel, 1U));
    EXPECT_THAT(requests[11].u32s(32), ElementsAre(FakeXServer::white_pixel, 1U));
    EXPECT_EQ(requests[10].i16(8), 100);
    EXPECT_EQ(requests[10].i16(10), 0);
    EXPECT_THAT(server.unanswered(), IsEmpty());
}

/*
 * The ignore list keeps 64-bit request numbers and compares them with the full_sequence of an event or error, the low
 * 32 bits of the number of the last request the server had processed. Past 65536 requests the event's 16-bit sequence
 * has wrapped: request 0x10005's events carry full_sequence 0x10005 and sequence 5, which i3 compared before, and which
 * matched no request number from there on.
 */
TEST(SequenceMatchesTest, TheFullSequenceIsComparedPastTheSixteenBitWrap) {
    EXPECT_TRUE(sequence_matches(5, 5));
    EXPECT_FALSE(sequence_matches(5, 6));
    EXPECT_TRUE(sequence_matches(0xffff, 0xffff));
    EXPECT_TRUE(sequence_matches(0x10000, 0x10000));
    EXPECT_FALSE(sequence_matches(0x10000, 0));
    EXPECT_TRUE(sequence_matches(0x10005, 0x10005));
    EXPECT_FALSE(sequence_matches(0x10005, 5)) << "the 16-bit sequence of request 0x10005's events";
}

/* Past 2^32 requests the full_sequence wraps too: the low 32 bits of the request number are what it is compared with. */
TEST(SequenceMatchesTest, TheLow32BitsAreComparedPastTheirWrap) {
    EXPECT_TRUE(sequence_matches(0xffff'ffff, 0xffff'ffff));
    EXPECT_FALSE(sequence_matches(0xffff'ffff, 0));
    EXPECT_TRUE(sequence_matches(0x1'0000'0000, 0));
    EXPECT_FALSE(sequence_matches(0x1'0000'0000, 0xffff'ffff));
    EXPECT_TRUE(sequence_matches(0x1'0000'0005, 5));
    EXPECT_FALSE(sequence_matches(0x1'0000'0005, 6));
    EXPECT_TRUE(sequence_matches(0x2'0001'0005, 0x1'0005));
}
