
// Generated from commandsGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "commandsGrammar.h"


/**
 * This interface defines an abstract listener for a parse tree produced by commandsGrammar.
 */
class  commandsGrammarListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterCommands(commandsGrammar::CommandsContext *ctx) = 0;
  virtual void exitCommands(commandsGrammar::CommandsContext *ctx) = 0;

  virtual void enterCommand(commandsGrammar::CommandContext *ctx) = 0;
  virtual void exitCommand(commandsGrammar::CommandContext *ctx) = 0;

  virtual void enterCommand_move(commandsGrammar::Command_moveContext *ctx) = 0;
  virtual void exitCommand_move(commandsGrammar::Command_moveContext *ctx) = 0;

  virtual void enterCommand_exec(commandsGrammar::Command_execContext *ctx) = 0;
  virtual void exitCommand_exec(commandsGrammar::Command_execContext *ctx) = 0;

  virtual void enterCommand_exit(commandsGrammar::Command_exitContext *ctx) = 0;
  virtual void exitCommand_exit(commandsGrammar::Command_exitContext *ctx) = 0;

  virtual void enterCommand_kill(commandsGrammar::Command_killContext *ctx) = 0;
  virtual void exitCommand_kill(commandsGrammar::Command_killContext *ctx) = 0;

  virtual void enterCommand_fullscreen(commandsGrammar::Command_fullscreenContext *ctx) = 0;
  virtual void exitCommand_fullscreen(commandsGrammar::Command_fullscreenContext *ctx) = 0;

  virtual void enterCommand_sticky(commandsGrammar::Command_stickyContext *ctx) = 0;
  virtual void exitCommand_sticky(commandsGrammar::Command_stickyContext *ctx) = 0;

  virtual void enterCommand_debuglog(commandsGrammar::Command_debuglogContext *ctx) = 0;
  virtual void exitCommand_debuglog(commandsGrammar::Command_debuglogContext *ctx) = 0;

  virtual void enterCommand_border(commandsGrammar::Command_borderContext *ctx) = 0;
  virtual void exitCommand_border(commandsGrammar::Command_borderContext *ctx) = 0;

  virtual void enterCommand_layout(commandsGrammar::Command_layoutContext *ctx) = 0;
  virtual void exitCommand_layout(commandsGrammar::Command_layoutContext *ctx) = 0;

  virtual void enterCommand_append_layout(commandsGrammar::Command_append_layoutContext *ctx) = 0;
  virtual void exitCommand_append_layout(commandsGrammar::Command_append_layoutContext *ctx) = 0;

  virtual void enterCommand_workspace(commandsGrammar::Command_workspaceContext *ctx) = 0;
  virtual void exitCommand_workspace(commandsGrammar::Command_workspaceContext *ctx) = 0;

  virtual void enterCommand_focus(commandsGrammar::Command_focusContext *ctx) = 0;
  virtual void exitCommand_focus(commandsGrammar::Command_focusContext *ctx) = 0;

  virtual void enterCommand_split(commandsGrammar::Command_splitContext *ctx) = 0;
  virtual void exitCommand_split(commandsGrammar::Command_splitContext *ctx) = 0;

  virtual void enterCommand_floating(commandsGrammar::Command_floatingContext *ctx) = 0;
  virtual void exitCommand_floating(commandsGrammar::Command_floatingContext *ctx) = 0;

  virtual void enterCommand_resize(commandsGrammar::Command_resizeContext *ctx) = 0;
  virtual void exitCommand_resize(commandsGrammar::Command_resizeContext *ctx) = 0;

  virtual void enterCommand_rename(commandsGrammar::Command_renameContext *ctx) = 0;
  virtual void exitCommand_rename(commandsGrammar::Command_renameContext *ctx) = 0;

  virtual void enterCommand_nop(commandsGrammar::Command_nopContext *ctx) = 0;
  virtual void exitCommand_nop(commandsGrammar::Command_nopContext *ctx) = 0;

  virtual void enterCommand_reload(commandsGrammar::Command_reloadContext *ctx) = 0;
  virtual void exitCommand_reload(commandsGrammar::Command_reloadContext *ctx) = 0;

  virtual void enterCommand_restart(commandsGrammar::Command_restartContext *ctx) = 0;
  virtual void exitCommand_restart(commandsGrammar::Command_restartContext *ctx) = 0;

  virtual void enterCommand_mode(commandsGrammar::Command_modeContext *ctx) = 0;
  virtual void exitCommand_mode(commandsGrammar::Command_modeContext *ctx) = 0;

  virtual void enterCommand_open(commandsGrammar::Command_openContext *ctx) = 0;
  virtual void exitCommand_open(commandsGrammar::Command_openContext *ctx) = 0;

  virtual void enterCommand_scratchpad(commandsGrammar::Command_scratchpadContext *ctx) = 0;
  virtual void exitCommand_scratchpad(commandsGrammar::Command_scratchpadContext *ctx) = 0;

  virtual void enterCommand_title_format(commandsGrammar::Command_title_formatContext *ctx) = 0;
  virtual void exitCommand_title_format(commandsGrammar::Command_title_formatContext *ctx) = 0;

  virtual void enterCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext *ctx) = 0;
  virtual void exitCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext *ctx) = 0;


};

