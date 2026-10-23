/*
 * The X requests of i3's drawing state (draw.cpp's DrawContext), on a fake X server (fake_x_server.cppm).
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

import std;
import i3;
import xcb;
import xcb.xcbcompat;
import mocks;

using testing::ElementsAre;
using testing::IsEmpty;

namespace {
    constexpr std::uint8_t GetInputFocus = 43;
    constexpr std::uint8_t CreateGC = 55;
    constexpr std::uint8_t FreeGC = 60;

    constexpr std::uint8_t bad_drawable = 9;

    std::vector<std::uint8_t> majors(std::vector<FakeRequest> const &requests) {
        std::vector<std::uint8_t> result{};
        std::ranges::transform(requests, std::back_inserter(result), &FakeRequest::major);
        return result;
    }

    /* The requests with major opcode @p major. */
    std::vector<FakeRequest> only(std::vector<FakeRequest> const &requests, std::uint8_t major) {
        std::vector<FakeRequest> result{};
        std::ranges::copy_if(requests, std::back_inserter(result), [major](FakeRequest const &request) { return request.major() == major; });
        return result;
    }
}  // namespace

/* A DrawContext on a connection of its own to a fake server (as a nagbar's or the placeholders' is). */
class DrawContextTest : public testing::Test {
  protected:
    FakeXServer server{};
    xcb::xcbcompat::unique_handle handle{xcb::xcbcompat::adopt(server.connect())};
    xcb::Connection &connection{*xcb::xcbcompat::connection(handle.get())};
    xcb_screen_t *screen{xcb_setup_roots_iterator(xcb_get_setup(handle.get())).data};

    std::vector<FakeRequest> sent() {
        return server.take_requests(connection);
    }
};

/*
 * get_gc() creates one GC per depth (CreateGC, checked: a round trip), with no values, on the drawable it is given,
 * and returns it again for that depth without a request.
 */
TEST_F(DrawContextTest, GetGcCreatesOneGcPerDepth) {
    std::optional<DrawContext> context{std::in_place, handle.get(), screen};

    auto const gc = context->get_gc(24, FakeXServer::root_window);
    auto const requests = sent();
    ASSERT_THAT(majors(requests), ElementsAre(CreateGC, GetInputFocus));
    EXPECT_EQ(requests[0].u32(4), gc);
    EXPECT_EQ(requests[0].u32(8), FakeXServer::root_window);
    EXPECT_EQ(requests[0].u32(12), 0U); /* no values */
    EXPECT_EQ(requests[0].bytes.size(), 16U);

    EXPECT_EQ(context->get_gc(24, 0x500000), gc);
    EXPECT_THAT(sent(), IsEmpty());

    auto const gc1 = context->get_gc(1, 0x600000);
    EXPECT_NE(gc1, gc);
    EXPECT_THAT(majors(sent()), ElementsAre(CreateGC, GetInputFocus));

    /* The destructor frees them (and finishes cairo's device for the connection). */
    context.reset();
    auto const freed = only(sent(), FreeGC);
    ASSERT_EQ(freed.size(), 2U);
    std::vector<std::uint32_t> gcs{freed[0].u32(4), freed[1].u32(4)};
    EXPECT_THAT(gcs, testing::UnorderedElementsAre(gc, gc1));
}

/* A GC that couldn't be created (the error is get_gc()'s, not the event loop's) isn't kept: the next call tries again. */
TEST_F(DrawContextTest, AFailedGcIsNotKept) {
    std::optional<DrawContext> context{std::in_place, handle.get(), screen};
    server.on(CreateGC, [](FakeRequest const &request) { return FakeXServer::error(request, bad_drawable, request.u32(8)); });

    auto const first = context->get_gc(24, 0x500000);
    EXPECT_THAT(majors(sent()), ElementsAre(CreateGC, GetInputFocus));
    EXPECT_EQ(connection.poll_for_queued_event(), nullptr);

    auto const second = context->get_gc(24, 0x500000);
    EXPECT_THAT(majors(sent()), ElementsAre(CreateGC, GetInputFocus));
    EXPECT_NE(second, first);

    context.reset();
    EXPECT_THAT(only(sent(), FreeGC), IsEmpty());
}
