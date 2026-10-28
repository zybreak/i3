/*
 * i3's window properties on xcbpp's GetProperty: property_request, i3's parsers (handle_property.cpp, window.cpp,
 * xcb.cpp, startup.cpp) and the xcb.icccm and xcb.ewmh functions they use, on hand-built replies and on a fake X server
 * (fake_x_server.cppm).
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>
#include <cairo.h>

import std;
import i3;
import xcb;
import mocks;

using testing::ElementsAre;

namespace {
    constexpr std::uint8_t GetProperty = 20;
    constexpr std::uint8_t bad_window = 3;

    constexpr std::uint32_t ATOM = 4, CARDINAL = 6, STRING = 31, WINDOW = 33, WM_HINTS = 35, WM_NAME = 39, WM_NORMAL_HINTS = 40,
                            WM_SIZE_HINTS = 41;
    constexpr std::uint32_t UTF8_STRING = 300; /* stands for the interned atom */
    constexpr xcb_window_t window = 0x00600001;

    xcb::xproto::GetPropertyReplyObj reply(std::uint32_t type, std::uint8_t format, std::span<std::uint8_t const> value) {
        std::vector<xcb::xproto::xcbvoid_t> copy(value.begin(), value.end());
        return xcb::xproto::GetPropertyReplyObj{format, type, 0, copy};
    }

    xcb::xproto::GetPropertyReplyObj reply8(std::uint32_t type, std::string_view value) {
        return reply(type, 8, std::span{reinterpret_cast<std::uint8_t const *>(value.data()), value.size()});
    }

    std::vector<std::uint8_t> bytes32(std::vector<std::uint32_t> const &values) {
        std::vector<std::uint8_t> bytes(values.size() * 4);
        std::memcpy(bytes.data(), values.data(), bytes.size());
        return bytes;
    }

    xcb::xproto::GetPropertyReplyObj reply32(std::uint32_t type, std::vector<std::uint32_t> const &values) {
        return reply(type, 32, bytes32(values));
    }

    std::span<std::byte const> as_bytes(std::vector<std::uint8_t> const &bytes) {
        return std::as_bytes(std::span{bytes});
    }

    /* WM_SIZE_HINTS' 18 elements: @p flags, then the given members by index, the others 0. */
    std::vector<std::uint32_t> size_hints(std::uint32_t flags, std::initializer_list<std::pair<std::size_t, std::int32_t>> members) {
        std::vector<std::uint32_t> values(18, 0);
        values[0] = flags;
        for (auto const &[index, value] : members) {
            values[index] = std::bit_cast<std::uint32_t>(value);
        }
        return values;
    }
}  // namespace

/*
 * i3's X on a fake server, its atoms interned, and a configuration. Each test starts with no request recorded.
 */
class PropertyTest : public testing::Test {
  protected:
    FakeXServer server{};
    /* After the server: the server outlives the client's connection. */
    FakeX x{server};
    ConfigurationManager configManager{x};

    PropertyTest() {
        global.x = &x;
        global.configManager = &configManager;
        global.configManager->config = std::make_unique<Config>();
        setup_atoms(x);
        std::ignore = sent();
    }

