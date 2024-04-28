
// Generated from commandsGrammar.g4 by ANTLR 4.13.1


#include "commandsGrammarListener.h"

#include "commandsGrammar.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct CommandsGrammarStaticData final {
  CommandsGrammarStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CommandsGrammarStaticData(const CommandsGrammarStaticData&) = delete;
  CommandsGrammarStaticData(CommandsGrammarStaticData&&) = delete;
  CommandsGrammarStaticData& operator=(const CommandsGrammarStaticData&) = delete;
  CommandsGrammarStaticData& operator=(CommandsGrammarStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag commandsgrammarParserOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
CommandsGrammarStaticData *commandsgrammarParserStaticData = nullptr;

void commandsgrammarParserInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (commandsgrammarParserStaticData != nullptr) {
    return;
  }
#else
  assert(commandsgrammarParserStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CommandsGrammarStaticData>(
    std::vector<std::string>{
      "commands", "command", "command_move", "command_exec", "command_exit", 
      "command_kill", "command_fullscreen", "command_sticky", "command_debuglog", 
      "command_border", "command_layout", "command_append_layout", "command_workspace", 
      "command_focus", "command_split", "command_floating", "command_resize", 
      "command_rename", "command_nop", "command_reload", "command_restart", 
      "command_mode", "command_open", "command_title_format", "command_title_window_icon"
    },
    std::vector<std::string>{
      "", "'$('", "')'", "';'", "'move'", "'exec'", "'exit'", "'debuglog'", 
      "'border'", "'layout'", "'append_layout'", "'workspace'", "'focus'", 
      "'kill'", "'reload'", "'restart'", "'open'", "'fullscreen'", "'sticky'", 
      "'split'", "'floating'", "'resize'", "'rename'", "'nop'", "'title_format'", 
      "'title_window_icon'", "'mode'", "' '"
    },
    std::vector<std::string>{
      "", "OPEN_COMMAND", "CLOSE_COMMAND", "COMMAND_SEP", "COMMAND_MOVE", 
      "COMMAND_EXEC", "COMMAND_EXIT", "COMMAND_DEBUGLOG", "COMMAND_BORDER", 
      "COMMAND_LAYOUT", "COMMAND_APPEND_LAYOUT", "COMMAND_WORKSPACE", "COMMAND_FOCUS", 
      "COMMAND_KILL", "COMMAND_RELOAD", "COMMAND_RESTART", "COMMAND_OPEN", 
      "COMMAND_FULLSCREEN", "COMMAND_STICKY", "COMMAND_SPLIT", "COMMAND_FLOATING", 
      "COMMAND_RESIZE", "COMMAND_RENAME", "COMMAND_NOP", "COMMAND_TITLE_FORMAT", 
      "COMMAND_TITLE_WINDOW_ICON", "COMMAND_MODE", "COMMAND_WS", "COMMAND_OPTION", 
      "COMMAND_NUMBER", "COMMAND_STRING"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,30,174,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,1,0,1,0,1,0,1,0,5,0,55,8,0,10,0,12,0,
  	58,9,0,1,0,1,0,3,0,62,8,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,89,8,1,1,
  	2,1,2,1,2,1,3,1,3,5,3,96,8,3,10,3,12,3,99,9,3,1,3,1,3,1,4,1,4,1,5,1,5,
  	1,5,1,6,1,6,1,6,3,6,111,8,6,1,7,1,7,1,7,1,8,1,8,1,8,1,9,1,9,1,9,3,9,122,
  	8,9,1,10,1,10,1,10,3,10,127,8,10,1,11,1,11,1,11,1,12,1,12,5,12,134,8,
  	12,10,12,12,12,137,9,12,1,12,1,12,1,13,1,13,1,13,3,13,144,8,13,1,14,1,
  	14,1,14,1,15,1,15,1,15,1,16,1,16,1,17,1,17,1,18,1,18,3,18,158,8,18,1,
  	19,1,19,1,20,1,20,1,21,1,21,1,21,1,22,1,22,1,23,1,23,1,23,1,24,1,24,1,
  	24,0,0,25,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,
  	44,46,48,0,1,1,0,29,30,179,0,50,1,0,0,0,2,88,1,0,0,0,4,90,1,0,0,0,6,93,
  	1,0,0,0,8,102,1,0,0,0,10,104,1,0,0,0,12,107,1,0,0,0,14,112,1,0,0,0,16,
  	115,1,0,0,0,18,118,1,0,0,0,20,123,1,0,0,0,22,128,1,0,0,0,24,131,1,0,0,
  	0,26,140,1,0,0,0,28,145,1,0,0,0,30,148,1,0,0,0,32,151,1,0,0,0,34,153,
  	1,0,0,0,36,155,1,0,0,0,38,159,1,0,0,0,40,161,1,0,0,0,42,163,1,0,0,0,44,
  	166,1,0,0,0,46,168,1,0,0,0,48,171,1,0,0,0,50,56,5,1,0,0,51,52,3,2,1,0,
  	52,53,5,3,0,0,53,55,1,0,0,0,54,51,1,0,0,0,55,58,1,0,0,0,56,54,1,0,0,0,
  	56,57,1,0,0,0,57,59,1,0,0,0,58,56,1,0,0,0,59,61,3,2,1,0,60,62,5,3,0,0,
  	61,60,1,0,0,0,61,62,1,0,0,0,62,63,1,0,0,0,63,64,5,2,0,0,64,1,1,0,0,0,
  	65,89,3,4,2,0,66,89,3,6,3,0,67,89,3,8,4,0,68,89,3,16,8,0,69,89,3,18,9,
  	0,70,89,3,20,10,0,71,89,3,22,11,0,72,89,3,24,12,0,73,89,3,26,13,0,74,
  	89,3,10,5,0,75,89,3,42,21,0,76,89,3,44,22,0,77,89,3,12,6,0,78,89,3,14,
  	7,0,79,89,3,28,14,0,80,89,3,30,15,0,81,89,3,32,16,0,82,89,3,34,17,0,83,
  	89,3,36,18,0,84,89,3,38,19,0,85,89,3,40,20,0,86,89,3,46,23,0,87,89,3,
  	48,24,0,88,65,1,0,0,0,88,66,1,0,0,0,88,67,1,0,0,0,88,68,1,0,0,0,88,69,
  	1,0,0,0,88,70,1,0,0,0,88,71,1,0,0,0,88,72,1,0,0,0,88,73,1,0,0,0,88,74,
  	1,0,0,0,88,75,1,0,0,0,88,76,1,0,0,0,88,77,1,0,0,0,88,78,1,0,0,0,88,79,
  	1,0,0,0,88,80,1,0,0,0,88,81,1,0,0,0,88,82,1,0,0,0,88,83,1,0,0,0,88,84,
  	1,0,0,0,88,85,1,0,0,0,88,86,1,0,0,0,88,87,1,0,0,0,89,3,1,0,0,0,90,91,
  	5,4,0,0,91,92,7,0,0,0,92,5,1,0,0,0,93,97,5,5,0,0,94,96,5,28,0,0,95,94,
  	1,0,0,0,96,99,1,0,0,0,97,95,1,0,0,0,97,98,1,0,0,0,98,100,1,0,0,0,99,97,
  	1,0,0,0,100,101,5,30,0,0,101,7,1,0,0,0,102,103,5,6,0,0,103,9,1,0,0,0,
  	104,105,5,13,0,0,105,106,5,30,0,0,106,11,1,0,0,0,107,108,5,17,0,0,108,
  	110,5,30,0,0,109,111,5,30,0,0,110,109,1,0,0,0,110,111,1,0,0,0,111,13,
  	1,0,0,0,112,113,5,18,0,0,113,114,5,30,0,0,114,15,1,0,0,0,115,116,5,7,
  	0,0,116,117,5,30,0,0,117,17,1,0,0,0,118,119,5,8,0,0,119,121,5,30,0,0,
  	120,122,5,29,0,0,121,120,1,0,0,0,121,122,1,0,0,0,122,19,1,0,0,0,123,124,
  	5,9,0,0,124,126,5,30,0,0,125,127,5,30,0,0,126,125,1,0,0,0,126,127,1,0,
  	0,0,127,21,1,0,0,0,128,129,5,10,0,0,129,130,5,30,0,0,130,23,1,0,0,0,131,
  	135,5,11,0,0,132,134,5,28,0,0,133,132,1,0,0,0,134,137,1,0,0,0,135,133,
  	1,0,0,0,135,136,1,0,0,0,136,138,1,0,0,0,137,135,1,0,0,0,138,139,5,30,
  	0,0,139,25,1,0,0,0,140,141,5,12,0,0,141,143,5,30,0,0,142,144,5,30,0,0,
  	143,142,1,0,0,0,143,144,1,0,0,0,144,27,1,0,0,0,145,146,5,19,0,0,146,147,
  	5,30,0,0,147,29,1,0,0,0,148,149,5,20,0,0,149,150,5,30,0,0,150,31,1,0,
  	0,0,151,152,5,21,0,0,152,33,1,0,0,0,153,154,5,22,0,0,154,35,1,0,0,0,155,
  	157,5,23,0,0,156,158,5,30,0,0,157,156,1,0,0,0,157,158,1,0,0,0,158,37,
  	1,0,0,0,159,160,5,14,0,0,160,39,1,0,0,0,161,162,5,15,0,0,162,41,1,0,0,
  	0,163,164,5,26,0,0,164,165,5,30,0,0,165,43,1,0,0,0,166,167,5,16,0,0,167,
  	45,1,0,0,0,168,169,5,24,0,0,169,170,5,30,0,0,170,47,1,0,0,0,171,172,5,
  	25,0,0,172,49,1,0,0,0,10,56,61,88,97,110,121,126,135,143,157
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  commandsgrammarParserStaticData = staticData.release();
}

}

commandsGrammar::commandsGrammar(TokenStream *input) : commandsGrammar(input, antlr4::atn::ParserATNSimulatorOptions()) {}

commandsGrammar::commandsGrammar(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  commandsGrammar::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *commandsgrammarParserStaticData->atn, commandsgrammarParserStaticData->decisionToDFA, commandsgrammarParserStaticData->sharedContextCache, options);
}

commandsGrammar::~commandsGrammar() {
  delete _interpreter;
}

const atn::ATN& commandsGrammar::getATN() const {
  return *commandsgrammarParserStaticData->atn;
}

std::string commandsGrammar::getGrammarFileName() const {
  return "commandsGrammar.g4";
}

const std::vector<std::string>& commandsGrammar::getRuleNames() const {
  return commandsgrammarParserStaticData->ruleNames;
}

const dfa::Vocabulary& commandsGrammar::getVocabulary() const {
  return commandsgrammarParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView commandsGrammar::getSerializedATN() const {
  return commandsgrammarParserStaticData->serializedATN;
}


//----------------- CommandsContext ------------------------------------------------------------------

commandsGrammar::CommandsContext::CommandsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::CommandsContext::OPEN_COMMAND() {
  return getToken(commandsGrammar::OPEN_COMMAND, 0);
}

std::vector<commandsGrammar::CommandContext *> commandsGrammar::CommandsContext::command() {
  return getRuleContexts<commandsGrammar::CommandContext>();
}

commandsGrammar::CommandContext* commandsGrammar::CommandsContext::command(size_t i) {
  return getRuleContext<commandsGrammar::CommandContext>(i);
}

tree::TerminalNode* commandsGrammar::CommandsContext::CLOSE_COMMAND() {
  return getToken(commandsGrammar::CLOSE_COMMAND, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::CommandsContext::COMMAND_SEP() {
  return getTokens(commandsGrammar::COMMAND_SEP);
}

tree::TerminalNode* commandsGrammar::CommandsContext::COMMAND_SEP(size_t i) {
  return getToken(commandsGrammar::COMMAND_SEP, i);
}


size_t commandsGrammar::CommandsContext::getRuleIndex() const {
  return commandsGrammar::RuleCommands;
}

void commandsGrammar::CommandsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommands(this);
}

void commandsGrammar::CommandsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommands(this);
}

commandsGrammar::CommandsContext* commandsGrammar::commands() {
  CommandsContext *_localctx = _tracker.createInstance<CommandsContext>(_ctx, getState());
  enterRule(_localctx, 0, commandsGrammar::RuleCommands);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(50);
    match(commandsGrammar::OPEN_COMMAND);
    setState(56);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(51);
        command();
        setState(52);
        match(commandsGrammar::COMMAND_SEP); 
      }
      setState(58);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    }
    setState(59);
    command();
    setState(61);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_SEP) {
      setState(60);
      match(commandsGrammar::COMMAND_SEP);
    }
    setState(63);
    match(commandsGrammar::CLOSE_COMMAND);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CommandContext ------------------------------------------------------------------

