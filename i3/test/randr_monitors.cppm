/*
 * RandR 1.5 monitors for the tests: what `xrandr --setmonitor`, `--delmonitor` and `--output … --primary` do, through
 * xcbpp.
 *
 * A monitor is a named rectangle of a screen, which i3 makes an output of (RandR::randr_query_outputs_15()). Monitors
 * on one Xvfb screen are how a test gives i3 several outputs: i3 manages its default screen only.
 */
module;
#include <cstdint>
export module randr_monitors;

import std;
import xcb;
import xcb.proto.randr;

export {
    /** A RandR monitor. */
    struct Monitor {
        std::string name;
        std::int16_t x{0};
        std::int16_t y{0};
        std::uint16_t width{0};
        std::uint16_t height{0};
        bool primary{false};
        /** The RandR outputs it shows. An output that is part of no monitor has a monitor of its own, an automatic
         *  one named after it: Xvfb's output "screen" covers its whole screen until a monitor takes it. */
        std::vector<std::string> outputs{};

        bool operator==(Monitor const &) const = default;

        friend std::ostream &operator<<(std::ostream &out, Monitor const &monitor) {
            out << std::format("{}{} {}x{}+{}+{}", monitor.primary ? "*" : "", monitor.name, monitor.width, monitor.height, monitor.x, monitor.y);
            for (auto const &output : monitor.outputs) {
                out << ' ' << output;
            }
            return out;
        }
    };

    /** Sets, deletes and lists the monitors of screen 0 of a display, on a connection of its own. */
    class RandrMonitors {
      public:
        /** Connects to @p display, and checks that its server has RandR 1.5. */
        [[nodiscard]] static std::expected<RandrMonitors, std::string> connect(std::string const &display) {
            int screen = 0;
            auto connection = xcb::connect(display, &screen);
            if (!connection) {
                return std::unexpected{std::format("can't connect to {}: error {}", display, std::to_underlying(connection.error()))};
            }
            auto const *setup = xcb::get_setup(**connection);
            auto const roots = setup->get_roots();
            if (screen < 0 || static_cast<std::size_t>(screen) >= roots.size()) {
                return std::unexpected{std::format("{} has no screen {}", display, screen)};
            }
            xcb::xproto::window_t const root = roots[static_cast<std::size_t>(screen)].root;

            auto const extension = xcb::get_extension_data(**connection, xcb::randr::id);
            if (!extension || !extension->present) {
                return std::unexpected{std::format("{} has no RandR", display)};
            }
            auto const version = xcb::randr::query_version(connection->get(), 1, 6).get();
            if (!version) {
                return std::unexpected{std::format("RRQueryVersion failed: X error {}", version.error().error_code)};
            }
            if (version->major_version < 1 || (version->major_version == 1 && version->minor_version < 5)) {
                return std::unexpected{std::format("{} has RandR {}.{}, not 1.5", display, version->major_version, version->minor_version)};
            }
            return RandrMonitors{std::move(*connection), root};
        }

        /** Adds @p monitor (RRSetMonitor). The server has made the change when this returns. Xvfb refuses the name
         *  of a monitor it has already (BadValue, as `xrandr --setmonitor` gets): remove() that one first. */
        std::expected<void, std::string> set(Monitor const &monitor) {
            std::vector<xcb::randr::output_t> outputs;
            for (auto const &name : monitor.outputs) {
                auto const output = output_named(name);
                if (!output) {
                    return std::unexpected{output.error()};
                }
                outputs.push_back(*output);
            }
            auto const name = atom(monitor.name);
            if (!name) {
                return std::unexpected{name.error()};
            }

            /* MONITORINFO is followed by its outputs on the wire, and the request sends xcb_sizeof() of it. */
            static_assert(sizeof(xcb::randr::monitor_info_t) == 24 && sizeof(xcb::randr::output_t) == 4);
            std::vector<std::uint32_t> buffer(sizeof(xcb::randr::monitor_info_t) / 4 + outputs.size());
            auto* info = ::new (static_cast<void*>(buffer.data())) xcb::randr::monitor_info_t{
                .name = *name,
                .primary = monitor.primary,
                .automatic = false,
                .nOutput = static_cast<std::uint16_t>(outputs.size()),
                .x = monitor.x,
                .y = monitor.y,
                .width = monitor.width,
                .height = monitor.height,
                /* As `xrandr --setmonitor` with W/mmW: at 96 DPI. */
                .width_in_millimeters = static_cast<std::uint32_t>(monitor.width * 254 / 960),
                .height_in_millimeters = static_cast<std::uint32_t>(monitor.height * 254 / 960),
            };
            std::ranges::copy(outputs, buffer.begin() + sizeof(xcb::randr::monitor_info_t) / 4);

            if (auto const error = xcb::randr::set_monitor_checked(connection_.get(), root_, info).get()) {
                return std::unexpected{std::format("RRSetMonitor {}: X error {}", monitor.name, error->error_code)};
            }
            return {};
        }

        /** Deletes the monitor named @p name (RRDeleteMonitor). The server has made the change when this returns. */
        std::expected<void, std::string> remove(std::string const &name) {
            auto const atom_of_name = atom(name);
            if (!atom_of_name) {
                return std::unexpected{atom_of_name.error()};
            }
            if (auto const error = xcb::randr::delete_monitor_checked(connection_.get(), root_, *atom_of_name).get()) {
                return std::unexpected{std::format("RRDeleteMonitor {}: X error {}", name, error->error_code)};
            }
            return {};
        }

        /** Makes the RandR output named @p name the primary output (RRSetOutputPrimary, as `xrandr --output … --primary`).
         *  The server has made the change when this returns. */
        std::expected<void, std::string> set_primary_output(std::string const &name) {
            auto const output = output_named(name);
            if (!output) {
                return std::unexpected{output.error()};
            }
            if (auto const error = xcb::randr::set_output_primary_checked(connection_.get(), root_, *output).get()) {
                return std::unexpected{std::format("RRSetOutputPrimary {}: X error {}", name, error->error_code)};
            }
            return {};
        }

        /** The active monitors (RRGetMonitors), as i3 asks for them, automatic ones included. */
        std::expected<std::vector<Monitor>, std::string> list() {
            auto const reply = xcb::randr::get_monitors(connection_.get(), root_, true).get();
            if (!reply) {
                return std::unexpected{std::format("RRGetMonitors: X error {}", reply.error().error_code)};
            }
            auto const resources = xcb::randr::get_screen_resources_current(connection_.get(), root_).get();
            if (!resources) {
                return std::unexpected{std::format("RRGetScreenResourcesCurrent: X error {}", resources.error().error_code)};
            }
            std::vector<Monitor> monitors;
            for (auto const &info : reply->monitors) {
                auto const name = xcb::xproto::get_atom_name(connection_.get(), info.name).get();
                if (!name) {
                    return std::unexpected{std::format("GetAtomName {}: X error {}", info.name, name.error().error_code)};
                }
                Monitor monitor{.name = name->name, .x = info.x, .y = info.y, .width = info.width, .height = info.height, .primary = info.primary != 0};
                for (auto const output : info.outputs) {
                    auto const output_info = xcb::randr::get_output_info(connection_.get(), output, resources->config_timestamp).get();
                    if (!output_info) {
                        return std::unexpected{std::format("RRGetOutputInfo {}: X error {}", output, output_info.error().error_code)};
                    }
                    monitor.outputs.emplace_back(reinterpret_cast<char const *>(output_info->name.data()), output_info->name.size());
                }
                monitors.push_back(std::move(monitor));
            }
            return monitors;
        }

      private:
        RandrMonitors(std::unique_ptr<xcb::Connection> connection, xcb::xproto::window_t root) : connection_(std::move(connection)), root_(root) {
        }

        std::expected<xcb::xproto::atom_t, std::string> atom(std::string const &name) {
            auto const reply = xcb::xproto::intern_atom(connection_.get(), false, name).get();
            if (!reply) {
                return std::unexpected{std::format("InternAtom {}: X error {}", name, reply.error().error_code)};
            }
            return reply->atom;
        }

        std::expected<xcb::randr::output_t, std::string> output_named(std::string const& name) {
            auto const resources = xcb::randr::get_screen_resources_current(connection_.get(), root_).get();
            if (!resources) {
                return std::unexpected{std::format("RRGetScreenResourcesCurrent: X error {}", resources.error().error_code)};
            }
            for (auto const output : resources->outputs) {
                auto const info = xcb::randr::get_output_info(connection_.get(), output, resources->config_timestamp).get();
                if (info && std::string_view{reinterpret_cast<char const *>(info->name.data()), info->name.size()} == name) {
                    return output;
                }
            }
            return std::unexpected{std::format("the screen has no RandR output {}", name)};
        }

        std::unique_ptr<xcb::Connection> connection_;
        xcb::xproto::window_t root_;
    };
}
