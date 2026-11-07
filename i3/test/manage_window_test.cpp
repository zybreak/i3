/*
 * i3 managing windows (manage.cpp) on Xvfb, with a test client on xcbpp.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <csignal>
#include <sys/syscall.h>
#include <unistd.h>

import std;
import i3ipc;
import i3.json;
import i3_runner;
import xvfb_fixture;
import xcb;
import xcb.proto.shape;
import xcb.proto.xtest;

namespace {
    /* The containers named @p name in i3's tree @p node. */
    void find_named(json const &node, std::string_view name, std::vector<json> &found) {
        if (node.contains("name") && node.at("name").is_string() && node.at("name").get<std::string>() == name) {
            found.push_back(node);
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    find_named(child, name, found);
                }
            }
        }
    }

    /* The X window of the only container named @p name in i3's tree @p root, 0 if it has none. */
    std::uint32_t window_of_container(json const &root, std::string_view name) {
        std::vector<json> found;
        find_named(root, name, found);
        if (found.size() != 1 || !found[0].contains("window") || !found[0].at("window").is_number()) {
            return 0;
        }
        return found[0].at("window").get<std::uint32_t>();
    }

    /* How often @p text contains @p what. */
    std::size_t occurrences(std::string_view text, std::string_view what) {
        std::size_t count = 0;
        for (auto at = text.find(what); at != std::string_view::npos; at = text.find(what, at + what.size())) {
            ++count;
        }
        return count;
    }

    /* Whether i3's tree @p node holds the container of X window @p window. */
    bool manages(json const &node, std::uint32_t window) {
        if (node.contains("window") && node.at("window").is_number() && node.at("window").get<std::uint32_t>() == window) {
            return true;
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    if (manages(child, window)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    class ManageWindowTest : public XvfbTest {
      protected:
        /* Starts i3 with a configuration of a font, the IPC socket, and @p extra_config. */
        void start_i3(std::string_view extra_config = {}) {
            auto const config = work_dir() / "config";
            std::ofstream{config} << std::format("# i3 config file (v4)\nfont pango:monospace 8\nipc-socket {}\n{}", ipc_socket().native(), extra_config);
            auto i3 = I3Process::start({.binary = I3_TEST_I3,
                                        .config = config,
                                        .ipc_socket = ipc_socket(),
                                        .log = work_dir() / "i3.log",
                                        .arguments = {"-V", "-d", "all"}});
            ASSERT_TRUE(i3) << i3.error();
            i3_.emplace(std::move(*i3));
        }

        [[nodiscard]] std::filesystem::path ipc_socket() const {
            return work_dir() / "ipc";
        }

        [[nodiscard]] json request(i3ipc::MESSAGE_TYPE type, std::string_view payload = {}) {
            if (!i3_) {
                throw std::logic_error{"i3 isn't running: start_i3() failed or wasn't called"};
            }
            auto reply = i3_->ipc().request_json(type, payload);
            if (!reply) {
                throw std::runtime_error{reply.error()};
            }
            return std::move(*reply);
        }

        /* The i3 process that start_i3() started. */
        [[nodiscard]] ChildProcess &i3_process() {
            if (!i3_) {
                throw std::logic_error{"i3 isn't running: start_i3() failed or wasn't called"};
            }
            return i3_->process();
        }

        /* Runs an i3 command and checks that it succeeded. */
        void command(std::string_view text) {
            auto const reply = request(i3ipc::MESSAGE_TYPE::RUN_COMMAND, text);
            ASSERT_TRUE(reply.is_array() && !reply.empty() && reply.at(0).value("success", false)) << text << ": " << reply.dump();
        }

        /* A connection of the test's own to the server, and its screen's root window. */
        void connect_client() {
            int screen = 0;
            auto connection = xcb::connect(server().display(), &screen);
            ASSERT_TRUE(connection) << "error " << std::to_underlying(connection.error());
            client_ = std::move(*connection);
            root_ = xcb::get_setup(*client_)->get_roots()[static_cast<std::size_t>(screen)].root;
        }

        [[nodiscard]] std::uint32_t root() const noexcept {
            return root_;
        }

        [[nodiscard]] xcb::Connection &client() {
            if (!client_) {
                throw std::logic_error{"the test has no client connection: connect_client() failed or wasn't called"};
            }
            return *client_;
        }

        /* An unmapped 300x200 InputOutput window of the client's. */
        std::uint32_t create_window() {
            auto const id = client().generate_id();
            auto const error = xcb::xproto::create_window_checked(&client(), 0, id, root_, 0, 0, 300, 200, 0,
                                                                  xcb::xproto::window_class_t::input_output, 0,
                                                                  xcb::xproto::CreateWindowValueList{})
                                   .get();
            EXPECT_FALSE(error) << "CreateWindow: X error " << (error ? error->error_code : 0);
            return id;
        }

        /* The parent of @p window: its frame once i3 manages it. */
        std::uint32_t parent_of(std::uint32_t window) {
            auto const tree = xcb::xproto::query_tree(&client(), window).get();
            return tree ? tree->parent : 0;
        }

        /* @p window's property @p property, of any type, as bytes; std::nullopt if it doesn't have it. */
        std::optional<std::string> property(std::uint32_t window, std::uint32_t property) {
            auto const reply = xcb::xproto::get_property(&client(), false, window, property, 0, 0, 1024).get();
            if (!reply || reply->type == 0) {
                return std::nullopt;
            }
            return std::string{reinterpret_cast<char const *>(reply->value.data()), reply->value.size()};
        }

        std::uint32_t atom(std::string_view name) {
            auto const reply = xcb::xproto::intern_atom(&client(), false, name).get();
            EXPECT_TRUE(reply) << "InternAtom " << name;
            return reply ? reply->atom : 0;
        }

        /* The pointer's position on the root window. */
        std::pair<int, int> pointer() {
            auto const reply = xcb::xproto::query_pointer(&client(), root_).get();
            return reply ? std::pair{static_cast<int>(reply->root_x), static_cast<int>(reply->root_y)} : std::pair{-1, -1};
        }

        /* XTEST: moves the pointer to (@p x, @p y) on the root window. */
        void move_pointer(int x, int y) {
            ASSERT_FALSE(xcb::xtest::fake_input_checked(&client(), 6 /* MotionNotify */, 0, 0, root_, static_cast<std::int16_t>(x),
                                                        static_cast<std::int16_t>(y), 0)
                             .get());
        }

        /* XTEST: presses (@p press) or releases the key @p keycode. */
        void key(std::uint8_t keycode, bool press) {
            ASSERT_FALSE(xcb::xtest::fake_input_checked(&client(), press ? 2 /* KeyPress */ : 3 /* KeyRelease */, keycode, 0, root_, 0, 0, 0)
                             .get());
        }

        /* The first keycode whose first keysym is @p keysym, 0 if none. */
        std::uint8_t keycode_of(std::uint32_t keysym) {
            auto const *setup = xcb::get_setup(client());
            std::uint8_t const first = setup->min_keycode;
            auto const count = static_cast<std::uint8_t>(setup->max_keycode - first + 1);
            auto const mapping = xcb::xproto::get_keyboard_mapping(&client(), first, count).get();
            if (!mapping || mapping->keysyms_per_keycode == 0) {
                return 0;
            }
            for (std::size_t i = 0; i < count; ++i) {
                if (mapping->keysyms.at(i * mapping->keysyms_per_keycode) == keysym) {
                    return static_cast<std::uint8_t>(first + i);
                }
            }
            return 0;
        }

        /* The root window's viewable override-redirect children: i3's crash dialogs, among others. */
        std::vector<std::uint32_t> override_redirect_children() {
            std::vector<std::uint32_t> windows;
            auto const tree = xcb::xproto::query_tree(&client(), root_).get();
            if (!tree) {
                return windows;
            }
            for (auto const child : tree->children) {
                auto const attributes = xcb::xproto::get_window_attributes(&client(), child).get();
                if (attributes && attributes->override_redirect && attributes->map_state == xcb::xproto::map_state_t::viewable) {
                    windows.push_back(child);
                }
            }
            return windows;
        }

        /* Whether the client can grab the keyboard (and then ungrabs it): not while i3 has it grabbed. */
        bool keyboard_grabbable() {
            auto const reply = xcb::xproto::grab_keyboard(&client(), false, root_, 0, xcb::xproto::grab_mode_t::async,
                                                          xcb::xproto::grab_mode_t::async)
                                   .get();
            bool const grabbed = reply && reply->status == xcb::xproto::grab_status_t::success;
            if (grabbed) {
                xcb::xproto::ungrab_keyboard(&client(), 0);
                client().flush();
            }
            return grabbed;
        }

        /* XTEST: presses (@p press) or releases button 1. */
        void button1(bool press) {
            ASSERT_FALSE(xcb::xtest::fake_input_checked(&client(), press ? 4 /* ButtonPress */ : 5 /* ButtonRelease */, 1, 0, root_, 0, 0, 0)
                             .get());
        }

        /* Sends a ClientMessage of type @p type about @p window, with @p data, to the root window, as EWMH and ICCCM
         * ask clients to (SubstructureRedirect and SubstructureNotify). */
        void client_message(std::uint32_t window, std::uint32_t type, std::array<std::uint32_t, 5> data) { // NOLINT(bugprone-easily-swappable-parameters)
            xcb::xproto::client_message_event_t event{};
            event.response_type = 33; /* ClientMessage */
            event.format = 32;
            event.window = window;
            event.type = type;
            event.data.data32 = data;
            ASSERT_FALSE(xcb::xproto::send_event_checked(&client(), false, root_,
                                                         xcb::xproto::event_mask_t::substructure_redirect |
                                                             xcb::xproto::event_mask_t::substructure_notify,
                                                         std::bit_cast<std::array<char, 32>>(event))
                             .get());
        }

        /* Whether @p window exists (GetGeometry succeeds). */
        bool exists(std::uint32_t window) {
            return xcb::xproto::get_geometry(&client(), window).get().has_value();
        }

        /* Whether @p window has a bounding shape of its own (SHAPE QueryExtents). */
        bool bounding_shaped(std::uint32_t window) {
            auto const extents = xcb::shape::query_extents(&client(), window).get();
            if (!extents) {
                ADD_FAILURE() << "ShapeQueryExtents: X error " << extents.error().error_code;
                return false;
            }
            return extents->bounding_shaped != 0;
        }

      private:
        std::optional<I3Process> i3_{};
        std::unique_ptr<xcb::Connection> client_{};
        std::uint32_t root_{};
    };
}  // namespace

/*
 * manage_window() asks SHAPE whether a new window has a bounding shape (QueryExtents), and selects its ShapeNotify
 * events. A window shaped before i3 manages it gets no ShapeNotify: only that answer makes i3 shape its frame once it
 * floats (set_shape_state()).
 */
TEST_F(ManageWindowTest, AWindowShapedBeforeItIsManagedGetsAShapedFrameWhenItFloats) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    std::array rectangles{xcb::xproto::rectangle_t{.x = 0, .y = 0, .width = 100, .height = 50}};
    xcb::shape::rectangles(&client(), xcb::shape::so_t::set, xcb::shape::sk_t::bounding,
                           xcb::xproto::clip_ordering_t::unsorted, window, 0, 0, std::span<xcb::xproto::rectangle_t>{rectangles});
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}))
        << "i3 manages the window";
    auto const frame = parent_of(window);
    ASSERT_NE(frame, 0U);
    EXPECT_FALSE(bounding_shaped(frame)) << "a tiling frame isn't shaped";

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));

    EXPECT_EQ(parent_of(window), frame);
    EXPECT_TRUE(bounding_shaped(frame)) << "the floating frame takes the window's shape";
}