commandsGrammar::CommandContext::CommandContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

commandsGrammar::Command_moveContext* commandsGrammar::CommandContext::command_move() {
  return getRuleContext<commandsGrammar::Command_moveContext>(0);
}

commandsGrammar::Command_execContext* commandsGrammar::CommandContext::command_exec() {
  return getRuleContext<commandsGrammar::Command_execContext>(0);
}

commandsGrammar::Command_exitContext* commandsGrammar::CommandContext::command_exit() {
  return getRuleContext<commandsGrammar::Command_exitContext>(0);
}

commandsGrammar::Command_debuglogContext* commandsGrammar::CommandContext::command_debuglog() {
  return getRuleContext<commandsGrammar::Command_debuglogContext>(0);
}

commandsGrammar::Command_borderContext* commandsGrammar::CommandContext::command_border() {
  return getRuleContext<commandsGrammar::Command_borderContext>(0);
}

commandsGrammar::Command_layoutContext* commandsGrammar::CommandContext::command_layout() {
  return getRuleContext<commandsGrammar::Command_layoutContext>(0);
}

commandsGrammar::Command_append_layoutContext* commandsGrammar::CommandContext::command_append_layout() {
  return getRuleContext<commandsGrammar::Command_append_layoutContext>(0);
}

commandsGrammar::Command_workspaceContext* commandsGrammar::CommandContext::command_workspace() {
  return getRuleContext<commandsGrammar::Command_workspaceContext>(0);
}

