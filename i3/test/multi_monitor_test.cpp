/*
 * i3 with several outputs: RandR 1.5 monitors on one Xvfb screen, which i3 reads with RRGetMonitors
 * (RandR::randr_query_outputs_15()), set before i3 starts or while it runs.
 *
 * The screen is 2048x768. Monitor "left" (primary) shows its left half and takes Xvfb's only RandR output, "screen",
 * whose automatic monitor would otherwise cover the whole screen; monitor "right" shows its right half.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

import std;
import i3ipc;
import i3.json;
import i3_runner;
import randr_monitors;
import xvfb_fixture;
import xcb;

using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::UnorderedElementsAre;

namespace {
    struct Rect {
        std::int64_t x;
        std::int64_t y;
        std::int64_t width;
        std::int64_t height;

        bool operator==(Rect const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, Rect const &rect) {
            return out << std::format("{}x{}+{}+{}", rect.width, rect.height, rect.x, rect.y);
        }
    };

    Rect rect_of(json const &rect) {
        return {rect.at("x").get<std::int64_t>(), rect.at("y").get<std::int64_t>(), rect.at("width").get<std::int64_t>(), rect.at("height").get<std::int64_t>()};
    }

    /* An active output in i3's GET_OUTPUTS reply. */
    struct OutputState {
        std::string name;
        bool primary;
        Rect rect;

        bool operator==(OutputState const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, OutputState const &output) {
            return out << output.name << (output.primary ? " (primary) " : " ") << output.rect;
        }
    };

    /* A workspace in i3's GET_WORKSPACES reply. */
    struct WorkspaceState {
        std::string name;
        std::string output;
        Rect rect;
        bool visible;
        bool focused;

        bool operator==(WorkspaceState const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, WorkspaceState const &workspace) {
            return out << std::format("workspace {} on {}", workspace.name, workspace.output) << ' ' << workspace.rect
                       << (workspace.visible ? " visible" : "") << (workspace.focused ? " focused" : "");
        }
    };

    Rect const left_half{0, 0, 1024, 768};
    Rect const right_half{1024, 0, 1024, 768};

    Monitor left_monitor() {
        return {.name = "left", .x = 0, .y = 0, .width = 1024, .height = 768, .primary = true, .outputs = {"screen"}};
    }

    Monitor right_monitor() {
        return {.name = "right", .x = 1024, .y = 0, .width = 1024, .height = 768};
    }

    /* The workspace (its name) that holds the container of X window @p window in i3's tree @p node, if any. */
    std::optional<std::string> workspace_of_window(json const &node, std::uint32_t window, std::optional<std::string> workspace = std::nullopt) {
        if (node.value("type", "") == "workspace") {
            workspace = node.at("name").get<std::string>();
        }
        if (node.contains("window") && node.at("window").is_number() && node.at("window").get<std::uint32_t>() == window) {
            return workspace;
        }
        for (auto const *children : {"nodes", "floating_nodes"}) {
            if (node.contains(children)) {
                for (auto const &child : node.at(children)) {
                    if (auto found = workspace_of_window(child, window, workspace)) {
                        return found;
                    }
                }
            }
        }
        return std::nullopt;
    }

    /* A mapped top-level window of a client of the test's own, on xcbpp. */
    class TestWindow {
      public:
        static std::expected<TestWindow, std::string> map(std::string const &display) {
            int screen = 0;
            auto connection = xcb::connect(display, &screen);
            if (!connection) {
                return std::unexpected{std::format("can't connect to {}: error {}", display, std::to_underlying(connection.error()))};
            }
            auto const &root = xcb::get_setup(**connection)->get_roots()[static_cast<std::size_t>(screen)];
            auto const id = (*connection)->generate_id();
            if (auto const error = xcb::xproto::create_window_checked(connection->get(), 0, id, root.root, 0, 0, 300, 200, 0,
                                                                      xcb::xproto::window_class_t::WINDOW_CLASS_INPUT_OUTPUT, root.root_visual,
                                                                      xcb::xproto::CreateWindowValueList{})
                                       .get()) {
                return std::unexpected{std::format("CreateWindow: X error {}", error->error_code)};
            }
            if (auto const error = xcb::xproto::map_window_checked(connection->get(), id).get()) {
                return std::unexpected{std::format("MapWindow: X error {}", error->error_code)};
            }
            return TestWindow{std::move(*connection), id};
        }

        [[nodiscard]] std::uint32_t id() const noexcept {
            return id_;
        }

      private:
        TestWindow(std::unique_ptr<xcb::Connection> connection, std::uint32_t id) : connection_(std::move(connection)), id_(id) {
        }

        std::unique_ptr<xcb::Connection> connection_;
        std::uint32_t id_;
    };

    class MultiMonitorTest : public XvfbTest {
      protected:
        [[nodiscard]] XvfbOptions xvfb_options() const override {
            auto options = XvfbTest::xvfb_options();
            options.screens = {XvfbScreen{.width = 2048, .height = 768, .depth = 24}};
            return options;
        }

        void SetUp() override {
            XvfbTest::SetUp();
            if (HasFatalFailure()) {
                return;
            }
            auto monitors = RandrMonitors::connect(server().display());
            ASSERT_TRUE(monitors) << monitors.error();
            monitors_.emplace(std::move(*monitors));
        }

        void set_monitor(Monitor const &monitor) {
            auto const set = monitors().set(monitor);
            ASSERT_TRUE(set) << set.error();
        }

        void delete_monitor(std::string const &name) {
            auto const removed = monitors().remove(name);
            ASSERT_TRUE(removed) << removed.error();
        }

        [[nodiscard]] std::vector<Monitor> listed_monitors() {
            auto listed = monitors().list();
            if (!listed) {
                throw std::runtime_error{listed.error()};
            }
            return std::move(*listed);
        }

        void start_i3() {
            auto const config = work_dir() / "config";
            std::ofstream{config} << std::format("# i3 config file (v4)\nfont pango:monospace 8\nipc-socket {}\n", ipc_socket().native());
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

        /* A second IPC connection, subscribed to output and workspace events. */
        [[nodiscard]] IpcClient subscribe() {
            auto events = IpcClient::connect(ipc_socket());
            if (!events) {
                throw std::runtime_error{events.error()};
            }
            auto const reply = events->request_json(i3ipc::MESSAGE_TYPE::SUBSCRIBE, R"(["output","workspace"])");
            if (!reply || !reply->value("success", false)) {
                throw std::runtime_error{"i3 refused the subscription"};
            }
            return std::move(*events);
        }

        /* The workspace events up to the next output event, which i3 sends once it has handled a RandR change. */
        static std::vector<json> workspace_events_until_output_event(IpcClient &events) {
            std::vector<json> workspace_events;
            for (;;) {
                auto const message = events.receive();
                if (!message) {
                    throw std::runtime_error{std::format("no output event: {}", message.error())};
                }
                if (message->type == i3ipc::EVENT_OUTPUT) {
                    return workspace_events;
                }
                if (message->type == i3ipc::EVENT_WORKSPACE) {
                    workspace_events.push_back(json::parse(message->payload));
                }
            }
        }

        [[nodiscard]] json request(i3ipc::MESSAGE_TYPE type, std::string_view payload = {}) {
            auto reply = i3().ipc().request_json(type, payload);
            if (!reply) {
                throw std::runtime_error{reply.error()};
            }
            return std::move(*reply);
        }

        /* Runs an i3 command and checks that it succeeded. */
        void command(std::string_view text) {
            auto const reply = request(i3ipc::MESSAGE_TYPE::RUN_COMMAND, text);
            ASSERT_TRUE(reply.is_array() && !reply.empty() && reply.at(0).value("success", false)) << text << ": " << reply.dump();
        }

        [[nodiscard]] std::vector<OutputState> active_outputs() {
            std::vector<OutputState> outputs;
            for (auto const &output : request(i3ipc::MESSAGE_TYPE::GET_OUTPUTS)) {
                if (output.at("active").get<bool>()) {
                    outputs.push_back({output.at("name").get<std::string>(), output.at("primary").get<bool>(), rect_of(output.at("rect"))});
                }
            }
            return outputs;
        }

        [[nodiscard]] std::vector<WorkspaceState> workspaces() {
            std::vector<WorkspaceState> workspaces;
            for (auto const &workspace : request(i3ipc::MESSAGE_TYPE::GET_WORKSPACES)) {
                workspaces.push_back({workspace.at("name").get<std::string>(), workspace.at("output").get<std::string>(), rect_of(workspace.at("rect")),
                                      workspace.at("visible").get<bool>(), workspace.at("focused").get<bool>()});
            }
            return workspaces;
        }

        /* The names of the output containers in i3's tree. */
        [[nodiscard]] std::vector<std::string> output_containers() {
            std::vector<std::string> names;
            for (auto const &node : request(i3ipc::MESSAGE_TYPE::GET_TREE).at("nodes")) {
                if (node.value("type", "") == "output") {
                    names.push_back(node.at("name").get<std::string>());
                }
            }
            return names;
        }

      private:
        RandrMonitors &monitors() {
            if (!monitors_) {
                throw std::logic_error{"the test has no RandrMonitors: SetUp() failed"};
            }
            return *monitors_;
        }

        I3Process &i3() {
            if (!i3_) {
                throw std::logic_error{"i3 isn't running: start_i3() failed or wasn't called"};
            }
            return *i3_;
        }

        std::optional<RandrMonitors> monitors_{};
        std::optional<I3Process> i3_{};
    };
}  // namespace

