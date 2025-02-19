#include <gtest/gtest.h>
#include <gmock/gmock.h>

import i3;
import mocks;

TEST(WorkspaceManagerTest, References_Does_Not_Work){
    MockX x{};
    ConfigurationManager configManager{x};
    WorkspaceManager workspaceManager{configManager};
    
    workspaceManager.add_workspace_config(WorkspaceConfig{"1", "output"});
    
    auto optConf = workspaceManager.get_workspace_config("1");
    
    ASSERT_TRUE(optConf.has_value());
    
    optConf->output = "eDP-1";
    
    auto optConf2 = workspaceManager.get_workspace_config("1");
    
    ASSERT_EQ(optConf2->output, "output");
}

TEST(WorkspaceManagerTest, DISABLED_FindTheRightOutput){
    MockX x{};
    Output output{};
    output.names.push_back("eDP-1");
    global.x = &x;
    ConfigurationManager configManager{x};
    WorkspaceManager workspaceManager{configManager};
    TreeManager treeManager{x};
    MockRandR randr{x, configManager, workspaceManager, treeManager};
    global.randr = &randr;
    
    EXPECT_CALL(randr, get_output_by_name("eDP-1",testing::_))
    .Times(1)
    .WillOnce(testing::Return(&output));
    
    workspaceManager.add_workspace_config(WorkspaceConfig{"1", "wrong-output"});
    workspaceManager.add_workspace_config(WorkspaceConfig{"name", "eDP-1"});

    auto output2 = workspaceManager.get_assigned_output("name", 1);

    ASSERT_TRUE(output2 == &output);
}