/* Without a shape, the floating frame stays unshaped. */
TEST_F(ManageWindowTest, AnUnshapedWindowsFloatingFrameIsNotShaped) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}))
        << "i3 manages the window";

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));

    EXPECT_FALSE(bounding_shaped(parent_of(window)));
}

/*
 * append_layout opens a placeholder window for a container that waits for a window to swallow (restore_layout.cpp, on
 * a connection of its own): named as the container, which i3 manages in the container. The window that matches the
 * container's swallow criteria takes its place, and the placeholder is destroyed.
 */
TEST_F(ManageWindowTest, APlaceholderWaitsForTheWindowItSwallows) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const layout = work_dir() / "layout.json";
    std::ofstream{layout} << R"({"name": "waiting for swallowme", "type": "con", "swallows": [{"class": "^swallowme$"}]})";
    ASSERT_NO_FATAL_FAILURE(command(std::format("append_layout {}", layout.native())));

    std::uint32_t placeholder = 0;
    ASSERT_TRUE(poll_until([&] { return (placeholder = window_of_container(request(i3ipc::MESSAGE_TYPE::GET_TREE), "waiting for swallowme")) != 0; },
                           std::chrono::seconds{5}))
        << "i3 manages the placeholder window in the container";
    EXPECT_EQ(property(placeholder, atom("_NET_WM_NAME")), "waiting for swallowme");
    /* WM_HINTS (xcb.icccm, on the placeholders' connection): the InputHint flag, input false, and nothing else. Until
     * i3 used xcb.icccm, the other flags and fields were uninitialised stack bytes (xcb_icccm_wm_hints_set_none() clears
     * only the state flag of an xcb_icccm_wm_hints_t that i3 didn't initialise). */
    auto const hints = property(placeholder, 35 /* WM_HINTS */);
    ASSERT_TRUE(hints);
    ASSERT_EQ(hints->size(), 36U);
    std::array<std::uint32_t, 9> values{};
    std::memcpy(values.data(), hints->data(), 36);
    EXPECT_THAT(values, testing::ElementsAre(1U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U));

    auto const window = create_window();
    std::string_view const wm_class{"swallowme\0swallowme\0", 20};
    xcb::xproto::change_property(&client(), xcb::xproto::prop_mode_t::replace, window, 67 /* WM_CLASS */, 31 /* STRING */, 8,
                                 std::span{reinterpret_cast<std::uint8_t*>(const_cast<char*>(wm_class.data())), wm_class.size()});
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());

    EXPECT_TRUE(poll_until([&] { return window_of_container(request(i3ipc::MESSAGE_TYPE::GET_TREE), "waiting for swallowme") == window ||
                                        manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); },
                           std::chrono::seconds{5}))
        << "the window is managed";
    EXPECT_FALSE(manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), placeholder));
    EXPECT_TRUE(poll_until([&] { return !exists(placeholder); }, std::chrono::seconds{5})) << "the placeholder window is destroyed";
}