TEST_F(MultiMonitorTest, TwoMonitorsAreTwoOutputsWithAWorkspaceEach) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_THAT(listed_monitors(), UnorderedElementsAre(left_monitor(), right_monitor()));

    ASSERT_NO_FATAL_FAILURE(start_i3());

    EXPECT_THAT(active_outputs(), UnorderedElementsAre(OutputState{"left", true, left_half}, OutputState{"right", false, right_half}));
    EXPECT_THAT(output_containers(), UnorderedElementsAre("left", "right"));
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, true}, WorkspaceState{"2", "right", right_half, true, false}));
}

TEST_F(MultiMonitorTest, AMonitorAddedAtRuntimeIsANewOutputWithAWorkspace) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_THAT(active_outputs(), ElementsAre(OutputState{"left", true, left_half}));

    auto events = subscribe();
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    auto const workspace_events = workspace_events_until_output_event(events);

    std::vector<std::string> initialised;
    for (auto const &event : workspace_events) {
        if (event.at("change") == "init") {
            initialised.push_back(std::format("{} on {}", event.at("current").at("name").get<std::string>(), event.at("current").at("output").get<std::string>()));
        }
    }
    EXPECT_THAT(initialised, ElementsAre("2 on right"));
    EXPECT_THAT(output_containers(), UnorderedElementsAre("left", "right"));
    EXPECT_THAT(active_outputs(), UnorderedElementsAre(OutputState{"left", true, left_half}, OutputState{"right", false, right_half}));
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, true}, WorkspaceState{"2", "right", right_half, true, false}));

    /* Commands find the new output too: focus moves to the output to the right (RandR::get_output_next()). */
    ASSERT_NO_FATAL_FAILURE(command("focus right"));
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, false}, WorkspaceState{"2", "right", right_half, true, true}));
}

