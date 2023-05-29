
// Generated from configLexer.g4 by ANTLR 4.12.0

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
    COMMAND_EXIT = 43, COMMAND_DEBUGLOG = 44, COMMAND_BORDER = 45, COMMAND_LAYOUT = 46, 
    COMMAND_APPEND_LAYOUT = 47, COMMAND_WORKSPACE = 48, COMMAND_FOCUS = 49, 
    COMMAND_KILL = 50, COMMAND_RELOAD = 51, COMMAND_RESTART = 52, COMMAND_OPEN = 53, 
    COMMAND_FULLSCREEN = 54, COMMAND_STICKY = 55, COMMAND_SPLIT = 56, COMMAND_FLOATING = 57, 
    COMMAND_RESIZE = 58, COMMAND_RENAME = 59, COMMAND_NOP = 60, COMMAND_SCRATCHPAD = 61, 
    COMMAND_TITLE_FORMAT = 62, COMMAND_TITLE_WINDOW_ICON = 63, COMMAND_MODE = 64, 
    COMMAND_WS = 65, COMMAND_OPTION = 66, COMMAND_NUMBER = 67, COMMAND_STRING = 68, 
    END_CRITERION = 69, CRITERION_STRING = 70, TILING = 71, FLOATING = 72, 
    ALL = 73, CLASS = 74, INSTANCE = 75, WINDOW_ROLE = 76, CON_ID = 77, 
    ID = 78, WINDOW_TYPE = 79, TITLE = 80, URGENT = 81, MACHINE = 82, FLOATING_FROM = 83, 
    TILING_FROM = 84, CRITERION_EQ = 85, CRITERION_SEP = 86
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