commandsGrammar::Command_focusContext* commandsGrammar::CommandContext::command_focus() {
  return getRuleContext<commandsGrammar::Command_focusContext>(0);
}

commandsGrammar::Command_killContext* commandsGrammar::CommandContext::command_kill() {
  return getRuleContext<commandsGrammar::Command_killContext>(0);
}

commandsGrammar::Command_modeContext* commandsGrammar::CommandContext::command_mode() {
  return getRuleContext<commandsGrammar::Command_modeContext>(0);
}

commandsGrammar::Command_openContext* commandsGrammar::CommandContext::command_open() {
  return getRuleContext<commandsGrammar::Command_openContext>(0);
}

commandsGrammar::Command_fullscreenContext* commandsGrammar::CommandContext::command_fullscreen() {
  return getRuleContext<commandsGrammar::Command_fullscreenContext>(0);
}

commandsGrammar::Command_stickyContext* commandsGrammar::CommandContext::command_sticky() {
  return getRuleContext<commandsGrammar::Command_stickyContext>(0);
}

commandsGrammar::Command_splitContext* commandsGrammar::CommandContext::command_split() {
  return getRuleContext<commandsGrammar::Command_splitContext>(0);
}

commandsGrammar::Command_floatingContext* commandsGrammar::CommandContext::command_floating() {
  return getRuleContext<commandsGrammar::Command_floatingContext>(0);
}

commandsGrammar::Command_resizeContext* commandsGrammar::CommandContext::command_resize() {
  return getRuleContext<commandsGrammar::Command_resizeContext>(0);
}

commandsGrammar::Command_renameContext* commandsGrammar::CommandContext::command_rename() {
  return getRuleContext<commandsGrammar::Command_renameContext>(0);
}

commandsGrammar::Command_nopContext* commandsGrammar::CommandContext::command_nop() {
  return getRuleContext<commandsGrammar::Command_nopContext>(0);
}

