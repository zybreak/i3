
// Generated from configGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  configGrammar : public antlr4::Parser {
public:
  enum {
    COMMENT = 1, STRING = 2, NUMBER = 3, DIMENSION_SEP = 4, OPTION = 5, 
    VAR = 6, WS = 7, EOL = 8, SET = 9, EQ = 10, EXEC = 11, BINDSYM = 12, 
    FONT = 13, POPUP_DURING_FULLSCREEN = 14, FLOATING_MINIMUM_SIZE = 15, 
    FLOATING_MAXIMUM_SIZE = 16, FLOATING_MODIFIER = 17, DEFAULT_ORIENTATION = 18, 
    WORKSPACE_LAYOUT = 19, DEFAULT_BORDER = 20, HIDE_EDGE_BORDERS = 21, 
    FOR_WINDOW = 22, ASSIGN = 23, NO_FOCUS = 24, FOCUS_FOLLOWS_MOUSE = 25, 
    MOUSE_WARPING = 26, FOCUS_WRAPPING = 27, FORCE_FOCUS_WRAPPING = 28, 
    WORKSPACE_AUTO_BACK_AND_FORTH = 29, FORCE_DISPLAY_URGENCY_HINT = 30, 
    FOCUS_ON_WINDOW_ACTIVATION = 31, TITLE_ALIGN = 32, WORKSPACE = 33, IPC_SOCKET = 34, 
    IPC_KILL_TIMEOUT = 35, RESTART_STATE = 36, OPEN_COMMAND = 37, BEGIN_CRITERION = 38, 
    CLOSE_COMMAND = 39, COMMAND_SEP = 40, COMMAND_MOVE = 41, COMMAND_EXEC = 42, 
    COMMAND_EXIT = 43, COMMAND_DEBUGLOG = 44, COMMAND_BORDER = 45, COMMAND_LAYOUT = 46, 
    COMMAND_APPEND_LAYOUT = 47, COMMAND_WORKSPACE = 48, COMMAND_FOCUS = 49, 
    COMMAND_KILL = 50, COMMAND_RELOAD = 51, COMMAND_RESTART = 52, COMMAND_OPEN = 53, 
    COMMAND_FULLSCREEN = 54, COMMAND_STICKY = 55, COMMAND_SPLIT = 56, COMMAND_FLOATING = 57, 
    COMMAND_RESIZE = 58, COMMAND_RENAME = 59, COMMAND_NOP = 60, COMMAND_TITLE_FORMAT = 61, 
    COMMAND_TITLE_WINDOW_ICON = 62, COMMAND_MODE = 63, COMMAND_WS = 64, 
    COMMAND_OPTION = 65, COMMAND_NUMBER = 66, COMMAND_STRING = 67, END_CRITERION = 68, 
    CRITERION_STRING = 69, TILING = 70, FLOATING = 71, ALL = 72, CLASS = 73, 
    INSTANCE = 74, WINDOW_ROLE = 75, CON_ID = 76, ID = 77, WINDOW_TYPE = 78, 
    TITLE = 79, URGENT = 80, MACHINE = 81, FLOATING_FROM = 82, TILING_FROM = 83, 
    CRITERION_EQ = 84, CRITERION_SEP = 85
  };

  enum {
    RuleConfig = 0, RuleStmt = 1, RuleCommands = 2, RuleSet = 3, RuleBinding = 4, 
    RuleFont = 5, RuleFloating_minimum_size = 6, RuleFloating_maximum_size = 7, 
    RuleFloating_modifier = 8, RuleDefault_orientation = 9, RuleWorkspace_layout = 10, 
    RuleDefault_border = 11, RuleHide_edge_borders = 12, RuleFor_window = 13, 
    RuleAssign = 14, RuleNo_focus = 15, RuleFocus_follows_mouse = 16, RuleMouse_warping = 17, 
    RuleFocus_wrapping = 18, RuleForce_focus_wrapping = 19, RuleWorkspace_auto_back_and_forth = 20, 
    RuleForce_display_urgency_hint = 21, RuleFocus_on_window_activation = 22, 
    RuleTitle_align = 23, RuleWorkspace = 24, RuleIpc_socket = 25, RuleIpc_kill_timeout = 26, 
    RuleRestart_state = 27, RulePopup_during_fullscreen = 28, RuleExec = 29, 
    RuleDimension = 30, RuleCommand = 31, RuleCommand_move = 32, RuleCommand_exec = 33, 
    RuleCommand_exit = 34, RuleCommand_kill = 35, RuleCommand_fullscreen = 36, 
    RuleCommand_sticky = 37, RuleCommand_debuglog = 38, RuleCommand_border = 39, 
    RuleCommand_layout = 40, RuleCommand_append_layout = 41, RuleCommand_workspace = 42, 
    RuleCommand_focus = 43, RuleCommand_split = 44, RuleCommand_floating = 45, 
    RuleCommand_resize = 46, RuleCommand_rename = 47, RuleCommand_nop = 48, 
    RuleCommand_reload = 49, RuleCommand_restart = 50, RuleCommand_mode = 51, 
    RuleCommand_open = 52, RuleCommand_title_format = 53, RuleCommand_title_window_icon = 54, 
    RuleCriteria = 55, RuleCriterion = 56, RuleValue_criterion = 57
  };

  explicit configGrammar(antlr4::TokenStream *input);

  configGrammar(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~configGrammar() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class ConfigContext;
  class StmtContext;
  class CommandsContext;
  class SetContext;
  class BindingContext;
  class FontContext;
  class Floating_minimum_sizeContext;
  class Floating_maximum_sizeContext;
  class Floating_modifierContext;
  class Default_orientationContext;
  class Workspace_layoutContext;
  class Default_borderContext;
  class Hide_edge_bordersContext;
  class For_windowContext;
  class AssignContext;
  class No_focusContext;
  class Focus_follows_mouseContext;
  class Mouse_warpingContext;
  class Focus_wrappingContext;
  class Force_focus_wrappingContext;
  class Workspace_auto_back_and_forthContext;
  class Force_display_urgency_hintContext;
  class Focus_on_window_activationContext;
  class Title_alignContext;
  class WorkspaceContext;
  class Ipc_socketContext;
  class Ipc_kill_timeoutContext;
  class Restart_stateContext;
  class Popup_during_fullscreenContext;
  class ExecContext;
  class DimensionContext;
  class CommandContext;
  class Command_moveContext;
  class Command_execContext;
  class Command_exitContext;
  class Command_killContext;
  class Command_fullscreenContext;
  class Command_stickyContext;
  class Command_debuglogContext;
  class Command_borderContext;
  class Command_layoutContext;
  class Command_append_layoutContext;
  class Command_workspaceContext;
  class Command_focusContext;
  class Command_splitContext;
  class Command_floatingContext;
  class Command_resizeContext;
  class Command_renameContext;
  class Command_nopContext;
  class Command_reloadContext;
  class Command_restartContext;
  class Command_modeContext;
  class Command_openContext;
  class Command_title_formatContext;
  class Command_title_window_iconContext;
  class CriteriaContext;
  class CriterionContext;
  class Value_criterionContext; 

  class  ConfigContext : public antlr4::ParserRuleContext {
  public:
    ConfigContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<antlr4::tree::TerminalNode *> EOL();
    antlr4::tree::TerminalNode* EOL(size_t i);
    std::vector<StmtContext *> stmt();
    StmtContext* stmt(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ConfigContext* config();

  class  StmtContext : public antlr4::ParserRuleContext {
  public:
    StmtContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    SetContext *set();
    BindingContext *binding();
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

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  StmtContext* stmt();

  class  CommandsContext : public antlr4::ParserRuleContext {
  public:
    CommandsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OPEN_COMMAND();
    std::vector<CommandContext *> command();
    CommandContext* command(size_t i);
    antlr4::tree::TerminalNode *CLOSE_COMMAND();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_SEP();
    antlr4::tree::TerminalNode* COMMAND_SEP(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CommandsContext* commands();

  class  SetContext : public antlr4::ParserRuleContext {
  public:
    SetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SET();
    antlr4::tree::TerminalNode *VAR();
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  SetContext* set();

  class  BindingContext : public antlr4::ParserRuleContext {
  public:
    BindingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BINDSYM();
    antlr4::tree::TerminalNode *STRING();
    CommandsContext *commands();
    std::vector<antlr4::tree::TerminalNode *> OPTION();
    antlr4::tree::TerminalNode* OPTION(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  BindingContext* binding();

  class  FontContext : public antlr4::ParserRuleContext {
  public:
    FontContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FONT();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  FontContext* font();

  class  Floating_minimum_sizeContext : public antlr4::ParserRuleContext {
  public:
    Floating_minimum_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FLOATING_MINIMUM_SIZE();
    DimensionContext *dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_minimum_sizeContext* floating_minimum_size();

  class  Floating_maximum_sizeContext : public antlr4::ParserRuleContext {
  public:
    Floating_maximum_sizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FLOATING_MAXIMUM_SIZE();
    DimensionContext *dimension();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_maximum_sizeContext* floating_maximum_size();

  class  Floating_modifierContext : public antlr4::ParserRuleContext {
  public:
    Floating_modifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FLOATING_MODIFIER();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Floating_modifierContext* floating_modifier();

  class  Default_orientationContext : public antlr4::ParserRuleContext {
  public:
    Default_orientationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFAULT_ORIENTATION();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_orientationContext* default_orientation();

  class  Workspace_layoutContext : public antlr4::ParserRuleContext {
  public:
    Workspace_layoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WORKSPACE_LAYOUT();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_layoutContext* workspace_layout();

  class  Default_borderContext : public antlr4::ParserRuleContext {
  public:
    Default_borderContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFAULT_BORDER();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Default_borderContext* default_border();

  class  Hide_edge_bordersContext : public antlr4::ParserRuleContext {
  public:
    Hide_edge_bordersContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HIDE_EDGE_BORDERS();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Hide_edge_bordersContext* hide_edge_borders();

  class  For_windowContext : public antlr4::ParserRuleContext {
  public:
    For_windowContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FOR_WINDOW();
    CriteriaContext *criteria();
    CommandsContext *commands();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  For_windowContext* for_window();

  class  AssignContext : public antlr4::ParserRuleContext {
  public:
    AssignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASSIGN();
    CriteriaContext *criteria();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  AssignContext* assign();

  class  No_focusContext : public antlr4::ParserRuleContext {
  public:
    No_focusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NO_FOCUS();
    CriteriaContext *criteria();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  No_focusContext* no_focus();

  class  Focus_follows_mouseContext : public antlr4::ParserRuleContext {
  public:
    Focus_follows_mouseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FOCUS_FOLLOWS_MOUSE();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_follows_mouseContext* focus_follows_mouse();

  class  Mouse_warpingContext : public antlr4::ParserRuleContext {
  public:
    Mouse_warpingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MOUSE_WARPING();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Mouse_warpingContext* mouse_warping();

  class  Focus_wrappingContext : public antlr4::ParserRuleContext {
  public:
    Focus_wrappingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FOCUS_WRAPPING();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_wrappingContext* focus_wrapping();

  class  Force_focus_wrappingContext : public antlr4::ParserRuleContext {
  public:
    Force_focus_wrappingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FORCE_FOCUS_WRAPPING();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Force_focus_wrappingContext* force_focus_wrapping();

  class  Workspace_auto_back_and_forthContext : public antlr4::ParserRuleContext {
  public:
    Workspace_auto_back_and_forthContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WORKSPACE_AUTO_BACK_AND_FORTH();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Workspace_auto_back_and_forthContext* workspace_auto_back_and_forth();

  class  Force_display_urgency_hintContext : public antlr4::ParserRuleContext {
  public:
    Force_display_urgency_hintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FORCE_DISPLAY_URGENCY_HINT();
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Force_display_urgency_hintContext* force_display_urgency_hint();

  class  Focus_on_window_activationContext : public antlr4::ParserRuleContext {
  public:
    Focus_on_window_activationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FOCUS_ON_WINDOW_ACTIVATION();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Focus_on_window_activationContext* focus_on_window_activation();

  class  Title_alignContext : public antlr4::ParserRuleContext {
  public:
    Title_alignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TITLE_ALIGN();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Title_alignContext* title_align();

  class  WorkspaceContext : public antlr4::ParserRuleContext {
  public:
    WorkspaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WORKSPACE();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *EQ();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  WorkspaceContext* workspace();

  class  Ipc_socketContext : public antlr4::ParserRuleContext {
  public:
    Ipc_socketContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IPC_SOCKET();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Ipc_socketContext* ipc_socket();

  class  Ipc_kill_timeoutContext : public antlr4::ParserRuleContext {
  public:
    Ipc_kill_timeoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IPC_KILL_TIMEOUT();
    antlr4::tree::TerminalNode *NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Ipc_kill_timeoutContext* ipc_kill_timeout();

  class  Restart_stateContext : public antlr4::ParserRuleContext {
  public:
    Restart_stateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RESTART_STATE();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Restart_stateContext* restart_state();

  class  Popup_during_fullscreenContext : public antlr4::ParserRuleContext {
  public:
    Popup_during_fullscreenContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *POPUP_DURING_FULLSCREEN();
    antlr4::tree::TerminalNode *STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Popup_during_fullscreenContext* popup_during_fullscreen();

  class  ExecContext : public antlr4::ParserRuleContext {
  public:
    ExecContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EXEC();
    antlr4::tree::TerminalNode *STRING();
    std::vector<antlr4::tree::TerminalNode *> OPTION();
    antlr4::tree::TerminalNode* OPTION(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  ExecContext* exec();

  class  DimensionContext : public antlr4::ParserRuleContext {
  public:
    DimensionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> NUMBER();
    antlr4::tree::TerminalNode* NUMBER(size_t i);
    antlr4::tree::TerminalNode *DIMENSION_SEP();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  DimensionContext* dimension();

  class  CommandContext : public antlr4::ParserRuleContext {
  public:
    CommandContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Command_moveContext *command_move();
    Command_execContext *command_exec();
    Command_exitContext *command_exit();
    Command_debuglogContext *command_debuglog();
    Command_borderContext *command_border();
    Command_layoutContext *command_layout();
    Command_append_layoutContext *command_append_layout();
    Command_workspaceContext *command_workspace();
    Command_focusContext *command_focus();
    Command_killContext *command_kill();
    Command_modeContext *command_mode();
    Command_openContext *command_open();
    Command_fullscreenContext *command_fullscreen();
    Command_stickyContext *command_sticky();
    Command_splitContext *command_split();
    Command_floatingContext *command_floating();
    Command_resizeContext *command_resize();
    Command_renameContext *command_rename();
    Command_nopContext *command_nop();
    Command_reloadContext *command_reload();
    Command_restartContext *command_restart();
    Command_title_formatContext *command_title_format();
    Command_title_window_iconContext *command_title_window_icon();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CommandContext* command();

  class  Command_moveContext : public antlr4::ParserRuleContext {
  public:
    Command_moveContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_MOVE();
    antlr4::tree::TerminalNode *COMMAND_NUMBER();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_moveContext* command_move();

  class  Command_execContext : public antlr4::ParserRuleContext {
  public:
    Command_execContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_EXEC();
    antlr4::tree::TerminalNode *COMMAND_STRING();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_OPTION();
    antlr4::tree::TerminalNode* COMMAND_OPTION(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_execContext* command_exec();

  class  Command_exitContext : public antlr4::ParserRuleContext {
  public:
    Command_exitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_EXIT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_exitContext* command_exit();

  class  Command_killContext : public antlr4::ParserRuleContext {
  public:
    Command_killContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_KILL();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_killContext* command_kill();

  class  Command_fullscreenContext : public antlr4::ParserRuleContext {
  public:
    Command_fullscreenContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_FULLSCREEN();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_STRING();
    antlr4::tree::TerminalNode* COMMAND_STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_fullscreenContext* command_fullscreen();

  class  Command_stickyContext : public antlr4::ParserRuleContext {
  public:
    Command_stickyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_STICKY();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_stickyContext* command_sticky();

  class  Command_debuglogContext : public antlr4::ParserRuleContext {
  public:
    Command_debuglogContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_DEBUGLOG();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_debuglogContext* command_debuglog();

  class  Command_borderContext : public antlr4::ParserRuleContext {
  public:
    Command_borderContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_BORDER();
    antlr4::tree::TerminalNode *COMMAND_STRING();
    antlr4::tree::TerminalNode *COMMAND_NUMBER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_borderContext* command_border();

  class  Command_layoutContext : public antlr4::ParserRuleContext {
  public:
    Command_layoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_LAYOUT();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_STRING();
    antlr4::tree::TerminalNode* COMMAND_STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_layoutContext* command_layout();

  class  Command_append_layoutContext : public antlr4::ParserRuleContext {
  public:
    Command_append_layoutContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_APPEND_LAYOUT();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_append_layoutContext* command_append_layout();

  class  Command_workspaceContext : public antlr4::ParserRuleContext {
  public:
    Command_workspaceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_WORKSPACE();
    antlr4::tree::TerminalNode *COMMAND_STRING();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_OPTION();
    antlr4::tree::TerminalNode* COMMAND_OPTION(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_workspaceContext* command_workspace();

  class  Command_focusContext : public antlr4::ParserRuleContext {
  public:
    Command_focusContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_FOCUS();
    std::vector<antlr4::tree::TerminalNode *> COMMAND_STRING();
    antlr4::tree::TerminalNode* COMMAND_STRING(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_focusContext* command_focus();

  class  Command_splitContext : public antlr4::ParserRuleContext {
  public:
    Command_splitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_SPLIT();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_splitContext* command_split();

  class  Command_floatingContext : public antlr4::ParserRuleContext {
  public:
    Command_floatingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_FLOATING();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_floatingContext* command_floating();

  class  Command_resizeContext : public antlr4::ParserRuleContext {
  public:
    Command_resizeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_RESIZE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_resizeContext* command_resize();

  class  Command_renameContext : public antlr4::ParserRuleContext {
  public:
    Command_renameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_RENAME();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_renameContext* command_rename();

  class  Command_nopContext : public antlr4::ParserRuleContext {
  public:
    Command_nopContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_NOP();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_nopContext* command_nop();

  class  Command_reloadContext : public antlr4::ParserRuleContext {
  public:
    Command_reloadContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_RELOAD();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_reloadContext* command_reload();

  class  Command_restartContext : public antlr4::ParserRuleContext {
  public:
    Command_restartContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_RESTART();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_restartContext* command_restart();

  class  Command_modeContext : public antlr4::ParserRuleContext {
  public:
    Command_modeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_MODE();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_modeContext* command_mode();

  class  Command_openContext : public antlr4::ParserRuleContext {
  public:
    Command_openContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_OPEN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_openContext* command_open();

  class  Command_title_formatContext : public antlr4::ParserRuleContext {
  public:
    Command_title_formatContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_TITLE_FORMAT();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_title_formatContext* command_title_format();

  class  Command_title_window_iconContext : public antlr4::ParserRuleContext {
  public:
    Command_title_window_iconContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_TITLE_WINDOW_ICON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_title_window_iconContext* command_title_window_icon();

  class  CriteriaContext : public antlr4::ParserRuleContext {
  public:
    CriteriaContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BEGIN_CRITERION();
    antlr4::tree::TerminalNode *END_CRITERION();
    std::vector<CriterionContext *> criterion();
    CriterionContext* criterion(size_t i);
    std::vector<antlr4::tree::TerminalNode *> CRITERION_SEP();
    antlr4::tree::TerminalNode* CRITERION_SEP(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CriteriaContext* criteria();

  class  CriterionContext : public antlr4::ParserRuleContext {
  public:
    CriterionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Value_criterionContext *value_criterion();
    antlr4::tree::TerminalNode *TILING();
    antlr4::tree::TerminalNode *FLOATING();
    antlr4::tree::TerminalNode *ALL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  CriterionContext* criterion();

  class  Value_criterionContext : public antlr4::ParserRuleContext {
  public:
    Value_criterionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CRITERION_EQ();
    antlr4::tree::TerminalNode *CRITERION_STRING();
    antlr4::tree::TerminalNode *CLASS();
    antlr4::tree::TerminalNode *INSTANCE();
    antlr4::tree::TerminalNode *WINDOW_ROLE();
    antlr4::tree::TerminalNode *CON_ID();
    antlr4::tree::TerminalNode *ID();
    antlr4::tree::TerminalNode *WINDOW_TYPE();
    antlr4::tree::TerminalNode *TITLE();
    antlr4::tree::TerminalNode *URGENT();
    antlr4::tree::TerminalNode *WORKSPACE();
    antlr4::tree::TerminalNode *MACHINE();
    antlr4::tree::TerminalNode *FLOATING_FROM();
    antlr4::tree::TerminalNode *TILING_FROM();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Value_criterionContext* value_criterion();


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

