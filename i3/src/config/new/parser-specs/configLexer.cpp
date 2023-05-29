
// Generated from configLexer.g4 by ANTLR 4.12.0


#include "configLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct ConfigLexerStaticData final {
  ConfigLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ConfigLexerStaticData(const ConfigLexerStaticData&) = delete;
  ConfigLexerStaticData(ConfigLexerStaticData&&) = delete;
  ConfigLexerStaticData& operator=(const ConfigLexerStaticData&) = delete;
  ConfigLexerStaticData& operator=(ConfigLexerStaticData&&) = delete;

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

::antlr4::internal::OnceFlag configlexerLexerOnceFlag;
ConfigLexerStaticData *configlexerLexerStaticData = nullptr;

void configlexerLexerInitialize() {
  assert(configlexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<ConfigLexerStaticData>(
    std::vector<std::string>{
      "COMMENT", "STRING", "NUMBER", "DIMENSION_SEP", "OPTION", "VAR", "WS", 
      "EOL", "SET", "EQ", "EXEC", "BINDSYM", "FONT", "POPUP_DURING_FULLSCREEN", 
      "FLOATING_MINIMUM_SIZE", "FLOATING_MAXIMUM_SIZE", "FLOATING_MODIFIER", 
      "DEFAULT_ORIENTATION", "WORKSPACE_LAYOUT", "DEFAULT_BORDER", "HIDE_EDGE_BORDERS", 
      "FOR_WINDOW", "ASSIGN", "NO_FOCUS", "FOCUS_FOLLOWS_MOUSE", "MOUSE_WARPING", 
      "FOCUS_WRAPPING", "FORCE_FOCUS_WRAPPING", "WORKSPACE_AUTO_BACK_AND_FORTH", 
      "FORCE_DISPLAY_URGENCY_HINT", "FOCUS_ON_WINDOW_ACTIVATION", "TITLE_ALIGN", 
      "WORKSPACE", "IPC_SOCKET", "IPC_KILL_TIMEOUT", "RESTART_STATE", "OPEN_COMMAND", 
      "BEGIN_CRITERION", "CLOSE_COMMAND", "COMMAND_SEP", "COMMAND_MOVE", 
      "COMMAND_EXEC", "COMMAND_EXIT", "COMMAND_DEBUGLOG", "COMMAND_BORDER", 
      "COMMAND_LAYOUT", "COMMAND_APPEND_LAYOUT", "COMMAND_WORKSPACE", "COMMAND_FOCUS", 
      "COMMAND_KILL", "COMMAND_RELOAD", "COMMAND_RESTART", "COMMAND_OPEN", 
      "COMMAND_FULLSCREEN", "COMMAND_STICKY", "COMMAND_SPLIT", "COMMAND_FLOATING", 
      "COMMAND_RESIZE", "COMMAND_RENAME", "COMMAND_NOP", "COMMAND_SCRATCHPAD", 
      "COMMAND_TITLE_FORMAT", "COMMAND_TITLE_WINDOW_ICON", "COMMAND_MODE", 
      "COMMAND_WS", "COMMAND_OPTION", "COMMAND_NUMBER", "COMMAND_STRING", 
      "END_CRITERION", "CRITERION_STRING", "TILING", "FLOATING", "ALL", 
      "CLASS", "INSTANCE", "WINDOW_ROLE", "CON_ID", "ID", "WINDOW_TYPE", 
      "TITLE", "URGENT", "MACHINE", "FLOATING_FROM", "TILING_FROM", "CRITERION_EQ", 
      "CRITERION_SEP"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE", "COMMAND", "CRITERION"
    },
    std::vector<std::string>{
      "", "", "", "", "'x'", "", "", "", "", "'set'", "", "", "'bindsym'", 
      "'font'", "'popup_during_fullscreen'", "'floating_minimum_size'", 
      "'floating_maximum_size'", "'floating_modifier'", "'default_orientation'", 
      "'workspace_layout'", "'default_border'", "'hide_edge_borders'", "'for_window'", 
      "'assign'", "'no_focus'", "'focus_follows_mouse'", "'mouse_warping'", 
      "'focus_wrapping'", "'force_focus_wrapping'", "'workspace_auto_back_and_forth'", 
      "'force_display_urgency_hint'", "'focus_on_window_activation'", "'title_align'", 
      "", "'ipc_socket'", "'ipc_kill_timeout'", "'restart_state'", "'$('", 
      "'['", "')'", "';'", "'move'", "", "'exit'", "'debuglog'", "'border'", 
      "'layout'", "'append_layout'", "", "'focus'", "'kill'", "'reload'", 
      "'restart'", "'open'", "'fullscreen'", "'sticky'", "'split'", "", 
      "'resize'", "'rename'", "'nop'", "'scratchpad'", "'title_format'", 
      "'title_window_icon'", "'mode'", "", "", "", "", "']'", "", "'tiling'", 
      "", "'all'", "'class'", "'instance'", "'window_role'", "'con_id'", 
      "'id'", "'window_type'", "'title'", "'urgent'", "'machine'", "'floating_from'", 
      "'tiling_from'", "", "','"
    },
    std::vector<std::string>{
      "", "COMMENT", "STRING", "NUMBER", "DIMENSION_SEP", "OPTION", "VAR", 
      "WS", "EOL", "SET", "EQ", "EXEC", "BINDSYM", "FONT", "POPUP_DURING_FULLSCREEN", 
      "FLOATING_MINIMUM_SIZE", "FLOATING_MAXIMUM_SIZE", "FLOATING_MODIFIER", 
      "DEFAULT_ORIENTATION", "WORKSPACE_LAYOUT", "DEFAULT_BORDER", "HIDE_EDGE_BORDERS", 
      "FOR_WINDOW", "ASSIGN", "NO_FOCUS", "FOCUS_FOLLOWS_MOUSE", "MOUSE_WARPING", 
      "FOCUS_WRAPPING", "FORCE_FOCUS_WRAPPING", "WORKSPACE_AUTO_BACK_AND_FORTH", 
      "FORCE_DISPLAY_URGENCY_HINT", "FOCUS_ON_WINDOW_ACTIVATION", "TITLE_ALIGN", 
      "WORKSPACE", "IPC_SOCKET", "IPC_KILL_TIMEOUT", "RESTART_STATE", "OPEN_COMMAND", 
      "BEGIN_CRITERION", "CLOSE_COMMAND", "COMMAND_SEP", "COMMAND_MOVE", 
      "COMMAND_EXEC", "COMMAND_EXIT", "COMMAND_DEBUGLOG", "COMMAND_BORDER", 
      "COMMAND_LAYOUT", "COMMAND_APPEND_LAYOUT", "COMMAND_WORKSPACE", "COMMAND_FOCUS", 
      "COMMAND_KILL", "COMMAND_RELOAD", "COMMAND_RESTART", "COMMAND_OPEN", 
      "COMMAND_FULLSCREEN", "COMMAND_STICKY", "COMMAND_SPLIT", "COMMAND_FLOATING", 
      "COMMAND_RESIZE", "COMMAND_RENAME", "COMMAND_NOP", "COMMAND_SCRATCHPAD", 
      "COMMAND_TITLE_FORMAT", "COMMAND_TITLE_WINDOW_ICON", "COMMAND_MODE", 
      "COMMAND_WS", "COMMAND_OPTION", "COMMAND_NUMBER", "COMMAND_STRING", 
      "END_CRITERION", "CRITERION_STRING", "TILING", "FLOATING", "ALL", 
      "CLASS", "INSTANCE", "WINDOW_ROLE", "CON_ID", "ID", "WINDOW_TYPE", 
      "TITLE", "URGENT", "MACHINE", "FLOATING_FROM", "TILING_FROM", "CRITERION_EQ", 
      "CRITERION_SEP"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,86,1023,6,-1,6,-1,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,
  	5,7,5,2,6,7,6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,
  	13,7,13,2,14,7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,
  	20,7,20,2,21,7,21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,
  	27,7,27,2,28,7,28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,
  	34,7,34,2,35,7,35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,
  	41,7,41,2,42,7,42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,
  	48,7,48,2,49,7,49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,
  	55,7,55,2,56,7,56,2,57,7,57,2,58,7,58,2,59,7,59,2,60,7,60,2,61,7,61,2,
  	62,7,62,2,63,7,63,2,64,7,64,2,65,7,65,2,66,7,66,2,67,7,67,2,68,7,68,2,
  	69,7,69,2,70,7,70,2,71,7,71,2,72,7,72,2,73,7,73,2,74,7,74,2,75,7,75,2,
  	76,7,76,2,77,7,77,2,78,7,78,2,79,7,79,2,80,7,80,2,81,7,81,2,82,7,82,2,
  	83,7,83,2,84,7,84,2,85,7,85,1,0,1,0,5,0,178,8,0,10,0,12,0,181,9,0,1,0,
  	1,0,1,1,1,1,1,1,1,1,5,1,189,8,1,10,1,12,1,192,9,1,1,1,1,1,1,2,4,2,197,
  	8,2,11,2,12,2,198,1,3,1,3,1,4,1,4,1,4,1,4,1,4,5,4,208,8,4,10,4,12,4,211,
  	9,4,1,5,1,5,1,5,5,5,216,8,5,10,5,12,5,219,9,5,1,6,1,6,1,6,1,6,1,7,4,7,
  	226,8,7,11,7,12,7,227,1,8,1,8,1,8,1,8,1,9,1,9,1,10,1,10,1,10,1,10,1,10,
  	1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,11,1,12,1,12,1,12,1,12,1,12,1,13,
  	1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,
  	1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,14,
  	1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,1,14,
  	1,14,1,14,1,14,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,
  	1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,15,1,16,1,16,1,16,
  	1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,1,16,
  	1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,17,
  	1,17,1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,1,18,1,18,1,18,1,18,
  	1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,18,1,19,1,19,1,19,1,19,
  	1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,20,1,20,1,20,
  	1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,1,20,
  	1,20,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,21,1,22,1,22,
  	1,22,1,22,1,22,1,22,1,22,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,1,23,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,1,24,
  	1,24,1,24,1,24,1,24,1,24,1,24,1,25,1,25,1,25,1,25,1,25,1,25,1,25,1,25,
  	1,25,1,25,1,25,1,25,1,25,1,25,1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,26,
  	1,26,1,26,1,26,1,26,1,26,1,26,1,26,1,27,1,27,1,27,1,27,1,27,1,27,1,27,
  	1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,1,27,
  	1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,1,28,
  	1,28,1,28,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,
  	1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,1,29,
  	1,29,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,
  	1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,1,30,
  	1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,32,1,32,
  	1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,32,1,33,1,33,1,33,1,33,1,33,1,33,
  	1,33,1,33,1,33,1,33,1,33,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,
  	1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,34,1,35,1,35,1,35,1,35,1,35,1,35,
  	1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,35,1,36,1,36,1,36,1,36,1,36,1,37,
  	1,37,1,37,1,37,1,38,1,38,1,38,1,38,1,39,1,39,1,40,1,40,1,40,1,40,1,40,
  	1,41,1,41,1,41,1,41,1,41,1,42,1,42,1,42,1,42,1,42,1,43,1,43,1,43,1,43,
  	1,43,1,43,1,43,1,43,1,43,1,44,1,44,1,44,1,44,1,44,1,44,1,44,1,45,1,45,
  	1,45,1,45,1,45,1,45,1,45,1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,46,1,46,
  	1,46,1,46,1,46,1,46,1,46,1,47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,1,47,
  	1,47,1,48,1,48,1,48,1,48,1,48,1,48,1,49,1,49,1,49,1,49,1,49,1,50,1,50,
  	1,50,1,50,1,50,1,50,1,50,1,51,1,51,1,51,1,51,1,51,1,51,1,51,1,51,1,52,
  	1,52,1,52,1,52,1,52,1,53,1,53,1,53,1,53,1,53,1,53,1,53,1,53,1,53,1,53,
  	1,53,1,54,1,54,1,54,1,54,1,54,1,54,1,54,1,55,1,55,1,55,1,55,1,55,1,55,
  	1,56,1,56,1,56,1,56,1,56,1,56,1,56,1,56,1,56,1,57,1,57,1,57,1,57,1,57,
  	1,57,1,57,1,58,1,58,1,58,1,58,1,58,1,58,1,58,1,59,1,59,1,59,1,59,1,60,
  	1,60,1,60,1,60,1,60,1,60,1,60,1,60,1,60,1,60,1,60,1,61,1,61,1,61,1,61,
  	1,61,1,61,1,61,1,61,1,61,1,61,1,61,1,61,1,61,1,62,1,62,1,62,1,62,1,62,
  	1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,62,1,63,
  	1,63,1,63,1,63,1,63,1,64,1,64,1,64,1,64,1,65,1,65,1,65,1,65,1,65,5,65,
  	870,8,65,10,65,12,65,873,9,65,1,66,4,66,876,8,66,11,66,12,66,877,1,67,
  	1,67,1,67,1,67,5,67,884,8,67,10,67,12,67,887,9,67,1,67,1,67,1,68,1,68,
  	1,68,1,68,1,69,1,69,5,69,897,8,69,10,69,12,69,900,9,69,1,69,1,69,1,70,
  	1,70,1,70,1,70,1,70,1,70,1,70,1,71,1,71,1,71,1,71,1,71,1,71,1,71,1,71,
  	1,71,1,72,1,72,1,72,1,72,1,73,1,73,1,73,1,73,1,73,1,73,1,74,1,74,1,74,
  	1,74,1,74,1,74,1,74,1,74,1,74,1,75,1,75,1,75,1,75,1,75,1,75,1,75,1,75,
  	1,75,1,75,1,75,1,75,1,76,1,76,1,76,1,76,1,76,1,76,1,76,1,77,1,77,1,77,
  	1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,78,1,79,1,79,
  	1,79,1,79,1,79,1,79,1,80,1,80,1,80,1,80,1,80,1,80,1,80,1,81,1,81,1,81,
  	1,81,1,81,1,81,1,81,1,81,1,82,1,82,1,82,1,82,1,82,1,82,1,82,1,82,1,82,
  	1,82,1,82,1,82,1,82,1,82,1,83,1,83,1,83,1,83,1,83,1,83,1,83,1,83,1,83,
  	1,83,1,83,1,83,1,84,1,84,1,85,1,85,0,0,86,3,1,5,2,7,3,9,4,11,5,13,6,15,
  	7,17,8,19,9,21,10,23,11,25,12,27,13,29,14,31,15,33,16,35,17,37,18,39,
  	19,41,20,43,21,45,22,47,23,49,24,51,25,53,26,55,27,57,28,59,29,61,30,
  	63,31,65,32,67,33,69,34,71,35,73,36,75,37,77,38,79,39,81,40,83,41,85,
  	42,87,43,89,44,91,45,93,46,95,47,97,48,99,49,101,50,103,51,105,52,107,
  	53,109,54,111,55,113,56,115,57,117,58,119,59,121,60,123,61,125,62,127,
  	63,129,64,131,65,133,66,135,67,137,68,139,69,141,70,143,71,145,72,147,
  	73,149,74,151,75,153,76,155,77,157,78,159,79,161,80,163,81,165,82,167,
  	83,169,84,171,85,173,86,3,0,1,2,7,2,0,10,10,13,13,1,0,34,34,1,0,48,57,
  	2,0,48,57,97,122,3,0,45,45,48,57,97,122,2,0,65,90,97,122,5,0,45,45,48,
  	57,65,90,95,95,97,122,1032,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,0,0,0,9,1,
  	0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,19,1,0,0,
  	0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,0,0,0,0,
  	31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,0,0,41,1,
  	0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,0,47,1,0,0,0,0,49,1,0,0,0,0,51,1,0,0,
  	0,0,53,1,0,0,0,0,55,1,0,0,0,0,57,1,0,0,0,0,59,1,0,0,0,0,61,1,0,0,0,0,
  	63,1,0,0,0,0,65,1,0,0,0,0,67,1,0,0,0,0,69,1,0,0,0,0,71,1,0,0,0,0,73,1,
  	0,0,0,0,75,1,0,0,0,0,77,1,0,0,0,1,79,1,0,0,0,1,81,1,0,0,0,1,83,1,0,0,
  	0,1,85,1,0,0,0,1,87,1,0,0,0,1,89,1,0,0,0,1,91,1,0,0,0,1,93,1,0,0,0,1,
  	95,1,0,0,0,1,97,1,0,0,0,1,99,1,0,0,0,1,101,1,0,0,0,1,103,1,0,0,0,1,105,
  	1,0,0,0,1,107,1,0,0,0,1,109,1,0,0,0,1,111,1,0,0,0,1,113,1,0,0,0,1,115,
  	1,0,0,0,1,117,1,0,0,0,1,119,1,0,0,0,1,121,1,0,0,0,1,123,1,0,0,0,1,125,
  	1,0,0,0,1,127,1,0,0,0,1,129,1,0,0,0,1,131,1,0,0,0,1,133,1,0,0,0,1,135,
  	1,0,0,0,1,137,1,0,0,0,2,139,1,0,0,0,2,141,1,0,0,0,2,143,1,0,0,0,2,145,
  	1,0,0,0,2,147,1,0,0,0,2,149,1,0,0,0,2,151,1,0,0,0,2,153,1,0,0,0,2,155,
  	1,0,0,0,2,157,1,0,0,0,2,159,1,0,0,0,2,161,1,0,0,0,2,163,1,0,0,0,2,165,
  	1,0,0,0,2,167,1,0,0,0,2,169,1,0,0,0,2,171,1,0,0,0,2,173,1,0,0,0,3,175,
  	1,0,0,0,5,184,1,0,0,0,7,196,1,0,0,0,9,200,1,0,0,0,11,202,1,0,0,0,13,212,
  	1,0,0,0,15,220,1,0,0,0,17,225,1,0,0,0,19,229,1,0,0,0,21,233,1,0,0,0,23,
  	235,1,0,0,0,25,240,1,0,0,0,27,248,1,0,0,0,29,253,1,0,0,0,31,277,1,0,0,
  	0,33,299,1,0,0,0,35,321,1,0,0,0,37,339,1,0,0,0,39,359,1,0,0,0,41,376,
  	1,0,0,0,43,391,1,0,0,0,45,409,1,0,0,0,47,420,1,0,0,0,49,427,1,0,0,0,51,
  	436,1,0,0,0,53,456,1,0,0,0,55,470,1,0,0,0,57,485,1,0,0,0,59,506,1,0,0,
  	0,61,536,1,0,0,0,63,563,1,0,0,0,65,590,1,0,0,0,67,602,1,0,0,0,69,612,
  	1,0,0,0,71,623,1,0,0,0,73,640,1,0,0,0,75,654,1,0,0,0,77,659,1,0,0,0,79,
  	663,1,0,0,0,81,667,1,0,0,0,83,669,1,0,0,0,85,674,1,0,0,0,87,679,1,0,0,
  	0,89,684,1,0,0,0,91,693,1,0,0,0,93,700,1,0,0,0,95,707,1,0,0,0,97,721,
  	1,0,0,0,99,731,1,0,0,0,101,737,1,0,0,0,103,742,1,0,0,0,105,749,1,0,0,
  	0,107,757,1,0,0,0,109,762,1,0,0,0,111,773,1,0,0,0,113,780,1,0,0,0,115,
  	786,1,0,0,0,117,795,1,0,0,0,119,802,1,0,0,0,121,809,1,0,0,0,123,813,1,
  	0,0,0,125,824,1,0,0,0,127,837,1,0,0,0,129,855,1,0,0,0,131,860,1,0,0,0,
  	133,864,1,0,0,0,135,875,1,0,0,0,137,879,1,0,0,0,139,890,1,0,0,0,141,894,
  	1,0,0,0,143,903,1,0,0,0,145,910,1,0,0,0,147,919,1,0,0,0,149,923,1,0,0,
  	0,151,929,1,0,0,0,153,938,1,0,0,0,155,950,1,0,0,0,157,957,1,0,0,0,159,
  	960,1,0,0,0,161,972,1,0,0,0,163,978,1,0,0,0,165,985,1,0,0,0,167,993,1,
  	0,0,0,169,1007,1,0,0,0,171,1019,1,0,0,0,173,1021,1,0,0,0,175,179,5,35,
  	0,0,176,178,8,0,0,0,177,176,1,0,0,0,178,181,1,0,0,0,179,177,1,0,0,0,179,
  	180,1,0,0,0,180,182,1,0,0,0,181,179,1,0,0,0,182,183,6,0,0,0,183,4,1,0,
  	0,0,184,190,5,34,0,0,185,186,5,92,0,0,186,189,5,34,0,0,187,189,8,1,0,
  	0,188,185,1,0,0,0,188,187,1,0,0,0,189,192,1,0,0,0,190,188,1,0,0,0,190,
  	191,1,0,0,0,191,193,1,0,0,0,192,190,1,0,0,0,193,194,5,34,0,0,194,6,1,
  	0,0,0,195,197,7,2,0,0,196,195,1,0,0,0,197,198,1,0,0,0,198,196,1,0,0,0,
  	198,199,1,0,0,0,199,8,1,0,0,0,200,201,5,120,0,0,201,10,1,0,0,0,202,203,
  	5,45,0,0,203,204,5,45,0,0,204,205,1,0,0,0,205,209,7,3,0,0,206,208,7,4,
  	0,0,207,206,1,0,0,0,208,211,1,0,0,0,209,207,1,0,0,0,209,210,1,0,0,0,210,
  	12,1,0,0,0,211,209,1,0,0,0,212,213,5,36,0,0,213,217,7,5,0,0,214,216,7,
  	6,0,0,215,214,1,0,0,0,216,219,1,0,0,0,217,215,1,0,0,0,217,218,1,0,0,0,
  	218,14,1,0,0,0,219,217,1,0,0,0,220,221,5,32,0,0,221,222,1,0,0,0,222,223,
  	6,6,1,0,223,16,1,0,0,0,224,226,7,0,0,0,225,224,1,0,0,0,226,227,1,0,0,
  	0,227,225,1,0,0,0,227,228,1,0,0,0,228,18,1,0,0,0,229,230,5,115,0,0,230,
  	231,5,101,0,0,231,232,5,116,0,0,232,20,1,0,0,0,233,234,5,61,0,0,234,22,
  	1,0,0,0,235,236,5,101,0,0,236,237,5,120,0,0,237,238,5,101,0,0,238,239,
  	5,99,0,0,239,24,1,0,0,0,240,241,5,98,0,0,241,242,5,105,0,0,242,243,5,
  	110,0,0,243,244,5,100,0,0,244,245,5,115,0,0,245,246,5,121,0,0,246,247,
  	5,109,0,0,247,26,1,0,0,0,248,249,5,102,0,0,249,250,5,111,0,0,250,251,
  	5,110,0,0,251,252,5,116,0,0,252,28,1,0,0,0,253,254,5,112,0,0,254,255,
  	5,111,0,0,255,256,5,112,0,0,256,257,5,117,0,0,257,258,5,112,0,0,258,259,
  	5,95,0,0,259,260,5,100,0,0,260,261,5,117,0,0,261,262,5,114,0,0,262,263,
  	5,105,0,0,263,264,5,110,0,0,264,265,5,103,0,0,265,266,5,95,0,0,266,267,
  	5,102,0,0,267,268,5,117,0,0,268,269,5,108,0,0,269,270,5,108,0,0,270,271,
  	5,115,0,0,271,272,5,99,0,0,272,273,5,114,0,0,273,274,5,101,0,0,274,275,
  	5,101,0,0,275,276,5,110,0,0,276,30,1,0,0,0,277,278,5,102,0,0,278,279,
  	5,108,0,0,279,280,5,111,0,0,280,281,5,97,0,0,281,282,5,116,0,0,282,283,
  	5,105,0,0,283,284,5,110,0,0,284,285,5,103,0,0,285,286,5,95,0,0,286,287,
  	5,109,0,0,287,288,5,105,0,0,288,289,5,110,0,0,289,290,5,105,0,0,290,291,
  	5,109,0,0,291,292,5,117,0,0,292,293,5,109,0,0,293,294,5,95,0,0,294,295,
  	5,115,0,0,295,296,5,105,0,0,296,297,5,122,0,0,297,298,5,101,0,0,298,32,
  	1,0,0,0,299,300,5,102,0,0,300,301,5,108,0,0,301,302,5,111,0,0,302,303,
  	5,97,0,0,303,304,5,116,0,0,304,305,5,105,0,0,305,306,5,110,0,0,306,307,
  	5,103,0,0,307,308,5,95,0,0,308,309,5,109,0,0,309,310,5,97,0,0,310,311,
  	5,120,0,0,311,312,5,105,0,0,312,313,5,109,0,0,313,314,5,117,0,0,314,315,
  	5,109,0,0,315,316,5,95,0,0,316,317,5,115,0,0,317,318,5,105,0,0,318,319,
  	5,122,0,0,319,320,5,101,0,0,320,34,1,0,0,0,321,322,5,102,0,0,322,323,
  	5,108,0,0,323,324,5,111,0,0,324,325,5,97,0,0,325,326,5,116,0,0,326,327,
  	5,105,0,0,327,328,5,110,0,0,328,329,5,103,0,0,329,330,5,95,0,0,330,331,
  	5,109,0,0,331,332,5,111,0,0,332,333,5,100,0,0,333,334,5,105,0,0,334,335,
  	5,102,0,0,335,336,5,105,0,0,336,337,5,101,0,0,337,338,5,114,0,0,338,36,
  	1,0,0,0,339,340,5,100,0,0,340,341,5,101,0,0,341,342,5,102,0,0,342,343,
  	5,97,0,0,343,344,5,117,0,0,344,345,5,108,0,0,345,346,5,116,0,0,346,347,
  	5,95,0,0,347,348,5,111,0,0,348,349,5,114,0,0,349,350,5,105,0,0,350,351,
  	5,101,0,0,351,352,5,110,0,0,352,353,5,116,0,0,353,354,5,97,0,0,354,355,
  	5,116,0,0,355,356,5,105,0,0,356,357,5,111,0,0,357,358,5,110,0,0,358,38,
  	1,0,0,0,359,360,5,119,0,0,360,361,5,111,0,0,361,362,5,114,0,0,362,363,
  	5,107,0,0,363,364,5,115,0,0,364,365,5,112,0,0,365,366,5,97,0,0,366,367,
  	5,99,0,0,367,368,5,101,0,0,368,369,5,95,0,0,369,370,5,108,0,0,370,371,
  	5,97,0,0,371,372,5,121,0,0,372,373,5,111,0,0,373,374,5,117,0,0,374,375,
  	5,116,0,0,375,40,1,0,0,0,376,377,5,100,0,0,377,378,5,101,0,0,378,379,
  	5,102,0,0,379,380,5,97,0,0,380,381,5,117,0,0,381,382,5,108,0,0,382,383,
  	5,116,0,0,383,384,5,95,0,0,384,385,5,98,0,0,385,386,5,111,0,0,386,387,
  	5,114,0,0,387,388,5,100,0,0,388,389,5,101,0,0,389,390,5,114,0,0,390,42,
  	1,0,0,0,391,392,5,104,0,0,392,393,5,105,0,0,393,394,5,100,0,0,394,395,
  	5,101,0,0,395,396,5,95,0,0,396,397,5,101,0,0,397,398,5,100,0,0,398,399,
  	5,103,0,0,399,400,5,101,0,0,400,401,5,95,0,0,401,402,5,98,0,0,402,403,
  	5,111,0,0,403,404,5,114,0,0,404,405,5,100,0,0,405,406,5,101,0,0,406,407,
  	5,114,0,0,407,408,5,115,0,0,408,44,1,0,0,0,409,410,5,102,0,0,410,411,
  	5,111,0,0,411,412,5,114,0,0,412,413,5,95,0,0,413,414,5,119,0,0,414,415,
  	5,105,0,0,415,416,5,110,0,0,416,417,5,100,0,0,417,418,5,111,0,0,418,419,
  	5,119,0,0,419,46,1,0,0,0,420,421,5,97,0,0,421,422,5,115,0,0,422,423,5,
  	115,0,0,423,424,5,105,0,0,424,425,5,103,0,0,425,426,5,110,0,0,426,48,
  	1,0,0,0,427,428,5,110,0,0,428,429,5,111,0,0,429,430,5,95,0,0,430,431,
  	5,102,0,0,431,432,5,111,0,0,432,433,5,99,0,0,433,434,5,117,0,0,434,435,
  	5,115,0,0,435,50,1,0,0,0,436,437,5,102,0,0,437,438,5,111,0,0,438,439,
  	5,99,0,0,439,440,5,117,0,0,440,441,5,115,0,0,441,442,5,95,0,0,442,443,
  	5,102,0,0,443,444,5,111,0,0,444,445,5,108,0,0,445,446,5,108,0,0,446,447,
  	5,111,0,0,447,448,5,119,0,0,448,449,5,115,0,0,449,450,5,95,0,0,450,451,
  	5,109,0,0,451,452,5,111,0,0,452,453,5,117,0,0,453,454,5,115,0,0,454,455,
  	5,101,0,0,455,52,1,0,0,0,456,457,5,109,0,0,457,458,5,111,0,0,458,459,
  	5,117,0,0,459,460,5,115,0,0,460,461,5,101,0,0,461,462,5,95,0,0,462,463,
  	5,119,0,0,463,464,5,97,0,0,464,465,5,114,0,0,465,466,5,112,0,0,466,467,
  	5,105,0,0,467,468,5,110,0,0,468,469,5,103,0,0,469,54,1,0,0,0,470,471,
  	5,102,0,0,471,472,5,111,0,0,472,473,5,99,0,0,473,474,5,117,0,0,474,475,
  	5,115,0,0,475,476,5,95,0,0,476,477,5,119,0,0,477,478,5,114,0,0,478,479,
  	5,97,0,0,479,480,5,112,0,0,480,481,5,112,0,0,481,482,5,105,0,0,482,483,
  	5,110,0,0,483,484,5,103,0,0,484,56,1,0,0,0,485,486,5,102,0,0,486,487,
  	5,111,0,0,487,488,5,114,0,0,488,489,5,99,0,0,489,490,5,101,0,0,490,491,
  	5,95,0,0,491,492,5,102,0,0,492,493,5,111,0,0,493,494,5,99,0,0,494,495,
  	5,117,0,0,495,496,5,115,0,0,496,497,5,95,0,0,497,498,5,119,0,0,498,499,
  	5,114,0,0,499,500,5,97,0,0,500,501,5,112,0,0,501,502,5,112,0,0,502,503,
  	5,105,0,0,503,504,5,110,0,0,504,505,5,103,0,0,505,58,1,0,0,0,506,507,
  	5,119,0,0,507,508,5,111,0,0,508,509,5,114,0,0,509,510,5,107,0,0,510,511,
  	5,115,0,0,511,512,5,112,0,0,512,513,5,97,0,0,513,514,5,99,0,0,514,515,
  	5,101,0,0,515,516,5,95,0,0,516,517,5,97,0,0,517,518,5,117,0,0,518,519,
  	5,116,0,0,519,520,5,111,0,0,520,521,5,95,0,0,521,522,5,98,0,0,522,523,
  	5,97,0,0,523,524,5,99,0,0,524,525,5,107,0,0,525,526,5,95,0,0,526,527,
  	5,97,0,0,527,528,5,110,0,0,528,529,5,100,0,0,529,530,5,95,0,0,530,531,
  	5,102,0,0,531,532,5,111,0,0,532,533,5,114,0,0,533,534,5,116,0,0,534,535,
  	5,104,0,0,535,60,1,0,0,0,536,537,5,102,0,0,537,538,5,111,0,0,538,539,
  	5,114,0,0,539,540,5,99,0,0,540,541,5,101,0,0,541,542,5,95,0,0,542,543,
  	5,100,0,0,543,544,5,105,0,0,544,545,5,115,0,0,545,546,5,112,0,0,546,547,
  	5,108,0,0,547,548,5,97,0,0,548,549,5,121,0,0,549,550,5,95,0,0,550,551,
  	5,117,0,0,551,552,5,114,0,0,552,553,5,103,0,0,553,554,5,101,0,0,554,555,
  	5,110,0,0,555,556,5,99,0,0,556,557,5,121,0,0,557,558,5,95,0,0,558,559,
  	5,104,0,0,559,560,5,105,0,0,560,561,5,110,0,0,561,562,5,116,0,0,562,62,
  	1,0,0,0,563,564,5,102,0,0,564,565,5,111,0,0,565,566,5,99,0,0,566,567,
  	5,117,0,0,567,568,5,115,0,0,568,569,5,95,0,0,569,570,5,111,0,0,570,571,
  	5,110,0,0,571,572,5,95,0,0,572,573,5,119,0,0,573,574,5,105,0,0,574,575,
  	5,110,0,0,575,576,5,100,0,0,576,577,5,111,0,0,577,578,5,119,0,0,578,579,
  	5,95,0,0,579,580,5,97,0,0,580,581,5,99,0,0,581,582,5,116,0,0,582,583,
  	5,105,0,0,583,584,5,118,0,0,584,585,5,97,0,0,585,586,5,116,0,0,586,587,
  	5,105,0,0,587,588,5,111,0,0,588,589,5,110,0,0,589,64,1,0,0,0,590,591,
  	5,116,0,0,591,592,5,105,0,0,592,593,5,116,0,0,593,594,5,108,0,0,594,595,
  	5,101,0,0,595,596,5,95,0,0,596,597,5,97,0,0,597,598,5,108,0,0,598,599,
  	5,105,0,0,599,600,5,103,0,0,600,601,5,110,0,0,601,66,1,0,0,0,602,603,
  	5,119,0,0,603,604,5,111,0,0,604,605,5,114,0,0,605,606,5,107,0,0,606,607,
  	5,115,0,0,607,608,5,112,0,0,608,609,5,97,0,0,609,610,5,99,0,0,610,611,
  	5,101,0,0,611,68,1,0,0,0,612,613,5,105,0,0,613,614,5,112,0,0,614,615,
  	5,99,0,0,615,616,5,95,0,0,616,617,5,115,0,0,617,618,5,111,0,0,618,619,
  	5,99,0,0,619,620,5,107,0,0,620,621,5,101,0,0,621,622,5,116,0,0,622,70,
  	1,0,0,0,623,624,5,105,0,0,624,625,5,112,0,0,625,626,5,99,0,0,626,627,
  	5,95,0,0,627,628,5,107,0,0,628,629,5,105,0,0,629,630,5,108,0,0,630,631,
  	5,108,0,0,631,632,5,95,0,0,632,633,5,116,0,0,633,634,5,105,0,0,634,635,
  	5,109,0,0,635,636,5,101,0,0,636,637,5,111,0,0,637,638,5,117,0,0,638,639,
  	5,116,0,0,639,72,1,0,0,0,640,641,5,114,0,0,641,642,5,101,0,0,642,643,
  	5,115,0,0,643,644,5,116,0,0,644,645,5,97,0,0,645,646,5,114,0,0,646,647,
  	5,116,0,0,647,648,5,95,0,0,648,649,5,115,0,0,649,650,5,116,0,0,650,651,
  	5,97,0,0,651,652,5,116,0,0,652,653,5,101,0,0,653,74,1,0,0,0,654,655,5,
  	36,0,0,655,656,5,40,0,0,656,657,1,0,0,0,657,658,6,36,2,0,658,76,1,0,0,
  	0,659,660,5,91,0,0,660,661,1,0,0,0,661,662,6,37,3,0,662,78,1,0,0,0,663,
  	664,5,41,0,0,664,665,1,0,0,0,665,666,6,38,4,0,666,80,1,0,0,0,667,668,
  	5,59,0,0,668,82,1,0,0,0,669,670,5,109,0,0,670,671,5,111,0,0,671,672,5,
  	118,0,0,672,673,5,101,0,0,673,84,1,0,0,0,674,675,5,101,0,0,675,676,5,
  	120,0,0,676,677,5,101,0,0,677,678,5,99,0,0,678,86,1,0,0,0,679,680,5,101,
  	0,0,680,681,5,120,0,0,681,682,5,105,0,0,682,683,5,116,0,0,683,88,1,0,
  	0,0,684,685,5,100,0,0,685,686,5,101,0,0,686,687,5,98,0,0,687,688,5,117,
  	0,0,688,689,5,103,0,0,689,690,5,108,0,0,690,691,5,111,0,0,691,692,5,103,
  	0,0,692,90,1,0,0,0,693,694,5,98,0,0,694,695,5,111,0,0,695,696,5,114,0,
  	0,696,697,5,100,0,0,697,698,5,101,0,0,698,699,5,114,0,0,699,92,1,0,0,
  	0,700,701,5,108,0,0,701,702,5,97,0,0,702,703,5,121,0,0,703,704,5,111,
  	0,0,704,705,5,117,0,0,705,706,5,116,0,0,706,94,1,0,0,0,707,708,5,97,0,
  	0,708,709,5,112,0,0,709,710,5,112,0,0,710,711,5,101,0,0,711,712,5,110,
  	0,0,712,713,5,100,0,0,713,714,5,95,0,0,714,715,5,108,0,0,715,716,5,97,
  	0,0,716,717,5,121,0,0,717,718,5,111,0,0,718,719,5,117,0,0,719,720,5,116,
  	0,0,720,96,1,0,0,0,721,722,5,119,0,0,722,723,5,111,0,0,723,724,5,114,
  	0,0,724,725,5,107,0,0,725,726,5,115,0,0,726,727,5,112,0,0,727,728,5,97,
  	0,0,728,729,5,99,0,0,729,730,5,101,0,0,730,98,1,0,0,0,731,732,5,102,0,
  	0,732,733,5,111,0,0,733,734,5,99,0,0,734,735,5,117,0,0,735,736,5,115,
  	0,0,736,100,1,0,0,0,737,738,5,107,0,0,738,739,5,105,0,0,739,740,5,108,
  	0,0,740,741,5,108,0,0,741,102,1,0,0,0,742,743,5,114,0,0,743,744,5,101,
  	0,0,744,745,5,108,0,0,745,746,5,111,0,0,746,747,5,97,0,0,747,748,5,100,
  	0,0,748,104,1,0,0,0,749,750,5,114,0,0,750,751,5,101,0,0,751,752,5,115,
  	0,0,752,753,5,116,0,0,753,754,5,97,0,0,754,755,5,114,0,0,755,756,5,116,
  	0,0,756,106,1,0,0,0,757,758,5,111,0,0,758,759,5,112,0,0,759,760,5,101,
  	0,0,760,761,5,110,0,0,761,108,1,0,0,0,762,763,5,102,0,0,763,764,5,117,
  	0,0,764,765,5,108,0,0,765,766,5,108,0,0,766,767,5,115,0,0,767,768,5,99,
  	0,0,768,769,5,114,0,0,769,770,5,101,0,0,770,771,5,101,0,0,771,772,5,110,
  	0,0,772,110,1,0,0,0,773,774,5,115,0,0,774,775,5,116,0,0,775,776,5,105,
  	0,0,776,777,5,99,0,0,777,778,5,107,0,0,778,779,5,121,0,0,779,112,1,0,
  	0,0,780,781,5,115,0,0,781,782,5,112,0,0,782,783,5,108,0,0,783,784,5,105,
  	0,0,784,785,5,116,0,0,785,114,1,0,0,0,786,787,5,102,0,0,787,788,5,108,
  	0,0,788,789,5,111,0,0,789,790,5,97,0,0,790,791,5,116,0,0,791,792,5,105,
  	0,0,792,793,5,110,0,0,793,794,5,103,0,0,794,116,1,0,0,0,795,796,5,114,
  	0,0,796,797,5,101,0,0,797,798,5,115,0,0,798,799,5,105,0,0,799,800,5,122,
  	0,0,800,801,5,101,0,0,801,118,1,0,0,0,802,803,5,114,0,0,803,804,5,101,
  	0,0,804,805,5,110,0,0,805,806,5,97,0,0,806,807,5,109,0,0,807,808,5,101,
  	0,0,808,120,1,0,0,0,809,810,5,110,0,0,810,811,5,111,0,0,811,812,5,112,
  	0,0,812,122,1,0,0,0,813,814,5,115,0,0,814,815,5,99,0,0,815,816,5,114,
  	0,0,816,817,5,97,0,0,817,818,5,116,0,0,818,819,5,99,0,0,819,820,5,104,
  	0,0,820,821,5,112,0,0,821,822,5,97,0,0,822,823,5,100,0,0,823,124,1,0,
  	0,0,824,825,5,116,0,0,825,826,5,105,0,0,826,827,5,116,0,0,827,828,5,108,
  	0,0,828,829,5,101,0,0,829,830,5,95,0,0,830,831,5,102,0,0,831,832,5,111,
  	0,0,832,833,5,114,0,0,833,834,5,109,0,0,834,835,5,97,0,0,835,836,5,116,
  	0,0,836,126,1,0,0,0,837,838,5,116,0,0,838,839,5,105,0,0,839,840,5,116,
  	0,0,840,841,5,108,0,0,841,842,5,101,0,0,842,843,5,95,0,0,843,844,5,119,
  	0,0,844,845,5,105,0,0,845,846,5,110,0,0,846,847,5,100,0,0,847,848,5,111,
  	0,0,848,849,5,119,0,0,849,850,5,95,0,0,850,851,5,105,0,0,851,852,5,99,
  	0,0,852,853,5,111,0,0,853,854,5,110,0,0,854,128,1,0,0,0,855,856,5,109,
  	0,0,856,857,5,111,0,0,857,858,5,100,0,0,858,859,5,101,0,0,859,130,1,0,
  	0,0,860,861,5,32,0,0,861,862,1,0,0,0,862,863,6,64,1,0,863,132,1,0,0,0,
  	864,865,5,45,0,0,865,866,5,45,0,0,866,867,1,0,0,0,867,871,7,3,0,0,868,
  	870,7,4,0,0,869,868,1,0,0,0,870,873,1,0,0,0,871,869,1,0,0,0,871,872,1,
  	0,0,0,872,134,1,0,0,0,873,871,1,0,0,0,874,876,7,2,0,0,875,874,1,0,0,0,
  	876,877,1,0,0,0,877,875,1,0,0,0,877,878,1,0,0,0,878,136,1,0,0,0,879,885,
  	5,34,0,0,880,881,5,92,0,0,881,884,5,34,0,0,882,884,8,1,0,0,883,880,1,
  	0,0,0,883,882,1,0,0,0,884,887,1,0,0,0,885,883,1,0,0,0,885,886,1,0,0,0,
  	886,888,1,0,0,0,887,885,1,0,0,0,888,889,5,34,0,0,889,138,1,0,0,0,890,
  	891,5,93,0,0,891,892,1,0,0,0,892,893,6,68,4,0,893,140,1,0,0,0,894,898,
  	5,34,0,0,895,897,8,1,0,0,896,895,1,0,0,0,897,900,1,0,0,0,898,896,1,0,
  	0,0,898,899,1,0,0,0,899,901,1,0,0,0,900,898,1,0,0,0,901,902,5,34,0,0,
  	902,142,1,0,0,0,903,904,5,116,0,0,904,905,5,105,0,0,905,906,5,108,0,0,
  	906,907,5,105,0,0,907,908,5,110,0,0,908,909,5,103,0,0,909,144,1,0,0,0,
  	910,911,5,102,0,0,911,912,5,108,0,0,912,913,5,111,0,0,913,914,5,97,0,
  	0,914,915,5,116,0,0,915,916,5,105,0,0,916,917,5,110,0,0,917,918,5,103,
  	0,0,918,146,1,0,0,0,919,920,5,97,0,0,920,921,5,108,0,0,921,922,5,108,
  	0,0,922,148,1,0,0,0,923,924,5,99,0,0,924,925,5,108,0,0,925,926,5,97,0,
  	0,926,927,5,115,0,0,927,928,5,115,0,0,928,150,1,0,0,0,929,930,5,105,0,
  	0,930,931,5,110,0,0,931,932,5,115,0,0,932,933,5,116,0,0,933,934,5,97,
  	0,0,934,935,5,110,0,0,935,936,5,99,0,0,936,937,5,101,0,0,937,152,1,0,
  	0,0,938,939,5,119,0,0,939,940,5,105,0,0,940,941,5,110,0,0,941,942,5,100,
  	0,0,942,943,5,111,0,0,943,944,5,119,0,0,944,945,5,95,0,0,945,946,5,114,
  	0,0,946,947,5,111,0,0,947,948,5,108,0,0,948,949,5,101,0,0,949,154,1,0,
  	0,0,950,951,5,99,0,0,951,952,5,111,0,0,952,953,5,110,0,0,953,954,5,95,
  	0,0,954,955,5,105,0,0,955,956,5,100,0,0,956,156,1,0,0,0,957,958,5,105,
  	0,0,958,959,5,100,0,0,959,158,1,0,0,0,960,961,5,119,0,0,961,962,5,105,
  	0,0,962,963,5,110,0,0,963,964,5,100,0,0,964,965,5,111,0,0,965,966,5,119,
  	0,0,966,967,5,95,0,0,967,968,5,116,0,0,968,969,5,121,0,0,969,970,5,112,
  	0,0,970,971,5,101,0,0,971,160,1,0,0,0,972,973,5,116,0,0,973,974,5,105,
  	0,0,974,975,5,116,0,0,975,976,5,108,0,0,976,977,5,101,0,0,977,162,1,0,
  	0,0,978,979,5,117,0,0,979,980,5,114,0,0,980,981,5,103,0,0,981,982,5,101,
  	0,0,982,983,5,110,0,0,983,984,5,116,0,0,984,164,1,0,0,0,985,986,5,109,
  	0,0,986,987,5,97,0,0,987,988,5,99,0,0,988,989,5,104,0,0,989,990,5,105,
  	0,0,990,991,5,110,0,0,991,992,5,101,0,0,992,166,1,0,0,0,993,994,5,102,
  	0,0,994,995,5,108,0,0,995,996,5,111,0,0,996,997,5,97,0,0,997,998,5,116,
  	0,0,998,999,5,105,0,0,999,1000,5,110,0,0,1000,1001,5,103,0,0,1001,1002,
  	5,95,0,0,1002,1003,5,102,0,0,1003,1004,5,114,0,0,1004,1005,5,111,0,0,
  	1005,1006,5,109,0,0,1006,168,1,0,0,0,1007,1008,5,116,0,0,1008,1009,5,
  	105,0,0,1009,1010,5,108,0,0,1010,1011,5,105,0,0,1011,1012,5,110,0,0,1012,
  	1013,5,103,0,0,1013,1014,5,95,0,0,1014,1015,5,102,0,0,1015,1016,5,114,
  	0,0,1016,1017,5,111,0,0,1017,1018,5,109,0,0,1018,170,1,0,0,0,1019,1020,
  	5,61,0,0,1020,172,1,0,0,0,1021,1022,5,44,0,0,1022,174,1,0,0,0,15,0,1,
  	2,179,188,190,198,209,217,227,871,877,883,885,898,5,0,1,0,6,0,0,5,1,0,
  	5,2,0,4,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  configlexerLexerStaticData = staticData.release();
}

}

configLexer::configLexer(CharStream *input) : Lexer(input) {
  configLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *configlexerLexerStaticData->atn, configlexerLexerStaticData->decisionToDFA, configlexerLexerStaticData->sharedContextCache);
}

configLexer::~configLexer() {
  delete _interpreter;
}

std::string configLexer::getGrammarFileName() const {
  return "configLexer.g4";
}

const std::vector<std::string>& configLexer::getRuleNames() const {
  return configlexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& configLexer::getChannelNames() const {
  return configlexerLexerStaticData->channelNames;
}

const std::vector<std::string>& configLexer::getModeNames() const {
  return configlexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& configLexer::getVocabulary() const {
  return configlexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView configLexer::getSerializedATN() const {
  return configlexerLexerStaticData->serializedATN;
}

const atn::ATN& configLexer::getATN() const {
  return *configlexerLexerStaticData->atn;
}




void configLexer::initialize() {
  ::antlr4::internal::call_once(configlexerLexerOnceFlag, configlexerLexerInitialize);
}