commandsGrammar::Command_reloadContext* commandsGrammar::CommandContext::command_reload() {
  return getRuleContext<commandsGrammar::Command_reloadContext>(0);
}

commandsGrammar::Command_restartContext* commandsGrammar::CommandContext::command_restart() {
  return getRuleContext<commandsGrammar::Command_restartContext>(0);
}

commandsGrammar::Command_title_formatContext* commandsGrammar::CommandContext::command_title_format() {
  return getRuleContext<commandsGrammar::Command_title_formatContext>(0);
}

commandsGrammar::Command_title_window_iconContext* commandsGrammar::CommandContext::command_title_window_icon() {
  return getRuleContext<commandsGrammar::Command_title_window_iconContext>(0);
}


size_t commandsGrammar::CommandContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand;
}

void commandsGrammar::CommandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand(this);
}

void commandsGrammar::CommandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand(this);
}

commandsGrammar::CommandContext* commandsGrammar::command() {
  CommandContext *_localctx = _tracker.createInstance<CommandContext>(_ctx, getState());
  enterRule(_localctx, 2, commandsGrammar::RuleCommand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(88);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case commandsGrammar::COMMAND_MOVE: {
        enterOuterAlt(_localctx, 1);
        setState(65);
        command_move();
        break;
      }

      case commandsGrammar::COMMAND_EXEC: {
        enterOuterAlt(_localctx, 2);
        setState(66);
        command_exec();
        break;
      }

      case commandsGrammar::COMMAND_EXIT: {
        enterOuterAlt(_localctx, 3);
        setState(67);
        command_exit();
        break;
      }

      case commandsGrammar::COMMAND_DEBUGLOG: {
        enterOuterAlt(_localctx, 4);
        setState(68);
        command_debuglog();
        break;
      }

      case commandsGrammar::COMMAND_BORDER: {
        enterOuterAlt(_localctx, 5);
        setState(69);
        command_border();
        break;
      }

      case commandsGrammar::COMMAND_LAYOUT: {
        enterOuterAlt(_localctx, 6);
        setState(70);
        command_layout();
        break;
      }

      case commandsGrammar::COMMAND_APPEND_LAYOUT: {
        enterOuterAlt(_localctx, 7);
        setState(71);
        command_append_layout();
        break;
      }

      case commandsGrammar::COMMAND_WORKSPACE: {
        enterOuterAlt(_localctx, 8);
        setState(72);
        command_workspace();
        break;
      }

      case commandsGrammar::COMMAND_FOCUS: {
        enterOuterAlt(_localctx, 9);
        setState(73);
        command_focus();
        break;
      }

      case commandsGrammar::COMMAND_KILL: {
        enterOuterAlt(_localctx, 10);
        setState(74);
        command_kill();
        break;
      }

      case commandsGrammar::COMMAND_MODE: {
        enterOuterAlt(_localctx, 11);
        setState(75);
        command_mode();
        break;
      }

      case commandsGrammar::COMMAND_OPEN: {
        enterOuterAlt(_localctx, 12);
        setState(76);
        command_open();
        break;
      }

      case commandsGrammar::COMMAND_FULLSCREEN: {
        enterOuterAlt(_localctx, 13);
        setState(77);
        command_fullscreen();
        break;
      }

      case commandsGrammar::COMMAND_STICKY: {
        enterOuterAlt(_localctx, 14);
        setState(78);
        command_sticky();
        break;
      }

      case commandsGrammar::COMMAND_SPLIT: {
        enterOuterAlt(_localctx, 15);
        setState(79);
        command_split();
        break;
      }

      case commandsGrammar::COMMAND_FLOATING: {
        enterOuterAlt(_localctx, 16);
        setState(80);
        command_floating();
        break;
      }

      case commandsGrammar::COMMAND_RESIZE: {
        enterOuterAlt(_localctx, 17);
        setState(81);
        command_resize();
        break;
      }

      case commandsGrammar::COMMAND_RENAME: {
        enterOuterAlt(_localctx, 18);
        setState(82);
        command_rename();
        break;
      }

      case commandsGrammar::COMMAND_NOP: {
        enterOuterAlt(_localctx, 19);
        setState(83);
        command_nop();
        break;
      }

      case commandsGrammar::COMMAND_RELOAD: {
        enterOuterAlt(_localctx, 20);
        setState(84);
        command_reload();
        break;
      }

      case commandsGrammar::COMMAND_RESTART: {
        enterOuterAlt(_localctx, 21);
        setState(85);
        command_restart();
        break;
      }

      case commandsGrammar::COMMAND_TITLE_FORMAT: {
        enterOuterAlt(_localctx, 22);
        setState(86);
        command_title_format();
        break;
      }

      case commandsGrammar::COMMAND_TITLE_WINDOW_ICON: {
        enterOuterAlt(_localctx, 23);
        setState(87);
        command_title_window_icon();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_moveContext ------------------------------------------------------------------

commandsGrammar::Command_moveContext::Command_moveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_moveContext::COMMAND_MOVE() {
  return getToken(commandsGrammar::COMMAND_MOVE, 0);
}

tree::TerminalNode* commandsGrammar::Command_moveContext::COMMAND_NUMBER() {
  return getToken(commandsGrammar::COMMAND_NUMBER, 0);
}

tree::TerminalNode* commandsGrammar::Command_moveContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_moveContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_move;
}

void commandsGrammar::Command_moveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_move(this);
}

void commandsGrammar::Command_moveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_move(this);
}