namespace {
    /* The containers of i3's tree @p node that hold an X window, with their rects and percents, in tree order. */
    void window_containers(json const &node, std::vector<json> &found) {
        if (node.contains("window") && node.at("window").is_number()) {
            found.push_back(node);
        }
        if (node.contains("nodes")) {
            for (auto const &child : node.at("nodes")) {
                window_containers(child, found);
            }
        }
    }
}  // namespace

/*
 * Dragging the border between two tiling windows with the mouse resizes them (resize.cpp's
 * resize_graphical_handler(), with drag.cpp's drag loop): the pointer is warped to the middle of the gap, a helper
 * window follows the pointer, and the containers' percentages change by the distance on release.
 */
TEST_F(ManageWindowTest, DraggingTheBorderBetweenTwoWindowsResizesThem) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const left = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), left).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), left); }, std::chrono::seconds{5}));
    auto const right = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), right).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), right); }, std::chrono::seconds{5}));
    std::vector<json> before;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), before);
    ASSERT_EQ(before.size(), 2U);
    auto const left_rect = before[0].at("rect");
    int const border_x = left_rect.at("x").get<int>() + left_rect.at("width").get<int>() - 1;
    int const border_y = left_rect.at("y").get<int>() + (left_rect.at("height").get<int>() / 2);
    int const gap_middle = border_x + 1;

    ASSERT_NO_FATAL_FAILURE(move_pointer(border_x, border_y));
    ASSERT_NO_FATAL_FAILURE(button1(true));
    ASSERT_TRUE(poll_until([&] { return pointer() == std::pair{gap_middle, border_y}; }, std::chrono::seconds{5}))
        << "i3 warps the pointer to the middle of the gap";
    ASSERT_NO_FATAL_FAILURE(move_pointer(gap_middle + 100, border_y));
    ASSERT_NO_FATAL_FAILURE(button1(false));

    EXPECT_TRUE(poll_until([&] {
        std::vector<json> after;
        window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), after);
        return after.size() == 2 && after[0].at("rect").at("width").get<int>() == left_rect.at("width").get<int>() + 100;
    }, std::chrono::seconds{5})) << "the left container is 100 pixels wider";
    std::vector<json> after;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), after);
    ASSERT_EQ(after.size(), 2U);
    EXPECT_EQ(after[0].at("window").get<std::uint32_t>(), left);
    EXPECT_NEAR(after[0].at("percent").get<double>() + after[1].at("percent").get<double>(), 1.0, 1e-6);
}

/*
 * On a crash signal i3 shows a dialog on each output (sighandler.cpp: a colormap and a window each, the keyboard and
 * the pointer grabbed) and reads the key presses in a loop of its own; 'r' restarts i3 in place.
 */
TEST_F(ManageWindowTest, TheCrashDialogRestartsI3OnR) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const r = keycode_of('r');
    ASSERT_NE(r, 0);
    auto const before = override_redirect_children();
    /* i3 idle in its event loop (an IPC round trip; nothing else sends it events): the handler isn't
     * async-signal-safe, and a signal while i3 holds the connection's lock or the logger's would deadlock it. */
    ASSERT_NO_FATAL_FAILURE(command("nop"));

    /* To i3's main thread, as a crash there would: a signal to the process may go to a thread of a library's
     * (fontconfig's), where the handler blocks. */
    ASSERT_EQ(::syscall(SYS_tgkill, i3_process().pid(), i3_process().pid(), SIGSEGV), 0);
    std::vector<std::uint32_t> dialogs;
    ASSERT_TRUE(poll_until([&] {
        dialogs.clear();
        for (auto const window : override_redirect_children()) {
            if (!std::ranges::contains(before, window)) {
                dialogs.push_back(window);
            }
        }
        return dialogs.size() == 1;
    }, std::chrono::seconds{5})) << "one dialog, for the one output";
    EXPECT_FALSE(keyboard_grabbable()) << "i3 grabs the keyboard";
    auto const geometry = xcb::xproto::get_geometry(&client(), dialogs[0]).get();
    ASSERT_TRUE(geometry);
    auto const screen = xcb::xproto::get_geometry(&client(), root()).get();
    ASSERT_TRUE(screen);
    EXPECT_LE(std::abs((geometry->x + (geometry->width / 2)) - (screen->width / 2)), 1) << "centered on the output";
    EXPECT_LE(std::abs((geometry->y + (geometry->height / 2)) - (screen->height / 2)), 1);

    ASSERT_NO_FATAL_FAILURE(key(r, true));
    ASSERT_NO_FATAL_FAILURE(key(r, false));

    EXPECT_TRUE(poll_until([&] {
        auto ipc = IpcClient::connect(ipc_socket());
        return ipc && ipc->request(i3ipc::MESSAGE_TYPE::GET_VERSION).has_value() && keyboard_grabbable();
    }, std::chrono::seconds{10})) << "the restarted i3 answers on its IPC socket, and the keyboard is free";
    EXPECT_TRUE(i3_process().running());
    EXPECT_FALSE(xcb::xproto::get_geometry(&client(), dialogs[0]).get()) << "the dialog is gone";
}

