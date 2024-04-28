
// Generated from commandsLexer.g4 by ANTLR 4.13.1


#include "commandsLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct CommandsLexerStaticData final {
  CommandsLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  CommandsLexerStaticData(const CommandsLexerStaticData&) = delete;
  CommandsLexerStaticData(CommandsLexerStaticData&&) = delete;
  CommandsLexerStaticData& operator=(const CommandsLexerStaticData&) = delete;
  CommandsLexerStaticData& operator=(CommandsLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag commandslexerLexerOnceFlag;
#if ANTLR4_USE_THREAD_LOCAL_CACHE
static thread_local
#endif
CommandsLexerStaticData *commandslexerLexerStaticData = nullptr;

void commandslexerLexerInitialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  if (commandslexerLexerStaticData != nullptr) {
    return;
  }
#else
  assert(commandslexerLexerStaticData == nullptr);
#endif
  auto staticData = std::make_unique<CommandsLexerStaticData>(
    std::vector<std::string>{
      "OPEN_COMMAND", "CLOSE_COMMAND", "COMMAND_SEP", "COMMAND_MOVE", "COMMAND_EXEC", 
      "COMMAND_EXIT", "COMMAND_DEBUGLOG", "COMMAND_BORDER", "COMMAND_LAYOUT", 
      "COMMAND_APPEND_LAYOUT", "COMMAND_WORKSPACE", "COMMAND_FOCUS", "COMMAND_KILL", 
      "COMMAND_RELOAD", "COMMAND_RESTART", "COMMAND_OPEN", "COMMAND_FULLSCREEN", 
      "COMMAND_STICKY", "COMMAND_SPLIT", "COMMAND_FLOATING", "COMMAND_RESIZE", 
      "COMMAND_RENAME", "COMMAND_NOP", "COMMAND_TITLE_FORMAT", "COMMAND_TITLE_WINDOW_ICON", 
      "COMMAND_MODE", "COMMAND_WS", "COMMAND_OPTION", "COMMAND_NUMBER", 
      "COMMAND_STRING"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE", "COMMAND"
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
  	4,0,30,283,6,-1,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,
  	2,6,7,6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,
  	13,2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,
  	20,2,21,7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,
  	27,2,28,7,28,2,29,7,29,1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,2,1,2,1,
  	3,1,3,1,3,1,3,1,3,1,4,1,4,1,4,1,4,1,4,1,5,1,5,1,5,1,5,1,5,1,6,1,6,1,6,
  	1,6,1,6,1,6,1,6,1,6,1,6,1,7,1,7,1,7,1,7,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,
  	8,1,8,1,8,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,9,1,10,
  	1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,10,1,11,1,11,1,11,1,11,1,11,
  	1,11,1,12,1,12,1,12,1,12,1,12,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,14,
  	1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,1,16,1,16,
  	1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,
  	1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,
  	1,19,1,19,1,19,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,21,1,21,1,21,1,21,
  	1,21,1,21,1,21,1,22,1,22,1,22,1,22,1,23,1,23,1,23,1,23,1,23,1,23,1,23,
  	1,23,1,23,1,23,1,23,1,23,1,23,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,25,1,25,1,25,1,25,
  	1,25,1,26,1,26,1,26,1,26,1,27,1,27,1,27,1,27,1,27,5,27,263,8,27,10,27,
  	12,27,266,9,27,1,28,4,28,269,8,28,11,28,12,28,270,1,29,1,29,1,29,1,29,
  	5,29,277,8,29,10,29,12,29,280,9,29,1,29,1,29,0,0,30,2,1,4,2,6,3,8,4,10,
  	5,12,6,14,7,16,8,18,9,20,10,22,11,24,12,26,13,28,14,30,15,32,16,34,17,
  	36,18,38,19,40,20,42,21,44,22,46,23,48,24,50,25,52,26,54,27,56,28,58,
  	29,60,30,2,0,1,4,2,0,48,57,97,122,3,0,45,45,48,57,97,122,1,0,48,57,1,
  	0,34,34,285,0,2,1,0,0,0,1,4,1,0,0,0,1,6,1,0,0,0,1,8,1,0,0,0,1,10,1,0,
  	0,0,1,12,1,0,0,0,1,14,1,0,0,0,1,16,1,0,0,0,1,18,1,0,0,0,1,20,1,0,0,0,
  	1,22,1,0,0,0,1,24,1,0,0,0,1,26,1,0,0,0,1,28,1,0,0,0,1,30,1,0,0,0,1,32,
  	1,0,0,0,1,34,1,0,0,0,1,36,1,0,0,0,1,38,1,0,0,0,1,40,1,0,0,0,1,42,1,0,
  	0,0,1,44,1,0,0,0,1,46,1,0,0,0,1,48,1,0,0,0,1,50,1,0,0,0,1,52,1,0,0,0,
  	1,54,1,0,0,0,1,56,1,0,0,0,1,58,1,0,0,0,1,60,1,0,0,0,2,62,1,0,0,0,4,67,
  	1,0,0,0,6,71,1,0,0,0,8,73,1,0,0,0,10,78,1,0,0,0,12,83,1,0,0,0,14,88,1,
  	0,0,0,16,97,1,0,0,0,18,104,1,0,0,0,20,111,1,0,0,0,22,125,1,0,0,0,24,135,
  	1,0,0,0,26,141,1,0,0,0,28,146,1,0,0,0,30,153,1,0,0,0,32,161,1,0,0,0,34,
  	166,1,0,0,0,36,177,1,0,0,0,38,184,1,0,0,0,40,190,1,0,0,0,42,199,1,0,0,
  	0,44,206,1,0,0,0,46,213,1,0,0,0,48,217,1,0,0,0,50,230,1,0,0,0,52,248,
  	1,0,0,0,54,253,1,0,0,0,56,257,1,0,0,0,58,268,1,0,0,0,60,272,1,0,0,0,62,
  	63,5,36,0,0,63,64,5,40,0,0,64,65,1,0,0,0,65,66,6,0,0,0,66,3,1,0,0,0,67,
  	68,5,41,0,0,68,69,1,0,0,0,69,70,6,1,1,0,70,5,1,0,0,0,71,72,5,59,0,0,72,
  	7,1,0,0,0,73,74,5,109,0,0,74,75,5,111,0,0,75,76,5,118,0,0,76,77,5,101,
  	0,0,77,9,1,0,0,0,78,79,5,101,0,0,79,80,5,120,0,0,80,81,5,101,0,0,81,82,
  	5,99,0,0,82,11,1,0,0,0,83,84,5,101,0,0,84,85,5,120,0,0,85,86,5,105,0,
  	0,86,87,5,116,0,0,87,13,1,0,0,0,88,89,5,100,0,0,89,90,5,101,0,0,90,91,
  	5,98,0,0,91,92,5,117,0,0,92,93,5,103,0,0,93,94,5,108,0,0,94,95,5,111,
  	0,0,95,96,5,103,0,0,96,15,1,0,0,0,97,98,5,98,0,0,98,99,5,111,0,0,99,100,
  	5,114,0,0,100,101,5,100,0,0,101,102,5,101,0,0,102,103,5,114,0,0,103,17,
  	1,0,0,0,104,105,5,108,0,0,105,106,5,97,0,0,106,107,5,121,0,0,107,108,
  	5,111,0,0,108,109,5,117,0,0,109,110,5,116,0,0,110,19,1,0,0,0,111,112,
  	5,97,0,0,112,113,5,112,0,0,113,114,5,112,0,0,114,115,5,101,0,0,115,116,
  	5,110,0,0,116,117,5,100,0,0,117,118,5,95,0,0,118,119,5,108,0,0,119,120,
  	5,97,0,0,120,121,5,121,0,0,121,122,5,111,0,0,122,123,5,117,0,0,123,124,
  	5,116,0,0,124,21,1,0,0,0,125,126,5,119,0,0,126,127,5,111,0,0,127,128,
  	5,114,0,0,128,129,5,107,0,0,129,130,5,115,0,0,130,131,5,112,0,0,131,132,
  	5,97,0,0,132,133,5,99,0,0,133,134,5,101,0,0,134,23,1,0,0,0,135,136,5,
  	102,0,0,136,137,5,111,0,0,137,138,5,99,0,0,138,139,5,117,0,0,139,140,
  	5,115,0,0,140,25,1,0,0,0,141,142,5,107,0,0,142,143,5,105,0,0,143,144,
  	5,108,0,0,144,145,5,108,0,0,145,27,1,0,0,0,146,147,5,114,0,0,147,148,
  	5,101,0,0,148,149,5,108,0,0,149,150,5,111,0,0,150,151,5,97,0,0,151,152,
  	5,100,0,0,152,29,1,0,0,0,153,154,5,114,0,0,154,155,5,101,0,0,155,156,
  	5,115,0,0,156,157,5,116,0,0,157,158,5,97,0,0,158,159,5,114,0,0,159,160,
  	5,116,0,0,160,31,1,0,0,0,161,162,5,111,0,0,162,163,5,112,0,0,163,164,
  	5,101,0,0,164,165,5,110,0,0,165,33,1,0,0,0,166,167,5,102,0,0,167,168,
  	5,117,0,0,168,169,5,108,0,0,169,170,5,108,0,0,170,171,5,115,0,0,171,172,
  	5,99,0,0,172,173,5,114,0,0,173,174,5,101,0,0,174,175,5,101,0,0,175,176,
  	5,110,0,0,176,35,1,0,0,0,177,178,5,115,0,0,178,179,5,116,0,0,179,180,
  	5,105,0,0,180,181,5,99,0,0,181,182,5,107,0,0,182,183,5,121,0,0,183,37,
  	1,0,0,0,184,185,5,115,0,0,185,186,5,112,0,0,186,187,5,108,0,0,187,188,
  	5,105,0,0,188,189,5,116,0,0,189,39,1,0,0,0,190,191,5,102,0,0,191,192,
  	5,108,0,0,192,193,5,111,0,0,193,194,5,97,0,0,194,195,5,116,0,0,195,196,
  	5,105,0,0,196,197,5,110,0,0,197,198,5,103,0,0,198,41,1,0,0,0,199,200,
  	5,114,0,0,200,201,5,101,0,0,201,202,5,115,0,0,202,203,5,105,0,0,203,204,
  	5,122,0,0,204,205,5,101,0,0,205,43,1,0,0,0,206,207,5,114,0,0,207,208,
  	5,101,0,0,208,209,5,110,0,0,209,210,5,97,0,0,210,211,5,109,0,0,211,212,
  	5,101,0,0,212,45,1,0,0,0,213,214,5,110,0,0,214,215,5,111,0,0,215,216,
  	5,112,0,0,216,47,1,0,0,0,217,218,5,116,0,0,218,219,5,105,0,0,219,220,
  	5,116,0,0,220,221,5,108,0,0,221,222,5,101,0,0,222,223,5,95,0,0,223,224,
  	5,102,0,0,224,225,5,111,0,0,225,226,5,114,0,0,226,227,5,109,0,0,227,228,
  	5,97,0,0,228,229,5,116,0,0,229,49,1,0,0,0,230,231,5,116,0,0,231,232,5,
  	105,0,0,232,233,5,116,0,0,233,234,5,108,0,0,234,235,5,101,0,0,235,236,
  	5,95,0,0,236,237,5,119,0,0,237,238,5,105,0,0,238,239,5,110,0,0,239,240,
  	5,100,0,0,240,241,5,111,0,0,241,242,5,119,0,0,242,243,5,95,0,0,243,244,
  	5,105,0,0,244,245,5,99,0,0,245,246,5,111,0,0,246,247,5,110,0,0,247,51,
  	1,0,0,0,248,249,5,109,0,0,249,250,5,111,0,0,250,251,5,100,0,0,251,252,
  	5,101,0,0,252,53,1,0,0,0,253,254,5,32,0,0,254,255,1,0,0,0,255,256,6,26,
  	2,0,256,55,1,0,0,0,257,258,5,45,0,0,258,259,5,45,0,0,259,260,1,0,0,0,
  	260,264,7,0,0,0,261,263,7,1,0,0,262,261,1,0,0,0,263,266,1,0,0,0,264,262,
  	1,0,0,0,264,265,1,0,0,0,265,57,1,0,0,0,266,264,1,0,0,0,267,269,7,2,0,
  	0,268,267,1,0,0,0,269,270,1,0,0,0,270,268,1,0,0,0,270,271,1,0,0,0,271,
  	59,1,0,0,0,272,278,5,34,0,0,273,274,5,92,0,0,274,277,5,34,0,0,275,277,
  	8,3,0,0,276,273,1,0,0,0,276,275,1,0,0,0,277,280,1,0,0,0,278,276,1,0,0,
  	0,278,279,1,0,0,0,279,281,1,0,0,0,280,278,1,0,0,0,281,282,5,34,0,0,282,
  	61,1,0,0,0,6,0,1,264,270,276,278,3,5,1,0,4,0,0,6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  commandslexerLexerStaticData = staticData.release();
}

}

commandsLexer::commandsLexer(CharStream *input) : Lexer(input) {
  commandsLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *commandslexerLexerStaticData->atn, commandslexerLexerStaticData->decisionToDFA, commandslexerLexerStaticData->sharedContextCache);
}

commandsLexer::~commandsLexer() {
  delete _interpreter;
}

std::string commandsLexer::getGrammarFileName() const {
  return "commandsLexer.g4";
}

const std::vector<std::string>& commandsLexer::getRuleNames() const {
  return commandslexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& commandsLexer::getChannelNames() const {
  return commandslexerLexerStaticData->channelNames;
}

const std::vector<std::string>& commandsLexer::getModeNames() const {
  return commandslexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& commandsLexer::getVocabulary() const {
  return commandslexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView commandsLexer::getSerializedATN() const {
  return commandslexerLexerStaticData->serializedATN;
}

const atn::ATN& commandsLexer::getATN() const {
  return *commandslexerLexerStaticData->atn;
}




void commandsLexer::initialize() {
#if ANTLR4_USE_THREAD_LOCAL_CACHE
  commandslexerLexerInitialize();
#else
  ::antlr4::internal::call_once(commandslexerLexerOnceFlag, commandslexerLexerInitialize);
#endif
}
