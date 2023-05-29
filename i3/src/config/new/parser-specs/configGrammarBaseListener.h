
// Generated from configGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "configGrammarListener.h"


/**
 * This class provides an empty implementation of configGrammarListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  configGrammarBaseListener : public configGrammarListener {
public:

  virtual void enterConfig(configGrammar::ConfigContext * /*ctx*/) override { }
  virtual void exitConfig(configGrammar::ConfigContext * /*ctx*/) override { }

  virtual void enterStmt(configGrammar::StmtContext * /*ctx*/) override { }
  virtual void exitStmt(configGrammar::StmtContext * /*ctx*/) override { }

  virtual void enterCommands(configGrammar::CommandsContext * /*ctx*/) override { }
  virtual void exitCommands(configGrammar::CommandsContext * /*ctx*/) override { }

  virtual void enterSet(configGrammar::SetContext * /*ctx*/) override { }
  virtual void exitSet(configGrammar::SetContext * /*ctx*/) override { }

  virtual void enterBinding(configGrammar::BindingContext * /*ctx*/) override { }
  virtual void exitBinding(configGrammar::BindingContext * /*ctx*/) override { }

  virtual void enterFont(configGrammar::FontContext * /*ctx*/) override { }
  virtual void exitFont(configGrammar::FontContext * /*ctx*/) override { }

  virtual void enterFloating_minimum_size(configGrammar::Floating_minimum_sizeContext * /*ctx*/) override { }
  virtual void exitFloating_minimum_size(configGrammar::Floating_minimum_sizeContext * /*ctx*/) override { }

  virtual void enterFloating_maximum_size(configGrammar::Floating_maximum_sizeContext * /*ctx*/) override { }
  virtual void exitFloating_maximum_size(configGrammar::Floating_maximum_sizeContext * /*ctx*/) override { }

  virtual void enterFloating_modifier(configGrammar::Floating_modifierContext * /*ctx*/) override { }
  virtual void exitFloating_modifier(configGrammar::Floating_modifierContext * /*ctx*/) override { }

  virtual void enterDefault_orientation(configGrammar::Default_orientationContext * /*ctx*/) override { }
  virtual void exitDefault_orientation(configGrammar::Default_orientationContext * /*ctx*/) override { }

  virtual void enterWorkspace_layout(configGrammar::Workspace_layoutContext * /*ctx*/) override { }
  virtual void exitWorkspace_layout(configGrammar::Workspace_layoutContext * /*ctx*/) override { }

  virtual void enterDefault_border(configGrammar::Default_borderContext * /*ctx*/) override { }
  virtual void exitDefault_border(configGrammar::Default_borderContext * /*ctx*/) override { }

  virtual void enterHide_edge_borders(configGrammar::Hide_edge_bordersContext * /*ctx*/) override { }
  virtual void exitHide_edge_borders(configGrammar::Hide_edge_bordersContext * /*ctx*/) override { }

  virtual void enterFor_window(configGrammar::For_windowContext * /*ctx*/) override { }
  virtual void exitFor_window(configGrammar::For_windowContext * /*ctx*/) override { }

  virtual void enterAssign(configGrammar::AssignContext * /*ctx*/) override { }
  virtual void exitAssign(configGrammar::AssignContext * /*ctx*/) override { }

  virtual void enterNo_focus(configGrammar::No_focusContext * /*ctx*/) override { }
  virtual void exitNo_focus(configGrammar::No_focusContext * /*ctx*/) override { }

  virtual void enterFocus_follows_mouse(configGrammar::Focus_follows_mouseContext * /*ctx*/) override { }
  virtual void exitFocus_follows_mouse(configGrammar::Focus_follows_mouseContext * /*ctx*/) override { }

  virtual void enterMouse_warping(configGrammar::Mouse_warpingContext * /*ctx*/) override { }
  virtual void exitMouse_warping(configGrammar::Mouse_warpingContext * /*ctx*/) override { }

  virtual void enterFocus_wrapping(configGrammar::Focus_wrappingContext * /*ctx*/) override { }
  virtual void exitFocus_wrapping(configGrammar::Focus_wrappingContext * /*ctx*/) override { }

  virtual void enterForce_focus_wrapping(configGrammar::Force_focus_wrappingContext * /*ctx*/) override { }
  virtual void exitForce_focus_wrapping(configGrammar::Force_focus_wrappingContext * /*ctx*/) override { }

  virtual void enterWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext * /*ctx*/) override { }
  virtual void exitWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext * /*ctx*/) override { }

  virtual void enterForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext * /*ctx*/) override { }
  virtual void exitForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext * /*ctx*/) override { }

  virtual void enterFocus_on_window_activation(configGrammar::Focus_on_window_activationContext * /*ctx*/) override { }
  virtual void exitFocus_on_window_activation(configGrammar::Focus_on_window_activationContext * /*ctx*/) override { }

  virtual void enterTitle_align(configGrammar::Title_alignContext * /*ctx*/) override { }
  virtual void exitTitle_align(configGrammar::Title_alignContext * /*ctx*/) override { }

  virtual void enterWorkspace(configGrammar::WorkspaceContext * /*ctx*/) override { }
  virtual void exitWorkspace(configGrammar::WorkspaceContext * /*ctx*/) override { }

  virtual void enterIpc_socket(configGrammar::Ipc_socketContext * /*ctx*/) override { }
  virtual void exitIpc_socket(configGrammar::Ipc_socketContext * /*ctx*/) override { }

  virtual void enterIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext * /*ctx*/) override { }
  virtual void exitIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext * /*ctx*/) override { }

  virtual void enterRestart_state(configGrammar::Restart_stateContext * /*ctx*/) override { }
  virtual void exitRestart_state(configGrammar::Restart_stateContext * /*ctx*/) override { }

  virtual void enterPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext * /*ctx*/) override { }
  virtual void exitPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext * /*ctx*/) override { }

  virtual void enterExec(configGrammar::ExecContext * /*ctx*/) override { }
  virtual void exitExec(configGrammar::ExecContext * /*ctx*/) override { }

  virtual void enterDimension(configGrammar::DimensionContext * /*ctx*/) override { }
  virtual void exitDimension(configGrammar::DimensionContext * /*ctx*/) override { }

  virtual void enterCommand(configGrammar::CommandContext * /*ctx*/) override { }
  virtual void exitCommand(configGrammar::CommandContext * /*ctx*/) override { }

  virtual void enterCommand_move(configGrammar::Command_moveContext * /*ctx*/) override { }
  virtual void exitCommand_move(configGrammar::Command_moveContext * /*ctx*/) override { }

  virtual void enterCommand_exec(configGrammar::Command_execContext * /*ctx*/) override { }
  virtual void exitCommand_exec(configGrammar::Command_execContext * /*ctx*/) override { }

  virtual void enterCommand_exit(configGrammar::Command_exitContext * /*ctx*/) override { }
  virtual void exitCommand_exit(configGrammar::Command_exitContext * /*ctx*/) override { }

  virtual void enterCommand_kill(configGrammar::Command_killContext * /*ctx*/) override { }
  virtual void exitCommand_kill(configGrammar::Command_killContext * /*ctx*/) override { }

  virtual void enterCommand_fullscreen(configGrammar::Command_fullscreenContext * /*ctx*/) override { }
  virtual void exitCommand_fullscreen(configGrammar::Command_fullscreenContext * /*ctx*/) override { }

  virtual void enterCommand_sticky(configGrammar::Command_stickyContext * /*ctx*/) override { }
  virtual void exitCommand_sticky(configGrammar::Command_stickyContext * /*ctx*/) override { }

  virtual void enterCommand_debuglog(configGrammar::Command_debuglogContext * /*ctx*/) override { }
  virtual void exitCommand_debuglog(configGrammar::Command_debuglogContext * /*ctx*/) override { }

  virtual void enterCommand_border(configGrammar::Command_borderContext * /*ctx*/) override { }
  virtual void exitCommand_border(configGrammar::Command_borderContext * /*ctx*/) override { }

  virtual void enterCommand_layout(configGrammar::Command_layoutContext * /*ctx*/) override { }
  virtual void exitCommand_layout(configGrammar::Command_layoutContext * /*ctx*/) override { }

  virtual void enterCommand_append_layout(configGrammar::Command_append_layoutContext * /*ctx*/) override { }
  virtual void exitCommand_append_layout(configGrammar::Command_append_layoutContext * /*ctx*/) override { }

  virtual void enterCommand_workspace(configGrammar::Command_workspaceContext * /*ctx*/) override { }
  virtual void exitCommand_workspace(configGrammar::Command_workspaceContext * /*ctx*/) override { }

  virtual void enterCommand_focus(configGrammar::Command_focusContext * /*ctx*/) override { }
  virtual void exitCommand_focus(configGrammar::Command_focusContext * /*ctx*/) override { }

  virtual void enterCommand_split(configGrammar::Command_splitContext * /*ctx*/) override { }
  virtual void exitCommand_split(configGrammar::Command_splitContext * /*ctx*/) override { }

  virtual void enterCommand_floating(configGrammar::Command_floatingContext * /*ctx*/) override { }
  virtual void exitCommand_floating(configGrammar::Command_floatingContext * /*ctx*/) override { }

  virtual void enterCommand_resize(configGrammar::Command_resizeContext * /*ctx*/) override { }
  virtual void exitCommand_resize(configGrammar::Command_resizeContext * /*ctx*/) override { }

  virtual void enterCommand_rename(configGrammar::Command_renameContext * /*ctx*/) override { }
  virtual void exitCommand_rename(configGrammar::Command_renameContext * /*ctx*/) override { }

  virtual void enterCommand_nop(configGrammar::Command_nopContext * /*ctx*/) override { }
  virtual void exitCommand_nop(configGrammar::Command_nopContext * /*ctx*/) override { }

  virtual void enterCommand_reload(configGrammar::Command_reloadContext * /*ctx*/) override { }
  virtual void exitCommand_reload(configGrammar::Command_reloadContext * /*ctx*/) override { }

  virtual void enterCommand_restart(configGrammar::Command_restartContext * /*ctx*/) override { }
  virtual void exitCommand_restart(configGrammar::Command_restartContext * /*ctx*/) override { }

  virtual void enterCommand_mode(configGrammar::Command_modeContext * /*ctx*/) override { }
  virtual void exitCommand_mode(configGrammar::Command_modeContext * /*ctx*/) override { }

  virtual void enterCommand_open(configGrammar::Command_openContext * /*ctx*/) override { }
  virtual void exitCommand_open(configGrammar::Command_openContext * /*ctx*/) override { }

  virtual void enterCommand_scratchpad(configGrammar::Command_scratchpadContext * /*ctx*/) override { }
  virtual void exitCommand_scratchpad(configGrammar::Command_scratchpadContext * /*ctx*/) override { }

  virtual void enterCommand_title_format(configGrammar::Command_title_formatContext * /*ctx*/) override { }
  virtual void exitCommand_title_format(configGrammar::Command_title_formatContext * /*ctx*/) override { }

  virtual void enterCommand_title_window_icon(configGrammar::Command_title_window_iconContext * /*ctx*/) override { }
  virtual void exitCommand_title_window_icon(configGrammar::Command_title_window_iconContext * /*ctx*/) override { }

  virtual void enterCriteria(configGrammar::CriteriaContext * /*ctx*/) override { }
  virtual void exitCriteria(configGrammar::CriteriaContext * /*ctx*/) override { }

  virtual void enterCriterion(configGrammar::CriterionContext * /*ctx*/) override { }
  virtual void exitCriterion(configGrammar::CriterionContext * /*ctx*/) override { }

  virtual void enterValue_criterion(configGrammar::Value_criterionContext * /*ctx*/) override { }
  virtual void exitValue_criterion(configGrammar::Value_criterionContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

