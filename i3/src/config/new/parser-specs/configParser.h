
// Generated from parser-specs/config.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"




class  configParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, T__22 = 23, T__23 = 24, T__24 = 25, T__25 = 26, 
    T__26 = 27, T__27 = 28, T__28 = 29, T__29 = 30, T__30 = 31, T__31 = 32, 
    T__32 = 33, T__33 = 34, T__34 = 35, T__35 = 36, T__36 = 37, T__37 = 38, 
    T__38 = 39, T__39 = 40, T__40 = 41, T__41 = 42, T__42 = 43, T__43 = 44, 
    T__44 = 45, T__45 = 46, T__46 = 47, T__47 = 48, T__48 = 49, T__49 = 50, 
    T__50 = 51, T__51 = 52, T__52 = 53, T__53 = 54, T__54 = 55, T__55 = 56, 
    T__56 = 57, T__57 = 58, T__58 = 59, T__59 = 60, T__60 = 61, T__61 = 62, 
    T__62 = 63, T__63 = 64, T__64 = 65, T__65 = 66, T__66 = 67, T__67 = 68, 
    T__68 = 69, T__69 = 70, T__70 = 71, T__71 = 72, T__72 = 73, T__73 = 74, 
    T__74 = 75, T__75 = 76, T__76 = 77, T__77 = 78, T__78 = 79, T__79 = 80, 
    T__80 = 81, T__81 = 82, T__82 = 83, T__83 = 84, T__84 = 85, T__85 = 86, 
    T__86 = 87, T__87 = 88, T__88 = 89, T__89 = 90, T__90 = 91, T__91 = 92, 
    T__92 = 93, T__93 = 94, T__94 = 95, T__95 = 96, T__96 = 97, T__97 = 98, 
    T__98 = 99, T__99 = 100, T__100 = 101, T__101 = 102, T__102 = 103, T__103 = 104, 
    T__104 = 105, T__105 = 106, T__106 = 107, T__107 = 108, T__108 = 109, 
    T__109 = 110, COMMENT = 111, WS = 112, BOOL = 113, VARIABLE = 114, STRING = 115, 
    NUMBER = 116, EOL = 117
  };

  enum {
    RuleFile = 0, RuleStmt = 1, RuleArguments = 2, RuleStmt_mode = 3, RuleStmt_mode_options = 4, 
    RuleStmt_mode_stmts = 5, RuleColor = 6, RuleColor_single = 7, RuleColor_multiple = 8, 
    RuleExec = 9, RuleExec_options = 10, RulePopup_during_fullscreen = 11, 
    RulePopup_during_fullscreen_value = 12, RuleRestart_state = 13, RuleIpc_kill_timeout = 14, 
    RuleIpc_socket = 15, RuleWorkspace = 16, RuleWorkspace_name = 17, RuleTitle_align = 18, 
    RuleTitle_align_value = 19, RuleFocus_on_window_activation = 20, RuleFocus_on_window_activation_value = 21, 
    RuleForce_display_urgency_hint = 22, RuleWorkspace_auto_back_and_forth = 23, 
    RuleFocus_wrapping = 24, RuleForce_focus_wrapping = 25, RuleMouse_warping = 26, 
    RuleMouse_warping_value = 27, RuleFocus_follows_mouse = 28, RuleNo_focus = 29, 
    RuleAssign = 30, RuleAssign_target = 31, RuleAssign_target_workspace = 32, 
    RuleAssign_target_number = 33, RuleAssign_target_output = 34, RuleFor_window = 35, 
    RuleCriteria = 36, RuleCriterion = 37, RuleValue_criterion = 38, RuleHide_edge_borders = 39, 
    RuleBorder_type = 40, RuleDefault_border = 41, RuleDefault_border_type = 42, 
    RuleBorder_style = 43, RuleWorkspace_layout = 44, RuleWorkspace_layout_value = 45, 
    RuleDefault_orientation = 46, RuleDefault_orientation_value = 47, RuleFloating_modifier = 48, 
    RuleFloating_minimum_size = 49, RuleFloating_maximum_size = 50, RuleDimension = 51, 
    RuleFont = 52, RuleSet = 53, RuleSet_from_resource = 54, RuleInclude = 55, 
    RuleBinding = 56, RuleBinding_options = 57, RuleModifier = 58, RuleKeybinding = 59
  };

  explicit configParser(antlr4::TokenStream *input);

  configParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~configParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class FileContext;
  class StmtContext;
  class ArgumentsContext;
  class Stmt_modeContext;
  class Stmt_mode_optionsContext;
  class Stmt_mode_stmtsContext;
  class ColorContext;
  class Color_singleContext;
  class Color_multipleContext;
  class ExecContext;
  class Exec_optionsContext;
  class Popup_during_fullscreenContext;
  class Popup_during_fullscreen_valueContext;
  class Restart_stateContext;
  class Ipc_kill_timeoutContext;
  class Ipc_socketContext;
  class WorkspaceContext;
  class Workspace_nameContext;
  class Title_alignContext;
  class Title_align_valueContext;
  class Focus_on_window_activationContext;
  class Focus_on_window_activation_valueContext;
  class Force_display_urgency_hintContext;
  class Workspace_auto_back_and_forthContext;
  class Focus_wrappingContext;
  class Force_focus_wrappingContext;
  class Mouse_warpingContext;
  class Mouse_warping_valueContext;
  class Focus_follows_mouseContext;
  class No_focusContext;
  class AssignContext;
  class Assign_targetContext;
  class Assign_target_workspaceContext;
  class Assign_target_numberContext;
  class Assign_target_outputContext;
  class For_windowContext;
  class CriteriaContext;
  class CriterionContext;
  class Value_criterionContext;
  class Hide_edge_bordersContext;
  class Border_typeContext;
  class Default_borderContext;
  class Default_border_typeContext;
  class Border_styleContext;
  class Workspace_layoutContext;
  class Workspace_layout_valueContext;
  class Default_orientationContext;
  class Default_orientation_valueContext;
  class Floating_modifierContext;
  class Floating_minimum_sizeContext;
  class Floating_maximum_sizeContext;
  class DimensionContext;
  class FontContext;
  class SetContext;
  class Set_from_resourceContext;
  class IncludeContext;
  class BindingContext;
  class Binding_optionsContext;
  class ModifierContext;
  class KeybindingContext; 

  class  FileContext : public antlr4::ParserRuleContext {
  public:
    FileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<StmtContext *> stmt();
    StmtContext* stmt(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EOL();
    antlr4::tree::TerminalNode* EOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FileContext* file();

  class  StmtContext : public antlr4::ParserRuleContext {
  public:
    StmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SetContext *set();
    Set_from_resourceContext *set_from_resource();
    IncludeContext *include();
    BindingContext *binding();
    Stmt_modeContext *stmt_mode();
    FontContext *font();
    Floating_minimum_sizeContext *floating_minimum_size();
    Floating_maximum_sizeContext *floating_maximum_size();
    Floating_modifierContext *floating_modifier();
    Default_orientationContext *default_orientation();
    Workspace_layoutContext *workspace_layout();
    Default_borderContext *default_border();
    Hide_edge_bordersContext *hide_edge_borders();
    For_windowContext *for_window();
    AssignContext *assign();
    No_focusContext *no_focus();
    Focus_follows_mouseContext *focus_follows_mouse();
    Mouse_warpingContext *mouse_warping();
    Focus_wrappingContext *focus_wrapping();
    Force_focus_wrappingContext *force_focus_wrapping();
    Workspace_auto_back_and_forthContext *workspace_auto_back_and_forth();
    Force_display_urgency_hintContext *force_display_urgency_hint();
    Focus_on_window_activationContext *focus_on_window_activation();
    Title_alignContext *title_align();
    WorkspaceContext *workspace();
    Ipc_socketContext *ipc_socket();
    Ipc_kill_timeoutContext *ipc_kill_timeout();
    Restart_stateContext *restart_state();
    Popup_during_fullscreenContext *popup_during_fullscreen();
    ExecContext *exec();
    ColorContext *color();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StmtContext* stmt();

  class  ArgumentsContext : public antlr4::ParserRuleContext {
  public:
    ArgumentsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ArgumentsContext* arguments();

  class  Stmt_modeContext : public antlr4::ParserRuleContext {
  public:
    Stmt_modeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    std::vector<Stmt_mode_optionsContext *> stmt_mode_options();
    Stmt_mode_optionsContext* stmt_mode_options(size_t i);
    std::vector<Stmt_mode_stmtsContext *> stmt_mode_stmts();
    Stmt_mode_stmtsContext* stmt_mode_stmts(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EOL();
    antlr4::tree::TerminalNode* EOL(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Stmt_modeContext* stmt_mode();

  class  Stmt_mode_optionsContext : public antlr4::ParserRuleContext {
  public:
    Stmt_mode_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Stmt_mode_optionsContext* stmt_mode_options();

  class  Stmt_mode_stmtsContext : public antlr4::ParserRuleContext {
  public:
    Stmt_mode_stmtsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SetContext *set();
    BindingContext *binding();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Stmt_mode_stmtsContext* stmt_mode_stmts();

  class  ColorContext : public antlr4::ParserRuleContext {
  public:
    ColorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Color_singleContext *color_single();
    Color_multipleContext *color_multiple();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ColorContext* color();

  class  Color_singleContext : public antlr4::ParserRuleContext {
  public:
    Color_singleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Color_singleContext* color_single();

  class  Color_multipleContext : public antlr4::ParserRuleContext {
  public:
    Color_multipleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Color_multipleContext* color_multiple();

  class  ExecContext : public antlr4::ParserRuleContext {
  public:
    ExecContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ArgumentsContext *arguments();
    std::vector<Exec_optionsContext *> exec_options();
    Exec_optionsContext* exec_options(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExecContext* exec();

  class  Exec_optionsContext : public antlr4::ParserRuleContext {
  public:
    Exec_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Exec_optionsContext* exec_options();

  class  Popup_during_fullscreenContext : public antlr4::ParserRuleContext {
  public:
    Popup_during_fullscreenContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Popup_during_fullscreen_valueContext *popup_during_fullscreen_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Popup_during_fullscreenContext* popup_during_fullscreen();

  class  Popup_during_fullscreen_valueContext : public antlr4::ParserRuleContext {
  public:
    Popup_during_fullscreen_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Popup_during_fullscreen_valueContext* popup_during_fullscreen_value();

  class  Restart_stateContext : public antlr4::ParserRuleContext {
  public:
    Restart_stateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Restart_stateContext* restart_state();

  class  Ipc_kill_timeoutContext : public antlr4::ParserRuleContext {
  public:
    Ipc_kill_timeoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Ipc_kill_timeoutContext* ipc_kill_timeout();

  class  Ipc_socketContext : public antlr4::ParserRuleContext {
  public:
    Ipc_socketContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Ipc_socketContext* ipc_socket();

  class  WorkspaceContext : public antlr4::ParserRuleContext {
  public:
    WorkspaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Workspace_nameContext *workspace_name();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  WorkspaceContext* workspace();

  class  Workspace_nameContext : public antlr4::ParserRuleContext {
  public:
    Workspace_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_nameContext* workspace_name();

  class  Title_alignContext : public antlr4::ParserRuleContext {
  public:
    Title_alignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Title_align_valueContext *title_align_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Title_alignContext* title_align();

  class  Title_align_valueContext : public antlr4::ParserRuleContext {
  public:
    Title_align_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Title_align_valueContext* title_align_value();

  class  Focus_on_window_activationContext : public antlr4::ParserRuleContext {
  public:
    Focus_on_window_activationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Focus_on_window_activation_valueContext *focus_on_window_activation_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_on_window_activationContext* focus_on_window_activation();

  class  Focus_on_window_activation_valueContext : public antlr4::ParserRuleContext {
  public:
    Focus_on_window_activation_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_on_window_activation_valueContext* focus_on_window_activation_value();

  class  Force_display_urgency_hintContext : public antlr4::ParserRuleContext {
  public:
    Force_display_urgency_hintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Force_display_urgency_hintContext* force_display_urgency_hint();

  class  Workspace_auto_back_and_forthContext : public antlr4::ParserRuleContext {
  public:
    Workspace_auto_back_and_forthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_auto_back_and_forthContext* workspace_auto_back_and_forth();

  class  Focus_wrappingContext : public antlr4::ParserRuleContext {
  public:
    Focus_wrappingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_wrappingContext* focus_wrapping();

  class  Force_focus_wrappingContext : public antlr4::ParserRuleContext {
  public:
    Force_focus_wrappingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Force_focus_wrappingContext* force_focus_wrapping();

  class  Mouse_warpingContext : public antlr4::ParserRuleContext {
  public:
    Mouse_warpingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Mouse_warping_valueContext *mouse_warping_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Mouse_warpingContext* mouse_warping();

  class  Mouse_warping_valueContext : public antlr4::ParserRuleContext {
  public:
    Mouse_warping_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Mouse_warping_valueContext* mouse_warping_value();

  class  Focus_follows_mouseContext : public antlr4::ParserRuleContext {
  public:
    Focus_follows_mouseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_follows_mouseContext* focus_follows_mouse();

  class  No_focusContext : public antlr4::ParserRuleContext {
  public:
    No_focusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CriteriaContext *criteria();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  No_focusContext* no_focus();

  class  AssignContext : public antlr4::ParserRuleContext {
  public:
    AssignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CriteriaContext *criteria();
    Assign_targetContext *assign_target();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AssignContext* assign();

  class  Assign_targetContext : public antlr4::ParserRuleContext {
  public:
    Assign_targetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Assign_target_workspaceContext *assign_target_workspace();
    Assign_target_numberContext *assign_target_number();
    Assign_target_outputContext *assign_target_output();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Assign_targetContext* assign_target();

  class  Assign_target_workspaceContext : public antlr4::ParserRuleContext {
  public:
    Assign_target_workspaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Assign_target_workspaceContext* assign_target_workspace();

  class  Assign_target_numberContext : public antlr4::ParserRuleContext {
  public:
    Assign_target_numberContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Assign_target_numberContext* assign_target_number();

  class  Assign_target_outputContext : public antlr4::ParserRuleContext {
  public:
    Assign_target_outputContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Assign_target_outputContext* assign_target_output();

  class  For_windowContext : public antlr4::ParserRuleContext {
  public:
    For_windowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    CriteriaContext *criteria();
    ArgumentsContext *arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  For_windowContext* for_window();

  class  CriteriaContext : public antlr4::ParserRuleContext {
  public:
    CriteriaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<CriterionContext *> criterion();
    CriterionContext* criterion(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CriteriaContext* criteria();

  class  CriterionContext : public antlr4::ParserRuleContext {
  public:
    CriterionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Value_criterionContext *value_criterion();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CriterionContext* criterion();

  class  Value_criterionContext : public antlr4::ParserRuleContext {
  public:
    Value_criterionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Value_criterionContext* value_criterion();

  class  Hide_edge_bordersContext : public antlr4::ParserRuleContext {
  public:
    Hide_edge_bordersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Border_typeContext *border_type();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Hide_edge_bordersContext* hide_edge_borders();

  class  Border_typeContext : public antlr4::ParserRuleContext {
  public:
    Border_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Border_typeContext* border_type();

  class  Default_borderContext : public antlr4::ParserRuleContext {
  public:
    Default_borderContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Default_border_typeContext *default_border_type();
    Border_styleContext *border_style();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_borderContext* default_border();

  class  Default_border_typeContext : public antlr4::ParserRuleContext {
  public:
    Default_border_typeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_border_typeContext* default_border_type();

  class  Border_styleContext : public antlr4::ParserRuleContext {
  public:
    Border_styleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Border_styleContext* border_style();

  class  Workspace_layoutContext : public antlr4::ParserRuleContext {
  public:
    Workspace_layoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Workspace_layout_valueContext *workspace_layout_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_layoutContext* workspace_layout();

  class  Workspace_layout_valueContext : public antlr4::ParserRuleContext {
  public:
    Workspace_layout_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_layout_valueContext* workspace_layout_value();

  class  Default_orientationContext : public antlr4::ParserRuleContext {
  public:
    Default_orientationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Default_orientation_valueContext *default_orientation_value();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_orientationContext* default_orientation();

  class  Default_orientation_valueContext : public antlr4::ParserRuleContext {
  public:
    Default_orientation_valueContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_orientation_valueContext* default_orientation_value();

  class  Floating_modifierContext : public antlr4::ParserRuleContext {
  public:
    Floating_modifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ModifierContext *> modifier();
    ModifierContext* modifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_modifierContext* floating_modifier();

  class  Floating_minimum_sizeContext : public antlr4::ParserRuleContext {
  public:
    Floating_minimum_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DimensionContext *dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_minimum_sizeContext* floating_minimum_size();

  class  Floating_maximum_sizeContext : public antlr4::ParserRuleContext {
  public:
    Floating_maximum_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DimensionContext *dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_maximum_sizeContext* floating_maximum_size();

  class  DimensionContext : public antlr4::ParserRuleContext {
  public:
    DimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> NUMBER();
    antlr4::tree::TerminalNode* NUMBER(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DimensionContext* dimension();

  class  FontContext : public antlr4::ParserRuleContext {
  public:
    FontContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ArgumentsContext *arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FontContext* font();

  class  SetContext : public antlr4::ParserRuleContext {
  public:
    SetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VARIABLE();
    ArgumentsContext *arguments();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SetContext* set();

  class  Set_from_resourceContext : public antlr4::ParserRuleContext {
  public:
    Set_from_resourceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VARIABLE();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Set_from_resourceContext* set_from_resource();

  class  IncludeContext : public antlr4::ParserRuleContext {
  public:
    IncludeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  IncludeContext* include();

  class  BindingContext : public antlr4::ParserRuleContext {
  public:
    BindingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    KeybindingContext *keybinding();
    ArgumentsContext *arguments();
    std::vector<Binding_optionsContext *> binding_options();
    Binding_optionsContext* binding_options(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BindingContext* binding();

  class  Binding_optionsContext : public antlr4::ParserRuleContext {
  public:
    Binding_optionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Binding_optionsContext* binding_options();

  class  ModifierContext : public antlr4::ParserRuleContext {
  public:
    ModifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VARIABLE();
    std::vector<ModifierContext *> modifier();
    ModifierContext* modifier(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ModifierContext* modifier();
  ModifierContext* modifier(int precedence);
  class  KeybindingContext : public antlr4::ParserRuleContext {
  public:
    KeybindingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  KeybindingContext* keybinding();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool modifierSempred(ModifierContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