commandsGrammar::Command_moveContext* commandsGrammar::command_move() {
  Command_moveContext *_localctx = _tracker.createInstance<Command_moveContext>(_ctx, getState());
  enterRule(_localctx, 4, commandsGrammar::RuleCommand_move);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(90);
    match(commandsGrammar::COMMAND_MOVE);
    setState(91);
    _la = _input->LA(1);
    if (!(_la == commandsGrammar::COMMAND_NUMBER

    || _la == commandsGrammar::COMMAND_STRING)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_execContext ------------------------------------------------------------------

commandsGrammar::Command_execContext::Command_execContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_execContext::COMMAND_EXEC() {
  return getToken(commandsGrammar::COMMAND_EXEC, 0);
}

tree::TerminalNode* commandsGrammar::Command_execContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::Command_execContext::COMMAND_OPTION() {
  return getTokens(commandsGrammar::COMMAND_OPTION);
}

tree::TerminalNode* commandsGrammar::Command_execContext::COMMAND_OPTION(size_t i) {
  return getToken(commandsGrammar::COMMAND_OPTION, i);
}


size_t commandsGrammar::Command_execContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_exec;
}

void commandsGrammar::Command_execContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_exec(this);
}

void commandsGrammar::Command_execContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_exec(this);
}

commandsGrammar::Command_execContext* commandsGrammar::command_exec() {
  Command_execContext *_localctx = _tracker.createInstance<Command_execContext>(_ctx, getState());
  enterRule(_localctx, 6, commandsGrammar::RuleCommand_exec);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(93);
    match(commandsGrammar::COMMAND_EXEC);
    setState(97);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == commandsGrammar::COMMAND_OPTION) {
      setState(94);
      match(commandsGrammar::COMMAND_OPTION);
      setState(99);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(100);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_exitContext ------------------------------------------------------------------

commandsGrammar::Command_exitContext::Command_exitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_exitContext::COMMAND_EXIT() {
  return getToken(commandsGrammar::COMMAND_EXIT, 0);
}


size_t commandsGrammar::Command_exitContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_exit;
}

void commandsGrammar::Command_exitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_exit(this);
}

void commandsGrammar::Command_exitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_exit(this);
}

commandsGrammar::Command_exitContext* commandsGrammar::command_exit() {
  Command_exitContext *_localctx = _tracker.createInstance<Command_exitContext>(_ctx, getState());
  enterRule(_localctx, 8, commandsGrammar::RuleCommand_exit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(102);
    match(commandsGrammar::COMMAND_EXIT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_killContext ------------------------------------------------------------------

commandsGrammar::Command_killContext::Command_killContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_killContext::COMMAND_KILL() {
  return getToken(commandsGrammar::COMMAND_KILL, 0);
}

tree::TerminalNode* commandsGrammar::Command_killContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_killContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_kill;
}

void commandsGrammar::Command_killContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_kill(this);
}

void commandsGrammar::Command_killContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_kill(this);
}

commandsGrammar::Command_killContext* commandsGrammar::command_kill() {
  Command_killContext *_localctx = _tracker.createInstance<Command_killContext>(_ctx, getState());
  enterRule(_localctx, 10, commandsGrammar::RuleCommand_kill);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(104);
    match(commandsGrammar::COMMAND_KILL);
    setState(105);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_fullscreenContext ------------------------------------------------------------------

commandsGrammar::Command_fullscreenContext::Command_fullscreenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_fullscreenContext::COMMAND_FULLSCREEN() {
  return getToken(commandsGrammar::COMMAND_FULLSCREEN, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::Command_fullscreenContext::COMMAND_STRING() {
  return getTokens(commandsGrammar::COMMAND_STRING);
}

tree::TerminalNode* commandsGrammar::Command_fullscreenContext::COMMAND_STRING(size_t i) {
  return getToken(commandsGrammar::COMMAND_STRING, i);
}


size_t commandsGrammar::Command_fullscreenContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_fullscreen;
}

void commandsGrammar::Command_fullscreenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_fullscreen(this);
}

void commandsGrammar::Command_fullscreenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_fullscreen(this);
}

commandsGrammar::Command_fullscreenContext* commandsGrammar::command_fullscreen() {
  Command_fullscreenContext *_localctx = _tracker.createInstance<Command_fullscreenContext>(_ctx, getState());
  enterRule(_localctx, 12, commandsGrammar::RuleCommand_fullscreen);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(107);
    match(commandsGrammar::COMMAND_FULLSCREEN);
    setState(108);
    match(commandsGrammar::COMMAND_STRING);
    setState(110);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_STRING) {
      setState(109);
      match(commandsGrammar::COMMAND_STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_stickyContext ------------------------------------------------------------------

commandsGrammar::Command_stickyContext::Command_stickyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_stickyContext::COMMAND_STICKY() {
  return getToken(commandsGrammar::COMMAND_STICKY, 0);
}

tree::TerminalNode* commandsGrammar::Command_stickyContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_stickyContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_sticky;
}

void commandsGrammar::Command_stickyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_sticky(this);
}

void commandsGrammar::Command_stickyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_sticky(this);
}

commandsGrammar::Command_stickyContext* commandsGrammar::command_sticky() {
  Command_stickyContext *_localctx = _tracker.createInstance<Command_stickyContext>(_ctx, getState());
  enterRule(_localctx, 14, commandsGrammar::RuleCommand_sticky);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(112);
    match(commandsGrammar::COMMAND_STICKY);
    setState(113);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_debuglogContext ------------------------------------------------------------------

commandsGrammar::Command_debuglogContext::Command_debuglogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_debuglogContext::COMMAND_DEBUGLOG() {
  return getToken(commandsGrammar::COMMAND_DEBUGLOG, 0);
}

tree::TerminalNode* commandsGrammar::Command_debuglogContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_debuglogContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_debuglog;
}

