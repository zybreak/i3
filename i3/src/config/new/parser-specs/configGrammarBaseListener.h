
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

  void enterConfig(configGrammar::ConfigContext * /*ctx*/) override { }
  void exitConfig(configGrammar::ConfigContext * /*ctx*/) override { }

  void enterStmt(configGrammar::StmtContext * /*ctx*/) override { }
  void exitStmt(configGrammar::StmtContext * /*ctx*/) override { }

  void enterCommands(configGrammar::CommandsContext * /*ctx*/) override { }
  void exitCommands(configGrammar::CommandsContext * /*ctx*/) override { }

  void enterSet(configGrammar::SetContext * /*ctx*/) override { }
  void exitSet(configGrammar::SetContext * /*ctx*/) override { }

  void enterBinding(configGrammar::BindingContext * /*ctx*/) override { }
  void exitBinding(configGrammar::BindingContext * /*ctx*/) override { }

  void enterFont(configGrammar::FontContext * /*ctx*/) override { }
  void exitFont(configGrammar::FontContext * /*ctx*/) override { }

  void enterFloating_minimum_size(configGrammar::Floating_minimum_sizeContext * /*ctx*/) override { }
  void exitFloating_minimum_size(configGrammar::Floating_minimum_sizeContext * /*ctx*/) override { }

  void enterFloating_maximum_size(configGrammar::Floating_maximum_sizeContext * /*ctx*/) override { }
  void exitFloating_maximum_size(configGrammar::Floating_maximum_sizeContext * /*ctx*/) override { }

  void enterFloating_modifier(configGrammar::Floating_modifierContext * /*ctx*/) override { }
  void exitFloating_modifier(configGrammar::Floating_modifierContext * /*ctx*/) override { }

  void enterDefault_orientation(configGrammar::Default_orientationContext * /*ctx*/) override { }
  void exitDefault_orientation(configGrammar::Default_orientationContext * /*ctx*/) override { }

  void enterWorkspace_layout(configGrammar::Workspace_layoutContext * /*ctx*/) override { }
  void exitWorkspace_layout(configGrammar::Workspace_layoutContext * /*ctx*/) override { }

  void enterDefault_border(configGrammar::Default_borderContext * /*ctx*/) override { }
  void exitDefault_border(configGrammar::Default_borderContext * /*ctx*/) override { }

  void enterHide_edge_borders(configGrammar::Hide_edge_bordersContext * /*ctx*/) override { }
  void exitHide_edge_borders(configGrammar::Hide_edge_bordersContext * /*ctx*/) override { }

  void enterFor_window(configGrammar::For_windowContext * /*ctx*/) override { }
  void exitFor_window(configGrammar::For_windowContext * /*ctx*/) override { }

  void enterAssign(configGrammar::AssignContext * /*ctx*/) override { }
  void exitAssign(configGrammar::AssignContext * /*ctx*/) override { }

  void enterNo_focus(configGrammar::No_focusContext * /*ctx*/) override { }
  void exitNo_focus(configGrammar::No_focusContext * /*ctx*/) override { }

  void enterFocus_follows_mouse(configGrammar::Focus_follows_mouseContext * /*ctx*/) override { }
  void exitFocus_follows_mouse(configGrammar::Focus_follows_mouseContext * /*ctx*/) override { }

  void enterMouse_warping(configGrammar::Mouse_warpingContext * /*ctx*/) override { }
  void exitMouse_warping(configGrammar::Mouse_warpingContext * /*ctx*/) override { }

  void enterFocus_wrapping(configGrammar::Focus_wrappingContext * /*ctx*/) override { }
  void exitFocus_wrapping(configGrammar::Focus_wrappingContext * /*ctx*/) override { }

  void enterForce_focus_wrapping(configGrammar::Force_focus_wrappingContext * /*ctx*/) override { }
  void exitForce_focus_wrapping(configGrammar::Force_focus_wrappingContext * /*ctx*/) override { }

  void enterWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext * /*ctx*/) override { }
  void exitWorkspace_auto_back_and_forth(configGrammar::Workspace_auto_back_and_forthContext * /*ctx*/) override { }

  void enterForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext * /*ctx*/) override { }
  void exitForce_display_urgency_hint(configGrammar::Force_display_urgency_hintContext * /*ctx*/) override { }

  void enterFocus_on_window_activation(configGrammar::Focus_on_window_activationContext * /*ctx*/) override { }
  void exitFocus_on_window_activation(configGrammar::Focus_on_window_activationContext * /*ctx*/) override { }

  void enterTitle_align(configGrammar::Title_alignContext * /*ctx*/) override { }
  void exitTitle_align(configGrammar::Title_alignContext * /*ctx*/) override { }

  void enterWorkspace(configGrammar::WorkspaceContext * /*ctx*/) override { }
  void exitWorkspace(configGrammar::WorkspaceContext * /*ctx*/) override { }

  void enterIpc_socket(configGrammar::Ipc_socketContext * /*ctx*/) override { }
  void exitIpc_socket(configGrammar::Ipc_socketContext * /*ctx*/) override { }

  void enterIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext * /*ctx*/) override { }
  void exitIpc_kill_timeout(configGrammar::Ipc_kill_timeoutContext * /*ctx*/) override { }

  void enterRestart_state(configGrammar::Restart_stateContext * /*ctx*/) override { }
  void exitRestart_state(configGrammar::Restart_stateContext * /*ctx*/) override { }

  void enterPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext * /*ctx*/) override { }
  void exitPopup_during_fullscreen(configGrammar::Popup_during_fullscreenContext * /*ctx*/) override { }

  void enterExec(configGrammar::ExecContext * /*ctx*/) override { }
  void exitExec(configGrammar::ExecContext * /*ctx*/) override { }

  void enterDimension(configGrammar::DimensionContext * /*ctx*/) override { }
  void exitDimension(configGrammar::DimensionContext * /*ctx*/) override { }

  void enterCommand(configGrammar::CommandContext * /*ctx*/) override { }
  void exitCommand(configGrammar::CommandContext * /*ctx*/) override { }

  void enterCommand_move(configGrammar::Command_moveContext * /*ctx*/) override { }
  void exitCommand_move(configGrammar::Command_moveContext * /*ctx*/) override { }

  void enterCommand_exec(configGrammar::Command_execContext * /*ctx*/) override { }
  void exitCommand_exec(configGrammar::Command_execContext * /*ctx*/) override { }

  void enterCommand_exit(configGrammar::Command_exitContext * /*ctx*/) override { }
  void exitCommand_exit(configGrammar::Command_exitContext * /*ctx*/) override { }

  void enterCommand_kill(configGrammar::Command_killContext * /*ctx*/) override { }
  void exitCommand_kill(configGrammar::Command_killContext * /*ctx*/) override { }

  void enterCommand_fullscreen(configGrammar::Command_fullscreenContext * /*ctx*/) override { }
  void exitCommand_fullscreen(configGrammar::Command_fullscreenContext * /*ctx*/) override { }

  void enterCommand_sticky(configGrammar::Command_stickyContext * /*ctx*/) override { }
  void exitCommand_sticky(configGrammar::Command_stickyContext * /*ctx*/) override { }

  void enterCommand_debuglog(configGrammar::Command_debuglogContext * /*ctx*/) override { }
  void exitCommand_debuglog(configGrammar::Command_debuglogContext * /*ctx*/) override { }

  void enterCommand_border(configGrammar::Command_borderContext * /*ctx*/) override { }
  void exitCommand_border(configGrammar::Command_borderContext * /*ctx*/) override { }

  void enterCommand_layout(configGrammar::Command_layoutContext * /*ctx*/) override { }
  void exitCommand_layout(configGrammar::Command_layoutContext * /*ctx*/) override { }

  void enterCommand_append_layout(configGrammar::Command_append_layoutContext * /*ctx*/) override { }
  void exitCommand_append_layout(configGrammar::Command_append_layoutContext * /*ctx*/) override { }

  void enterCommand_workspace(configGrammar::Command_workspaceContext * /*ctx*/) override { }
  void exitCommand_workspace(configGrammar::Command_workspaceContext * /*ctx*/) override { }

  void enterCommand_focus(configGrammar::Command_focusContext * /*ctx*/) override { }
  void exitCommand_focus(configGrammar::Command_focusContext * /*ctx*/) override { }

  void enterCommand_split(configGrammar::Command_splitContext * /*ctx*/) override { }
  void exitCommand_split(configGrammar::Command_splitContext * /*ctx*/) override { }

  void enterCommand_floating(configGrammar::Command_floatingContext * /*ctx*/) override { }
  void exitCommand_floating(configGrammar::Command_floatingContext * /*ctx*/) override { }

  void enterCommand_resize(configGrammar::Command_resizeContext * /*ctx*/) override { }
  void exitCommand_resize(configGrammar::Command_resizeContext * /*ctx*/) override { }

  void enterCommand_rename(configGrammar::Command_renameContext * /*ctx*/) override { }
  void exitCommand_rename(configGrammar::Command_renameContext * /*ctx*/) override { }

  void enterCommand_nop(configGrammar::Command_nopContext * /*ctx*/) override { }
  void exitCommand_nop(configGrammar::Command_nopContext * /*ctx*/) override { }

  void enterCommand_reload(configGrammar::Command_reloadContext * /*ctx*/) override { }
  void exitCommand_reload(configGrammar::Command_reloadContext * /*ctx*/) override { }

  void enterCommand_restart(configGrammar::Command_restartContext * /*ctx*/) override { }
  void exitCommand_restart(configGrammar::Command_restartContext * /*ctx*/) override { }

  void enterCommand_mode(configGrammar::Command_modeContext * /*ctx*/) override { }
  void exitCommand_mode(configGrammar::Command_modeContext * /*ctx*/) override { }

  void enterCommand_open(configGrammar::Command_openContext * /*ctx*/) override { }
  void exitCommand_open(configGrammar::Command_openContext * /*ctx*/) override { }

  void enterCommand_title_format(configGrammar::Command_title_formatContext * /*ctx*/) override { }
  void exitCommand_title_format(configGrammar::Command_title_formatContext * /*ctx*/) override { }

  void enterCommand_title_window_icon(configGrammar::Command_title_window_iconContext * /*ctx*/) override { }
  void exitCommand_title_window_icon(configGrammar::Command_title_window_iconContext * /*ctx*/) override { }

  void enterCriteria(configGrammar::CriteriaContext * /*ctx*/) override { }
  void exitCriteria(configGrammar::CriteriaContext * /*ctx*/) override { }

  void enterCriterion(configGrammar::CriterionContext * /*ctx*/) override { }
  void exitCriterion(configGrammar::CriterionContext * /*ctx*/) override { }

  void enterValue_criterion(configGrammar::Value_criterionContext * /*ctx*/) override { }
  void exitValue_criterion(configGrammar::Value_criterionContext * /*ctx*/) override { }


  void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