/*
 * A key pressed during the drag reverts it (drag.cpp's drag loop: DRAG_REVERT): on release, the containers keep their
 * sizes.
 */
TEST_F(ManageWindowTest, AKeyPressedWhileDraggingTheBorderRevertsTheResize) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const escape = keycode_of(0xff1b /* Escape */);
    ASSERT_NE(escape, 0);
    for (int i = 0; i < 2; ++i) {
        auto const window = create_window();
        ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
        ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    }
    std::vector<json> before;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), before);
    ASSERT_EQ(before.size(), 2U);
    auto const left_rect = before[0].at("rect");
    int const border_x = left_rect.at("x").get<int>() + left_rect.at("width").get<int>() - 1;
    int const border_y = left_rect.at("y").get<int>() + (left_rect.at("height").get<int>() / 2);
    int const gap_middle = border_x + 1;

    ASSERT_NO_FATAL_FAILURE(move_pointer(border_x, border_y));
    ASSERT_NO_FATAL_FAILURE(button1(true));
    ASSERT_TRUE(poll_until([&] { return pointer() == std::pair{gap_middle, border_y}; }, std::chrono::seconds{5}))
        << "i3 warps the pointer to the middle of the gap";
    ASSERT_NO_FATAL_FAILURE(move_pointer(gap_middle + 100, border_y));
    ASSERT_NO_FATAL_FAILURE(key(escape, true));
    ASSERT_NO_FATAL_FAILURE(key(escape, false));
    ASSERT_NO_FATAL_FAILURE(button1(false));

    /* The drag is over once i3 has ungrabbed the keyboard. */
    ASSERT_TRUE(poll_until([&] { return keyboard_grabbable(); }, std::chrono::seconds{5}));
    ASSERT_NO_FATAL_FAILURE(command("nop"));
    std::vector<json> after;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), after);
    ASSERT_EQ(after.size(), 2U);
    EXPECT_EQ(after[0].at("rect"), left_rect);
    EXPECT_EQ(after[1].at("rect"), before[1].at("rect"));
}

/*
 * i3 carries out the ConfigureRequest of a window it doesn't manage (an unmapped one) as asked: position (negative
 * here), size and border width (PropertyHandlers::handle_configure_request()).
 */
TEST_F(ManageWindowTest, AnUnmanagedWindowIsConfiguredAsItAsks) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();

    ASSERT_FALSE(xcb::xproto::configure_window_checked(&client(), window,
                                                       xcb::xproto::ConfigureWindowValueList{}
                                                           .with_x(-20)
                                                           .with_y(30)
                                                           .with_width(123)
                                                           .with_height(45)
                                                           .with_border_width(3))
                     .get());

    EXPECT_TRUE(poll_until([&] {
        auto const geometry = xcb::xproto::get_geometry(&client(), window).get();
        return geometry && geometry->x == -20 && geometry->y == 30 && geometry->width == 123 && geometry->height == 45 &&
               geometry->border_width == 3;
    }, std::chrono::seconds{5})) << "i3 configured the window";
    EXPECT_FALSE(manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window));
}

/*
 * Dragging a tiling window by its title bar (tiling_drag titlebar; handlers/tiling_drag.cpp) shows a drop indicator, an
 * override-redirect window of class "i3-drag", and drops the window where the indicator was: past the right edge of
 * the other window, so the two swap places.
 */
TEST_F(ManageWindowTest, DraggingATitleBarMovesTheWindow) {
    ASSERT_NO_FATAL_FAILURE(start_i3("tiling_drag titlebar\n"));
    ASSERT_NO_FATAL_FAILURE(connect_client());
    std::vector<std::uint32_t> windows;
    for (int i = 0; i < 2; ++i) {
        auto const window = create_window();
        ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
        ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
        windows.push_back(window);
    }
    std::vector<json> before;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), before);
    ASSERT_EQ(before.size(), 2U);
    ASSERT_EQ(before[0].at("window").get<std::uint32_t>(), windows[0]);
    auto const &left = before[0].at("rect");
    auto const &right = before[1].at("rect");
    int const title_x = left.at("x").get<int>() + (left.at("width").get<int>() / 2);
    int const title_y = left.at("y").get<int>() + (before[0].at("deco_rect").at("height").get<int>() / 2);
    int const drop_x = right.at("x").get<int>() + right.at("width").get<int>() - 10;
    int const drop_y = right.at("y").get<int>() + (right.at("height").get<int>() / 2);
    auto const existing = override_redirect_children();

    ASSERT_NO_FATAL_FAILURE(move_pointer(title_x, title_y));
    ASSERT_NO_FATAL_FAILURE(button1(true));
    ASSERT_TRUE(poll_until([&] { return !keyboard_grabbable(); }, std::chrono::seconds{5})) << "i3's drag loop grabs the keyboard";
    ASSERT_NO_FATAL_FAILURE(move_pointer(drop_x, drop_y));

    std::uint32_t indicator = 0;
    EXPECT_TRUE(poll_until([&] {
        for (auto const window : override_redirect_children()) {
            if (!std::ranges::contains(existing, window)) {
                indicator = window;
                return true;
            }
        }
        return false;
    }, std::chrono::seconds{5})) << "a drop indicator";
    if (indicator != 0) {
        EXPECT_EQ(property(indicator, 67 /* WM_CLASS */), (std::string{"i3-drag\0i3-drag\0", 16}));
    }
    ASSERT_NO_FATAL_FAILURE(button1(false));

    EXPECT_TRUE(poll_until([&] {
        std::vector<json> after;
        window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), after);
        return after.size() == 2 && after[0].at("window").get<std::uint32_t>() == windows[1] &&
               after[1].at("window").get<std::uint32_t>() == windows[0];
    }, std::chrono::seconds{5})) << "the windows swapped places";
    if (indicator != 0) {
        EXPECT_TRUE(poll_until([&] { return !exists(indicator); }, std::chrono::seconds{5})) << "the indicator is destroyed";
    }
}

