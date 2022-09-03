
// Generated from parser-specs/config.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "configListener.h"


/**
 * This class provides an empty implementation of configListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  configBaseListener : public configListener {
public:

  virtual void enterFile(configParser::FileContext * /*ctx*/) override { }
  virtual void exitFile(configParser::FileContext * /*ctx*/) override { }

  virtual void enterStmt(configParser::StmtContext * /*ctx*/) override { }
  virtual void exitStmt(configParser::StmtContext * /*ctx*/) override { }

  virtual void enterArguments(configParser::ArgumentsContext * /*ctx*/) override { }
  virtual void exitArguments(configParser::ArgumentsContext * /*ctx*/) override { }

  virtual void enterStmt_mode(configParser::Stmt_modeContext * /*ctx*/) override { }
  virtual void exitStmt_mode(configParser::Stmt_modeContext * /*ctx*/) override { }

  virtual void enterStmt_mode_options(configParser::Stmt_mode_optionsContext * /*ctx*/) override { }
  virtual void exitStmt_mode_options(configParser::Stmt_mode_optionsContext * /*ctx*/) override { }

  virtual void enterStmt_mode_stmts(configParser::Stmt_mode_stmtsContext * /*ctx*/) override { }
  virtual void exitStmt_mode_stmts(configParser::Stmt_mode_stmtsContext * /*ctx*/) override { }

  virtual void enterColor(configParser::ColorContext * /*ctx*/) override { }
  virtual void exitColor(configParser::ColorContext * /*ctx*/) override { }

  virtual void enterColor_single(configParser::Color_singleContext * /*ctx*/) override { }
  virtual void exitColor_single(configParser::Color_singleContext * /*ctx*/) override { }

  virtual void enterColor_multiple(configParser::Color_multipleContext * /*ctx*/) override { }
  virtual void exitColor_multiple(configParser::Color_multipleContext * /*ctx*/) override { }

  virtual void enterExec(configParser::ExecContext * /*ctx*/) override { }
  virtual void exitExec(configParser::ExecContext * /*ctx*/) override { }

  virtual void enterExec_options(configParser::Exec_optionsContext * /*ctx*/) override { }
  virtual void exitExec_options(configParser::Exec_optionsContext * /*ctx*/) override { }

  virtual void enterPopup_during_fullscreen(configParser::Popup_during_fullscreenContext * /*ctx*/) override { }
  virtual void exitPopup_during_fullscreen(configParser::Popup_during_fullscreenContext * /*ctx*/) override { }

  virtual void enterPopup_during_fullscreen_value(configParser::Popup_during_fullscreen_valueContext * /*ctx*/) override { }
  virtual void exitPopup_during_fullscreen_value(configParser::Popup_during_fullscreen_valueContext * /*ctx*/) override { }

  virtual void enterRestart_state(configParser::Restart_stateContext * /*ctx*/) override { }
  virtual void exitRestart_state(configParser::Restart_stateContext * /*ctx*/) override { }

  virtual void enterIpc_kill_timeout(configParser::Ipc_kill_timeoutContext * /*ctx*/) override { }
  virtual void exitIpc_kill_timeout(configParser::Ipc_kill_timeoutContext * /*ctx*/) override { }

  virtual void enterIpc_socket(configParser::Ipc_socketContext * /*ctx*/) override { }
  virtual void exitIpc_socket(configParser::Ipc_socketContext * /*ctx*/) override { }

  virtual void enterWorkspace(configParser::WorkspaceContext * /*ctx*/) override { }
  virtual void exitWorkspace(configParser::WorkspaceContext * /*ctx*/) override { }

  virtual void enterWorkspace_name(configParser::Workspace_nameContext * /*ctx*/) override { }
  virtual void exitWorkspace_name(configParser::Workspace_nameContext * /*ctx*/) override { }

  virtual void enterTitle_align(configParser::Title_alignContext * /*ctx*/) override { }
  virtual void exitTitle_align(configParser::Title_alignContext * /*ctx*/) override { }

  virtual void enterTitle_align_value(configParser::Title_align_valueContext * /*ctx*/) override { }
  virtual void exitTitle_align_value(configParser::Title_align_valueContext * /*ctx*/) override { }

  virtual void enterFocus_on_window_activation(configParser::Focus_on_window_activationContext * /*ctx*/) override { }
  virtual void exitFocus_on_window_activation(configParser::Focus_on_window_activationContext * /*ctx*/) override { }

  virtual void enterFocus_on_window_activation_value(configParser::Focus_on_window_activation_valueContext * /*ctx*/) override { }
  virtual void exitFocus_on_window_activation_value(configParser::Focus_on_window_activation_valueContext * /*ctx*/) override { }

  virtual void enterForce_display_urgency_hint(configParser::Force_display_urgency_hintContext * /*ctx*/) override { }
  virtual void exitForce_display_urgency_hint(configParser::Force_display_urgency_hintContext * /*ctx*/) override { }

  virtual void enterWorkspace_auto_back_and_forth(configParser::Workspace_auto_back_and_forthContext * /*ctx*/) override { }
  virtual void exitWorkspace_auto_back_and_forth(configParser::Workspace_auto_back_and_forthContext * /*ctx*/) override { }

  virtual void enterFocus_wrapping(configParser::Focus_wrappingContext * /*ctx*/) override { }
  virtual void exitFocus_wrapping(configParser::Focus_wrappingContext * /*ctx*/) override { }

  virtual void enterForce_focus_wrapping(configParser::Force_focus_wrappingContext * /*ctx*/) override { }
  virtual void exitForce_focus_wrapping(configParser::Force_focus_wrappingContext * /*ctx*/) override { }

  virtual void enterMouse_warping(configParser::Mouse_warpingContext * /*ctx*/) override { }
  virtual void exitMouse_warping(configParser::Mouse_warpingContext * /*ctx*/) override { }

  virtual void enterMouse_warping_value(configParser::Mouse_warping_valueContext * /*ctx*/) override { }
  virtual void exitMouse_warping_value(configParser::Mouse_warping_valueContext * /*ctx*/) override { }

  virtual void enterFocus_follows_mouse(configParser::Focus_follows_mouseContext * /*ctx*/) override { }
  virtual void exitFocus_follows_mouse(configParser::Focus_follows_mouseContext * /*ctx*/) override { }

  virtual void enterNo_focus(configParser::No_focusContext * /*ctx*/) override { }
  virtual void exitNo_focus(configParser::No_focusContext * /*ctx*/) override { }

  virtual void enterAssign(configParser::AssignContext * /*ctx*/) override { }
  virtual void exitAssign(configParser::AssignContext * /*ctx*/) override { }

  virtual void enterAssign_target(configParser::Assign_targetContext * /*ctx*/) override { }
  virtual void exitAssign_target(configParser::Assign_targetContext * /*ctx*/) override { }

  virtual void enterAssign_target_workspace(configParser::Assign_target_workspaceContext * /*ctx*/) override { }
  virtual void exitAssign_target_workspace(configParser::Assign_target_workspaceContext * /*ctx*/) override { }

  virtual void enterAssign_target_number(configParser::Assign_target_numberContext * /*ctx*/) override { }
  virtual void exitAssign_target_number(configParser::Assign_target_numberContext * /*ctx*/) override { }

  virtual void enterAssign_target_output(configParser::Assign_target_outputContext * /*ctx*/) override { }
  virtual void exitAssign_target_output(configParser::Assign_target_outputContext * /*ctx*/) override { }

  virtual void enterFor_window(configParser::For_windowContext * /*ctx*/) override { }
  virtual void exitFor_window(configParser::For_windowContext * /*ctx*/) override { }

  virtual void enterCriteria(configParser::CriteriaContext * /*ctx*/) override { }
  virtual void exitCriteria(configParser::CriteriaContext * /*ctx*/) override { }

  virtual void enterCriterion(configParser::CriterionContext * /*ctx*/) override { }
  virtual void exitCriterion(configParser::CriterionContext * /*ctx*/) override { }

  virtual void enterValue_criterion(configParser::Value_criterionContext * /*ctx*/) override { }
  virtual void exitValue_criterion(configParser::Value_criterionContext * /*ctx*/) override { }

  virtual void enterHide_edge_borders(configParser::Hide_edge_bordersContext * /*ctx*/) override { }
  virtual void exitHide_edge_borders(configParser::Hide_edge_bordersContext * /*ctx*/) override { }

  virtual void enterBorder_type(configParser::Border_typeContext * /*ctx*/) override { }
  virtual void exitBorder_type(configParser::Border_typeContext * /*ctx*/) override { }

  virtual void enterDefault_border(configParser::Default_borderContext * /*ctx*/) override { }
  virtual void exitDefault_border(configParser::Default_borderContext * /*ctx*/) override { }

  virtual void enterDefault_border_type(configParser::Default_border_typeContext * /*ctx*/) override { }
  virtual void exitDefault_border_type(configParser::Default_border_typeContext * /*ctx*/) override { }

  virtual void enterBorder_style(configParser::Border_styleContext * /*ctx*/) override { }
  virtual void exitBorder_style(configParser::Border_styleContext * /*ctx*/) override { }

  virtual void enterWorkspace_layout(configParser::Workspace_layoutContext * /*ctx*/) override { }
  virtual void exitWorkspace_layout(configParser::Workspace_layoutContext * /*ctx*/) override { }

  virtual void enterWorkspace_layout_value(configParser::Workspace_layout_valueContext * /*ctx*/) override { }
  virtual void exitWorkspace_layout_value(configParser::Workspace_layout_valueContext * /*ctx*/) override { }

  virtual void enterDefault_orientation(configParser::Default_orientationContext * /*ctx*/) override { }
  virtual void exitDefault_orientation(configParser::Default_orientationContext * /*ctx*/) override { }

  virtual void enterDefault_orientation_value(configParser::Default_orientation_valueContext * /*ctx*/) override { }
  virtual void exitDefault_orientation_value(configParser::Default_orientation_valueContext * /*ctx*/) override { }

  virtual void enterFloating_modifier(configParser::Floating_modifierContext * /*ctx*/) override { }
  virtual void exitFloating_modifier(configParser::Floating_modifierContext * /*ctx*/) override { }

  virtual void enterFloating_minimum_size(configParser::Floating_minimum_sizeContext * /*ctx*/) override { }
  virtual void exitFloating_minimum_size(configParser::Floating_minimum_sizeContext * /*ctx*/) override { }

  virtual void enterFloating_maximum_size(configParser::Floating_maximum_sizeContext * /*ctx*/) override { }
  virtual void exitFloating_maximum_size(configParser::Floating_maximum_sizeContext * /*ctx*/) override { }

  virtual void enterDimension(configParser::DimensionContext * /*ctx*/) override { }
  virtual void exitDimension(configParser::DimensionContext * /*ctx*/) override { }

  virtual void enterFont(configParser::FontContext * /*ctx*/) override { }
  virtual void exitFont(configParser::FontContext * /*ctx*/) override { }

  virtual void enterSet(configParser::SetContext * /*ctx*/) override { }
  virtual void exitSet(configParser::SetContext * /*ctx*/) override { }

  virtual void enterSet_from_resource(configParser::Set_from_resourceContext * /*ctx*/) override { }
  virtual void exitSet_from_resource(configParser::Set_from_resourceContext * /*ctx*/) override { }

  virtual void enterInclude(configParser::IncludeContext * /*ctx*/) override { }
  virtual void exitInclude(configParser::IncludeContext * /*ctx*/) override { }

  virtual void enterBinding(configParser::BindingContext * /*ctx*/) override { }
  virtual void exitBinding(configParser::BindingContext * /*ctx*/) override { }

  virtual void enterBinding_options(configParser::Binding_optionsContext * /*ctx*/) override { }
  virtual void exitBinding_options(configParser::Binding_optionsContext * /*ctx*/) override { }

  virtual void enterModifier(configParser::ModifierContext * /*ctx*/) override { }
  virtual void exitModifier(configParser::ModifierContext * /*ctx*/) override { }

  virtual void enterKeybinding(configParser::KeybindingContext * /*ctx*/) override { }
  virtual void exitKeybinding(configParser::KeybindingContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

