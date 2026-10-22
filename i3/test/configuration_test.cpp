#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>

#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <utility>

import i3;
import bindings;

namespace {
/* Storage filled with a non-zero byte pattern before an object is constructed
 * in it, so that a member its constructor leaves indeterminate reads as 0xAA…
 * instead of happening to be zero. */
template <typename T>
class PoisonedStorage {
    alignas(T) std::array<std::byte, sizeof(T)> bytes;
    T *object{nullptr};

  public:
    PoisonedStorage() {
        bytes.fill(std::byte{0xAA});
    }

    PoisonedStorage(PoisonedStorage const &) = delete;
    PoisonedStorage &operator=(PoisonedStorage const &) = delete;

    ~PoisonedStorage() {
        if (object != nullptr) {
            std::destroy_at(object);
        }
    }

    /* T(args...): with no arguments this value-initialises, as
     * std::make_unique<T>() does. */
    template <typename... Args>
    T &value_initialise(Args &&...args) {
        object = ::new (static_cast<void *>(bytes.data())) T(std::forward<Args>(args)...);
        return *object;
    }

    /* `T t;` */
    T &default_initialise() {
        object = ::new (static_cast<void *>(bytes.data())) T;
        return *object;
    }
};

/* Upstream i3's load_configuration() (src/config.c) zeroes the configuration
 * and then sets a few defaults; ConfigurationManager::init_color() later sets
 * the colours and scales the border widths. */
void expect_upstream_defaults(Config &config) {
    EXPECT_EQ(config.default_layout, layout_t::L_DEFAULT);
    EXPECT_EQ(config.container_stack_limit, 0);
    EXPECT_EQ(config.container_stack_limit_value, 0);
    EXPECT_EQ(config.default_border_width, 2);
    EXPECT_EQ(config.default_floating_border_width, 2);
    EXPECT_EQ(config.default_orientation, orientation_t::NO_ORIENTATION);
    EXPECT_FALSE(config.disable_focus_follows_mouse);
    EXPECT_EQ(config.mouse_warping, warping_t::POINTER_WARPING_OUTPUT);
    EXPECT_EQ(config.hide_edge_borders, hide_edge_borders_mode_t::HEBM_NONE);
    EXPECT_FALSE(config.disable_workspace_bar);
    EXPECT_EQ(config.focus_wrapping, focus_wrapping_t::FOCUS_WRAPPING_ON);
    EXPECT_FALSE(config.workspace_auto_back_and_forth);
    EXPECT_FLOAT_EQ(config.workspace_urgency_timer, 0.5f);
    EXPECT_EQ(config.focus_on_window_activation, conf_fowa_t::FOWA_SMART);
    EXPECT_EQ(config.title_align, title_align_t::ALIGN_LEFT);
    EXPECT_EQ(config.default_border, border_style_t::BS_NORMAL);
    EXPECT_EQ(config.default_floating_border, border_style_t::BS_NORMAL);
    EXPECT_EQ(config.floating_modifier, 0u);
    EXPECT_EQ(config.floating_maximum_width, 0);
    EXPECT_EQ(config.floating_maximum_height, 0);
    EXPECT_EQ(config.floating_minimum_width, 0);
    EXPECT_EQ(config.floating_minimum_height, 0);
    EXPECT_FALSE(config.client.got_focused_tab_title);
    EXPECT_EQ(config.bar, config_bar{});
    EXPECT_EQ(config.popup_during_fullscreen, conf_pdf_t::PDF_SMART);
    EXPECT_EQ(config.tiling_drag, tiling_drag_t::TILING_DRAG_MODIFIER);
    EXPECT_EQ(config.swap_modifier, static_cast<uint32_t>(XCB_KEY_BUT_MASK_SHIFT));
    EXPECT_EQ(config.gaps, gaps_t{});
    EXPECT_EQ(config.smart_gaps, smart_gaps_t::SMART_GAPS_OFF);
    ASSERT_TRUE(config.modes.contains("default"));
    EXPECT_EQ(config.current_mode()->name, "default");
}
}  // namespace

/* ConfigurationManager::load_configuration() creates the configuration with
 * std::make_unique<Config>(). */
TEST(ConfigurationTest, ValueInitialisedConfigHasUpstreamDefaults) {
    PoisonedStorage<Config> storage;
    expect_upstream_defaults(storage.value_initialise());
}

TEST(ConfigurationTest, DefaultInitialisedConfigHasUpstreamDefaults) {
    PoisonedStorage<Config> storage;
    expect_upstream_defaults(storage.default_initialise());
}

/* Created by `workspace <ws> output <output>` and `workspace <ws> gaps …`;
 * gaps_for_workspace() reads gaps_mask. */
TEST(ConfigurationTest, WorkspaceConfigHasNoGaps) {
    PoisonedStorage<WorkspaceConfig> by_name;
    auto const &named = by_name.value_initialise("1");
    EXPECT_EQ(std::to_underlying(named.gaps_mask), 0);
    EXPECT_EQ(named.gaps, gaps_t{});

    PoisonedStorage<WorkspaceConfig> by_output;
    auto const &assigned = by_output.value_initialise("1", "output");
    EXPECT_EQ(std::to_underlying(assigned.gaps_mask), 0);
    EXPECT_EQ(assigned.gaps, gaps_t{});
}

TEST(ConfigurationTest, foo) {
    auto config = std::make_unique<Config>();
    
    config->ipc_socket_path = "/tmp/i3-ipc.sock";

    ASSERT_EQ(config->ipc_socket_path, "/tmp/i3-ipc.sock");
    
    std::optional<std::string> foo;
    
    foo = "hej";
}

TEST(ConfigurationTest, ModeEquality) {
    Binding_Keycode keycode1{0, 0};
    Binding binding1{};
    binding1.command = "command";
    binding1.keycodes.push_back(keycode1);
    
    Mode mode1{"1", false};
    mode1.bindings.push_back(binding1);

    Binding_Keycode keycode2{0, 0};
    Binding binding2{};
    binding2.command = "command";
    binding2.keycodes.push_back(keycode2);

    Mode mode2{"1", false};
    mode2.bindings.push_back(binding2);

    ASSERT_TRUE(mode1 == mode2);
    ASSERT_FALSE(&mode1 == &mode2);
}