/*
 * A window that its client unmaps is released (tree_close_internal() without killing it): i3 reparents it to the root
 * window at its container's position, sets its WM_STATE to Withdrawn (ICCCM 4.1.3.1) and stops managing it.
 */
TEST_F(ManageWindowTest, AWindowUnmappedByItsClientIsReleased) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    std::vector<json> containers;
    window_containers(request(i3ipc::MESSAGE_TYPE::GET_TREE), containers);
    ASSERT_EQ(containers.size(), 1U);
    auto const rect = containers[0].at("rect");
    auto const wm_state = atom("WM_STATE");
    EXPECT_EQ(property(window, wm_state), (std::string{"\x01\0\0\0\0\0\0\0", 8})) << "Normal while managed";

    ASSERT_FALSE(xcb::xproto::unmap_window_checked(&client(), window).get());

    ASSERT_TRUE(poll_until([&] { return !manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    ASSERT_NO_FATAL_FAILURE(command("nop"));
    EXPECT_EQ(parent_of(window), root());
    auto const geometry = xcb::xproto::get_geometry(&client(), window).get();
    ASSERT_TRUE(geometry);
    EXPECT_EQ(geometry->x, rect.at("x").get<int>());
    EXPECT_EQ(geometry->y, rect.at("y").get<int>());
    EXPECT_EQ(property(window, wm_state), std::string(8, '\0')) << "Withdrawn, no icon window";
}

/*
 * A floating window has I3_FLOATING_WINDOW (CARDINAL 1), which goes when it tiles again (floating_set_hint_atom()), and
 * "move position mouse" centres it on the pointer (floating_move_to_pointer(): QueryPointer).
 */
TEST_F(ManageWindowTest, AFloatingWindowIsMarkedAndMovesToThePointer) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    auto const floating_window = atom("I3_FLOATING_WINDOW");
    EXPECT_EQ(property(window, floating_window), std::nullopt);

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));
    EXPECT_EQ(property(window, floating_window), (std::string{"\x01\0\0\0", 4}));

    ASSERT_NO_FATAL_FAILURE(move_pointer(400, 300));
    ASSERT_TRUE(poll_until([&] { return pointer() == std::pair{400, 300}; }, std::chrono::seconds{5}));
    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] move position mouse", window)));
    std::vector<json> floating;
    std::function<void(json const &)> find = [&](json const &node) {
        if (node.value("type", "") == "floating_con") {
            floating.push_back(node);
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    find(child);
                }
            }
        }
    };
    find(request(i3ipc::MESSAGE_TYPE::GET_TREE));
    ASSERT_EQ(floating.size(), 1U);
    auto const rect = floating[0].at("rect");
    EXPECT_EQ(rect.at("x").get<int>(), 400 - (rect.at("width").get<int>() / 2));
    EXPECT_EQ(rect.at("y").get<int>(), 300 - (rect.at("height").get<int>() / 2));

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating disable", window)));
    EXPECT_EQ(property(window, floating_window), std::nullopt);
}

/*
 * _NET_REQUEST_FRAME_EXTENTS (EWMH): i3 answers a window that isn't mapped yet with its estimate in
 * _NET_FRAME_EXTENTS: the default border width left, right and bottom, the title bar's height on top.
 */
TEST_F(ManageWindowTest, FrameExtentsAreEstimatedOnRequest) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    auto const frame_extents = atom("_NET_FRAME_EXTENTS");

    ASSERT_NO_FATAL_FAILURE(client_message(window, atom("_NET_REQUEST_FRAME_EXTENTS"), {}));

    std::optional<std::string> extents;
    ASSERT_TRUE(poll_until([&] { return (extents = property(window, frame_extents)).has_value(); }, std::chrono::seconds{5}));
    std::string const value = extents.value_or(std::string{});
    ASSERT_EQ(value.size(), 16U);
    std::array<std::uint32_t, 4> values{};
    std::memcpy(values.data(), value.data(), 16);
    EXPECT_EQ(values[0], 2U); /* left: the default border width */
    EXPECT_EQ(values[1], 2U); /* right */
    EXPECT_GT(values[2], 2U); /* top: the title bar */
    EXPECT_EQ(values[3], 2U); /* bottom */
}

/* WM_CHANGE_STATE to Iconic (ICCCM 4.1.4) is refused: i3 puts the window's WM_STATE back to Normal. */
TEST_F(ManageWindowTest, IconicStateIsRefused) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    auto const wm_state = atom("WM_STATE");
    /* Iconic (3), as a client that went ahead would have it. */
    std::array<std::uint8_t, 8> iconic{3, 0, 0, 0, 0, 0, 0, 0};
    xcb::xproto::change_property(&client(), xcb::xproto::prop_mode_t::replace, window, wm_state, wm_state, 32, iconic);
    ASSERT_EQ(property(window, wm_state), (std::string{"\x03\0\0\0\0\0\0\0", 8}));

    ASSERT_NO_FATAL_FAILURE(client_message(window, atom("WM_CHANGE_STATE"), {3, 0, 0, 0, 0}));

    EXPECT_TRUE(poll_until([&] { return property(window, wm_state) == std::string{"\x01\0\0\0\0\0\0\0", 8}; }, std::chrono::seconds{5}))
        << "WM_STATE is Normal again";
}

/*
 * _NET_WM_MOVERESIZE with _NET_WM_MOVERESIZE_MOVE (EWMH), as a client-side decorated window sends when its header bar
 * is dragged: i3 drags the floating window from the given pointer position (handle_move_resize() hands drag_pointer() a
 * ButtonPress of its own) until a button is released.
 */
