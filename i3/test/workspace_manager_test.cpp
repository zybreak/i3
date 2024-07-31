#include <gtest/gtest.h>
#include <gmock/gmock.h>

import i3;

class MockX : public X {
public:
    MockX() : X(nullptr) {
        root_depth = 32;
    };
};

class MockRandR : public RandR {
public:
    MockRandR() {}
    MOCK_METHOD(Output*, get_output_by_name, (const std::string &name, const bool require_active), (override));
};

TEST(WorkspaceManagerTest, References_Does_Not_Work){
    WorkspaceManager workspaceManager{};
    
    workspaceManager.add_workspace_config(WorkspaceConfig{"1", "output"});
    
    auto optConf = workspaceManager.get_workspace_config("1");
    
    ASSERT_TRUE(optConf.has_value());
    
    optConf->output = "eDP-1";
    
    auto optConf2 = workspaceManager.get_workspace_config("1");
    
    ASSERT_EQ(optConf2->output, "output");
}

TEST(WorkspaceManagerTest, FindTheRightOutput){
    MockX x{};
    MockRandR randr{};
    Output output{};
    output.names.push_back("eDP-1");
    global.x = &x;
    global.randr = &randr;
    
    EXPECT_CALL(randr, get_output_by_name("eDP-1",testing::_))
    .Times(1)
    .WillOnce(testing::Return(&output));
    
    WorkspaceManager workspaceManager{};

    workspaceManager.add_workspace_config(WorkspaceConfig{"1", "wrong-output"});
    workspaceManager.add_workspace_config(WorkspaceConfig{"name", "eDP-1"});

    auto output2 = workspaceManager.get_assigned_output("name", 1);

    ASSERT_TRUE(output2 == &output);
}
