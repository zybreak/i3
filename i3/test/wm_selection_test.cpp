/*
 * i3 taking the WM_Sn selection at startup (main.cpp's aquire_selection()) on Xvfb, with a test client on xcbpp: the
 * owner window, the MANAGER announcement, refusing to start while another window manager owns it, and --replace.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

import std;
import i3ipc;
import i3_runner;
import xvfb_fixture;
import xcb;

using ::testing::ElementsAre;

namespace {
    class WmSelectionTest : public XvfbTest {
      protected:
        void SetUp() override {
            XvfbTest::SetUp();
            if (HasFatalFailure()) {
                return;
            }
            int screen = 0;
            auto connection = xcb::connect(server().display(), &screen);
            ASSERT_TRUE(connection) << "error " << std::to_underlying(connection.error());
            client_ = std::move(*connection);
            root_ = xcb::get_setup(*client_)->get_roots()[static_cast<std::size_t>(screen)].root;
            wm_s0_ = atom("WM_S0");
        }

        /* The options of an i3 named @p name (its configuration, IPC socket and log are named after it). */
        [[nodiscard]] I3Options i3_options(std::string const &name, std::vector<std::string> arguments = {}) const {
            auto const config = work_dir() / std::format("{}.config", name);
            auto const ipc = work_dir() / std::format("{}.ipc", name);
            std::ofstream{config} << std::format("# i3 config file (v4)\nfont pango:monospace 8\nipc-socket {}\n", ipc.native());
            arguments.insert(arguments.begin(), {"-V", "-d", "all"});
            return {.binary = I3_TEST_I3, .config = config, .ipc_socket = ipc, .log = work_dir() / std::format("{}.log", name), .arguments = arguments};
        }

        [[nodiscard]] xcb::Connection &client() {
            return *client_;
        }

        [[nodiscard]] std::uint32_t root() const noexcept {
            return root_;
        }

        [[nodiscard]] std::uint32_t wm_s0() const noexcept {
            return wm_s0_;
        }

        std::uint32_t atom(std::string_view name) {
            auto const reply = xcb::xproto::intern_atom(&client(), false, name).get();
            EXPECT_TRUE(reply) << "InternAtom " << name;
            return reply ? reply->atom : 0;
        }

        /* The owner of WM_S0, 0 if none. */
        std::uint32_t selection_owner() {
            auto const reply = xcb::xproto::get_selection_owner(&client(), wm_s0_).get();
            return reply ? reply->owner : 0;
        }

        /* The next ClientMessage of type @p type that the client gets within five seconds. */
        std::optional<xcb::xproto::client_message_event_t> wait_for_client_message(std::uint32_t type) {
            auto const deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
            while (std::chrono::steady_clock::now() < deadline) {
                client().flush();
                if (auto const event = client().poll_for_event()) {
                    if ((event->response_type & 0x7fU) == 33 /* ClientMessage */) {
                        xcb::xproto::client_message_event_t message{};
                        std::memcpy(&message, event.get(), sizeof message);
                        if (message.type == type) {
                            return message;
                        }
                    }
                    continue;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
            }
            return std::nullopt;
        }

      private:
        std::unique_ptr<xcb::Connection> client_{};
        std::uint32_t root_{};
        std::uint32_t wm_s0_{};
    };
}  // namespace

/*
 * i3 owns WM_S0 with a window of its own (InputOutput, 1x1 at (-1, -1), WM_CLASS "i3-WM_Sn") and announces it with a
 * MANAGER ClientMessage to the root window (ICCCM 2.8): the timestamp, the selection and the owner.
 */
TEST_F(WmSelectionTest, I3OwnsTheSelectionAndAnnouncesIt) {
    ASSERT_FALSE(xcb::xproto::change_window_attributes_checked(&client(), root(),
                                                               xcb::xproto::ChangeWindowAttributesValueList{}.with_event_mask(
                                                                   xcb::xproto::event_mask_t::EVENT_MASK_STRUCTURE_NOTIFY))
                     .get());
    auto const manager = atom("MANAGER");
    auto i3 = I3Process::start(i3_options("i3"));
    ASSERT_TRUE(i3) << i3.error();

    auto const owner = selection_owner();
    ASSERT_NE(owner, 0U);
    auto const message = wait_for_client_message(manager);
    ASSERT_TRUE(message) << "a MANAGER ClientMessage";
    EXPECT_EQ(message->window, root());
    EXPECT_EQ(message->format, 32);
    EXPECT_NE(message->data.data32[0], 0U); /* the timestamp of the selection */
    EXPECT_EQ(message->data.data32[1], wm_s0());
    EXPECT_EQ(message->data.data32[2], owner);
    EXPECT_THAT((std::array{message->data.data32[3], message->data.data32[4]}), ElementsAre(0U, 0U));

    auto const geometry = xcb::xproto::get_geometry(&client(), owner).get();
    ASSERT_TRUE(geometry);
    EXPECT_EQ(geometry->x, -1);
    EXPECT_EQ(geometry->y, -1);
    EXPECT_EQ(geometry->width, 1);
    EXPECT_EQ(geometry->height, 1);
    auto const wm_class = xcb::xproto::get_property(&client(), false, owner, 67 /* WM_CLASS */, 31 /* STRING */, 0, 100).get();
    ASSERT_TRUE(wm_class);
    EXPECT_EQ(std::string_view(reinterpret_cast<char const *>(wm_class->value.data()), wm_class->value.size()),
              (std::string_view{"i3-WM_Sn\0i3-WM_Sn\0", 18}));
}

/* A second i3 doesn't start while the first one owns WM_S0; the first keeps it. */
TEST_F(WmSelectionTest, ASecondI3DoesNotStartWhileTheSelectionIsOwned) {
    auto first = I3Process::start(i3_options("first"));
    ASSERT_TRUE(first) << first.error();
    auto const owner = selection_owner();

    auto const options = i3_options("second");
    std::vector<std::string> argv{options.binary.native(), "-c", options.config.native()};
    argv.append_range(options.arguments);
    auto second = ChildProcess::spawn({.argv = argv, .output = options.log});
    ASSERT_TRUE(second) << second.error();
    auto const status = second->wait_for(std::chrono::seconds{10});

    ASSERT_TRUE(status) << "the second i3 ends";
    EXPECT_NE(*status, ExitStatus::exited(0)) << status->describe();
    EXPECT_THAT(read_file(options.log), ::testing::HasSubstr("Another window manager is already running (WM_Sn is owned)"));
    EXPECT_EQ(selection_owner(), owner);
    EXPECT_TRUE(first->process().running());
}

/*
 * With --replace, the second i3 takes WM_S0 over: the first gets SelectionClear and exits, and the second waits until
 * the first's owner window is gone (GetGeometry fails) before it goes on.
 */
TEST_F(WmSelectionTest, ReplaceTakesTheSelectionOver) {
    auto first = I3Process::start(i3_options("first"));
    ASSERT_TRUE(first) << first.error();
    auto const first_owner = selection_owner();

    auto second = I3Process::start(i3_options("second", {"--replace"}));
    ASSERT_TRUE(second) << second.error();

    EXPECT_EQ(first->process().wait_for(std::chrono::seconds{10}), ExitStatus::exited(0));
    auto const second_owner = selection_owner();
    EXPECT_NE(second_owner, 0U);
    EXPECT_NE(second_owner, first_owner);
    EXPECT_FALSE(xcb::xproto::get_geometry(&client(), first_owner).get()) << "the first i3's owner window is gone";
    EXPECT_THAT(read_file(work_dir() / "second.log"), ::testing::HasSubstr("waiting for old WM_Sn selection owner to exit"));
}