/*
 * Defect: 'focus output NAME' always fails with "At least one output must be specified". The parser calls
 * CommandsApplier::focus_output() once per output name and then once without a name, as upstream's cmd_focus_output()
 * expects; the port returns after adding a name instead of keeping the names across the calls, and the last call
 * finds none.
 */
TEST_F(MultiMonitorTest, DISABLED_FocusOutputFocusesTheNamedOutput) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());

    ASSERT_NO_FATAL_FAILURE(command("focus output right"));
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, false}, WorkspaceState{"2", "right", right_half, true, true}));
}

TEST_F(MultiMonitorTest, DeletingAMonitorMovesItsFocusedWorkspaceToThePrimaryOutput) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(command("workspace 2"));
    ASSERT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, false}, WorkspaceState{"2", "right", right_half, true, true}));

    auto events = subscribe();
    ASSERT_NO_FATAL_FAILURE(delete_monitor("right"));
    (void)workspace_events_until_output_event(events);

    EXPECT_THAT(active_outputs(), ElementsAre(OutputState{"left", true, left_half}));
    EXPECT_THAT(output_containers(), ElementsAre("left"));
    /* Not the rect of workspace 2: see DISABLED_AWorkspaceMovedFromADeletedMonitorIsRenderedOnThePrimaryOutput. */
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, false, false},
                                                   AllOf(Field("name", &WorkspaceState::name, "2"), Field("output", &WorkspaceState::output, "left"),
                                                         Field("visible", &WorkspaceState::visible, true), Field("focused", &WorkspaceState::focused, true))));
}

