module;
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>
export module mocks;

import std;
import i3;
import xcb;
import xcb.xcbcompat;
export import fake_x_server;

export {
    /* An X whose connection a fake X server set up and then closed: requests on it are dropped and no reply arrives,
     * so that code which sends requests (x_con_kill(), …) runs in tests. It creates no frames. */
    class MockX : public X {
    public:
        MockX() : X(xcb::xcbcompat::adopt(disconnected_connection()), 0) {
            root_depth = 32;
        };

        void con_init(Con *con, std::optional<xcb_drawable_t> id = std::nullopt) override {
        }
    };

    /* i3's X on a FakeXServer's connection, for tests of the requests i3's X code sends. Destroy it before the server.
     * The cursors are made-up XIDs instead of xcb-cursor's (xcursor_load_cursors()). */
    class FakeX : public X {
    public:
        static constexpr xcb_cursor_t first_cursor = 0x7000;

        explicit FakeX(FakeXServer &server) : X(xcb::xcbcompat::adopt(server.connect()), 0) {
            for (auto const cursor : {XCURSOR_CURSOR_POINTER, XCURSOR_CURSOR_RESIZE_HORIZONTAL, XCURSOR_CURSOR_RESIZE_VERTICAL,
                                      XCURSOR_CURSOR_TOP_LEFT_CORNER, XCURSOR_CURSOR_TOP_RIGHT_CORNER,
                                      XCURSOR_CURSOR_BOTTOM_LEFT_CORNER, XCURSOR_CURSOR_BOTTOM_RIGHT_CORNER,
                                      XCURSOR_CURSOR_WATCH, XCURSOR_CURSOR_MOVE}) {
                cursors[cursor] = first_cursor + cursor;
            }
        }
    };

    class MockRandR : public RandR {
    public:
        MockRandR(X &x, ConfigurationManager &configManager, WorkspaceManager &workspaceManager, TreeManager &treeManager) : RandR(x, configManager, workspaceManager, treeManager) {}
        MOCK_METHOD(Output*, get_output_by_name, (const std::string &name, const bool require_active), (override));
    };

}
