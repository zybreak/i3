
// Generated from configGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "configGrammar.h"


/**
 * This interface defines an abstract listener for a parse tree produced by configGrammar.
 */
class  configGrammarListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterConfig(configGrammar::ConfigContext *ctx) = 0;
  virtual void exitConfig(configGrammar::ConfigContext *ctx) = 0;

  virtual void enterStmt(configGrammar::StmtContext *ctx) = 0;
  virtual void exitStmt(configGrammar::StmtContext *ctx) = 0;

  virtual void enterCommands(configGrammar::CommandsContext *ctx) = 0;
  virtual void exitCommands(configGrammar::CommandsContext *ctx) = 0;

  virtual void enterSet(configGrammar::SetContext *ctx) = 0;
  virtual void exitSet(configGrammar::SetContext *ctx) = 0;

  virtual void enterBinding(configGrammar::BindingContext *ctx) = 0;
  virtual void exitBinding(configGrammar::BindingContext *ctx) = 0;

  virtual void enterFont(configGrammar::FontContext *ctx) = 0;
  virtual void exitFont(configGrammar::FontContext *ctx) = 0;

  virtual void enterFloating_minimum_size(configGrammar::Floating_minimum_sizeContext *ctx) = 0;
  virtual void exitFloating_minimum_size(configGrammar::Floating_minimum_sizeContext *ctx) = 0;

  virtual void enterFloating_maximum_size(configGrammar::Floating_maximum_sizeContext *ctx) = 0;
  virtual void exitFloating_maximum_size(configGrammar::Floating_maximum_sizeContext *ctx) = 0;

  virtual void enterFloating_modifier(configGrammar::Floating_modifierContext *ctx) = 0;
  virtual void exitFloating_modifier(configGrammar::Floating_modifierContext *ctx) = 0;

  virtual void enterDefault_orientation(configGrammar::Default_orientationContext *ctx) = 0;
  virtual void exitDefault_orientation(configGrammar::Default_orientationContext *ctx) = 0;

  virtual void enterWorkspace_layout(configGrammar::Workspace_layoutContext *ctx) = 0;
  virtual void exitWorkspace_layout(configGrammar::Workspace_layoutContext *ctx) = 0;

  virtual void enterDefault_border(configGrammar::Default_borderContext *ctx) = 0;
  virtual void exitDefault_border(configGrammar::Default_borderContext *ctx) = 0;

  virtual void enterHide_edge_borders(configGrammar::Hide_edge_bordersContext *ctx) = 0;
  virtual void exitHide_edge_borders(configGrammar::Hide_edge_bordersContext *ctx) = 0;

  virtual void enterFor_window(configGrammar::For_windowContext *ctx) = 0;
  virtual void exitFor_window(configGrammar::For_windowContext *ctx) = 0;

  virtual void enterAssign(configGrammar::AssignContext *ctx) = 0;
  virtual void exitAssign(configGrammar::AssignContext *ctx) = 0;

  virtual void enterNo_focus(configGrammar::No_focusContext *ctx) = 0;
  virtual void exitNo_focus(configGrammar::No_focusContext *ctx) = 0;

  virtual void enterFocus_follows_mouse(configGrammar::Focus_follows_mouseContext *ctx) = 0;
  virtual void exitFocus_follows_mouse(configGrammar::Focus_follows_mouseContext *ctx) = 0;

  virtual void enterMouse_warping(configGrammar::Mouse_warpingContext *ctx) = 0;
  virtual void exitMouse_warping(configGrammar::Mouse_warpingContext *ctx) = 0;

  virtual void enterFocus_wrapping(configGrammar::Focus_wrappingContext *ctx) = 0;
  virtual void exitFocus_wrapping(configGrammar::Focus_wrappingContext *ctx) = 0;

  virtual void enterForce_focus_wrapping(configGrammar::Force_focus_wrappingContext *ctx) = 0;
  virtual void exitForce_focus_wrapping(configGrammar::Force_focus_wrappingContext *ctx) = 0;

  virtual void enterWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext *ctx) = 0;
  virtual void exitWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext *ctx) = 0;

  virtual void enterForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext *ctx) = 0;
  virtual void exitForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext *ctx) = 0;

  virtual void enterFocus_on_window_activation(configGrammar::Focus_on_window_activationContext *ctx) = 0;
  virtual void exitFocus_on_window_activation(configGrammar::Focus_on_window_activationContext *ctx) = 0;

  virtual void enterTitle_align(configGrammar::Title_alignContext *ctx) = 0;
  virtual void exitTitle_align(configGrammar::Title_alignContext *ctx) = 0;

  virtual void enterWorkspace(configGrammar::WorkspaceContext *ctx) = 0;
  virtual void exitWorkspace(configGrammar::WorkspaceContext *ctx) = 0;

  virtual void enterIpc_socket(configGrammar::Ipc_socketContext *ctx) = 0;
  virtual void exitIpc_socket(configGrammar::Ipc_socketContext *ctx) = 0;

  virtual void enterIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext *ctx) = 0;
  virtual void exitIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext *ctx) = 0;

  virtual void enterRestart_state(configGrammar::Restart_stateContext *ctx) = 0;
  virtual void exitRestart_state(configGrammar::Restart_stateContext *ctx) = 0;

  virtual void enterPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext *ctx) = 0;
  virtual void exitPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext *ctx) = 0;

  virtual void enterExec(configGrammar::ExecContext *ctx) = 0;
  virtual void exitExec(configGrammar::ExecContext *ctx) = 0;

  virtual void enterDimension(configGrammar::DimensionContext *ctx) = 0;
  virtual void exitDimension(configGrammar::DimensionContext *ctx) = 0;

  virtual void enterCommand(configGrammar::CommandContext *ctx) = 0;
  virtual void exitCommand(configGrammar::CommandContext *ctx) = 0;

  virtual void enterCommand_move(configGrammar::Command_moveContext *ctx) = 0;
  virtual void exitCommand_move(configGrammar::Command_moveContext *ctx) = 0;

  virtual void enterCommand_exec(configGrammar::Command_execContext *ctx) = 0;
  virtual void exitCommand_exec(configGrammar::Command_execContext *ctx) = 0;

  virtual void enterCommand_exit(configGrammar::Command_exitContext *ctx) = 0;
  virtual void exitCommand_exit(configGrammar::Command_exitContext *ctx) = 0;

  virtual void enterCommand_kill(configGrammar::Command_killContext *ctx) = 0;
  virtual void exitCommand_kill(configGrammar::Command_killContext *ctx) = 0;

  virtual void enterCommand_fullscreen(configGrammar::Command_fullscreenContext *ctx) = 0;
  virtual void exitCommand_fullscreen(configGrammar::Command_fullscreenContext *ctx) = 0;

  virtual void enterCommand_sticky(configGrammar::Command_stickyContext *ctx) = 0;
  virtual void exitCommand_sticky(configGrammar::Command_stickyContext *ctx) = 0;

  virtual void enterCommand_debuglog(configGrammar::Command_debuglogContext *ctx) = 0;
  virtual void exitCommand_debuglog(configGrammar::Command_debuglogContext *ctx) = 0;

  virtual void enterCommand_border(configGrammar::Command_borderContext *ctx) = 0;
  virtual void exitCommand_border(configGrammar::Command_borderContext *ctx) = 0;

  virtual void enterCommand_layout(configGrammar::Command_layoutContext *ctx) = 0;
  virtual void exitCommand_layout(configGrammar::Command_layoutContext *ctx) = 0;

  virtual void enterCommand_append_layout(configGrammar::Command_append_layoutContext *ctx) = 0;
  virtual void exitCommand_append_layout(configGrammar::Command_append_layoutContext *ctx) = 0;

  virtual void enterCommand_workspace(configGrammar::Command_workspaceContext *ctx) = 0;
  virtual void exitCommand_workspace(configGrammar::Command_workspaceContext *ctx) = 0;

  virtual void enterCommand_focus(configGrammar::Command_focusContext *ctx) = 0;
  virtual void exitCommand_focus(configGrammar::Command_focusContext *ctx) = 0;

  virtual void enterCommand_split(configGrammar::Command_splitContext *ctx) = 0;
  virtual void exitCommand_split(configGrammar::Command_splitContext *ctx) = 0;

  virtual void enterCommand_floating(configGrammar::Command_floatingContext *ctx) = 0;
  virtual void exitCommand_floating(configGrammar::Command_floatingContext *ctx) = 0;

  virtual void enterCommand_resize(configGrammar::Command_resizeContext *ctx) = 0;
  virtual void exitCommand_resize(configGrammar::Command_resizeContext *ctx) = 0;

  virtual void enterCommand_rename(configGrammar::Command_renameContext *ctx) = 0;
  virtual void exitCommand_rename(configGrammar::Command_renameContext *ctx) = 0;

  virtual void enterCommand_nop(configGrammar::Command_nopContext *ctx) = 0;
  virtual void exitCommand_nop(configGrammar::Command_nopContext *ctx) = 0;

  virtual void enterCommand_reload(configGrammar::Command_reloadContext *ctx) = 0;
  virtual void exitCommand_reload(configGrammar::Command_reloadContext *ctx) = 0;

  virtual void enterCommand_restart(configGrammar::Command_restartContext *ctx) = 0;
  virtual void exitCommand_restart(configGrammar::Command_restartContext *ctx) = 0;

  virtual void enterCommand_mode(configGrammar::Command_modeContext *ctx) = 0;
  virtual void exitCommand_mode(configGrammar::Command_modeContext *ctx) = 0;

  virtual void enterCommand_open(configGrammar::Command_openContext *ctx) = 0;
  virtual void exitCommand_open(configGrammar::Command_openContext *ctx) = 0;

  virtual void enterCommand_title_format(configGrammar::Command_title_formatContext *ctx) = 0;
  virtual void exitCommand_title_format(configGrammar::Command_title_formatContext *ctx) = 0;

  virtual void enterCommand_title_window_icon(configGrammar::Command_title_window_iconContext *ctx) = 0;
  virtual void exitCommand_title_window_icon(configGrammar::Command_title_window_iconContext *ctx) = 0;

  virtual void enterCriteria(configGrammar::CriteriaContext *ctx) = 0;
  virtual void exitCriteria(configGrammar::CriteriaContext *ctx) = 0;

  virtual void enterCriterion(configGrammar::CriterionContext *ctx) = 0;
  virtual void exitCriterion(configGrammar::CriterionContext *ctx) = 0;

  virtual void enterValue_criterion(configGrammar::Value_criterionContext *ctx) = 0;
  virtual void exitValue_criterion(configGrammar::Value_criterionContext *ctx) = 0;


};

