module;
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <xcb/xcb.h>
export module mocks;

import i3;

export {
    class MockX : public X {
    public:
        MockX() : X(nullptr) {
            root_depth = 32;
        };

        void con_init(Con *con, std::optional<xcb_drawable_t> id = std::nullopt) override {
        }
    };

    class MockRandR : public RandR {
    public:
        MockRandR(X &x, ConfigurationManager &configManager, WorkspaceManager &workspaceManager, TreeManager &treeManager) : RandR(x, configManager, workspaceManager, treeManager) {}
        MOCK_METHOD(Output*, get_output_by_name, (const std::string &name, const bool require_active), (override));
    };

}