TEST_F(ManageWindowTest, NetWmMoveresizeDragsAFloatingWindow) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));
    auto const frame = parent_of(window);
    auto const before = xcb::xproto::get_geometry(&client(), frame).get();
    ASSERT_TRUE(before);
    ASSERT_NO_FATAL_FAILURE(move_pointer(400, 300));
    ASSERT_TRUE(poll_until([&] { return pointer() == std::pair{400, 300}; }, std::chrono::seconds{5}));

    ASSERT_NO_FATAL_FAILURE(client_message(window, atom("_NET_WM_MOVERESIZE"), {400, 300, 8 /* _NET_WM_MOVERESIZE_MOVE */, 1, 0}));
    ASSERT_TRUE(poll_until([&] { return !keyboard_grabbable(); }, std::chrono::seconds{5})) << "i3's drag loop grabs the keyboard";
    ASSERT_NO_FATAL_FAILURE(move_pointer(450, 340));
    ASSERT_NO_FATAL_FAILURE(button1(true));
    ASSERT_NO_FATAL_FAILURE(button1(false));

    ASSERT_TRUE(poll_until([&] { return keyboard_grabbable(); }, std::chrono::seconds{5})) << "the drag is over";
    EXPECT_TRUE(poll_until([&] {
        auto const geometry = xcb::xproto::get_geometry(&client(), frame).get();
        return geometry && geometry->x == before->x + 50 && geometry->y == before->y + 40;
    }, std::chrono::seconds{5})) << "the window moved with the pointer";
}

/*
 * The ignore list past 65536 requests, where the 16-bit sequence of i3's events and errors has wrapped: i3 compares
 * their full_sequence with the 64-bit numbers of its requests. A window that its client destroys is released
 * (tree_close_internal()), and i3's requests on it fail with BadWindow. i3 put two of them on the ignore list,
 * ReparentWindow and ChangeProperty, and logs their errors as expected. It logs the others as received: two
 * DeleteProperty, ChangeWindowAttributes, UnmapWindow, ChangeSaveSet and ShapeSelectInput. While i3 compared the 16-bit
 * sequence, it took the two for unexpected errors once it had sent 65536 requests.
 */
TEST_F(ManageWindowTest, ErrorsOnTheIgnoreListAreIgnoredPast65536Requests) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const net_wm_name = atom("_NET_WM_NAME");
    auto const utf8_string = atom("UTF8_STRING");
    auto const busy = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), busy).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), busy); }, std::chrono::seconds{5}));

    /* For each new title i3 reads the property and redraws the window's decoration, with about 30 requests (3000 titles
     * took it to request 87845 on 2026-10-24). */
    constexpr int titles = 6000;
    for (int i = 1; i <= titles; ++i) {
        std::string const title = std::format("title {}", i);
        std::vector<xcb::xproto::xcbvoid_t> bytes{title.begin(), title.end()};
        xcb::xproto::change_property(&client(), xcb::xproto::prop_mode_t::replace, busy, net_wm_name, utf8_string, 8, bytes);
    }
    client().flush();
    ASSERT_TRUE(poll_until([&] { return window_of_container(request(i3ipc::MESSAGE_TYPE::GET_TREE), std::format("title {}", titles)) == busy; },
                           std::chrono::seconds{60}))
        << "i3 shows the last title";

    auto const window = create_window();
    ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
    ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    auto const log = work_dir() / "i3.log";
    /* x_push_node() logs the number of the MapWindow request of the window's frame. */
    std::string const mapping = std::format("mapping container {:08x} (serial ", parent_of(window));
    std::string const before = read_file(log);
    auto const at = before.rfind(mapping);
    ASSERT_NE(at, std::string::npos) << "i3 logs " << mapping;
    std::uint64_t serial = 0;
    std::from_chars(before.data() + at + mapping.size(), before.data() + before.size(), serial);
    ASSERT_GT(serial, 65536U) << "i3 has sent more than 65536 requests";

    ASSERT_FALSE(xcb::xproto::destroy_window_checked(&client(), window).get());

    ASSERT_TRUE(poll_until([&] { return !manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}));
    std::string_view const ignored = "Expected X11 Error received for sequence";
    std::string_view const received = "X11 Error received (probably harmless)!";
    auto const errors_logged = [&] {
        std::string const after = read_file(log).substr(before.size());
        return std::pair{occurrences(after, ignored), occurrences(after, received)};
    };
    EXPECT_TRUE(poll_until([&] { auto const [i, r] = errors_logged(); return i + r >= 8; }, std::chrono::seconds{5}))
        << "i3 logs the errors of the eight requests";
    auto const [ignored_errors, received_errors] = errors_logged();
    EXPECT_EQ(ignored_errors, 2U) << "the errors of ReparentWindow and ChangeProperty";
    EXPECT_EQ(received_errors, 6U);
}

/* The properties i3 reads of a window, through GetProperty on xcbpp (property_request, xcb.icccm's parsers) */

namespace {
    /* The container of X window @p window in i3's tree @p node, std::nullopt if it has none. */
    std::optional<json> container_of(json const &node, std::uint32_t window) {
        if (node.contains("window") && node.at("window").is_number() && node.at("window").get<std::uint32_t>() == window) {
            return node;
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    if (auto found = container_of(child, window)) {
                        return found;
                    }
                }
            }
        }
        return std::nullopt;
    }

    /* The name of the workspace (or dock area) whose subtree of @p node holds X window @p window, "" if none. */
    std::string workspace_of(json const &node, std::uint32_t window, std::string const &workspace = {}) {
        auto const here = (node.value("type", "") == "workspace" || node.value("type", "") == "dockarea") ? node.value("name", "") : workspace;
        if (node.contains("window") && node.at("window").is_number() && node.at("window").get<std::uint32_t>() == window) {
            return here;
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    if (auto found = workspace_of(child, window, here); !found.empty()) {
                        return found;
                    }
                }
            }
        }
        return {};
    }

    std::vector<xcb::xproto::xcbvoid_t> bytes_of(std::string_view value) {
        return {value.begin(), value.end()};
    }

    std::vector<xcb::xproto::xcbvoid_t> bytes_of(std::vector<std::uint32_t> const &values) {
        std::vector<xcb::xproto::xcbvoid_t> bytes(values.size() * 4);
        std::memcpy(bytes.data(), values.data(), bytes.size());
        return bytes;
    }

    constexpr std::uint32_t ATOM = 4, CARDINAL = 6, STRING = 31, WINDOW = 33, WM_HINTS = 35, WM_CLIENT_MACHINE = 36, WM_NAME = 39,
                            WM_NORMAL_HINTS = 40, WM_SIZE_HINTS = 41, WM_CLASS = 67, WM_TRANSIENT_FOR = 68;
}  // namespace

class WindowPropertiesTest : public ManageWindowTest {
  protected:
    void set(std::uint32_t window, std::uint32_t property, std::uint32_t type, std::uint8_t format,
             std::vector<xcb::xproto::xcbvoid_t> bytes) {
        ASSERT_FALSE(xcb::xproto::change_property_checked(&client(), xcb::xproto::prop_mode_t::replace, window, property, type, format,
                                                          bytes)
                         .get());
    }