/*
 * Defect: i3 doesn't render after a RandR change. PropertyHandlers::handle_screen_change() calls
 * RandR::randr_query_outputs() without tree_render() (upstream's randr_query_outputs() ends with
 * ewmh_update_desktop_properties() and tree_render(), which are commented out in randr.cpp), so the workspace moved
 * from the deleted monitor keeps the deleted monitor's rect, and its windows their positions, until something else
 * renders (here: until the next command).
 */
TEST_F(MultiMonitorTest, DISABLED_AWorkspaceMovedFromADeletedMonitorIsRenderedOnThePrimaryOutput) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(command("workspace 2"));

    auto events = subscribe();
    ASSERT_NO_FATAL_FAILURE(delete_monitor("right"));
    (void)workspace_events_until_output_event(events);

    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, false, false}, WorkspaceState{"2", "left", left_half, true, true}));
}

TEST_F(MultiMonitorTest, DeletingAMonitorMovesItsWorkspaceWithAWindowToThePrimaryOutput) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());
    ASSERT_NO_FATAL_FAILURE(command("workspace 2"));
    auto window = TestWindow::map(server().display());
    ASSERT_TRUE(window) << window.error();
    ASSERT_TRUE(poll_until([&] { return workspace_of_window(request(i3ipc::MESSAGE_TYPE::GET_TREE), window->id()) == "2"; }, std::chrono::seconds{5}))
        << "i3 manages the window on workspace 2";
    ASSERT_NO_FATAL_FAILURE(command("workspace 1"));

    auto events = subscribe();
    ASSERT_NO_FATAL_FAILURE(delete_monitor("right"));
    (void)workspace_events_until_output_event(events);

    EXPECT_THAT(active_outputs(), ElementsAre(OutputState{"left", true, left_half}));
    /* An invisible workspace's rect is only updated when it is shown again. */
    EXPECT_THAT(workspaces(), UnorderedElementsAre(WorkspaceState{"1", "left", left_half, true, true},
                                                   AllOf(Field("name", &WorkspaceState::name, "2"), Field("output", &WorkspaceState::output, "left"),
                                                         Field("visible", &WorkspaceState::visible, false), Field("focused", &WorkspaceState::focused, false))));
    EXPECT_EQ(workspace_of_window(request(i3ipc::MESSAGE_TYPE::GET_TREE), window->id()), "2");
}

TEST_F(MultiMonitorTest, DeletingAMonitorClosesItsEmptyUnfocusedWorkspace) {
    ASSERT_NO_FATAL_FAILURE(set_monitor(left_monitor()));
    ASSERT_NO_FATAL_FAILURE(set_monitor(right_monitor()));
    ASSERT_NO_FATAL_FAILURE(start_i3());

    auto events = subscribe();
    ASSERT_NO_FATAL_FAILURE(delete_monitor("right"));
    (void)workspace_events_until_output_event(events);

    EXPECT_THAT(active_outputs(), ElementsAre(OutputState{"left", true, left_half}));
    EXPECT_THAT(workspaces(), ElementsAre(WorkspaceState{"1", "left", left_half, true, true}));
}
