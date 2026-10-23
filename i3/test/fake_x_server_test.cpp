/*
 * The fake X server itself (fake_x_server.cppm), and i3's X set up on it.
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

TEST(FakeXServerTest, AnswersTheSetupAndRecordsRequests) {
    FakeXServer server{};
    auto connection = server.connect();

    auto const *setup = xcb::get_setup(*connection);
    ASSERT_NE(setup, nullptr);
    ASSERT_EQ(setup->roots_length(), 1U);
    auto const &screen = setup->get_roots()[0];
    EXPECT_EQ(screen.root, FakeXServer::root_window);
    EXPECT_EQ(screen.width_in_pixels, 1280);
    EXPECT_EQ(screen.root_depth, 24);

    xcb::xproto::map_window(connection.get(), 0x1234);
    auto const atom = xcb::xproto::intern_atom(connection.get(), false, "I3_TEST").get();
    ASSERT_TRUE(atom);
    EXPECT_EQ(atom->atom, server.atom("I3_TEST"));
    auto const wm_name = xcb::xproto::intern_atom(connection.get(), true, "WM_NAME").get();
    ASSERT_TRUE(wm_name);
    EXPECT_EQ(wm_name->atom, 39U);

    auto const requests = server.take_requests(*connection);
    ASSERT_EQ(requests.size(), 3U);
    EXPECT_EQ(requests[0].major(), 8);
    EXPECT_EQ(requests[0].u32(4), 0x1234U);
    EXPECT_EQ(requests[1].major(), 16);
    EXPECT_EQ(requests[1].sequence, requests[0].sequence + 1);
    EXPECT_THAT(server.take_requests(*connection), IsEmpty());
    EXPECT_THAT(server.unanswered(), IsEmpty());
}

TEST(FakeXServerTest, KeepsProperties) {
    FakeXServer server{};
    auto connection = server.connect();
    std::array<std::uint32_t, 2> const values{1, 2};
    auto const value_bytes = std::as_bytes(std::span{values});
    std::vector<std::uint8_t> data(value_bytes.size());
    std::memcpy(data.data(), value_bytes.data(), value_bytes.size());
    xcb::xproto::change_property(connection.get(), xcb::xproto::prop_mode_t::PROP_MODE_REPLACE, 0x1234, 39, 6, 32, data);
    data.assign({3, 0, 0, 0});
    xcb::xproto::change_property(connection.get(), xcb::xproto::prop_mode_t::PROP_MODE_APPEND, 0x1234, 39, 6, 32, data);

    auto const reply = xcb::xproto::get_property(connection.get(), false, 0x1234, 39, 0, 0, 100).get();
    ASSERT_TRUE(reply);
    EXPECT_EQ(reply->format, 32);
    EXPECT_EQ(reply->type, 6U);
    EXPECT_EQ(reply->bytes_after, 0U);
    ASSERT_EQ(reply->value.size(), 12U);
    std::array<std::uint32_t, 3> got{};
    std::memcpy(got.data(), reply->value.data(), reply->value.size());
    EXPECT_THAT(got, ElementsAre(1U, 2U, 3U));
}

TEST(FakeXServerTest, SendsEventsWithTheLastSequenceNumber) {
    FakeXServer server{};
    auto connection = server.connect();
    std::ignore = server.take_requests(*connection);

    std::array<std::byte, 32> event{};
    event[0] = std::byte{XCB_MAP_NOTIFY};
    server.send_event(event);
    auto received = connection->wait_for_event();
    ASSERT_NE(received, nullptr);
    EXPECT_EQ(received->response_type, XCB_MAP_NOTIFY);
}

TEST(FakeXServerTest, ADisconnectedConnectionKeepsItsSetup) {
    auto connection = disconnected_connection();
    EXPECT_TRUE(connection->connection_has_error());
    auto const *setup = xcb::get_setup(*connection);
    ASSERT_NE(setup, nullptr);
    EXPECT_EQ(setup->get_roots()[0].root, FakeXServer::root_window);
    EXPECT_FALSE(xcb::xproto::get_input_focus(connection.get()).get());
}

/* i3's X on the fake server: it looks up SHAPE 1.1 at start. */
TEST(FakeXTest, LooksUpShape) {
    FakeXServer server{{.extensions = {{.name = "SHAPE",
                                        .major_opcode = 129,
                                        .first_event = 64,
                                        .first_error = 128,
                                        .handler = [](FakeRequest const &request) {
                                            /* ShapeQueryVersion: 1.1. */
                                            EXPECT_EQ(request.minor(), 0);
                                            return FakeXServer::reply(request, 0, ByteWriter{}.u16(1).u16(1).take());
                                        }}}}};
    FakeX x{server};

    EXPECT_TRUE(x.shape_supported);
    EXPECT_EQ(x.shape_base, 64);
    EXPECT_EQ(x.root, FakeXServer::root_window);
    EXPECT_EQ(x.root_depth, 24);
    EXPECT_EQ(x.visual_type->visual_id, FakeXServer::root_visual);
    auto const requests = server.take_requests(x.connection());
    std::vector<std::uint8_t> majors{};
    std::ranges::transform(requests, std::back_inserter(majors), &FakeRequest::major);
    EXPECT_THAT(majors, ElementsAre(98, 129));
    EXPECT_EQ(requests[0].string(8, requests[0].u16(4)), "SHAPE");
}

TEST(FakeXTest, WithoutShape) {
    FakeXServer server{};
    FakeX x{server};

    EXPECT_FALSE(x.shape_supported);
    auto const requests = server.take_requests(x.connection());
    ASSERT_EQ(requests.size(), 1U);
    EXPECT_EQ(requests[0].major(), 98);
}

/* With a 32-bit visual, i3 uses it, and creates a colormap for it, checking the request. */
TEST(FakeXTest, UsesTheArgbVisual) {
    FakeXServer server{{.argb_visual = true}};
    FakeX x{server};

    EXPECT_EQ(x.root_depth, 32);
    EXPECT_EQ(x.visual_type->visual_id, FakeXServer::argb_visual);
    auto const requests = server.take_requests(x.connection());
    ASSERT_GE(requests.size(), 1U);
    EXPECT_EQ(requests[0].major(), 78); /* CreateColormap */
    EXPECT_EQ(requests[0].u32(4), x.colormap);
    EXPECT_EQ(requests[0].u32(8), FakeXServer::root_window);
    EXPECT_EQ(requests[0].u32(12), FakeXServer::argb_visual);
}