    /* The window_properties of @p window's container, null if i3 doesn't manage it. */
    json properties(std::uint32_t window) {
        auto const container = container_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), window);
        return container ? container->value("window_properties", json::object()) : json{};
    }

    /* @p window's container, an empty object if i3 doesn't manage it. */
    json container(std::uint32_t window) {
        return container_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), window).value_or(json::object());
    }

    void map_and_wait(std::uint32_t window) {
        ASSERT_FALSE(xcb::xproto::map_window_checked(&client(), window).get());
        ASSERT_TRUE(poll_until([&] { return manages(request(i3ipc::MESSAGE_TYPE::GET_TREE), window); }, std::chrono::seconds{5}))
            << "i3 manages the window";
    }
};

/*
 * manage_window() reads WM_CLASS, WM_NAME, _NET_WM_NAME, WM_WINDOW_ROLE, WM_CLIENT_MACHINE and WM_TRANSIENT_FOR
 * (xcb.icccm's parse_wm_transient_for()) with its other properties, and the tree has them. A WM_CLASS without its
 * trailing NUL is read to its end.
 */
TEST_F(WindowPropertiesTest, AreReadWhenTheWindowIsManaged) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const parent = create_window();
    auto const window = create_window();
    ASSERT_NO_FATAL_FAILURE(set(window, WM_CLASS, STRING, 8, bytes_of(std::string_view{"inst\0Class", 10})));
    ASSERT_NO_FATAL_FAILURE(set(window, WM_NAME, STRING, 8, bytes_of("legacy title")));
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_NAME"), atom("UTF8_STRING"), 8, bytes_of("caf\xc3\xa9")));
    ASSERT_NO_FATAL_FAILURE(set(window, atom("WM_WINDOW_ROLE"), STRING, 8, bytes_of("a role")));
    ASSERT_NO_FATAL_FAILURE(set(window, WM_CLIENT_MACHINE, STRING, 8, bytes_of("a host")));
    ASSERT_NO_FATAL_FAILURE(set(window, WM_TRANSIENT_FOR, WINDOW, 32, bytes_of({parent})));

    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));

    auto const p = properties(window);
    EXPECT_EQ(p.value("instance", ""), "inst");
    EXPECT_EQ(p.value("class", ""), "Class");
    EXPECT_EQ(p.value("title", ""), "caf\xc3\xa9");
    EXPECT_EQ(p.value("window_role", ""), "a role");
    EXPECT_EQ(p.value("machine", ""), "a host");
    EXPECT_EQ(p.value("transient_for", 0U), parent);
}

/*
 * PropertyNotify: i3 reads the property again (property_request, any type, as long as its handler asks) and the tree
 * follows. A deleted WM_TRANSIENT_FOR clears it; one of another type than WINDOW is no WM_TRANSIENT_FOR, and leaves
 * it as it was.
 */
TEST_F(WindowPropertiesTest, FollowTheirChanges) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const parent = create_window();
    auto const window = create_window();
    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));

    ASSERT_NO_FATAL_FAILURE(set(window, WM_CLASS, STRING, 8, bytes_of(std::string_view{"one\0One\0", 8})));
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_NAME"), atom("UTF8_STRING"), 8, bytes_of("first")));
    ASSERT_NO_FATAL_FAILURE(set(window, atom("WM_WINDOW_ROLE"), STRING, 8, bytes_of("role one")));
    ASSERT_NO_FATAL_FAILURE(set(window, WM_CLIENT_MACHINE, STRING, 8, bytes_of("host one")));
    ASSERT_NO_FATAL_FAILURE(set(window, WM_TRANSIENT_FOR, WINDOW, 32, bytes_of({parent})));
    EXPECT_TRUE(poll_until([&] {
        auto const p = properties(window);
        return p.value("class", "") == "One" && p.value("instance", "") == "one" && p.value("title", "") == "first" &&
               p.value("window_role", "") == "role one" && p.value("machine", "") == "host one" && p.value("transient_for", 0U) == parent;
    }, std::chrono::seconds{5})) << properties(window).dump();

    /* WM_TRANSIENT_FOR of type CARDINAL isn't one: i3 keeps the window it had. */
    ASSERT_NO_FATAL_FAILURE(set(window, WM_TRANSIENT_FOR, CARDINAL, 32, bytes_of({0x1234})));
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_NAME"), atom("UTF8_STRING"), 8, bytes_of("second")));
    ASSERT_TRUE(poll_until([&] { return properties(window).value("title", "") == "second"; }, std::chrono::seconds{5}));
    EXPECT_EQ(properties(window).value("transient_for", 0U), parent);

    ASSERT_FALSE(xcb::xproto::delete_property_checked(&client(), window, WM_TRANSIENT_FOR).get());
    EXPECT_TRUE(poll_until([&] { return !properties(window).contains("transient_for"); }, std::chrono::seconds{5}))
        << "a deleted WM_TRANSIENT_FOR clears it";
}

/*
 * WM_HINTS' urgency hint makes a window that isn't focused urgent, and clearing it makes it not urgent again. i3 reads
 * WM_HINTS whole on PropertyNotify: one of more than 9 elements is read as its first 9 (xcb-icccm copied up to 72 bytes
 * into its 36-byte struct, past i3's stack variable). One of fewer than 8 isn't WM_HINTS, and the window isn't urgent.
 */
TEST_F(WindowPropertiesTest, UrgencyFollowsWmHints) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));
    auto const focused = create_window();
    ASSERT_NO_FATAL_FAILURE(map_and_wait(focused));
    ASSERT_TRUE(poll_until([&] { return container(focused).value("focused", false); }, std::chrono::seconds{5}));
    constexpr std::uint32_t urgency = 1U << 8U, input = 1U;

    ASSERT_NO_FATAL_FAILURE(set(window, WM_HINTS, WM_HINTS, 32, bytes_of({urgency | input, 1, 0, 0, 0, 0, 0, 0, 0})));
    EXPECT_TRUE(poll_until([&] { return container(window).value("urgent", false); }, std::chrono::seconds{5})) << "urgent";

    ASSERT_NO_FATAL_FAILURE(set(window, WM_HINTS, WM_HINTS, 32, bytes_of({input, 1, 0, 0, 0, 0, 0, 0, 0})));
    EXPECT_TRUE(poll_until([&] { return !container(window).value("urgent", true); }, std::chrono::seconds{5})) << "not urgent";

    ASSERT_NO_FATAL_FAILURE(set(window, WM_HINTS, WM_HINTS, 32, bytes_of({urgency, 1, 0, 0, 0, 0, 0, 0, 0, 0xdead, 0xbeef, 0xcafe, 0xf00d})));
    EXPECT_TRUE(poll_until([&] { return container(window).value("urgent", false); }, std::chrono::seconds{5})) << "urgent, from 13 elements";

    ASSERT_NO_FATAL_FAILURE(set(window, WM_HINTS, WM_HINTS, 32, bytes_of({urgency, 1, 0, 0, 0, 0, 0})));
    EXPECT_TRUE(poll_until([&] { return !container(window).value("urgent", true); }, std::chrono::seconds{5})) << "7 elements aren't WM_HINTS";
    ASSERT_NO_FATAL_FAILURE(command("nop"));
    EXPECT_TRUE(i3_process().running()) << "i3 is still running";
}