void commandsGrammar::Command_debuglogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_debuglog(this);
}

void commandsGrammar::Command_debuglogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_debuglog(this);
}

commandsGrammar::Command_debuglogContext* commandsGrammar::command_debuglog() {
  Command_debuglogContext *_localctx = _tracker.createInstance<Command_debuglogContext>(_ctx, getState());
  enterRule(_localctx, 16, commandsGrammar::RuleCommand_debuglog);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(115);
    match(commandsGrammar::COMMAND_DEBUGLOG);
    setState(116);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_borderContext ------------------------------------------------------------------

commandsGrammar::Command_borderContext::Command_borderContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_borderContext::COMMAND_BORDER() {
  return getToken(commandsGrammar::COMMAND_BORDER, 0);
}

tree::TerminalNode* commandsGrammar::Command_borderContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}

tree::TerminalNode* commandsGrammar::Command_borderContext::COMMAND_NUMBER() {
  return getToken(commandsGrammar::COMMAND_NUMBER, 0);
}


size_t commandsGrammar::Command_borderContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_border;
}

void commandsGrammar::Command_borderContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_border(this);
}

void commandsGrammar::Command_borderContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_border(this);
}

commandsGrammar::Command_borderContext* commandsGrammar::command_border() {
  Command_borderContext *_localctx = _tracker.createInstance<Command_borderContext>(_ctx, getState());
  enterRule(_localctx, 18, commandsGrammar::RuleCommand_border);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(118);
    match(commandsGrammar::COMMAND_BORDER);
    setState(119);
    match(commandsGrammar::COMMAND_STRING);
    setState(121);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_NUMBER) {
      setState(120);
      match(commandsGrammar::COMMAND_NUMBER);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_layoutContext ------------------------------------------------------------------

commandsGrammar::Command_layoutContext::Command_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_layoutContext::COMMAND_LAYOUT() {
  return getToken(commandsGrammar::COMMAND_LAYOUT, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::Command_layoutContext::COMMAND_STRING() {
  return getTokens(commandsGrammar::COMMAND_STRING);
}

tree::TerminalNode* commandsGrammar::Command_layoutContext::COMMAND_STRING(size_t i) {
  return getToken(commandsGrammar::COMMAND_STRING, i);
}


size_t commandsGrammar::Command_layoutContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_layout;
}

void commandsGrammar::Command_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_layout(this);
}

void commandsGrammar::Command_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_layout(this);
}

commandsGrammar::Command_layoutContext* commandsGrammar::command_layout() {
  Command_layoutContext *_localctx = _tracker.createInstance<Command_layoutContext>(_ctx, getState());
  enterRule(_localctx, 20, commandsGrammar::RuleCommand_layout);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(123);
    match(commandsGrammar::COMMAND_LAYOUT);
    setState(124);
    match(commandsGrammar::COMMAND_STRING);
    setState(126);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_STRING) {
      setState(125);
      match(commandsGrammar::COMMAND_STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_append_layoutContext ------------------------------------------------------------------

commandsGrammar::Command_append_layoutContext::Command_append_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_append_layoutContext::COMMAND_APPEND_LAYOUT() {
  return getToken(commandsGrammar::COMMAND_APPEND_LAYOUT, 0);
}

tree::TerminalNode* commandsGrammar::Command_append_layoutContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_append_layoutContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_append_layout;
}

void commandsGrammar::Command_append_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_append_layout(this);
}

void commandsGrammar::Command_append_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_append_layout(this);
}

commandsGrammar::Command_append_layoutContext* commandsGrammar::command_append_layout() {
  Command_append_layoutContext *_localctx = _tracker.createInstance<Command_append_layoutContext>(_ctx, getState());
  enterRule(_localctx, 22, commandsGrammar::RuleCommand_append_layout);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(128);
    match(commandsGrammar::COMMAND_APPEND_LAYOUT);
    setState(129);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_workspaceContext ------------------------------------------------------------------

commandsGrammar::Command_workspaceContext::Command_workspaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_workspaceContext::COMMAND_WORKSPACE() {
  return getToken(commandsGrammar::COMMAND_WORKSPACE, 0);
}

tree::TerminalNode* commandsGrammar::Command_workspaceContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::Command_workspaceContext::COMMAND_OPTION() {
  return getTokens(commandsGrammar::COMMAND_OPTION);
}

tree::TerminalNode* commandsGrammar::Command_workspaceContext::COMMAND_OPTION(size_t i) {
  return getToken(commandsGrammar::COMMAND_OPTION, i);
}


size_t commandsGrammar::Command_workspaceContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_workspace;
}

void commandsGrammar::Command_workspaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_workspace(this);
}

void commandsGrammar::Command_workspaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_workspace(this);
}

