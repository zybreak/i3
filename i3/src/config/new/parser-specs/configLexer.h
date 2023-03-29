
// Generated from configLexer.g4 by ANTLR 4.11.1

#pragma once


#include "antlr4-runtime.h"




class  configLexer : public antlr4::Lexer {
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
    COMMAND_DEBUGLOG = 43, COMMAND_BORDER = 44, COMMAND_LAYOUT = 45, COMMAND_APPEND_LAYOUT = 46, 
    COMMAND_WORKSPACE = 47, COMMAND_FOCUS = 48, COMMAND_KILL = 49, COMMAND_RELOAD = 50, 
    COMMAND_RESTART = 51, COMMAND_OPEN = 52, COMMAND_FULLSCREEN = 53, COMMAND_STICKY = 54, 
    COMMAND_SPLIT = 55, COMMAND_FLOATING = 56, COMMAND_RESIZE = 57, COMMAND_RENAME = 58, 
    COMMAND_NOP = 59, COMMAND_SCRATCHPAD = 60, COMMAND_TITLE_FORMAT = 61, 
    COMMAND_TITLE_WINDOW_ICON = 62, COMMAND_MODE = 63, COMMAND_WS = 64, 
    COMMAND_OPTION = 65, COMMAND_NUMBER = 66, COMMAND_STRING = 67, END_CRITERION = 68, 
    CRITERION_STRING = 69, TILING = 70, FLOATING = 71, ALL = 72, CLASS = 73, 
    INSTANCE = 74, WINDOW_ROLE = 75, CON_ID = 76, ID = 77, WINDOW_TYPE = 78, 
    TITLE = 79, URGENT = 80, MACHINE = 81, FLOATING_FROM = 82, TILING_FROM = 83, 
    CRITERION_EQ = 84, CRITERION_SEP = 85
  };

  enum {
    COMMAND = 1, CRITERION = 2
  };

  explicit configLexer(antlr4::CharStream *input);

  ~configLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

