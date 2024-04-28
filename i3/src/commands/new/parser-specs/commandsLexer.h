
// Generated from commandsLexer.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"




class  commandsLexer : public antlr4::Lexer {
public:
  enum {
    OPEN_COMMAND = 1, CLOSE_COMMAND = 2, COMMAND_SEP = 3, COMMAND_MOVE = 4, 
    COMMAND_EXEC = 5, COMMAND_EXIT = 6, COMMAND_DEBUGLOG = 7, COMMAND_BORDER = 8, 
    COMMAND_LAYOUT = 9, COMMAND_APPEND_LAYOUT = 10, COMMAND_WORKSPACE = 11, 
    COMMAND_FOCUS = 12, COMMAND_KILL = 13, COMMAND_RELOAD = 14, COMMAND_RESTART = 15, 
    COMMAND_OPEN = 16, COMMAND_FULLSCREEN = 17, COMMAND_STICKY = 18, COMMAND_SPLIT = 19, 
    COMMAND_FLOATING = 20, COMMAND_RESIZE = 21, COMMAND_RENAME = 22, COMMAND_NOP = 23, 
    COMMAND_TITLE_FORMAT = 24, COMMAND_TITLE_WINDOW_ICON = 25, COMMAND_MODE = 26, 
    COMMAND_WS = 27, COMMAND_OPTION = 28, COMMAND_NUMBER = 29, COMMAND_STRING = 30
  };

  enum {
    COMMAND = 1
  };

  explicit commandsLexer(antlr4::CharStream *input);

  ~commandsLexer() override;


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