commandsGrammar::Command_workspaceContext* commandsGrammar::command_workspace() {
  Command_workspaceContext *_localctx = _tracker.createInstance<Command_workspaceContext>(_ctx, getState());
  enterRule(_localctx, 24, commandsGrammar::RuleCommand_workspace);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(131);
    match(commandsGrammar::COMMAND_WORKSPACE);
    setState(135);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == commandsGrammar::COMMAND_OPTION) {
      setState(132);
      match(commandsGrammar::COMMAND_OPTION);
      setState(137);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(138);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_focusContext ------------------------------------------------------------------

commandsGrammar::Command_focusContext::Command_focusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_focusContext::COMMAND_FOCUS() {
  return getToken(commandsGrammar::COMMAND_FOCUS, 0);
}

std::vector<tree::TerminalNode *> commandsGrammar::Command_focusContext::COMMAND_STRING() {
  return getTokens(commandsGrammar::COMMAND_STRING);
}

tree::TerminalNode* commandsGrammar::Command_focusContext::COMMAND_STRING(size_t i) {
  return getToken(commandsGrammar::COMMAND_STRING, i);
}


size_t commandsGrammar::Command_focusContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_focus;
}

void commandsGrammar::Command_focusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_focus(this);
}

void commandsGrammar::Command_focusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_focus(this);
}

commandsGrammar::Command_focusContext* commandsGrammar::command_focus() {
  Command_focusContext *_localctx = _tracker.createInstance<Command_focusContext>(_ctx, getState());
  enterRule(_localctx, 26, commandsGrammar::RuleCommand_focus);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(140);
    match(commandsGrammar::COMMAND_FOCUS);
    setState(141);
    match(commandsGrammar::COMMAND_STRING);
    setState(143);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_STRING) {
      setState(142);
      match(commandsGrammar::COMMAND_STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_splitContext ------------------------------------------------------------------

commandsGrammar::Command_splitContext::Command_splitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_splitContext::COMMAND_SPLIT() {
  return getToken(commandsGrammar::COMMAND_SPLIT, 0);
}

tree::TerminalNode* commandsGrammar::Command_splitContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_splitContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_split;
}

void commandsGrammar::Command_splitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_split(this);
}

void commandsGrammar::Command_splitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_split(this);
}

commandsGrammar::Command_splitContext* commandsGrammar::command_split() {
  Command_splitContext *_localctx = _tracker.createInstance<Command_splitContext>(_ctx, getState());
  enterRule(_localctx, 28, commandsGrammar::RuleCommand_split);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(145);
    match(commandsGrammar::COMMAND_SPLIT);
    setState(146);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_floatingContext ------------------------------------------------------------------

commandsGrammar::Command_floatingContext::Command_floatingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_floatingContext::COMMAND_FLOATING() {
  return getToken(commandsGrammar::COMMAND_FLOATING, 0);
}

tree::TerminalNode* commandsGrammar::Command_floatingContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_floatingContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_floating;
}

void commandsGrammar::Command_floatingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_floating(this);
}

void commandsGrammar::Command_floatingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_floating(this);
}

commandsGrammar::Command_floatingContext* commandsGrammar::command_floating() {
  Command_floatingContext *_localctx = _tracker.createInstance<Command_floatingContext>(_ctx, getState());
  enterRule(_localctx, 30, commandsGrammar::RuleCommand_floating);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(148);
    match(commandsGrammar::COMMAND_FLOATING);
    setState(149);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_resizeContext ------------------------------------------------------------------

commandsGrammar::Command_resizeContext::Command_resizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_resizeContext::COMMAND_RESIZE() {
  return getToken(commandsGrammar::COMMAND_RESIZE, 0);
}


size_t commandsGrammar::Command_resizeContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_resize;
}

void commandsGrammar::Command_resizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_resize(this);
}

void commandsGrammar::Command_resizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_resize(this);
}

commandsGrammar::Command_resizeContext* commandsGrammar::command_resize() {
  Command_resizeContext *_localctx = _tracker.createInstance<Command_resizeContext>(_ctx, getState());
  enterRule(_localctx, 32, commandsGrammar::RuleCommand_resize);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(151);
    match(commandsGrammar::COMMAND_RESIZE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_renameContext ------------------------------------------------------------------

commandsGrammar::Command_renameContext::Command_renameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_renameContext::COMMAND_RENAME() {
  return getToken(commandsGrammar::COMMAND_RENAME, 0);
}


size_t commandsGrammar::Command_renameContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_rename;
}

void commandsGrammar::Command_renameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_rename(this);
}

void commandsGrammar::Command_renameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_rename(this);
}

commandsGrammar::Command_renameContext* commandsGrammar::command_rename() {
  Command_renameContext *_localctx = _tracker.createInstance<Command_renameContext>(_ctx, getState());
  enterRule(_localctx, 34, commandsGrammar::RuleCommand_rename);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(153);
    match(commandsGrammar::COMMAND_RENAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_nopContext ------------------------------------------------------------------

commandsGrammar::Command_nopContext::Command_nopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_nopContext::COMMAND_NOP() {
  return getToken(commandsGrammar::COMMAND_NOP, 0);
}

tree::TerminalNode* commandsGrammar::Command_nopContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_nopContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_nop;
}

void commandsGrammar::Command_nopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_nop(this);
}

void commandsGrammar::Command_nopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_nop(this);
}

commandsGrammar::Command_nopContext* commandsGrammar::command_nop() {
  Command_nopContext *_localctx = _tracker.createInstance<Command_nopContext>(_ctx, getState());
  enterRule(_localctx, 36, commandsGrammar::RuleCommand_nop);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(155);
    match(commandsGrammar::COMMAND_NOP);
    setState(157);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == commandsGrammar::COMMAND_STRING) {
      setState(156);
      match(commandsGrammar::COMMAND_STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_reloadContext ------------------------------------------------------------------

commandsGrammar::Command_reloadContext::Command_reloadContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_reloadContext::COMMAND_RELOAD() {
  return getToken(commandsGrammar::COMMAND_RELOAD, 0);
}


size_t commandsGrammar::Command_reloadContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_reload;
}

void commandsGrammar::Command_reloadContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_reload(this);
}

