
// Generated from configLexer.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  configLexer : public antlr4::Lexer {
public:
  enum {
    COMMENT = 1, STRING = 2, NUMBER = 3, DIMENSION_SEP = 4, OPTION = 5, 
    VAR = 6, WS = 7, EOL = 8, SET = 9, EQ = 10, EXEC = 11, EXEC_ALWAYS = 12, 
    BINDSYM = 13, FONT = 14, POPUP_DURING_FULLSCREEN = 15, FLOATING_MINIMUM_SIZE = 16, 
    FLOATING_MAXIMUM_SIZE = 17, FLOATING_MODIFIER = 18, DEFAULT_ORIENTATION = 19, 
    WORKSPACE_LAYOUT = 20, DEFAULT_BORDER = 21, NEW_WINDOW = 22, DEFAULT_FLOATING_BORDER = 23, 
    NEW_FLOAT = 24, HIDE_EDGE_BORDERS = 25, FOR_WINDOW = 26, ASSIGN = 27, 
    NO_FOCUS = 28, FOCUS_FOLLOWS_MOUSE = 29, MOUSE_WARPING = 30, FOCUS_WRAPPING = 31, 
    FORCE_FOCUS_WRAPPING = 32, WORKSPACE_AUTO_BACK_AND_FORTH = 33, FORCE_DISPLAY_URGENCY_HINT = 34, 
    FOCUS_ON_WINDOW_ACTIVATION = 35, TITLE_ALIGN = 36, WORKSPACE = 37, IPC_SOCKET = 38, 
    IPC_KILL_TIMEOUT = 39, RESTART_STATE = 40, OPEN_COMMAND = 41, BEGIN_CRITERION = 42, 
    CLOSE_COMMAND = 43, COMMAND_SEP = 44, COMMAND_MOVE = 45, COMMAND_EXEC = 46, 
    COMMAND_EXIT = 47, COMMAND_DEBUGLOG = 48, COMMAND_BORDER = 49, COMMAND_LAYOUT = 50, 
    COMMAND_APPEND_LAYOUT = 51, COMMAND_WORKSPACE = 52, COMMAND_FOCUS = 53, 
    COMMAND_KILL = 54, COMMAND_RELOAD = 55, COMMAND_RESTART = 56, COMMAND_OPEN = 57, 
    COMMAND_FULLSCREEN = 58, COMMAND_STICKY = 59, COMMAND_SPLIT = 60, COMMAND_FLOATING = 61, 
    COMMAND_RESIZE = 62, COMMAND_RENAME = 63, COMMAND_NOP = 64, COMMAND_TITLE_FORMAT = 65, 
    COMMAND_TITLE_WINDOW_ICON = 66, COMMAND_MODE = 67, COMMAND_WS = 68, 
    COMMAND_OPTION = 69, COMMAND_NUMBER = 70, COMMAND_STRING = 71, END_CRITERION = 72, 
    CRITERION_STRING = 73, TILING = 74, FLOATING = 75, ALL = 76, CLASS = 77, 
    INSTANCE = 78, WINDOW_ROLE = 79, CON_ID = 80, ID = 81, WINDOW_TYPE = 82, 
    TITLE = 83, URGENT = 84, MACHINE = 85, FLOATING_FROM = 86, TILING_FROM = 87, 
    CRITERION_EQ = 88, CRITERION_SEP = 89
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

