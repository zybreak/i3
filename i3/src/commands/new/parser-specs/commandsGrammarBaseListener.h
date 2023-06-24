
// Generated from commandsGrammar.g4 by ANTLR 4.12.0

#pragma once


#include "antlr4-runtime.h"
#include "commandsGrammarListener.h"


/**
 * This class provides an empty implementation of commandsGrammarListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  commandsGrammarBaseListener : public commandsGrammarListener {
public:

  virtual void enterCommands(commandsGrammar::CommandsContext * /*ctx*/) override { }
  virtual void exitCommands(commandsGrammar::CommandsContext * /*ctx*/) override { }

  virtual void enterCommand(commandsGrammar::CommandContext * /*ctx*/) override { }
  virtual void exitCommand(commandsGrammar::CommandContext * /*ctx*/) override { }

  virtual void enterCommand_move(commandsGrammar::Command_moveContext * /*ctx*/) override { }
  virtual void exitCommand_move(commandsGrammar::Command_moveContext * /*ctx*/) override { }

  virtual void enterCommand_exec(commandsGrammar::Command_execContext * /*ctx*/) override { }
  virtual void exitCommand_exec(commandsGrammar::Command_execContext * /*ctx*/) override { }

  virtual void enterCommand_exit(commandsGrammar::Command_exitContext * /*ctx*/) override { }
  virtual void exitCommand_exit(commandsGrammar::Command_exitContext * /*ctx*/) override { }

  virtual void enterCommand_kill(commandsGrammar::Command_killContext * /*ctx*/) override { }
  virtual void exitCommand_kill(commandsGrammar::Command_killContext * /*ctx*/) override { }

  virtual void enterCommand_fullscreen(commandsGrammar::Command_fullscreenContext * /*ctx*/) override { }
  virtual void exitCommand_fullscreen(commandsGrammar::Command_fullscreenContext * /*ctx*/) override { }

  virtual void enterCommand_sticky(commandsGrammar::Command_stickyContext * /*ctx*/) override { }
  virtual void exitCommand_sticky(commandsGrammar::Command_stickyContext * /*ctx*/) override { }

  virtual void enterCommand_debuglog(commandsGrammar::Command_debuglogContext * /*ctx*/) override { }
  virtual void exitCommand_debuglog(commandsGrammar::Command_debuglogContext * /*ctx*/) override { }

  virtual void enterCommand_border(commandsGrammar::Command_borderContext * /*ctx*/) override { }
  virtual void exitCommand_border(commandsGrammar::Command_borderContext * /*ctx*/) override { }

  virtual void enterCommand_layout(commandsGrammar::Command_layoutContext * /*ctx*/) override { }
  virtual void exitCommand_layout(commandsGrammar::Command_layoutContext * /*ctx*/) override { }

  virtual void enterCommand_append_layout(commandsGrammar::Command_append_layoutContext * /*ctx*/) override { }
  virtual void exitCommand_append_layout(commandsGrammar::Command_append_layoutContext * /*ctx*/) override { }

  virtual void enterCommand_workspace(commandsGrammar::Command_workspaceContext * /*ctx*/) override { }
  virtual void exitCommand_workspace(commandsGrammar::Command_workspaceContext * /*ctx*/) override { }

  virtual void enterCommand_focus(commandsGrammar::Command_focusContext * /*ctx*/) override { }
  virtual void exitCommand_focus(commandsGrammar::Command_focusContext * /*ctx*/) override { }

  virtual void enterCommand_split(commandsGrammar::Command_splitContext * /*ctx*/) override { }
  virtual void exitCommand_split(commandsGrammar::Command_splitContext * /*ctx*/) override { }

  virtual void enterCommand_floating(commandsGrammar::Command_floatingContext * /*ctx*/) override { }
  virtual void exitCommand_floating(commandsGrammar::Command_floatingContext * /*ctx*/) override { }

  virtual void enterCommand_resize(commandsGrammar::Command_resizeContext * /*ctx*/) override { }
  virtual void exitCommand_resize(commandsGrammar::Command_resizeContext * /*ctx*/) override { }

  virtual void enterCommand_rename(commandsGrammar::Command_renameContext * /*ctx*/) override { }
  virtual void exitCommand_rename(commandsGrammar::Command_renameContext * /*ctx*/) override { }

  virtual void enterCommand_nop(commandsGrammar::Command_nopContext * /*ctx*/) override { }
  virtual void exitCommand_nop(commandsGrammar::Command_nopContext * /*ctx*/) override { }

  virtual void enterCommand_reload(commandsGrammar::Command_reloadContext * /*ctx*/) override { }
  virtual void exitCommand_reload(commandsGrammar::Command_reloadContext * /*ctx*/) override { }

  virtual void enterCommand_restart(commandsGrammar::Command_restartContext * /*ctx*/) override { }
  virtual void exitCommand_restart(commandsGrammar::Command_restartContext * /*ctx*/) override { }

  virtual void enterCommand_mode(commandsGrammar::Command_modeContext * /*ctx*/) override { }
  virtual void exitCommand_mode(commandsGrammar::Command_modeContext * /*ctx*/) override { }

  virtual void enterCommand_open(commandsGrammar::Command_openContext * /*ctx*/) override { }
  virtual void exitCommand_open(commandsGrammar::Command_openContext * /*ctx*/) override { }

  virtual void enterCommand_title_format(commandsGrammar::Command_title_formatContext * /*ctx*/) override { }
  virtual void exitCommand_title_format(commandsGrammar::Command_title_formatContext * /*ctx*/) override { }

  virtual void enterCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext * /*ctx*/) override { }
  virtual void exitCommand_title_window_icon(commandsGrammar::Command_title_window_iconContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