void commandsGrammar::Command_reloadContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_reload(this);
}

commandsGrammar::Command_reloadContext* commandsGrammar::command_reload() {
  Command_reloadContext *_localctx = _tracker.createInstance<Command_reloadContext>(_ctx, getState());
  enterRule(_localctx, 38, commandsGrammar::RuleCommand_reload);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(159);
    match(commandsGrammar::COMMAND_RELOAD);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_restartContext ------------------------------------------------------------------

commandsGrammar::Command_restartContext::Command_restartContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_restartContext::COMMAND_RESTART() {
  return getToken(commandsGrammar::COMMAND_RESTART, 0);
}


size_t commandsGrammar::Command_restartContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_restart;
}

void commandsGrammar::Command_restartContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_restart(this);
}

void commandsGrammar::Command_restartContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_restart(this);
}

commandsGrammar::Command_restartContext* commandsGrammar::command_restart() {
  Command_restartContext *_localctx = _tracker.createInstance<Command_restartContext>(_ctx, getState());
  enterRule(_localctx, 40, commandsGrammar::RuleCommand_restart);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(161);
    match(commandsGrammar::COMMAND_RESTART);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_modeContext ------------------------------------------------------------------

commandsGrammar::Command_modeContext::Command_modeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_modeContext::COMMAND_MODE() {
  return getToken(commandsGrammar::COMMAND_MODE, 0);
}

tree::TerminalNode* commandsGrammar::Command_modeContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_modeContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_mode;
}

void commandsGrammar::Command_modeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_mode(this);
}

void commandsGrammar::Command_modeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_mode(this);
}

commandsGrammar::Command_modeContext* commandsGrammar::command_mode() {
  Command_modeContext *_localctx = _tracker.createInstance<Command_modeContext>(_ctx, getState());
  enterRule(_localctx, 42, commandsGrammar::RuleCommand_mode);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(163);
    match(commandsGrammar::COMMAND_MODE);
    setState(164);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_openContext ------------------------------------------------------------------

commandsGrammar::Command_openContext::Command_openContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_openContext::COMMAND_OPEN() {
  return getToken(commandsGrammar::COMMAND_OPEN, 0);
}


size_t commandsGrammar::Command_openContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_open;
}

void commandsGrammar::Command_openContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_open(this);
}

void commandsGrammar::Command_openContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_open(this);
}

commandsGrammar::Command_openContext* commandsGrammar::command_open() {
  Command_openContext *_localctx = _tracker.createInstance<Command_openContext>(_ctx, getState());
  enterRule(_localctx, 44, commandsGrammar::RuleCommand_open);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(166);
    match(commandsGrammar::COMMAND_OPEN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_title_formatContext ------------------------------------------------------------------

commandsGrammar::Command_title_formatContext::Command_title_formatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_title_formatContext::COMMAND_TITLE_FORMAT() {
  return getToken(commandsGrammar::COMMAND_TITLE_FORMAT, 0);
}

tree::TerminalNode* commandsGrammar::Command_title_formatContext::COMMAND_STRING() {
  return getToken(commandsGrammar::COMMAND_STRING, 0);
}


size_t commandsGrammar::Command_title_formatContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_title_format;
}

void commandsGrammar::Command_title_formatContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_title_format(this);
}

void commandsGrammar::Command_title_formatContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_title_format(this);
}

commandsGrammar::Command_title_formatContext* commandsGrammar::command_title_format() {
  Command_title_formatContext *_localctx = _tracker.createInstance<Command_title_formatContext>(_ctx, getState());
  enterRule(_localctx, 46, commandsGrammar::RuleCommand_title_format);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(168);
    match(commandsGrammar::COMMAND_TITLE_FORMAT);
    setState(169);
    match(commandsGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_title_window_iconContext ------------------------------------------------------------------

commandsGrammar::Command_title_window_iconContext::Command_title_window_iconContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* commandsGrammar::Command_title_window_iconContext::COMMAND_TITLE_WINDOW_ICON() {
  return getToken(commandsGrammar::COMMAND_TITLE_WINDOW_ICON, 0);
}


size_t commandsGrammar::Command_title_window_iconContext::getRuleIndex() const {
  return commandsGrammar::RuleCommand_title_window_icon;
}

void commandsGrammar::Command_title_window_iconContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_title_window_icon(this);
}

void commandsGrammar::Command_title_window_iconContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<commandsGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_title_window_icon(this);
}

commandsGrammar::Command_title_window_iconContext* commandsGrammar::command_title_window_icon() {
  Command_title_window_iconContext *_localctx = _tracker.createInstance<Command_title_window_iconContext>(_ctx, getState());
  enterRule(_localctx, 48, commandsGrammar::RuleCommand_title_window_icon);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(171);
    match(commandsGrammar::COMMAND_TITLE_WINDOW_ICON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void commandsGrammar::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  commandsgrammarParserInitialize();
#else
  ::antlr4::internal::call_once(commandsgrammarParserOnceFlag, commandsgrammarParserInitialize);
#endif
}