/*
 * WM_NORMAL_HINTS (xcb.icccm's parse_wm_size_hints()): read when the window is managed (18 elements, type
 * WM_SIZE_HINTS), its minimum size bounds the window once it floats; read again on PropertyNotify (the whole property,
 * any type), its maximum size bounds it when it floats again. (handle_normal_hints() checks the size of the window's
 * container, not of its floating container, so the change itself doesn't resize it, at HEAD too.)
 */
TEST_F(WindowPropertiesTest, NormalHintsBoundAFloatingWindow) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    constexpr std::uint32_t p_min_size = 1U << 4U, p_max_size = 1U << 5U;
    std::vector<std::uint32_t> hints(18, 0);
    hints[0] = p_min_size;
    hints[5] = 600; /* min_width */
    hints[6] = 500; /* min_height */
    ASSERT_NO_FATAL_FAILURE(set(window, WM_NORMAL_HINTS, WM_SIZE_HINTS, 32, bytes_of(hints)));
    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));
    auto rect = container(window).value("window_rect", json::object());
    EXPECT_GE(rect.value("width", 0), 600) << rect.dump();
    EXPECT_GE(rect.value("height", 0), 500) << rect.dump();

    hints[0] = p_max_size;
    hints[7] = 300; /* max_width */
    hints[8] = 250; /* max_height */
    ASSERT_NO_FATAL_FAILURE(set(window, WM_NORMAL_HINTS, WM_SIZE_HINTS, 32, bytes_of(hints)));
    ASSERT_TRUE(poll_until([&] { return read_file(work_dir() / "i3.log").contains("Maximum size: 300 (width) x 250 (height)"); },
                           std::chrono::seconds{5}))
        << "i3 reads the new WM_NORMAL_HINTS";

    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating disable", window)));
    ASSERT_NO_FATAL_FAILURE(command(std::format("[id={}] floating enable", window)));
    rect = container(window).value("window_rect", json::object());
    EXPECT_LE(rect.value("width", 0), 300) << rect.dump();
    EXPECT_LE(rect.value("height", 0), 250) << rect.dump();
}

/*
 * _NET_WM_WINDOW_TYPE_DOCK with _NET_WM_STRUT_PARTIAL: a dock client, in the dock area that its strut names; a new
 * strut on PropertyNotify moves it to the other one.
 */
TEST_F(WindowPropertiesTest, TheStrutPlacesADockClient) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    auto const window = create_window();
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_WINDOW_TYPE"), ATOM, 32, bytes_of({atom("_NET_WM_WINDOW_TYPE_DOCK")})));
    std::vector<std::uint32_t> strut(12, 0);
    strut[2] = 20; /* top */
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_STRUT_PARTIAL"), CARDINAL, 32, bytes_of(strut)));
    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));
    EXPECT_EQ(workspace_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), window), "topdock");

    strut[2] = 0;
    strut[3] = 20; /* bottom */
    ASSERT_NO_FATAL_FAILURE(set(window, atom("_NET_WM_STRUT_PARTIAL"), CARDINAL, 32, bytes_of(strut)));
    EXPECT_TRUE(poll_until([&] { return workspace_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), window) == "bottomdock"; }, std::chrono::seconds{5}));
}

/*
 * _NET_STARTUP_ID: a window of a program that i3 started (exec, with startup notification) goes to the workspace the
 * program was started on, though another one is focused now; so does one whose WM_CLIENT_LEADER has the ID, if it has
 * none (startup_sequence_get() reads the leader's _NET_STARTUP_ID through property_request).
 */
TEST_F(WindowPropertiesTest, TheStartupIdPlacesTheWindowOnItsWorkspace) {
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(connect_client());
    ASSERT_NO_FATAL_FAILURE(command("workspace started-here"));
    ASSERT_NO_FATAL_FAILURE(command("exec true"));
    std::string startup_id;
    ASSERT_TRUE(poll_until([&] {
        std::string const log = read_file(work_dir() / "i3.log");
        std::string_view const marker = "startup id = ";
        auto const at = log.rfind(marker);
        if (at == std::string::npos) {
            return false;
        }
        startup_id = log.substr(at + marker.size(), log.find('\n', at) - at - marker.size());
        return !startup_id.empty();
    }, std::chrono::seconds{5})) << "i3 logs the startup ID";
    ASSERT_NO_FATAL_FAILURE(command("workspace elsewhere"));
    auto const net_startup_id = atom("_NET_STARTUP_ID");

    auto const window = create_window();
    ASSERT_NO_FATAL_FAILURE(set(window, net_startup_id, atom("UTF8_STRING"), 8, bytes_of(startup_id)));
    ASSERT_NO_FATAL_FAILURE(map_and_wait(window));
    EXPECT_EQ(workspace_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), window), "started-here");

    auto const leader = create_window();
    ASSERT_NO_FATAL_FAILURE(set(leader, net_startup_id, atom("UTF8_STRING"), 8, bytes_of(startup_id)));
    auto const led = create_window();
    ASSERT_NO_FATAL_FAILURE(set(led, atom("WM_CLIENT_LEADER"), WINDOW, 32, bytes_of({leader})));
    ASSERT_NO_FATAL_FAILURE(map_and_wait(led));
    EXPECT_EQ(workspace_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), led), "started-here");

    auto const other = create_window();
    ASSERT_NO_FATAL_FAILURE(map_and_wait(other));
    EXPECT_EQ(workspace_of(request(i3ipc::MESSAGE_TYPE::GET_TREE), other), "elsewhere");
}