    ~PropertyTest() override {
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

    /* Checks that @p request is GetProperty (delete false) of @p property on @p of, of @p type, from offset 0. */
    static void expect_get_property(FakeRequest const &request, xcb_window_t of, xcb_atom_t property, xcb_atom_t type, std::uint32_t long_length) {
        EXPECT_EQ(request.major(), GetProperty);
        EXPECT_EQ(request.minor(), 0) << "delete";
        EXPECT_EQ(request.u32(4), of);
        EXPECT_EQ(request.u32(8), property);
        EXPECT_EQ(request.u32(12), type);
        EXPECT_EQ(request.u32(16), 0U) << "offset";
        EXPECT_EQ(request.u32(20), long_length);
    }
};

// ---------------------------------------------------------------------------------------------------------------
// property_request

/* The request goes out when the object is made, and the reply is waited for on first use and kept. */
TEST_F(PropertyTest, PropertyRequestSendsGetPropertyAndKeepsTheReply) {
    std::string_view const title{"a title"};
    server.set_property(window, WM_NAME, STRING, 8, std::as_bytes(std::span{title}));

    property_request request{x.connection(), window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128};
    auto const requests = sent();
    ASSERT_EQ(requests.size(), 1U);
    expect_get_property(requests[0], window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128);

    auto const *reply = request.get();
    ASSERT_NE(reply, nullptr);
    EXPECT_EQ(reply->type, STRING);
    EXPECT_EQ(reply->format, 8);
    EXPECT_EQ((std::string{reply->value.begin(), reply->value.end()}), title);
    EXPECT_EQ(request.get(), reply) << "the same reply";
    EXPECT_EQ(request.value_len(), title.size());
}

/* A window without the property: a reply of type None and no value, not a failure. */
TEST_F(PropertyTest, PropertyRequestOfAMissingProperty) {
    property_request request{x.connection(), window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128};
    ASSERT_NE(request.get(), nullptr);
    EXPECT_EQ(request.get()->type, 0U);
    EXPECT_EQ(request.value_len(), 0U);
}

/* An X error fails the request, and is its own: nothing reaches the event queue. */
TEST_F(PropertyTest, PropertyRequestOfABadWindowFails) {
    server.on(GetProperty, [](FakeRequest const &request) { return FakeXServer::error(request, bad_window, request.u32(4)); });
    property_request request{x.connection(), window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128};
    EXPECT_EQ(request.get(), nullptr);
    EXPECT_EQ(request.value_len(), 0U);
    std::ignore = sent();
    EXPECT_EQ(x.connection().poll_for_queued_event(), nullptr);
}

/* A request never used is discarded, and one moved keeps its reply. */
TEST_F(PropertyTest, PropertyRequestUnusedOrMoved) {
    std::string_view const title{"t"};
    server.set_property(window, WM_NAME, STRING, 8, std::as_bytes(std::span{title}));
    {
        property_request unused{x.connection(), window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128};
    }
    std::optional<property_request> moved{};
    {
        property_request request{x.connection(), window, WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 128};
        moved.emplace(std::move(request));
    }
    ASSERT_NE(moved->get(), nullptr);
    EXPECT_EQ(moved->value_len(), 1U);
    EXPECT_EQ(sent().size(), 2U);
    EXPECT_EQ(x.connection().poll_for_queued_event(), nullptr);
}

/* property_value32(): the bytes of any format as 32-bit values, those past the end of the value 0. */
TEST(PropertyValue32Test, ReadsTheBytesAndZerosPastThem) {
    auto const words = reply32(CARDINAL, {0x11223344, 0x55667788});
    EXPECT_EQ(property_value32(words, 0), 0x11223344U);
    EXPECT_EQ(property_value32(words, 1), 0x55667788U);
    EXPECT_EQ(property_value32(words, 2), 0U);
    auto const bytes = reply8(STRING, "abcdef");
    EXPECT_EQ(property_value32(bytes, 0), 0x64636261U);
    EXPECT_EQ(property_value32(bytes, 1), 0x00006665U) << "the last two bytes, zero-extended";
    EXPECT_EQ(property_value32(bytes, 2), 0U);
}

// ---------------------------------------------------------------------------------------------------------------
// handle_property.cpp

TEST(HandlePropertyTest, ClassIsInstanceAndClass) {
    using handle_property::window_update_class;
    auto const terminated = reply8(STRING, std::string_view{"inst\0Class\0", 11});
    auto const both = window_update_class(&terminated);
    ASSERT_TRUE(both);
    EXPECT_EQ(both->class_instance, "inst");
    EXPECT_EQ(both->class_name, "Class");
    /* Without the trailing NUL, the class runs to the end; of any type. */
    auto const unterminated = reply8(UTF8_STRING, std::string_view{"inst\0Class", 10});
    auto const to_the_end = window_update_class(&unterminated);
    ASSERT_TRUE(to_the_end);
    EXPECT_EQ(to_the_end->class_name, "Class");
    auto const instance_only = reply8(STRING, "inst");
    auto const instance = window_update_class(&instance_only);
    ASSERT_TRUE(instance);
    EXPECT_EQ(instance->class_instance, "inst");
    EXPECT_EQ(instance->class_name, "");
    auto const empty = reply8(STRING, "");
    EXPECT_FALSE(window_update_class(&empty));
    EXPECT_FALSE(window_update_class(nullptr));
}

TEST(HandlePropertyTest, NamesRolesAndMachinesAreTheBytes) {
    auto const name = reply8(UTF8_STRING, std::string_view{"caf\xc3\xa9\0x", 7});
    EXPECT_EQ(handle_property::window_update_name(&name), (std::string{"caf\xc3\xa9\0x", 7}));
    EXPECT_EQ(handle_property::window_update_name_legacy(&name), (std::string{"caf\xc3\xa9\0x", 7}));
    EXPECT_EQ(handle_property::window_update_role(&name), (std::string{"caf\xc3\xa9\0x", 7}));
    EXPECT_EQ(handle_property::window_update_machine(&name), (std::string{"caf\xc3\xa9\0x", 7}));
    auto const empty = reply8(STRING, "");
    EXPECT_FALSE(handle_property::window_update_name(&empty));
    EXPECT_FALSE(handle_property::window_update_name_legacy(nullptr));
    EXPECT_FALSE(handle_property::window_update_role(&empty));
    EXPECT_FALSE(handle_property::window_update_machine(nullptr));
}

/* WM_CLIENT_LEADER: its first 32 bits, of any type and format. */
TEST(HandlePropertyTest, LeaderIsTheFirstValue) {
    auto const leader = reply32(WINDOW, {0x00400001, 0x00400002});
    EXPECT_EQ(handle_property::window_update_leader(&leader), 0x00400001U);
    auto const short_leader = reply8(STRING, "ab");
    EXPECT_EQ(handle_property::window_update_leader(&short_leader), 0x6261U);
    EXPECT_FALSE(handle_property::window_update_leader(nullptr));
}

/* WM_TRANSIENT_FOR: xcb.icccm's parse_wm_transient_for(), a WINDOW of format 32 only. */
TEST(HandlePropertyTest, TransientForIsAWindow) {
    auto const transient = reply32(WINDOW, {0x00400001});
    EXPECT_EQ(handle_property::window_update_transient_for(&transient), 0x00400001U);
    auto const cardinal = reply32(CARDINAL, {0x00400001});
    EXPECT_FALSE(handle_property::window_update_transient_for(&cardinal));
    auto const empty = reply32(WINDOW, {});
    EXPECT_FALSE(handle_property::window_update_transient_for(&empty));
    EXPECT_FALSE(handle_property::window_update_transient_for(nullptr));
}

/* _NET_WM_STRUT_PARTIAL: its first four values; past a shorter value, 0. */
TEST(HandlePropertyTest, StrutIsTheFirstFourValues) {
    auto const strut = reply32(CARDINAL, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    auto const reserved = handle_property::window_update_strut_partial(&strut);
    ASSERT_TRUE(reserved);
    EXPECT_EQ(std::tie(reserved->left, reserved->right, reserved->top, reserved->bottom), std::tuple(1U, 2U, 3U, 4U));
    auto const short_strut = reply32(CARDINAL, {7, 8});
    auto const partial = handle_property::window_update_strut_partial(&short_strut);
    ASSERT_TRUE(partial);
    EXPECT_EQ(std::tie(partial->left, partial->right, partial->top, partial->bottom), std::tuple(7U, 8U, 0U, 0U));
    EXPECT_FALSE(handle_property::window_update_strut_partial(nullptr));
}

/* WM_HINTS: xcb.icccm's parse_wm_hints(), the input hint and the urgency hint. */
TEST(HandlePropertyTest, HintsAreInputAndUrgency) {
    constexpr std::uint32_t input = 1, urgency = 1U << 8U;
    auto const hints = reply32(WM_HINTS, {input | urgency, 0, 0, 0, 0, 0, 0, 0, 0});
    auto const parsed = handle_property::window_update_hints(&hints);
    ASSERT_TRUE(parsed);
    EXPECT_TRUE(parsed->doesnt_accept_focus);
    EXPECT_TRUE(parsed->urgency_hint);
    /* Without the input flag, the input hint is ignored; 8 elements are enough. */
    auto const no_input = reply32(WM_HINTS, {0, 0, 0, 0, 0, 0, 0, 0});
    auto const without_input = handle_property::window_update_hints(&no_input);
    ASSERT_TRUE(without_input);
    EXPECT_FALSE(without_input->doesnt_accept_focus);
    EXPECT_FALSE(without_input->urgency_hint);
    /* More than 9 elements: the first 9. */
    auto const longer = reply32(WM_HINTS, {urgency, 1, 0, 0, 0, 0, 0, 0, 0, 0xdead, 0xbeef});
    auto const first_nine = handle_property::window_update_hints(&longer);
    ASSERT_TRUE(first_nine);
    EXPECT_TRUE(first_nine->urgency_hint);
    auto const shorter = reply32(WM_HINTS, {urgency, 1, 0, 0, 0, 0, 0});
    EXPECT_FALSE(handle_property::window_update_hints(&shorter));
    auto const cardinal = reply32(CARDINAL, {urgency, 1, 0, 0, 0, 0, 0, 0, 0});
    EXPECT_FALSE(handle_property::window_update_hints(&cardinal));
    EXPECT_FALSE(handle_property::window_update_hints(nullptr));
}

/* _MOTIF_WM_HINTS: the decorations (third value) if the flags (first) say so; past a shorter value, 0. */
TEST(HandlePropertyTest, MotifHintsAreTheDecorations) {
    constexpr std::uint32_t decorations = 1U << 1U, title = 1U << 3U;
    auto const titled = reply32(CARDINAL, {decorations, 0, title, 0, 0});
    EXPECT_EQ(handle_property::window_update_motif_hints(&titled), std::to_underlying(border_style_t::BS_NORMAL));
    auto const none = reply32(CARDINAL, {decorations});
    EXPECT_EQ(handle_property::window_update_motif_hints(&none), std::to_underlying(border_style_t::BS_NONE));
    auto const no_flags = reply32(CARDINAL, {0, 0, title});
    EXPECT_FALSE(handle_property::window_update_motif_hints(&no_flags));
    EXPECT_FALSE(handle_property::window_update_motif_hints(nullptr));
}

// ---------------------------------------------------------------------------------------------------------------
// i3Window (window.cpp) and xcb.cpp

/* _NET_WM_WINDOW_TYPE: the first type i3 knows of the list, as the window's type. */
TEST_F(PropertyTest, TheFirstKnownTypeIsTheWindowsType) {
    auto const types = reply32(ATOM, {0x7777, atom(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG), atom(i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL)});
    EXPECT_EQ(xcb_get_preferred_window_type(&types), atom(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG));
    EXPECT_TRUE(xcb_reply_contains_atom(&types, atom(i3::Atom::_NET_WM_WINDOW_TYPE_NORMAL)));
    EXPECT_TRUE(xcb_reply_contains_atom(&types, 0x7777));
    EXPECT_FALSE(xcb_reply_contains_atom(&types, atom(i3::Atom::_NET_WM_WINDOW_TYPE_DOCK)));
    auto const unknown = reply32(ATOM, {0x7777});
    EXPECT_EQ(xcb_get_preferred_window_type(&unknown), XCB_NONE);
    EXPECT_EQ(xcb_get_preferred_window_type(nullptr), XCB_NONE);
    EXPECT_FALSE(xcb_reply_contains_atom(nullptr, 0x7777));

    i3Window w{window};
    EXPECT_TRUE(w.window_update_type(&types));
    EXPECT_EQ(w.window_type, atom(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG));
    EXPECT_FALSE(w.window_update_type(&unknown));
    EXPECT_EQ(w.window_type, atom(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG));
    EXPECT_EQ(handle_property::window_update_type(&types), atom(i3::Atom::_NET_WM_WINDOW_TYPE_DIALOG));
}

TEST_F(PropertyTest, WindowHintsSetInputAndUrgency) {
    i3Window w{window};
    bool urgent = true;
    w.window_update_hints(nullptr, &urgent);
    EXPECT_FALSE(urgent);
    EXPECT_FALSE(w.doesnt_accept_focus);

    auto const hints = reply32(WM_HINTS, {1U | (1U << 8U), 0, 0, 0, 0, 0, 0, 0, 0});
    w.window_update_hints(&hints, &urgent);
    EXPECT_TRUE(urgent);
    EXPECT_TRUE(w.doesnt_accept_focus);

    auto const accepts = reply32(WM_HINTS, {1U, 1, 0, 0, 0, 0, 0, 0});
    w.window_update_hints(&accepts, &urgent);
    EXPECT_FALSE(urgent);
    EXPECT_FALSE(w.doesnt_accept_focus);
}

TEST_F(PropertyTest, WindowLeaderTransientForStrutRoleAndMotifHints) {
    i3Window w{window};
    auto const leader = reply32(WINDOW, {0x00400005});
    w.window_update_leader(&leader);
    EXPECT_EQ(w.leader, 0x00400005U);
    w.window_update_leader(nullptr);
    EXPECT_EQ(w.leader, XCB_NONE);

    auto const transient = reply32(WINDOW, {0x00400006});
    w.window_update_transient_for(&transient);
    EXPECT_EQ(w.transient_for, 0x00400006U);
    auto const cardinal = reply32(CARDINAL, {0x00400007});
    w.window_update_transient_for(&cardinal);
    EXPECT_EQ(w.transient_for, 0x00400006U) << "not a WM_TRANSIENT_FOR: kept";
    w.window_update_transient_for(nullptr);
    EXPECT_EQ(w.transient_for, XCB_NONE);

    auto const strut = reply32(CARDINAL, {0, 0, 25});
    w.window_update_strut_partial(&strut);
    EXPECT_EQ(std::tie(w.reserved.left, w.reserved.right, w.reserved.top, w.reserved.bottom), std::tuple(0U, 0U, 25U, 0U));

    auto const role = reply8(STRING, "browser");
    w.window_update_role(&role);
    EXPECT_EQ(w.role, "browser");

    border_style_t style{border_style_t::BS_NORMAL};
    auto const motif = reply32(CARDINAL, {1U << 1U, 0, (1U << 0U) | (1U << 3U)});
    EXPECT_TRUE(w.window_update_motif_hints(&motif, &style));
    EXPECT_EQ(style, border_style_t::BS_PIXEL);
    auto const empty = reply32(CARDINAL, {});
    EXPECT_FALSE(w.window_update_motif_hints(&empty, &style));
}

/* WM_NORMAL_HINTS from a reply (xcb.icccm's parse_wm_size_hints()): what i3 takes of it, and whether it changed. */
TEST_F(PropertyTest, NormalHintsFromAReply) {
    constexpr std::uint32_t us_position = 1U << 0U, us_size = 1U << 1U, p_min_size = 1U << 4U, p_max_size = 1U << 5U,
                            p_resize_inc = 1U << 6U, p_aspect = 1U << 7U, base_size = 1U << 8U;
    i3Window w{window};
    xcb::xproto::GetGeometryReplyObj geometry{0, 0, 0, 0, 0, 0, 0};
    auto const hints = reply32(WM_SIZE_HINTS, size_hints(us_position | us_size | p_min_size | p_max_size | p_resize_inc | p_aspect | base_size,
                                                         {{1, 10}, {2, 20}, {3, 300}, {4, 200}, {5, 100}, {6, 50}, {7, -1}, {8, 900},
                                                          {9, 8}, {10, 0xffff}, {11, 1}, {12, 2}, {13, 3}, {14, 4}, {15, 6}, {16, 7}}));
    EXPECT_TRUE(w.window_update_normal_hints(&hints, &geometry));
    EXPECT_EQ(std::tie(w.min_width, w.min_height), std::tuple(100U, 50U));
    EXPECT_EQ(std::tie(w.max_width, w.max_height), std::tuple(0U, 900U)) << "a negative maximum is 0";
    EXPECT_EQ(std::tie(w.width_increment, w.height_increment), std::tuple(8, 0)) << "an increment of 0xffff is none";
    EXPECT_EQ(std::tie(w.base_width, w.base_height), std::tuple(6, 7));
    EXPECT_DOUBLE_EQ(w.min_aspect_ratio, 0.5);
    EXPECT_DOUBLE_EQ(w.max_aspect_ratio, 0.75);
    EXPECT_EQ(std::tie(geometry.x, geometry.y, geometry.width, geometry.height), std::tuple(10, 20, 300, 200));
    EXPECT_FALSE(w.window_update_normal_hints(&hints, nullptr)) << "unchanged";

    /* A pre-ICCCM client's 15 elements: no base size. */
    auto values = size_hints(base_size, {{15, 6}, {16, 7}});
    values.resize(15);
    auto const old = reply32(WM_SIZE_HINTS, values);
    EXPECT_TRUE(w.window_update_normal_hints(&old, nullptr));
    EXPECT_EQ(std::tie(w.base_width, w.base_height), std::tuple(0, 0));

    auto const cardinal = reply32(CARDINAL, size_hints(p_min_size, {{5, 1}, {6, 1}}));
    EXPECT_FALSE(w.window_update_normal_hints(&cardinal, nullptr)) << "not WM_SIZE_HINTS";
}

/* Without a reply, i3 asks for WM_NORMAL_HINTS (xcb.icccm's get_wm_normal_hints_unchecked(): type WM_SIZE_HINTS, 18
 * elements). */
TEST_F(PropertyTest, NormalHintsAreAskedFor) {
    constexpr std::uint32_t p_min_size = 1U << 4U;
    i3Window w{window};
    EXPECT_FALSE(w.window_update_normal_hints(nullptr, nullptr)) << "the window has none";
    auto requests = sent();
    ASSERT_EQ(requests.size(), 1U);
    expect_get_property(requests[0], window, WM_NORMAL_HINTS, WM_SIZE_HINTS, 18);

    auto const bytes = bytes32(size_hints(p_min_size, {{5, 120}, {6, 80}}));
    server.set_property(window, WM_NORMAL_HINTS, WM_SIZE_HINTS, 32, as_bytes(bytes));
    EXPECT_TRUE(w.window_update_normal_hints(nullptr, nullptr));
    EXPECT_EQ(std::tie(w.min_width, w.min_height), std::tuple(120U, 80U));
}

/* WM_PROTOCOLS (xcb.icccm's get_wm_protocols(): type ATOM, the whole property). */
TEST_F(PropertyTest, WindowSupportsTheProtocolsItLists) {
    EXPECT_FALSE(window_supports_protocol(window, atom(i3::Atom::WM_DELETE_WINDOW)));
    auto const requests = sent();
    ASSERT_EQ(requests.size(), 1U);
    expect_get_property(requests[0], window, atom(i3::Atom::WM_PROTOCOLS), ATOM, std::numeric_limits<std::uint32_t>::max());

    auto const protocols = bytes32({atom(i3::Atom::WM_TAKE_FOCUS), atom(i3::Atom::WM_DELETE_WINDOW)});
    server.set_property(window, atom(i3::Atom::WM_PROTOCOLS), ATOM, 32, as_bytes(protocols));
    EXPECT_TRUE(window_supports_protocol(window, atom(i3::Atom::WM_DELETE_WINDOW)));
    EXPECT_TRUE(window_supports_protocol(window, atom(i3::Atom::WM_TAKE_FOCUS)));
    EXPECT_FALSE(window_supports_protocol(window, atom(i3::Atom::_NET_WM_NAME)));

    server.set_property(window, atom(i3::Atom::WM_PROTOCOLS), CARDINAL, 32, as_bytes(protocols));
    EXPECT_FALSE(window_supports_protocol(window, atom(i3::Atom::WM_DELETE_WINDOW))) << "not of type ATOM";
}

/* _NET_WM_ICON (xcb.ewmh's parse_wm_icon()): i3 takes the icon of the title bar's size, else the smallest larger one,
 * else the largest. An icon too large for the property ends the list (xcb-ewmh's arithmetic overflowed on 2^31 ×
 * 2^31, and i3's, which was the same, took it). */
TEST_F(PropertyTest, TheIconOfThePreferredSize) {
    global.configManager->config->font = load_font(x.c_handle(), x.root_screen, "pango:monospace 8");
    auto const pref = static_cast<std::uint32_t>(render_deco_height() - logical_px(x.root_screen, 2));
    ASSERT_GT(pref, 4U);
    std::vector<std::uint32_t> icons{};
    for (auto const size : {pref - 4, pref + 8, pref, pref + 2}) {
        icons.push_back(size);
        icons.push_back(size);
        icons.insert(icons.end(), std::size_t{size} * size, 0xff000000 | size);
    }
    i3Window w{window};
    auto const all = reply32(CARDINAL, icons);
    w.window_update_icon(&all);
    ASSERT_TRUE(w.icon);
    EXPECT_EQ(static_cast<std::uint32_t>(cairo_image_surface_get_width(w.icon->data())), pref);
    EXPECT_EQ(static_cast<std::uint32_t>(cairo_image_surface_get_height(w.icon->data())), pref);

    i3Window larger{window};
    std::vector<std::uint32_t> two{pref - 4, pref - 4};
    two.insert(two.end(), std::size_t{pref - 4} * (pref - 4), 0);
    two.insert(two.end(), {pref + 8, pref + 8});
    two.insert(two.end(), std::size_t{pref + 8} * (pref + 8), 0);
    auto const smaller_and_larger = reply32(CARDINAL, two);
    larger.window_update_icon(&smaller_and_larger);
    ASSERT_TRUE(larger.icon);
    EXPECT_EQ(static_cast<std::uint32_t>(cairo_image_surface_get_width(larger.icon->data())), pref + 8);

    i3Window overflow{window};
    auto const huge = reply32(CARDINAL, {0x80000000, 0x80000000, 1, 2});
    overflow.window_update_icon(&huge);
    EXPECT_FALSE(overflow.icon);
    auto const not_cardinal = reply32(WINDOW, {1, 1, 0xff00ff00});
    overflow.window_update_icon(&not_cardinal);
    EXPECT_FALSE(overflow.icon);
    overflow.window_update_icon(nullptr);
    EXPECT_FALSE(overflow.icon);
    EXPECT_THAT(server.unanswered(), testing::IsEmpty());
}

// ---------------------------------------------------------------------------------------------------------------
// startup.cpp

/* _NET_STARTUP_ID: a window without it is looked up through its leader's (512 elements, any type). */
TEST_F(PropertyTest, TheStartupIdOfTheLeaderIsAskedFor) {
    ApplicationLauncher launcher{x};
    std::ignore = sent();
    i3Window w{window};
    EXPECT_EQ(launcher.startup_workspace_for_window(&w, nullptr), std::nullopt);
    EXPECT_THAT(sent(), testing::IsEmpty()) << "no leader, nothing to ask";

    w.leader = 0x00600002;
    std::string_view const id{"i3/true/1-2-host_TIME0"};
    server.set_property(w.leader, atom(i3::Atom::_NET_STARTUP_ID), UTF8_STRING, 8, std::as_bytes(std::span{id}));
    EXPECT_EQ(launcher.startup_workspace_for_window(&w, nullptr), std::nullopt) << "no sequence of that ID";
    auto const requests = sent();
    ASSERT_EQ(requests.size(), 1U);
    expect_get_property(requests[0], w.leader, atom(i3::Atom::_NET_STARTUP_ID), XCB_GET_PROPERTY_TYPE_ANY, 512);

    launcher.startup_sequence_delete_by_window(&w);
    auto const deleted = sent();
    ASSERT_EQ(deleted.size(), 2U);
    expect_get_property(deleted[0], window, atom(i3::Atom::_NET_STARTUP_ID), XCB_GET_PROPERTY_TYPE_ANY, 512);
    expect_get_property(deleted[1], w.leader, atom(i3::Atom::_NET_STARTUP_ID), XCB_GET_PROPERTY_TYPE_ANY, 512);
}
