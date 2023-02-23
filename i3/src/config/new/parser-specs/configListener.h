
// Generated from parser-specs/config.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"
#include "configParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by configParser.
 */
class  configListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterFile(configParser::FileContext *ctx) = 0;
  virtual void exitFile(configParser::FileContext *ctx) = 0;

  virtual void enterStmt(configParser::StmtContext *ctx) = 0;
  virtual void exitStmt(configParser::StmtContext *ctx) = 0;

  virtual void enterArguments(configParser::ArgumentsContext *ctx) = 0;
  virtual void exitArguments(configParser::ArgumentsContext *ctx) = 0;

  virtual void enterStmt_mode(configParser::Stmt_modeContext *ctx) = 0;
  virtual void exitStmt_mode(configParser::Stmt_modeContext *ctx) = 0;

  virtual void enterStmt_mode_options(configParser::Stmt_mode_optionsContext *ctx) = 0;
  virtual void exitStmt_mode_options(configParser::Stmt_mode_optionsContext *ctx) = 0;

  virtual void enterStmt_mode_stmts(configParser::Stmt_mode_stmtsContext *ctx) = 0;
  virtual void exitStmt_mode_stmts(configParser::Stmt_mode_stmtsContext *ctx) = 0;

  virtual void enterColor(configParser::ColorContext *ctx) = 0;
  virtual void exitColor(configParser::ColorContext *ctx) = 0;

  virtual void enterColor_single(configParser::Color_singleContext *ctx) = 0;
  virtual void exitColor_single(configParser::Color_singleContext *ctx) = 0;

  virtual void enterColor_multiple(configParser::Color_multipleContext *ctx) = 0;
  virtual void exitColor_multiple(configParser::Color_multipleContext *ctx) = 0;

  virtual void enterExec(configParser::ExecContext *ctx) = 0;
  virtual void exitExec(configParser::ExecContext *ctx) = 0;

  virtual void enterExec_options(configParser::Exec_optionsContext *ctx) = 0;
  virtual void exitExec_options(configParser::Exec_optionsContext *ctx) = 0;

  virtual void enterPopup_during_fullscreen(configParser::Popup_during_fullscreenContext *ctx) = 0;
  virtual void exitPopup_during_fullscreen(configParser::Popup_during_fullscreenContext *ctx) = 0;

  virtual void enterPopup_during_fullscreen_value(configParser::Popup_during_fullscreen_valueContext *ctx) = 0;
  virtual void exitPopup_during_fullscreen_value(configParser::Popup_during_fullscreen_valueContext *ctx) = 0;

  virtual void enterRestart_state(configParser::Restart_stateContext *ctx) = 0;
  virtual void exitRestart_state(configParser::Restart_stateContext *ctx) = 0;

  virtual void enterIpc_kill_timeout(configParser::Ipc_kill_timeoutContext *ctx) = 0;
  virtual void exitIpc_kill_timeout(configParser::Ipc_kill_timeoutContext *ctx) = 0;

  virtual void enterIpc_socket(configParser::Ipc_socketContext *ctx) = 0;
  virtual void exitIpc_socket(configParser::Ipc_socketContext *ctx) = 0;

  virtual void enterWorkspace(configParser::WorkspaceContext *ctx) = 0;
  virtual void exitWorkspace(configParser::WorkspaceContext *ctx) = 0;

  virtual void enterWorkspace_name(configParser::Workspace_nameContext *ctx) = 0;
  virtual void exitWorkspace_name(configParser::Workspace_nameContext *ctx) = 0;

  virtual void enterTitle_align(configParser::Title_alignContext *ctx) = 0;
  virtual void exitTitle_align(configParser::Title_alignContext *ctx) = 0;

  virtual void enterTitle_align_value(configParser::Title_align_valueContext *ctx) = 0;
  virtual void exitTitle_align_value(configParser::Title_align_valueContext *ctx) = 0;

  virtual void enterFocus_on_window_activation(configParser::Focus_on_window_activationContext *ctx) = 0;
  virtual void exitFocus_on_window_activation(configParser::Focus_on_window_activationContext *ctx) = 0;

  virtual void enterFocus_on_window_activation_value(configParser::Focus_on_window_activation_valueContext *ctx) = 0;
  virtual void exitFocus_on_window_activation_value(configParser::Focus_on_window_activation_valueContext *ctx) = 0;

  virtual void enterForce_display_urgency_hint(configParser::Force_display_urgency_hintContext *ctx) = 0;
  virtual void exitForce_display_urgency_hint(configParser::Force_display_urgency_hintContext *ctx) = 0;

  virtual void enterWorkspace_auto_back_and_forth(configParser::Workspace_auto_back_and_forthContext *ctx) = 0;
  virtual void exitWorkspace_auto_back_and_forth(configParser::Workspace_auto_back_and_forthContext *ctx) = 0;

  virtual void enterFocus_wrapping(configParser::Focus_wrappingContext *ctx) = 0;
  virtual void exitFocus_wrapping(configParser::Focus_wrappingContext *ctx) = 0;

  virtual void enterForce_focus_wrapping(configParser::Force_focus_wrappingContext *ctx) = 0;
  virtual void exitForce_focus_wrapping(configParser::Force_focus_wrappingContext *ctx) = 0;

  virtual void enterMouse_warping(configParser::Mouse_warpingContext *ctx) = 0;
  virtual void exitMouse_warping(configParser::Mouse_warpingContext *ctx) = 0;

  virtual void enterMouse_warping_value(configParser::Mouse_warping_valueContext *ctx) = 0;
  virtual void exitMouse_warping_value(configParser::Mouse_warping_valueContext *ctx) = 0;

  virtual void enterFocus_follows_mouse(configParser::Focus_follows_mouseContext *ctx) = 0;
  virtual void exitFocus_follows_mouse(configParser::Focus_follows_mouseContext *ctx) = 0;

  virtual void enterNo_focus(configParser::No_focusContext *ctx) = 0;
  virtual void exitNo_focus(configParser::No_focusContext *ctx) = 0;

  virtual void enterAssign(configParser::AssignContext *ctx) = 0;
  virtual void exitAssign(configParser::AssignContext *ctx) = 0;

  virtual void enterAssign_target(configParser::Assign_targetContext *ctx) = 0;
  virtual void exitAssign_target(configParser::Assign_targetContext *ctx) = 0;

  virtual void enterAssign_target_workspace(configParser::Assign_target_workspaceContext *ctx) = 0;
  virtual void exitAssign_target_workspace(configParser::Assign_target_workspaceContext *ctx) = 0;

  virtual void enterAssign_target_number(configParser::Assign_target_numberContext *ctx) = 0;
  virtual void exitAssign_target_number(configParser::Assign_target_numberContext *ctx) = 0;

  virtual void enterAssign_target_output(configParser::Assign_target_outputContext *ctx) = 0;
  virtual void exitAssign_target_output(configParser::Assign_target_outputContext *ctx) = 0;

  virtual void enterFor_window(configParser::For_windowContext *ctx) = 0;
  virtual void exitFor_window(configParser::For_windowContext *ctx) = 0;

  virtual void enterCriteria(configParser::CriteriaContext *ctx) = 0;
  virtual void exitCriteria(configParser::CriteriaContext *ctx) = 0;

  virtual void enterCriterion(configParser::CriterionContext *ctx) = 0;
  virtual void exitCriterion(configParser::CriterionContext *ctx) = 0;

  virtual void enterValue_criterion(configParser::Value_criterionContext *ctx) = 0;
  virtual void exitValue_criterion(configParser::Value_criterionContext *ctx) = 0;

  virtual void enterHide_edge_borders(configParser::Hide_edge_bordersContext *ctx) = 0;
  virtual void exitHide_edge_borders(configParser::Hide_edge_bordersContext *ctx) = 0;

  virtual void enterBorder_type(configParser::Border_typeContext *ctx) = 0;
  virtual void exitBorder_type(configParser::Border_typeContext *ctx) = 0;

  virtual void enterDefault_border(configParser::Default_borderContext *ctx) = 0;
  virtual void exitDefault_border(configParser::Default_borderContext *ctx) = 0;

  virtual void enterDefault_border_type(configParser::Default_border_typeContext *ctx) = 0;
  virtual void exitDefault_border_type(configParser::Default_border_typeContext *ctx) = 0;

  virtual void enterBorder_style(configParser::Border_styleContext *ctx) = 0;
  virtual void exitBorder_style(configParser::Border_styleContext *ctx) = 0;

  virtual void enterWorkspace_layout(configParser::Workspace_layoutContext *ctx) = 0;
  virtual void exitWorkspace_layout(configParser::Workspace_layoutContext *ctx) = 0;

  virtual void enterWorkspace_layout_value(configParser::Workspace_layout_valueContext *ctx) = 0;
  virtual void exitWorkspace_layout_value(configParser::Workspace_layout_valueContext *ctx) = 0;

  virtual void enterDefault_orientation(configParser::Default_orientationContext *ctx) = 0;
  virtual void exitDefault_orientation(configParser::Default_orientationContext *ctx) = 0;

  virtual void enterDefault_orientation_value(configParser::Default_orientation_valueContext *ctx) = 0;
  virtual void exitDefault_orientation_value(configParser::Default_orientation_valueContext *ctx) = 0;

  virtual void enterFloating_modifier(configParser::Floating_modifierContext *ctx) = 0;
  virtual void exitFloating_modifier(configParser::Floating_modifierContext *ctx) = 0;

  virtual void enterFloating_minimum_size(configParser::Floating_minimum_sizeContext *ctx) = 0;
  virtual void exitFloating_minimum_size(configParser::Floating_minimum_sizeContext *ctx) = 0;

  virtual void enterFloating_maximum_size(configParser::Floating_maximum_sizeContext *ctx) = 0;
  virtual void exitFloating_maximum_size(configParser::Floating_maximum_sizeContext *ctx) = 0;

  virtual void enterDimension(configParser::DimensionContext *ctx) = 0;
  virtual void exitDimension(configParser::DimensionContext *ctx) = 0;

  virtual void enterFont(configParser::FontContext *ctx) = 0;
  virtual void exitFont(configParser::FontContext *ctx) = 0;

  virtual void enterSet(configParser::SetContext *ctx) = 0;
  virtual void exitSet(configParser::SetContext *ctx) = 0;

  virtual void enterSet_from_resource(configParser::Set_from_resourceContext *ctx) = 0;
  virtual void exitSet_from_resource(configParser::Set_from_resourceContext *ctx) = 0;

  virtual void enterInclude(configParser::IncludeContext *ctx) = 0;
  virtual void exitInclude(configParser::IncludeContext *ctx) = 0;

  virtual void enterBinding(configParser::BindingContext *ctx) = 0;
  virtual void exitBinding(configParser::BindingContext *ctx) = 0;

  virtual void enterBinding_options(configParser::Binding_optionsContext *ctx) = 0;
  virtual void exitBinding_options(configParser::Binding_optionsContext *ctx) = 0;

  virtual void enterModifier(configParser::ModifierContext *ctx) = 0;
  virtual void exitModifier(configParser::ModifierContext *ctx) = 0;

  virtual void enterKeybinding(configParser::KeybindingContext *ctx) = 0;
  virtual void exitKeybinding(configParser::KeybindingContext *ctx) = 0;


};

