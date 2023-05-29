
// Generated from commandsGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"




class  commandsGrammar : public antlr4::Parser {
public:
  enum {
    OPEN_COMMAND = 1, CLOSE_COMMAND = 2, COMMAND_SEP = 3, COMMAND_MOVE = 4, 
    COMMAND_EXEC = 5, COMMAND_EXIT = 6, COMMAND_DEBUGLOG = 7, COMMAND_BORDER = 8, 
    COMMAND_LAYOUT = 9, COMMAND_APPEND_LAYOUT = 10, COMMAND_WORKSPACE = 11, 
    COMMAND_FOCUS = 12, COMMAND_KILL = 13, COMMAND_RELOAD = 14, COMMAND_RESTART = 15, 
    COMMAND_OPEN = 16, COMMAND_FULLSCREEN = 17, COMMAND_STICKY = 18, COMMAND_SPLIT = 19, 
    COMMAND_FLOATING = 20, COMMAND_RESIZE = 21, COMMAND_RENAME = 22, COMMAND_NOP = 23, 
    COMMAND_SCRATCHPAD = 24, COMMAND_TITLE_FORMAT = 25, COMMAND_TITLE_WINDOW_ICON = 26, 
    COMMAND_MODE = 27, COMMAND_WS = 28, COMMAND_OPTION = 29, COMMAND_NUMBER = 30, 
    COMMAND_STRING = 31
  };

  enum {
    RuleCommands = 0, RuleCommand = 1, RuleCommand_move = 2, RuleCommand_exec = 3, 
    RuleCommand_exit = 4, RuleCommand_kill = 5, RuleCommand_fullscreen = 6, 
    RuleCommand_sticky = 7, RuleCommand_debuglog = 8, RuleCommand_border = 9, 
    RuleCommand_layout = 10, RuleCommand_append_layout = 11, RuleCommand_workspace = 12, 
    RuleCommand_focus = 13, RuleCommand_split = 14, RuleCommand_floating = 15, 
    RuleCommand_resize = 16, RuleCommand_rename = 17, RuleCommand_nop = 18, 
    RuleCommand_reload = 19, RuleCommand_restart = 20, RuleCommand_mode = 21, 
    RuleCommand_open = 22, RuleCommand_scratchpad = 23, RuleCommand_title_format = 24, 
    RuleCommand_title_window_icon = 25
  };

  explicit commandsGrammar(antlr4::TokenStream *input);

  commandsGrammar(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~commandsGrammar() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class CommandsContext;
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
  class Command_scratchpadContext;
  class Command_title_formatContext;
  class Command_title_window_iconContext; 

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
    Command_scratchpadContext *command_scratchpad();
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

  class  Command_scratchpadContext : public antlr4::ParserRuleContext {
  public:
    Command_scratchpadContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COMMAND_SCRATCHPAD();
    antlr4::tree::TerminalNode *COMMAND_STRING();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;
   
  };

  Command_scratchpadContext* command_scratchpad();

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


  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

