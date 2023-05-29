
// Generated from configGrammar.g4 by ANTLR 4.12.0


#include "configGrammarListener.h"

#include "configGrammar.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct ConfigGrammarStaticData final {
  ConfigGrammarStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ConfigGrammarStaticData(const ConfigGrammarStaticData&) = delete;
  ConfigGrammarStaticData(ConfigGrammarStaticData&&) = delete;
  ConfigGrammarStaticData& operator=(const ConfigGrammarStaticData&) = delete;
  ConfigGrammarStaticData& operator=(ConfigGrammarStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag configgrammarParserOnceFlag;
ConfigGrammarStaticData *configgrammarParserStaticData = nullptr;

void configgrammarParserInitialize() {
  assert(configgrammarParserStaticData == nullptr);
  auto staticData = std::make_unique<ConfigGrammarStaticData>(
    std::vector<std::string>{
      "config", "stmt", "commands", "set", "binding", "font", "floating_minimum_size", 
      "floating_maximum_size", "floating_modifier", "default_orientation", 
      "workspace_layout", "default_border", "hide_edge_borders", "for_window", 
      "assign", "no_focus", "focus_follows_mouse", "mouse_warping", "focus_wrapping", 
      "force_focus_wrapping", "workspace_auto_back_and_forth", "force_display_urgency_hint", 
      "focus_on_window_activation", "title_align", "workspace", "ipc_socket", 
      "ipc_kill_timeout", "restart_state", "popup_during_fullscreen", "exec", 
      "dimension", "command", "command_move", "command_exec", "command_exit", 
      "command_kill", "command_fullscreen", "command_sticky", "command_debuglog", 
      "command_border", "command_layout", "command_append_layout", "command_workspace", 
      "command_focus", "command_split", "command_floating", "command_resize", 
      "command_rename", "command_nop", "command_reload", "command_restart", 
      "command_mode", "command_open", "command_scratchpad", "command_title_format", 
      "command_title_window_icon", "criteria", "criterion", "value_criterion"
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
  	4,1,86,431,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,1,0,5,0,120,8,0,10,0,12,0,123,9,0,1,0,1,0,4,0,
  	127,8,0,11,0,12,0,128,5,0,131,8,0,10,0,12,0,134,9,0,1,0,1,0,5,0,138,8,
  	0,10,0,12,0,141,9,0,3,0,143,8,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,3,1,174,8,1,1,2,1,2,1,2,1,2,5,2,180,8,2,10,2,12,2,183,9,2,1,2,
  	1,2,3,2,187,8,2,1,2,1,2,1,3,1,3,1,3,1,3,1,3,1,4,1,4,5,4,198,8,4,10,4,
  	12,4,201,9,4,1,4,1,4,1,4,1,5,1,5,1,5,1,6,1,6,1,6,1,7,1,7,1,7,1,8,1,8,
  	1,8,1,9,1,9,1,9,1,10,1,10,1,10,1,11,1,11,1,11,1,12,1,12,1,12,1,13,1,13,
  	1,13,1,13,1,14,1,14,1,14,1,14,1,15,1,15,1,15,1,16,1,16,1,16,1,17,1,17,
  	1,17,1,18,1,18,1,18,1,19,1,19,1,19,1,20,1,20,1,20,1,21,1,21,1,21,1,22,
  	1,22,1,22,1,23,1,23,1,23,1,24,1,24,1,24,1,24,3,24,269,8,24,1,25,1,25,
  	1,25,1,26,1,26,1,26,1,27,1,27,1,27,1,28,1,28,1,28,1,29,1,29,5,29,285,
  	8,29,10,29,12,29,288,9,29,1,29,1,29,1,30,1,30,1,30,1,30,1,31,1,31,1,31,
  	1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,1,31,
  	1,31,1,31,1,31,1,31,1,31,1,31,1,31,3,31,320,8,31,1,32,1,32,1,32,1,33,
  	1,33,5,33,327,8,33,10,33,12,33,330,9,33,1,33,1,33,1,34,1,34,1,35,1,35,
  	1,35,1,36,1,36,1,36,3,36,342,8,36,1,37,1,37,1,37,1,38,1,38,1,38,1,39,
  	1,39,1,39,3,39,353,8,39,1,40,1,40,1,40,3,40,358,8,40,1,41,1,41,1,41,1,
  	42,1,42,5,42,365,8,42,10,42,12,42,368,9,42,1,42,1,42,1,43,1,43,1,43,3,
  	43,375,8,43,1,44,1,44,1,44,1,45,1,45,1,45,1,46,1,46,1,47,1,47,1,48,1,
  	48,3,48,389,8,48,1,49,1,49,1,50,1,50,1,51,1,51,1,51,1,52,1,52,1,53,1,
  	53,1,53,1,54,1,54,1,54,1,55,1,55,1,56,1,56,1,56,1,56,5,56,412,8,56,10,
  	56,12,56,415,9,56,1,56,1,56,1,56,1,56,1,57,1,57,1,57,1,57,3,57,425,8,
  	57,1,58,1,58,1,58,1,58,1,58,0,0,59,0,2,4,6,8,10,12,14,16,18,20,22,24,
  	26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,
  	72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,
  	114,116,0,2,1,0,67,68,2,0,33,33,74,84,441,0,121,1,0,0,0,2,173,1,0,0,0,
  	4,175,1,0,0,0,6,190,1,0,0,0,8,195,1,0,0,0,10,205,1,0,0,0,12,208,1,0,0,
  	0,14,211,1,0,0,0,16,214,1,0,0,0,18,217,1,0,0,0,20,220,1,0,0,0,22,223,
  	1,0,0,0,24,226,1,0,0,0,26,229,1,0,0,0,28,233,1,0,0,0,30,237,1,0,0,0,32,
  	240,1,0,0,0,34,243,1,0,0,0,36,246,1,0,0,0,38,249,1,0,0,0,40,252,1,0,0,
  	0,42,255,1,0,0,0,44,258,1,0,0,0,46,261,1,0,0,0,48,264,1,0,0,0,50,270,
  	1,0,0,0,52,273,1,0,0,0,54,276,1,0,0,0,56,279,1,0,0,0,58,282,1,0,0,0,60,
  	291,1,0,0,0,62,319,1,0,0,0,64,321,1,0,0,0,66,324,1,0,0,0,68,333,1,0,0,
  	0,70,335,1,0,0,0,72,338,1,0,0,0,74,343,1,0,0,0,76,346,1,0,0,0,78,349,
  	1,0,0,0,80,354,1,0,0,0,82,359,1,0,0,0,84,362,1,0,0,0,86,371,1,0,0,0,88,
  	376,1,0,0,0,90,379,1,0,0,0,92,382,1,0,0,0,94,384,1,0,0,0,96,386,1,0,0,
  	0,98,390,1,0,0,0,100,392,1,0,0,0,102,394,1,0,0,0,104,397,1,0,0,0,106,
  	399,1,0,0,0,108,402,1,0,0,0,110,405,1,0,0,0,112,407,1,0,0,0,114,424,1,
  	0,0,0,116,426,1,0,0,0,118,120,5,8,0,0,119,118,1,0,0,0,120,123,1,0,0,0,
  	121,119,1,0,0,0,121,122,1,0,0,0,122,142,1,0,0,0,123,121,1,0,0,0,124,126,
  	3,2,1,0,125,127,5,8,0,0,126,125,1,0,0,0,127,128,1,0,0,0,128,126,1,0,0,
  	0,128,129,1,0,0,0,129,131,1,0,0,0,130,124,1,0,0,0,131,134,1,0,0,0,132,
  	130,1,0,0,0,132,133,1,0,0,0,133,135,1,0,0,0,134,132,1,0,0,0,135,139,3,
  	2,1,0,136,138,5,8,0,0,137,136,1,0,0,0,138,141,1,0,0,0,139,137,1,0,0,0,
  	139,140,1,0,0,0,140,143,1,0,0,0,141,139,1,0,0,0,142,132,1,0,0,0,142,143,
  	1,0,0,0,143,144,1,0,0,0,144,145,5,0,0,1,145,1,1,0,0,0,146,174,3,6,3,0,
  	147,174,3,8,4,0,148,174,3,10,5,0,149,174,3,12,6,0,150,174,3,14,7,0,151,
  	174,3,16,8,0,152,174,3,18,9,0,153,174,3,20,10,0,154,174,3,22,11,0,155,
  	174,3,24,12,0,156,174,3,26,13,0,157,174,3,28,14,0,158,174,3,30,15,0,159,
  	174,3,32,16,0,160,174,3,34,17,0,161,174,3,36,18,0,162,174,3,38,19,0,163,
  	174,3,40,20,0,164,174,3,42,21,0,165,174,3,44,22,0,166,174,3,46,23,0,167,
  	174,3,48,24,0,168,174,3,50,25,0,169,174,3,52,26,0,170,174,3,54,27,0,171,
  	174,3,56,28,0,172,174,3,58,29,0,173,146,1,0,0,0,173,147,1,0,0,0,173,148,
  	1,0,0,0,173,149,1,0,0,0,173,150,1,0,0,0,173,151,1,0,0,0,173,152,1,0,0,
  	0,173,153,1,0,0,0,173,154,1,0,0,0,173,155,1,0,0,0,173,156,1,0,0,0,173,
  	157,1,0,0,0,173,158,1,0,0,0,173,159,1,0,0,0,173,160,1,0,0,0,173,161,1,
  	0,0,0,173,162,1,0,0,0,173,163,1,0,0,0,173,164,1,0,0,0,173,165,1,0,0,0,
  	173,166,1,0,0,0,173,167,1,0,0,0,173,168,1,0,0,0,173,169,1,0,0,0,173,170,
  	1,0,0,0,173,171,1,0,0,0,173,172,1,0,0,0,174,3,1,0,0,0,175,181,5,37,0,
  	0,176,177,3,62,31,0,177,178,5,40,0,0,178,180,1,0,0,0,179,176,1,0,0,0,
  	180,183,1,0,0,0,181,179,1,0,0,0,181,182,1,0,0,0,182,184,1,0,0,0,183,181,
  	1,0,0,0,184,186,3,62,31,0,185,187,5,40,0,0,186,185,1,0,0,0,186,187,1,
  	0,0,0,187,188,1,0,0,0,188,189,5,39,0,0,189,5,1,0,0,0,190,191,5,9,0,0,
  	191,192,5,6,0,0,192,193,5,10,0,0,193,194,5,2,0,0,194,7,1,0,0,0,195,199,
  	5,12,0,0,196,198,5,5,0,0,197,196,1,0,0,0,198,201,1,0,0,0,199,197,1,0,
  	0,0,199,200,1,0,0,0,200,202,1,0,0,0,201,199,1,0,0,0,202,203,5,2,0,0,203,
  	204,3,4,2,0,204,9,1,0,0,0,205,206,5,13,0,0,206,207,5,2,0,0,207,11,1,0,
  	0,0,208,209,5,15,0,0,209,210,3,60,30,0,210,13,1,0,0,0,211,212,5,16,0,
  	0,212,213,3,60,30,0,213,15,1,0,0,0,214,215,5,17,0,0,215,216,5,2,0,0,216,
  	17,1,0,0,0,217,218,5,18,0,0,218,219,5,2,0,0,219,19,1,0,0,0,220,221,5,
  	19,0,0,221,222,5,2,0,0,222,21,1,0,0,0,223,224,5,20,0,0,224,225,5,2,0,
  	0,225,23,1,0,0,0,226,227,5,21,0,0,227,228,5,2,0,0,228,25,1,0,0,0,229,
  	230,5,22,0,0,230,231,3,112,56,0,231,232,3,4,2,0,232,27,1,0,0,0,233,234,
  	5,23,0,0,234,235,3,112,56,0,235,236,5,2,0,0,236,29,1,0,0,0,237,238,5,
  	24,0,0,238,239,3,112,56,0,239,31,1,0,0,0,240,241,5,25,0,0,241,242,5,2,
  	0,0,242,33,1,0,0,0,243,244,5,26,0,0,244,245,5,2,0,0,245,35,1,0,0,0,246,
  	247,5,27,0,0,247,248,5,2,0,0,248,37,1,0,0,0,249,250,5,28,0,0,250,251,
  	5,2,0,0,251,39,1,0,0,0,252,253,5,29,0,0,253,254,5,2,0,0,254,41,1,0,0,
  	0,255,256,5,30,0,0,256,257,5,3,0,0,257,43,1,0,0,0,258,259,5,31,0,0,259,
  	260,5,2,0,0,260,45,1,0,0,0,261,262,5,32,0,0,262,263,5,2,0,0,263,47,1,
  	0,0,0,264,265,5,33,0,0,265,268,5,2,0,0,266,267,5,10,0,0,267,269,5,2,0,
  	0,268,266,1,0,0,0,268,269,1,0,0,0,269,49,1,0,0,0,270,271,5,34,0,0,271,
  	272,5,2,0,0,272,51,1,0,0,0,273,274,5,35,0,0,274,275,5,3,0,0,275,53,1,
  	0,0,0,276,277,5,36,0,0,277,278,5,2,0,0,278,55,1,0,0,0,279,280,5,14,0,
  	0,280,281,5,2,0,0,281,57,1,0,0,0,282,286,5,11,0,0,283,285,5,5,0,0,284,
  	283,1,0,0,0,285,288,1,0,0,0,286,284,1,0,0,0,286,287,1,0,0,0,287,289,1,
  	0,0,0,288,286,1,0,0,0,289,290,5,2,0,0,290,59,1,0,0,0,291,292,5,3,0,0,
  	292,293,5,4,0,0,293,294,5,3,0,0,294,61,1,0,0,0,295,320,3,64,32,0,296,
  	320,3,66,33,0,297,320,3,68,34,0,298,320,3,76,38,0,299,320,3,78,39,0,300,
  	320,3,80,40,0,301,320,3,82,41,0,302,320,3,84,42,0,303,320,3,86,43,0,304,
  	320,3,70,35,0,305,320,3,102,51,0,306,320,3,104,52,0,307,320,3,72,36,0,
  	308,320,3,74,37,0,309,320,3,88,44,0,310,320,3,90,45,0,311,320,3,92,46,
  	0,312,320,3,94,47,0,313,320,3,96,48,0,314,320,3,98,49,0,315,320,3,100,
  	50,0,316,320,3,106,53,0,317,320,3,108,54,0,318,320,3,110,55,0,319,295,
  	1,0,0,0,319,296,1,0,0,0,319,297,1,0,0,0,319,298,1,0,0,0,319,299,1,0,0,
  	0,319,300,1,0,0,0,319,301,1,0,0,0,319,302,1,0,0,0,319,303,1,0,0,0,319,
  	304,1,0,0,0,319,305,1,0,0,0,319,306,1,0,0,0,319,307,1,0,0,0,319,308,1,
  	0,0,0,319,309,1,0,0,0,319,310,1,0,0,0,319,311,1,0,0,0,319,312,1,0,0,0,
  	319,313,1,0,0,0,319,314,1,0,0,0,319,315,1,0,0,0,319,316,1,0,0,0,319,317,
  	1,0,0,0,319,318,1,0,0,0,320,63,1,0,0,0,321,322,5,41,0,0,322,323,7,0,0,
  	0,323,65,1,0,0,0,324,328,5,42,0,0,325,327,5,66,0,0,326,325,1,0,0,0,327,
  	330,1,0,0,0,328,326,1,0,0,0,328,329,1,0,0,0,329,331,1,0,0,0,330,328,1,
  	0,0,0,331,332,5,68,0,0,332,67,1,0,0,0,333,334,5,43,0,0,334,69,1,0,0,0,
  	335,336,5,50,0,0,336,337,5,68,0,0,337,71,1,0,0,0,338,339,5,54,0,0,339,
  	341,5,68,0,0,340,342,5,68,0,0,341,340,1,0,0,0,341,342,1,0,0,0,342,73,
  	1,0,0,0,343,344,5,55,0,0,344,345,5,68,0,0,345,75,1,0,0,0,346,347,5,44,
  	0,0,347,348,5,68,0,0,348,77,1,0,0,0,349,350,5,45,0,0,350,352,5,68,0,0,
  	351,353,5,67,0,0,352,351,1,0,0,0,352,353,1,0,0,0,353,79,1,0,0,0,354,355,
  	5,46,0,0,355,357,5,68,0,0,356,358,5,68,0,0,357,356,1,0,0,0,357,358,1,
  	0,0,0,358,81,1,0,0,0,359,360,5,47,0,0,360,361,5,68,0,0,361,83,1,0,0,0,
  	362,366,5,48,0,0,363,365,5,66,0,0,364,363,1,0,0,0,365,368,1,0,0,0,366,
  	364,1,0,0,0,366,367,1,0,0,0,367,369,1,0,0,0,368,366,1,0,0,0,369,370,5,
  	68,0,0,370,85,1,0,0,0,371,372,5,49,0,0,372,374,5,68,0,0,373,375,5,68,
  	0,0,374,373,1,0,0,0,374,375,1,0,0,0,375,87,1,0,0,0,376,377,5,56,0,0,377,
  	378,5,68,0,0,378,89,1,0,0,0,379,380,5,57,0,0,380,381,5,68,0,0,381,91,
  	1,0,0,0,382,383,5,58,0,0,383,93,1,0,0,0,384,385,5,59,0,0,385,95,1,0,0,
  	0,386,388,5,60,0,0,387,389,5,68,0,0,388,387,1,0,0,0,388,389,1,0,0,0,389,
  	97,1,0,0,0,390,391,5,51,0,0,391,99,1,0,0,0,392,393,5,52,0,0,393,101,1,
  	0,0,0,394,395,5,64,0,0,395,396,5,68,0,0,396,103,1,0,0,0,397,398,5,53,
  	0,0,398,105,1,0,0,0,399,400,5,61,0,0,400,401,5,68,0,0,401,107,1,0,0,0,
  	402,403,5,62,0,0,403,404,5,68,0,0,404,109,1,0,0,0,405,406,5,63,0,0,406,
  	111,1,0,0,0,407,413,5,38,0,0,408,409,3,114,57,0,409,410,5,86,0,0,410,
  	412,1,0,0,0,411,408,1,0,0,0,412,415,1,0,0,0,413,411,1,0,0,0,413,414,1,
  	0,0,0,414,416,1,0,0,0,415,413,1,0,0,0,416,417,3,114,57,0,417,418,1,0,
  	0,0,418,419,5,69,0,0,419,113,1,0,0,0,420,425,3,116,58,0,421,425,5,71,
  	0,0,422,425,5,72,0,0,423,425,5,73,0,0,424,420,1,0,0,0,424,421,1,0,0,0,
  	424,422,1,0,0,0,424,423,1,0,0,0,425,115,1,0,0,0,426,427,7,1,0,0,427,428,
  	5,85,0,0,428,429,5,70,0,0,429,117,1,0,0,0,21,121,128,132,139,142,173,
  	181,186,199,268,286,319,328,341,352,357,366,374,388,413,424
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  configgrammarParserStaticData = staticData.release();
}

}

configGrammar::configGrammar(TokenStream *input) : configGrammar(input, antlr4::atn::ParserATNSimulatorOptions()) {}

configGrammar::configGrammar(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  configGrammar::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *configgrammarParserStaticData->atn, configgrammarParserStaticData->decisionToDFA, configgrammarParserStaticData->sharedContextCache, options);
}

configGrammar::~configGrammar() {
  delete _interpreter;
}

const atn::ATN& configGrammar::getATN() const {
  return *configgrammarParserStaticData->atn;
}

std::string configGrammar::getGrammarFileName() const {
  return "configGrammar.g4";
}

const std::vector<std::string>& configGrammar::getRuleNames() const {
  return configgrammarParserStaticData->ruleNames;
}

const dfa::Vocabulary& configGrammar::getVocabulary() const {
  return configgrammarParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView configGrammar::getSerializedATN() const {
  return configgrammarParserStaticData->serializedATN;
}


//----------------- ConfigContext ------------------------------------------------------------------

configGrammar::ConfigContext::ConfigContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::ConfigContext::EOF() {
  return getToken(configGrammar::EOF, 0);
}

std::vector<tree::TerminalNode *> configGrammar::ConfigContext::EOL() {
  return getTokens(configGrammar::EOL);
}

tree::TerminalNode* configGrammar::ConfigContext::EOL(size_t i) {
  return getToken(configGrammar::EOL, i);
}

std::vector<configGrammar::StmtContext *> configGrammar::ConfigContext::stmt() {
  return getRuleContexts<configGrammar::StmtContext>();
}

configGrammar::StmtContext* configGrammar::ConfigContext::stmt(size_t i) {
  return getRuleContext<configGrammar::StmtContext>(i);
}


size_t configGrammar::ConfigContext::getRuleIndex() const {
  return configGrammar::RuleConfig;
}

void configGrammar::ConfigContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConfig(this);
}

void configGrammar::ConfigContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConfig(this);
}

configGrammar::ConfigContext* configGrammar::config() {
  ConfigContext *_localctx = _tracker.createInstance<ConfigContext>(_ctx, getState());
  enterRule(_localctx, 0, configGrammar::RuleConfig);
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
    setState(121);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configGrammar::EOL) {
      setState(118);
      match(configGrammar::EOL);
      setState(123);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(142);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & 137438951936) != 0)) {
      setState(132);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(124);
          stmt();
          setState(126); 
          _errHandler->sync(this);
          _la = _input->LA(1);
          do {
            setState(125);
            match(configGrammar::EOL);
            setState(128); 
            _errHandler->sync(this);
            _la = _input->LA(1);
          } while (_la == configGrammar::EOL); 
        }
        setState(134);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      }
      setState(135);
      stmt();
      setState(139);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == configGrammar::EOL) {
        setState(136);
        match(configGrammar::EOL);
        setState(141);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(144);
    match(configGrammar::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

configGrammar::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configGrammar::SetContext* configGrammar::StmtContext::set() {
  return getRuleContext<configGrammar::SetContext>(0);
}

configGrammar::BindingContext* configGrammar::StmtContext::binding() {
  return getRuleContext<configGrammar::BindingContext>(0);
}

configGrammar::FontContext* configGrammar::StmtContext::font() {
  return getRuleContext<configGrammar::FontContext>(0);
}

configGrammar::Floating_minimum_sizeContext* configGrammar::StmtContext::floating_minimum_size() {
  return getRuleContext<configGrammar::Floating_minimum_sizeContext>(0);
}

configGrammar::Floating_maximum_sizeContext* configGrammar::StmtContext::floating_maximum_size() {
  return getRuleContext<configGrammar::Floating_maximum_sizeContext>(0);
}

configGrammar::Floating_modifierContext* configGrammar::StmtContext::floating_modifier() {
  return getRuleContext<configGrammar::Floating_modifierContext>(0);
}

configGrammar::Default_orientationContext* configGrammar::StmtContext::default_orientation() {
  return getRuleContext<configGrammar::Default_orientationContext>(0);
}

configGrammar::Workspace_layoutContext* configGrammar::StmtContext::workspace_layout() {
  return getRuleContext<configGrammar::Workspace_layoutContext>(0);
}

configGrammar::Default_borderContext* configGrammar::StmtContext::default_border() {
  return getRuleContext<configGrammar::Default_borderContext>(0);
}

configGrammar::Hide_edge_bordersContext* configGrammar::StmtContext::hide_edge_borders() {
  return getRuleContext<configGrammar::Hide_edge_bordersContext>(0);
}

configGrammar::For_windowContext* configGrammar::StmtContext::for_window() {
  return getRuleContext<configGrammar::For_windowContext>(0);
}

configGrammar::AssignContext* configGrammar::StmtContext::assign() {
  return getRuleContext<configGrammar::AssignContext>(0);
}

configGrammar::No_focusContext* configGrammar::StmtContext::no_focus() {
  return getRuleContext<configGrammar::No_focusContext>(0);
}

configGrammar::Focus_follows_mouseContext* configGrammar::StmtContext::focus_follows_mouse() {
  return getRuleContext<configGrammar::Focus_follows_mouseContext>(0);
}

configGrammar::Mouse_warpingContext* configGrammar::StmtContext::mouse_warping() {
  return getRuleContext<configGrammar::Mouse_warpingContext>(0);
}

configGrammar::Focus_wrappingContext* configGrammar::StmtContext::focus_wrapping() {
  return getRuleContext<configGrammar::Focus_wrappingContext>(0);
}

configGrammar::Force_focus_wrappingContext* configGrammar::StmtContext::force_focus_wrapping() {
  return getRuleContext<configGrammar::Force_focus_wrappingContext>(0);
}

configGrammar::Workspace_auto_back_and_forthContext* configGrammar::StmtContext::workspace_auto_back_and_forth() {
  return getRuleContext<configGrammar::Workspace_auto_back_and_forthContext>(0);
}

configGrammar::Force_display_urgency_hintContext* configGrammar::StmtContext::force_display_urgency_hint() {
  return getRuleContext<configGrammar::Force_display_urgency_hintContext>(0);
}

configGrammar::Focus_on_window_activationContext* configGrammar::StmtContext::focus_on_window_activation() {
  return getRuleContext<configGrammar::Focus_on_window_activationContext>(0);
}

configGrammar::Title_alignContext* configGrammar::StmtContext::title_align() {
  return getRuleContext<configGrammar::Title_alignContext>(0);
}

configGrammar::WorkspaceContext* configGrammar::StmtContext::workspace() {
  return getRuleContext<configGrammar::WorkspaceContext>(0);
}

configGrammar::Ipc_socketContext* configGrammar::StmtContext::ipc_socket() {
  return getRuleContext<configGrammar::Ipc_socketContext>(0);
}

configGrammar::Ipc_kill_timeoutContext* configGrammar::StmtContext::ipc_kill_timeout() {
  return getRuleContext<configGrammar::Ipc_kill_timeoutContext>(0);
}

configGrammar::Restart_stateContext* configGrammar::StmtContext::restart_state() {
  return getRuleContext<configGrammar::Restart_stateContext>(0);
}

configGrammar::Popup_during_fullscreenContext* configGrammar::StmtContext::popup_during_fullscreen() {
  return getRuleContext<configGrammar::Popup_during_fullscreenContext>(0);
}

configGrammar::ExecContext* configGrammar::StmtContext::exec() {
  return getRuleContext<configGrammar::ExecContext>(0);
}


size_t configGrammar::StmtContext::getRuleIndex() const {
  return configGrammar::RuleStmt;
}

void configGrammar::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void configGrammar::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}

configGrammar::StmtContext* configGrammar::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 2, configGrammar::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(173);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configGrammar::SET: {
        enterOuterAlt(_localctx, 1);
        setState(146);
        set();
        break;
      }

      case configGrammar::BINDSYM: {
        enterOuterAlt(_localctx, 2);
        setState(147);
        binding();
        break;
      }

      case configGrammar::FONT: {
        enterOuterAlt(_localctx, 3);
        setState(148);
        font();
        break;
      }

      case configGrammar::FLOATING_MINIMUM_SIZE: {
        enterOuterAlt(_localctx, 4);
        setState(149);
        floating_minimum_size();
        break;
      }

      case configGrammar::FLOATING_MAXIMUM_SIZE: {
        enterOuterAlt(_localctx, 5);
        setState(150);
        floating_maximum_size();
        break;
      }

      case configGrammar::FLOATING_MODIFIER: {
        enterOuterAlt(_localctx, 6);
        setState(151);
        floating_modifier();
        break;
      }

      case configGrammar::DEFAULT_ORIENTATION: {
        enterOuterAlt(_localctx, 7);
        setState(152);
        default_orientation();
        break;
      }

      case configGrammar::WORKSPACE_LAYOUT: {
        enterOuterAlt(_localctx, 8);
        setState(153);
        workspace_layout();
        break;
      }

      case configGrammar::DEFAULT_BORDER: {
        enterOuterAlt(_localctx, 9);
        setState(154);
        default_border();
        break;
      }

      case configGrammar::HIDE_EDGE_BORDERS: {
        enterOuterAlt(_localctx, 10);
        setState(155);
        hide_edge_borders();
        break;
      }

      case configGrammar::FOR_WINDOW: {
        enterOuterAlt(_localctx, 11);
        setState(156);
        for_window();
        break;
      }

      case configGrammar::ASSIGN: {
        enterOuterAlt(_localctx, 12);
        setState(157);
        assign();
        break;
      }

      case configGrammar::NO_FOCUS: {
        enterOuterAlt(_localctx, 13);
        setState(158);
        no_focus();
        break;
      }

      case configGrammar::FOCUS_FOLLOWS_MOUSE: {
        enterOuterAlt(_localctx, 14);
        setState(159);
        focus_follows_mouse();
        break;
      }

      case configGrammar::MOUSE_WARPING: {
        enterOuterAlt(_localctx, 15);
        setState(160);
        mouse_warping();
        break;
      }

      case configGrammar::FOCUS_WRAPPING: {
        enterOuterAlt(_localctx, 16);
        setState(161);
        focus_wrapping();
        break;
      }

      case configGrammar::FORCE_FOCUS_WRAPPING: {
        enterOuterAlt(_localctx, 17);
        setState(162);
        force_focus_wrapping();
        break;
      }

      case configGrammar::WORKSPACE_AUTO_BACK_AND_FORTH: {
        enterOuterAlt(_localctx, 18);
        setState(163);
        workspace_auto_back_and_forth();
        break;
      }

      case configGrammar::FORCE_DISPLAY_URGENCY_HINT: {
        enterOuterAlt(_localctx, 19);
        setState(164);
        force_display_urgency_hint();
        break;
      }

      case configGrammar::FOCUS_ON_WINDOW_ACTIVATION: {
        enterOuterAlt(_localctx, 20);
        setState(165);
        focus_on_window_activation();
        break;
      }

      case configGrammar::TITLE_ALIGN: {
        enterOuterAlt(_localctx, 21);
        setState(166);
        title_align();
        break;
      }

      case configGrammar::WORKSPACE: {
        enterOuterAlt(_localctx, 22);
        setState(167);
        workspace();
        break;
      }

      case configGrammar::IPC_SOCKET: {
        enterOuterAlt(_localctx, 23);
        setState(168);
        ipc_socket();
        break;
      }

      case configGrammar::IPC_KILL_TIMEOUT: {
        enterOuterAlt(_localctx, 24);
        setState(169);
        ipc_kill_timeout();
        break;
      }

      case configGrammar::RESTART_STATE: {
        enterOuterAlt(_localctx, 25);
        setState(170);
        restart_state();
        break;
      }

      case configGrammar::POPUP_DURING_FULLSCREEN: {
        enterOuterAlt(_localctx, 26);
        setState(171);
        popup_during_fullscreen();
        break;
      }

      case configGrammar::EXEC: {
        enterOuterAlt(_localctx, 27);
        setState(172);
        exec();
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

//----------------- CommandsContext ------------------------------------------------------------------

configGrammar::CommandsContext::CommandsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::CommandsContext::OPEN_COMMAND() {
  return getToken(configGrammar::OPEN_COMMAND, 0);
}

std::vector<configGrammar::CommandContext *> configGrammar::CommandsContext::command() {
  return getRuleContexts<configGrammar::CommandContext>();
}

configGrammar::CommandContext* configGrammar::CommandsContext::command(size_t i) {
  return getRuleContext<configGrammar::CommandContext>(i);
}

tree::TerminalNode* configGrammar::CommandsContext::CLOSE_COMMAND() {
  return getToken(configGrammar::CLOSE_COMMAND, 0);
}

std::vector<tree::TerminalNode *> configGrammar::CommandsContext::COMMAND_SEP() {
  return getTokens(configGrammar::COMMAND_SEP);
}

tree::TerminalNode* configGrammar::CommandsContext::COMMAND_SEP(size_t i) {
  return getToken(configGrammar::COMMAND_SEP, i);
}


size_t configGrammar::CommandsContext::getRuleIndex() const {
  return configGrammar::RuleCommands;
}

void configGrammar::CommandsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommands(this);
}

void configGrammar::CommandsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommands(this);
}

configGrammar::CommandsContext* configGrammar::commands() {
  CommandsContext *_localctx = _tracker.createInstance<CommandsContext>(_ctx, getState());
  enterRule(_localctx, 4, configGrammar::RuleCommands);
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
    setState(175);
    match(configGrammar::OPEN_COMMAND);
    setState(181);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(176);
        command();
        setState(177);
        match(configGrammar::COMMAND_SEP); 
      }
      setState(183);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx);
    }
    setState(184);
    command();
    setState(186);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_SEP) {
      setState(185);
      match(configGrammar::COMMAND_SEP);
    }
    setState(188);
    match(configGrammar::CLOSE_COMMAND);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SetContext ------------------------------------------------------------------

configGrammar::SetContext::SetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::SetContext::SET() {
  return getToken(configGrammar::SET, 0);
}

tree::TerminalNode* configGrammar::SetContext::VAR() {
  return getToken(configGrammar::VAR, 0);
}

tree::TerminalNode* configGrammar::SetContext::EQ() {
  return getToken(configGrammar::EQ, 0);
}

tree::TerminalNode* configGrammar::SetContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::SetContext::getRuleIndex() const {
  return configGrammar::RuleSet;
}

void configGrammar::SetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSet(this);
}

void configGrammar::SetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSet(this);
}

configGrammar::SetContext* configGrammar::set() {
  SetContext *_localctx = _tracker.createInstance<SetContext>(_ctx, getState());
  enterRule(_localctx, 6, configGrammar::RuleSet);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(190);
    match(configGrammar::SET);
    setState(191);
    match(configGrammar::VAR);
    setState(192);
    match(configGrammar::EQ);
    setState(193);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BindingContext ------------------------------------------------------------------

configGrammar::BindingContext::BindingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::BindingContext::BINDSYM() {
  return getToken(configGrammar::BINDSYM, 0);
}

tree::TerminalNode* configGrammar::BindingContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}

configGrammar::CommandsContext* configGrammar::BindingContext::commands() {
  return getRuleContext<configGrammar::CommandsContext>(0);
}

std::vector<tree::TerminalNode *> configGrammar::BindingContext::OPTION() {
  return getTokens(configGrammar::OPTION);
}

tree::TerminalNode* configGrammar::BindingContext::OPTION(size_t i) {
  return getToken(configGrammar::OPTION, i);
}


size_t configGrammar::BindingContext::getRuleIndex() const {
  return configGrammar::RuleBinding;
}

void configGrammar::BindingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinding(this);
}

void configGrammar::BindingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinding(this);
}

configGrammar::BindingContext* configGrammar::binding() {
  BindingContext *_localctx = _tracker.createInstance<BindingContext>(_ctx, getState());
  enterRule(_localctx, 8, configGrammar::RuleBinding);
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
    setState(195);
    match(configGrammar::BINDSYM);
    setState(199);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configGrammar::OPTION) {
      setState(196);
      match(configGrammar::OPTION);
      setState(201);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(202);
    match(configGrammar::STRING);
    setState(203);
    commands();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FontContext ------------------------------------------------------------------

configGrammar::FontContext::FontContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::FontContext::FONT() {
  return getToken(configGrammar::FONT, 0);
}

tree::TerminalNode* configGrammar::FontContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::FontContext::getRuleIndex() const {
  return configGrammar::RuleFont;
}

void configGrammar::FontContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFont(this);
}

void configGrammar::FontContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFont(this);
}

configGrammar::FontContext* configGrammar::font() {
  FontContext *_localctx = _tracker.createInstance<FontContext>(_ctx, getState());
  enterRule(_localctx, 10, configGrammar::RuleFont);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(205);
    match(configGrammar::FONT);
    setState(206);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Floating_minimum_sizeContext ------------------------------------------------------------------

configGrammar::Floating_minimum_sizeContext::Floating_minimum_sizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Floating_minimum_sizeContext::FLOATING_MINIMUM_SIZE() {
  return getToken(configGrammar::FLOATING_MINIMUM_SIZE, 0);
}

configGrammar::DimensionContext* configGrammar::Floating_minimum_sizeContext::dimension() {
  return getRuleContext<configGrammar::DimensionContext>(0);
}


size_t configGrammar::Floating_minimum_sizeContext::getRuleIndex() const {
  return configGrammar::RuleFloating_minimum_size;
}

void configGrammar::Floating_minimum_sizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_minimum_size(this);
}

void configGrammar::Floating_minimum_sizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_minimum_size(this);
}

configGrammar::Floating_minimum_sizeContext* configGrammar::floating_minimum_size() {
  Floating_minimum_sizeContext *_localctx = _tracker.createInstance<Floating_minimum_sizeContext>(_ctx, getState());
  enterRule(_localctx, 12, configGrammar::RuleFloating_minimum_size);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    match(configGrammar::FLOATING_MINIMUM_SIZE);
    setState(209);
    dimension();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Floating_maximum_sizeContext ------------------------------------------------------------------

configGrammar::Floating_maximum_sizeContext::Floating_maximum_sizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Floating_maximum_sizeContext::FLOATING_MAXIMUM_SIZE() {
  return getToken(configGrammar::FLOATING_MAXIMUM_SIZE, 0);
}

configGrammar::DimensionContext* configGrammar::Floating_maximum_sizeContext::dimension() {
  return getRuleContext<configGrammar::DimensionContext>(0);
}


size_t configGrammar::Floating_maximum_sizeContext::getRuleIndex() const {
  return configGrammar::RuleFloating_maximum_size;
}

void configGrammar::Floating_maximum_sizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_maximum_size(this);
}

void configGrammar::Floating_maximum_sizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_maximum_size(this);
}

configGrammar::Floating_maximum_sizeContext* configGrammar::floating_maximum_size() {
  Floating_maximum_sizeContext *_localctx = _tracker.createInstance<Floating_maximum_sizeContext>(_ctx, getState());
  enterRule(_localctx, 14, configGrammar::RuleFloating_maximum_size);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(211);
    match(configGrammar::FLOATING_MAXIMUM_SIZE);
    setState(212);
    dimension();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Floating_modifierContext ------------------------------------------------------------------

configGrammar::Floating_modifierContext::Floating_modifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Floating_modifierContext::FLOATING_MODIFIER() {
  return getToken(configGrammar::FLOATING_MODIFIER, 0);
}

tree::TerminalNode* configGrammar::Floating_modifierContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Floating_modifierContext::getRuleIndex() const {
  return configGrammar::RuleFloating_modifier;
}

void configGrammar::Floating_modifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_modifier(this);
}

void configGrammar::Floating_modifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_modifier(this);
}

configGrammar::Floating_modifierContext* configGrammar::floating_modifier() {
  Floating_modifierContext *_localctx = _tracker.createInstance<Floating_modifierContext>(_ctx, getState());
  enterRule(_localctx, 16, configGrammar::RuleFloating_modifier);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(214);
    match(configGrammar::FLOATING_MODIFIER);
    setState(215);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Default_orientationContext ------------------------------------------------------------------

configGrammar::Default_orientationContext::Default_orientationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Default_orientationContext::DEFAULT_ORIENTATION() {
  return getToken(configGrammar::DEFAULT_ORIENTATION, 0);
}

tree::TerminalNode* configGrammar::Default_orientationContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Default_orientationContext::getRuleIndex() const {
  return configGrammar::RuleDefault_orientation;
}

void configGrammar::Default_orientationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_orientation(this);
}

void configGrammar::Default_orientationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_orientation(this);
}

configGrammar::Default_orientationContext* configGrammar::default_orientation() {
  Default_orientationContext *_localctx = _tracker.createInstance<Default_orientationContext>(_ctx, getState());
  enterRule(_localctx, 18, configGrammar::RuleDefault_orientation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(217);
    match(configGrammar::DEFAULT_ORIENTATION);
    setState(218);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Workspace_layoutContext ------------------------------------------------------------------

configGrammar::Workspace_layoutContext::Workspace_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Workspace_layoutContext::WORKSPACE_LAYOUT() {
  return getToken(configGrammar::WORKSPACE_LAYOUT, 0);
}

tree::TerminalNode* configGrammar::Workspace_layoutContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Workspace_layoutContext::getRuleIndex() const {
  return configGrammar::RuleWorkspace_layout;
}

void configGrammar::Workspace_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_layout(this);
}

void configGrammar::Workspace_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_layout(this);
}

configGrammar::Workspace_layoutContext* configGrammar::workspace_layout() {
  Workspace_layoutContext *_localctx = _tracker.createInstance<Workspace_layoutContext>(_ctx, getState());
  enterRule(_localctx, 20, configGrammar::RuleWorkspace_layout);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(220);
    match(configGrammar::WORKSPACE_LAYOUT);
    setState(221);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Default_borderContext ------------------------------------------------------------------

configGrammar::Default_borderContext::Default_borderContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Default_borderContext::DEFAULT_BORDER() {
  return getToken(configGrammar::DEFAULT_BORDER, 0);
}

tree::TerminalNode* configGrammar::Default_borderContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Default_borderContext::getRuleIndex() const {
  return configGrammar::RuleDefault_border;
}

void configGrammar::Default_borderContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_border(this);
}

void configGrammar::Default_borderContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_border(this);
}

configGrammar::Default_borderContext* configGrammar::default_border() {
  Default_borderContext *_localctx = _tracker.createInstance<Default_borderContext>(_ctx, getState());
  enterRule(_localctx, 22, configGrammar::RuleDefault_border);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(223);
    match(configGrammar::DEFAULT_BORDER);
    setState(224);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Hide_edge_bordersContext ------------------------------------------------------------------

configGrammar::Hide_edge_bordersContext::Hide_edge_bordersContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Hide_edge_bordersContext::HIDE_EDGE_BORDERS() {
  return getToken(configGrammar::HIDE_EDGE_BORDERS, 0);
}

tree::TerminalNode* configGrammar::Hide_edge_bordersContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Hide_edge_bordersContext::getRuleIndex() const {
  return configGrammar::RuleHide_edge_borders;
}

void configGrammar::Hide_edge_bordersContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHide_edge_borders(this);
}

void configGrammar::Hide_edge_bordersContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHide_edge_borders(this);
}

configGrammar::Hide_edge_bordersContext* configGrammar::hide_edge_borders() {
  Hide_edge_bordersContext *_localctx = _tracker.createInstance<Hide_edge_bordersContext>(_ctx, getState());
  enterRule(_localctx, 24, configGrammar::RuleHide_edge_borders);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(226);
    match(configGrammar::HIDE_EDGE_BORDERS);
    setState(227);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- For_windowContext ------------------------------------------------------------------

configGrammar::For_windowContext::For_windowContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::For_windowContext::FOR_WINDOW() {
  return getToken(configGrammar::FOR_WINDOW, 0);
}

configGrammar::CriteriaContext* configGrammar::For_windowContext::criteria() {
  return getRuleContext<configGrammar::CriteriaContext>(0);
}

configGrammar::CommandsContext* configGrammar::For_windowContext::commands() {
  return getRuleContext<configGrammar::CommandsContext>(0);
}


size_t configGrammar::For_windowContext::getRuleIndex() const {
  return configGrammar::RuleFor_window;
}

void configGrammar::For_windowContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFor_window(this);
}

void configGrammar::For_windowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFor_window(this);
}

configGrammar::For_windowContext* configGrammar::for_window() {
  For_windowContext *_localctx = _tracker.createInstance<For_windowContext>(_ctx, getState());
  enterRule(_localctx, 26, configGrammar::RuleFor_window);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    match(configGrammar::FOR_WINDOW);
    setState(230);
    criteria();
    setState(231);
    commands();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignContext ------------------------------------------------------------------

configGrammar::AssignContext::AssignContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::AssignContext::ASSIGN() {
  return getToken(configGrammar::ASSIGN, 0);
}

configGrammar::CriteriaContext* configGrammar::AssignContext::criteria() {
  return getRuleContext<configGrammar::CriteriaContext>(0);
}

tree::TerminalNode* configGrammar::AssignContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::AssignContext::getRuleIndex() const {
  return configGrammar::RuleAssign;
}

void configGrammar::AssignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign(this);
}

void configGrammar::AssignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign(this);
}

configGrammar::AssignContext* configGrammar::assign() {
  AssignContext *_localctx = _tracker.createInstance<AssignContext>(_ctx, getState());
  enterRule(_localctx, 28, configGrammar::RuleAssign);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(233);
    match(configGrammar::ASSIGN);
    setState(234);
    criteria();
    setState(235);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- No_focusContext ------------------------------------------------------------------

configGrammar::No_focusContext::No_focusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::No_focusContext::NO_FOCUS() {
  return getToken(configGrammar::NO_FOCUS, 0);
}

configGrammar::CriteriaContext* configGrammar::No_focusContext::criteria() {
  return getRuleContext<configGrammar::CriteriaContext>(0);
}


size_t configGrammar::No_focusContext::getRuleIndex() const {
  return configGrammar::RuleNo_focus;
}

void configGrammar::No_focusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNo_focus(this);
}

void configGrammar::No_focusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNo_focus(this);
}

configGrammar::No_focusContext* configGrammar::no_focus() {
  No_focusContext *_localctx = _tracker.createInstance<No_focusContext>(_ctx, getState());
  enterRule(_localctx, 30, configGrammar::RuleNo_focus);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(237);
    match(configGrammar::NO_FOCUS);
    setState(238);
    criteria();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Focus_follows_mouseContext ------------------------------------------------------------------

configGrammar::Focus_follows_mouseContext::Focus_follows_mouseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Focus_follows_mouseContext::FOCUS_FOLLOWS_MOUSE() {
  return getToken(configGrammar::FOCUS_FOLLOWS_MOUSE, 0);
}

tree::TerminalNode* configGrammar::Focus_follows_mouseContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Focus_follows_mouseContext::getRuleIndex() const {
  return configGrammar::RuleFocus_follows_mouse;
}

void configGrammar::Focus_follows_mouseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_follows_mouse(this);
}

void configGrammar::Focus_follows_mouseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_follows_mouse(this);
}

configGrammar::Focus_follows_mouseContext* configGrammar::focus_follows_mouse() {
  Focus_follows_mouseContext *_localctx = _tracker.createInstance<Focus_follows_mouseContext>(_ctx, getState());
  enterRule(_localctx, 32, configGrammar::RuleFocus_follows_mouse);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(240);
    match(configGrammar::FOCUS_FOLLOWS_MOUSE);
    setState(241);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Mouse_warpingContext ------------------------------------------------------------------

configGrammar::Mouse_warpingContext::Mouse_warpingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Mouse_warpingContext::MOUSE_WARPING() {
  return getToken(configGrammar::MOUSE_WARPING, 0);
}

tree::TerminalNode* configGrammar::Mouse_warpingContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Mouse_warpingContext::getRuleIndex() const {
  return configGrammar::RuleMouse_warping;
}

void configGrammar::Mouse_warpingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMouse_warping(this);
}

void configGrammar::Mouse_warpingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMouse_warping(this);
}

configGrammar::Mouse_warpingContext* configGrammar::mouse_warping() {
  Mouse_warpingContext *_localctx = _tracker.createInstance<Mouse_warpingContext>(_ctx, getState());
  enterRule(_localctx, 34, configGrammar::RuleMouse_warping);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(243);
    match(configGrammar::MOUSE_WARPING);
    setState(244);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Focus_wrappingContext ------------------------------------------------------------------

configGrammar::Focus_wrappingContext::Focus_wrappingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Focus_wrappingContext::FOCUS_WRAPPING() {
  return getToken(configGrammar::FOCUS_WRAPPING, 0);
}

tree::TerminalNode* configGrammar::Focus_wrappingContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Focus_wrappingContext::getRuleIndex() const {
  return configGrammar::RuleFocus_wrapping;
}

void configGrammar::Focus_wrappingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_wrapping(this);
}

void configGrammar::Focus_wrappingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_wrapping(this);
}

configGrammar::Focus_wrappingContext* configGrammar::focus_wrapping() {
  Focus_wrappingContext *_localctx = _tracker.createInstance<Focus_wrappingContext>(_ctx, getState());
  enterRule(_localctx, 36, configGrammar::RuleFocus_wrapping);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(246);
    match(configGrammar::FOCUS_WRAPPING);
    setState(247);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Force_focus_wrappingContext ------------------------------------------------------------------

configGrammar::Force_focus_wrappingContext::Force_focus_wrappingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Force_focus_wrappingContext::FORCE_FOCUS_WRAPPING() {
  return getToken(configGrammar::FORCE_FOCUS_WRAPPING, 0);
}

tree::TerminalNode* configGrammar::Force_focus_wrappingContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Force_focus_wrappingContext::getRuleIndex() const {
  return configGrammar::RuleForce_focus_wrapping;
}

void configGrammar::Force_focus_wrappingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForce_focus_wrapping(this);
}

void configGrammar::Force_focus_wrappingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForce_focus_wrapping(this);
}

configGrammar::Force_focus_wrappingContext* configGrammar::force_focus_wrapping() {
  Force_focus_wrappingContext *_localctx = _tracker.createInstance<Force_focus_wrappingContext>(_ctx, getState());
  enterRule(_localctx, 38, configGrammar::RuleForce_focus_wrapping);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(249);
    match(configGrammar::FORCE_FOCUS_WRAPPING);
    setState(250);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Workspace_auto_back_and_forthContext ------------------------------------------------------------------

configGrammar::Workspace_auto_back_and_forthContext::Workspace_auto_back_and_forthContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Workspace_auto_back_and_forthContext::WORKSPACE_AUTO_BACK_AND_FORTH() {
  return getToken(configGrammar::WORKSPACE_AUTO_BACK_AND_FORTH, 0);
}

tree::TerminalNode* configGrammar::Workspace_auto_back_and_forthContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Workspace_auto_back_and_forthContext::getRuleIndex() const {
  return configGrammar::RuleWorkspace_auto_back_and_forth;
}

void configGrammar::Workspace_auto_back_and_forthContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_auto_back_and_forth(this);
}

void configGrammar::Workspace_auto_back_and_forthContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_auto_back_and_forth(this);
}

configGrammar::Workspace_auto_back_and_forthContext* configGrammar::workspace_auto_back_and_forth() {
  Workspace_auto_back_and_forthContext *_localctx = _tracker.createInstance<Workspace_auto_back_and_forthContext>(_ctx, getState());
  enterRule(_localctx, 40, configGrammar::RuleWorkspace_auto_back_and_forth);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(252);
    match(configGrammar::WORKSPACE_AUTO_BACK_AND_FORTH);
    setState(253);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Force_display_urgency_hintContext ------------------------------------------------------------------

configGrammar::Force_display_urgency_hintContext::Force_display_urgency_hintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Force_display_urgency_hintContext::FORCE_DISPLAY_URGENCY_HINT() {
  return getToken(configGrammar::FORCE_DISPLAY_URGENCY_HINT, 0);
}

tree::TerminalNode* configGrammar::Force_display_urgency_hintContext::NUMBER() {
  return getToken(configGrammar::NUMBER, 0);
}


size_t configGrammar::Force_display_urgency_hintContext::getRuleIndex() const {
  return configGrammar::RuleForce_display_urgency_hint;
}

void configGrammar::Force_display_urgency_hintContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForce_display_urgency_hint(this);
}

void configGrammar::Force_display_urgency_hintContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForce_display_urgency_hint(this);
}

configGrammar::Force_display_urgency_hintContext* configGrammar::force_display_urgency_hint() {
  Force_display_urgency_hintContext *_localctx = _tracker.createInstance<Force_display_urgency_hintContext>(_ctx, getState());
  enterRule(_localctx, 42, configGrammar::RuleForce_display_urgency_hint);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(255);
    match(configGrammar::FORCE_DISPLAY_URGENCY_HINT);
    setState(256);
    match(configGrammar::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Focus_on_window_activationContext ------------------------------------------------------------------

configGrammar::Focus_on_window_activationContext::Focus_on_window_activationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Focus_on_window_activationContext::FOCUS_ON_WINDOW_ACTIVATION() {
  return getToken(configGrammar::FOCUS_ON_WINDOW_ACTIVATION, 0);
}

tree::TerminalNode* configGrammar::Focus_on_window_activationContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Focus_on_window_activationContext::getRuleIndex() const {
  return configGrammar::RuleFocus_on_window_activation;
}

void configGrammar::Focus_on_window_activationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_on_window_activation(this);
}

void configGrammar::Focus_on_window_activationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_on_window_activation(this);
}

configGrammar::Focus_on_window_activationContext* configGrammar::focus_on_window_activation() {
  Focus_on_window_activationContext *_localctx = _tracker.createInstance<Focus_on_window_activationContext>(_ctx, getState());
  enterRule(_localctx, 44, configGrammar::RuleFocus_on_window_activation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(258);
    match(configGrammar::FOCUS_ON_WINDOW_ACTIVATION);
    setState(259);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Title_alignContext ------------------------------------------------------------------

configGrammar::Title_alignContext::Title_alignContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Title_alignContext::TITLE_ALIGN() {
  return getToken(configGrammar::TITLE_ALIGN, 0);
}

tree::TerminalNode* configGrammar::Title_alignContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Title_alignContext::getRuleIndex() const {
  return configGrammar::RuleTitle_align;
}

void configGrammar::Title_alignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTitle_align(this);
}

void configGrammar::Title_alignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTitle_align(this);
}

configGrammar::Title_alignContext* configGrammar::title_align() {
  Title_alignContext *_localctx = _tracker.createInstance<Title_alignContext>(_ctx, getState());
  enterRule(_localctx, 46, configGrammar::RuleTitle_align);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(261);
    match(configGrammar::TITLE_ALIGN);
    setState(262);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WorkspaceContext ------------------------------------------------------------------

configGrammar::WorkspaceContext::WorkspaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::WorkspaceContext::WORKSPACE() {
  return getToken(configGrammar::WORKSPACE, 0);
}

std::vector<tree::TerminalNode *> configGrammar::WorkspaceContext::STRING() {
  return getTokens(configGrammar::STRING);
}

tree::TerminalNode* configGrammar::WorkspaceContext::STRING(size_t i) {
  return getToken(configGrammar::STRING, i);
}

tree::TerminalNode* configGrammar::WorkspaceContext::EQ() {
  return getToken(configGrammar::EQ, 0);
}


size_t configGrammar::WorkspaceContext::getRuleIndex() const {
  return configGrammar::RuleWorkspace;
}

void configGrammar::WorkspaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace(this);
}

void configGrammar::WorkspaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace(this);
}

configGrammar::WorkspaceContext* configGrammar::workspace() {
  WorkspaceContext *_localctx = _tracker.createInstance<WorkspaceContext>(_ctx, getState());
  enterRule(_localctx, 48, configGrammar::RuleWorkspace);
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
    setState(264);
    match(configGrammar::WORKSPACE);
    setState(265);
    match(configGrammar::STRING);
    setState(268);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::EQ) {
      setState(266);
      match(configGrammar::EQ);
      setState(267);
      match(configGrammar::STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ipc_socketContext ------------------------------------------------------------------

configGrammar::Ipc_socketContext::Ipc_socketContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Ipc_socketContext::IPC_SOCKET() {
  return getToken(configGrammar::IPC_SOCKET, 0);
}

tree::TerminalNode* configGrammar::Ipc_socketContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Ipc_socketContext::getRuleIndex() const {
  return configGrammar::RuleIpc_socket;
}

void configGrammar::Ipc_socketContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIpc_socket(this);
}

void configGrammar::Ipc_socketContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIpc_socket(this);
}

configGrammar::Ipc_socketContext* configGrammar::ipc_socket() {
  Ipc_socketContext *_localctx = _tracker.createInstance<Ipc_socketContext>(_ctx, getState());
  enterRule(_localctx, 50, configGrammar::RuleIpc_socket);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(270);
    match(configGrammar::IPC_SOCKET);
    setState(271);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ipc_kill_timeoutContext ------------------------------------------------------------------

configGrammar::Ipc_kill_timeoutContext::Ipc_kill_timeoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Ipc_kill_timeoutContext::IPC_KILL_TIMEOUT() {
  return getToken(configGrammar::IPC_KILL_TIMEOUT, 0);
}

tree::TerminalNode* configGrammar::Ipc_kill_timeoutContext::NUMBER() {
  return getToken(configGrammar::NUMBER, 0);
}


size_t configGrammar::Ipc_kill_timeoutContext::getRuleIndex() const {
  return configGrammar::RuleIpc_kill_timeout;
}

void configGrammar::Ipc_kill_timeoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIpc_kill_timeout(this);
}

void configGrammar::Ipc_kill_timeoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIpc_kill_timeout(this);
}

configGrammar::Ipc_kill_timeoutContext* configGrammar::ipc_kill_timeout() {
  Ipc_kill_timeoutContext *_localctx = _tracker.createInstance<Ipc_kill_timeoutContext>(_ctx, getState());
  enterRule(_localctx, 52, configGrammar::RuleIpc_kill_timeout);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(273);
    match(configGrammar::IPC_KILL_TIMEOUT);
    setState(274);
    match(configGrammar::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Restart_stateContext ------------------------------------------------------------------

configGrammar::Restart_stateContext::Restart_stateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Restart_stateContext::RESTART_STATE() {
  return getToken(configGrammar::RESTART_STATE, 0);
}

tree::TerminalNode* configGrammar::Restart_stateContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Restart_stateContext::getRuleIndex() const {
  return configGrammar::RuleRestart_state;
}

void configGrammar::Restart_stateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRestart_state(this);
}

void configGrammar::Restart_stateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRestart_state(this);
}

configGrammar::Restart_stateContext* configGrammar::restart_state() {
  Restart_stateContext *_localctx = _tracker.createInstance<Restart_stateContext>(_ctx, getState());
  enterRule(_localctx, 54, configGrammar::RuleRestart_state);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(276);
    match(configGrammar::RESTART_STATE);
    setState(277);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Popup_during_fullscreenContext ------------------------------------------------------------------

configGrammar::Popup_during_fullscreenContext::Popup_during_fullscreenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Popup_during_fullscreenContext::POPUP_DURING_FULLSCREEN() {
  return getToken(configGrammar::POPUP_DURING_FULLSCREEN, 0);
}

tree::TerminalNode* configGrammar::Popup_during_fullscreenContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}


size_t configGrammar::Popup_during_fullscreenContext::getRuleIndex() const {
  return configGrammar::RulePopup_during_fullscreen;
}

void configGrammar::Popup_during_fullscreenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPopup_during_fullscreen(this);
}

void configGrammar::Popup_during_fullscreenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPopup_during_fullscreen(this);
}

configGrammar::Popup_during_fullscreenContext* configGrammar::popup_during_fullscreen() {
  Popup_during_fullscreenContext *_localctx = _tracker.createInstance<Popup_during_fullscreenContext>(_ctx, getState());
  enterRule(_localctx, 56, configGrammar::RulePopup_during_fullscreen);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(279);
    match(configGrammar::POPUP_DURING_FULLSCREEN);
    setState(280);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExecContext ------------------------------------------------------------------

configGrammar::ExecContext::ExecContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::ExecContext::EXEC() {
  return getToken(configGrammar::EXEC, 0);
}

tree::TerminalNode* configGrammar::ExecContext::STRING() {
  return getToken(configGrammar::STRING, 0);
}

std::vector<tree::TerminalNode *> configGrammar::ExecContext::OPTION() {
  return getTokens(configGrammar::OPTION);
}

tree::TerminalNode* configGrammar::ExecContext::OPTION(size_t i) {
  return getToken(configGrammar::OPTION, i);
}


size_t configGrammar::ExecContext::getRuleIndex() const {
  return configGrammar::RuleExec;
}

void configGrammar::ExecContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExec(this);
}

void configGrammar::ExecContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExec(this);
}

configGrammar::ExecContext* configGrammar::exec() {
  ExecContext *_localctx = _tracker.createInstance<ExecContext>(_ctx, getState());
  enterRule(_localctx, 58, configGrammar::RuleExec);
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
    setState(282);
    match(configGrammar::EXEC);
    setState(286);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configGrammar::OPTION) {
      setState(283);
      match(configGrammar::OPTION);
      setState(288);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(289);
    match(configGrammar::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DimensionContext ------------------------------------------------------------------

configGrammar::DimensionContext::DimensionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> configGrammar::DimensionContext::NUMBER() {
  return getTokens(configGrammar::NUMBER);
}

tree::TerminalNode* configGrammar::DimensionContext::NUMBER(size_t i) {
  return getToken(configGrammar::NUMBER, i);
}

tree::TerminalNode* configGrammar::DimensionContext::DIMENSION_SEP() {
  return getToken(configGrammar::DIMENSION_SEP, 0);
}


size_t configGrammar::DimensionContext::getRuleIndex() const {
  return configGrammar::RuleDimension;
}

void configGrammar::DimensionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDimension(this);
}

void configGrammar::DimensionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDimension(this);
}

configGrammar::DimensionContext* configGrammar::dimension() {
  DimensionContext *_localctx = _tracker.createInstance<DimensionContext>(_ctx, getState());
  enterRule(_localctx, 60, configGrammar::RuleDimension);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(291);
    match(configGrammar::NUMBER);
    setState(292);
    match(configGrammar::DIMENSION_SEP);
    setState(293);
    match(configGrammar::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CommandContext ------------------------------------------------------------------

configGrammar::CommandContext::CommandContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configGrammar::Command_moveContext* configGrammar::CommandContext::command_move() {
  return getRuleContext<configGrammar::Command_moveContext>(0);
}

configGrammar::Command_execContext* configGrammar::CommandContext::command_exec() {
  return getRuleContext<configGrammar::Command_execContext>(0);
}

configGrammar::Command_exitContext* configGrammar::CommandContext::command_exit() {
  return getRuleContext<configGrammar::Command_exitContext>(0);
}

configGrammar::Command_debuglogContext* configGrammar::CommandContext::command_debuglog() {
  return getRuleContext<configGrammar::Command_debuglogContext>(0);
}

configGrammar::Command_borderContext* configGrammar::CommandContext::command_border() {
  return getRuleContext<configGrammar::Command_borderContext>(0);
}

configGrammar::Command_layoutContext* configGrammar::CommandContext::command_layout() {
  return getRuleContext<configGrammar::Command_layoutContext>(0);
}

configGrammar::Command_append_layoutContext* configGrammar::CommandContext::command_append_layout() {
  return getRuleContext<configGrammar::Command_append_layoutContext>(0);
}

configGrammar::Command_workspaceContext* configGrammar::CommandContext::command_workspace() {
  return getRuleContext<configGrammar::Command_workspaceContext>(0);
}

configGrammar::Command_focusContext* configGrammar::CommandContext::command_focus() {
  return getRuleContext<configGrammar::Command_focusContext>(0);
}

configGrammar::Command_killContext* configGrammar::CommandContext::command_kill() {
  return getRuleContext<configGrammar::Command_killContext>(0);
}

configGrammar::Command_modeContext* configGrammar::CommandContext::command_mode() {
  return getRuleContext<configGrammar::Command_modeContext>(0);
}

configGrammar::Command_openContext* configGrammar::CommandContext::command_open() {
  return getRuleContext<configGrammar::Command_openContext>(0);
}

configGrammar::Command_fullscreenContext* configGrammar::CommandContext::command_fullscreen() {
  return getRuleContext<configGrammar::Command_fullscreenContext>(0);
}

configGrammar::Command_stickyContext* configGrammar::CommandContext::command_sticky() {
  return getRuleContext<configGrammar::Command_stickyContext>(0);
}

configGrammar::Command_splitContext* configGrammar::CommandContext::command_split() {
  return getRuleContext<configGrammar::Command_splitContext>(0);
}

configGrammar::Command_floatingContext* configGrammar::CommandContext::command_floating() {
  return getRuleContext<configGrammar::Command_floatingContext>(0);
}

configGrammar::Command_resizeContext* configGrammar::CommandContext::command_resize() {
  return getRuleContext<configGrammar::Command_resizeContext>(0);
}

configGrammar::Command_renameContext* configGrammar::CommandContext::command_rename() {
  return getRuleContext<configGrammar::Command_renameContext>(0);
}

configGrammar::Command_nopContext* configGrammar::CommandContext::command_nop() {
  return getRuleContext<configGrammar::Command_nopContext>(0);
}

configGrammar::Command_reloadContext* configGrammar::CommandContext::command_reload() {
  return getRuleContext<configGrammar::Command_reloadContext>(0);
}

configGrammar::Command_restartContext* configGrammar::CommandContext::command_restart() {
  return getRuleContext<configGrammar::Command_restartContext>(0);
}

configGrammar::Command_scratchpadContext* configGrammar::CommandContext::command_scratchpad() {
  return getRuleContext<configGrammar::Command_scratchpadContext>(0);
}

configGrammar::Command_title_formatContext* configGrammar::CommandContext::command_title_format() {
  return getRuleContext<configGrammar::Command_title_formatContext>(0);
}

configGrammar::Command_title_window_iconContext* configGrammar::CommandContext::command_title_window_icon() {
  return getRuleContext<configGrammar::Command_title_window_iconContext>(0);
}


size_t configGrammar::CommandContext::getRuleIndex() const {
  return configGrammar::RuleCommand;
}

void configGrammar::CommandContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand(this);
}

void configGrammar::CommandContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand(this);
}

configGrammar::CommandContext* configGrammar::command() {
  CommandContext *_localctx = _tracker.createInstance<CommandContext>(_ctx, getState());
  enterRule(_localctx, 62, configGrammar::RuleCommand);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(319);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configGrammar::COMMAND_MOVE: {
        enterOuterAlt(_localctx, 1);
        setState(295);
        command_move();
        break;
      }

      case configGrammar::COMMAND_EXEC: {
        enterOuterAlt(_localctx, 2);
        setState(296);
        command_exec();
        break;
      }

      case configGrammar::COMMAND_EXIT: {
        enterOuterAlt(_localctx, 3);
        setState(297);
        command_exit();
        break;
      }

      case configGrammar::COMMAND_DEBUGLOG: {
        enterOuterAlt(_localctx, 4);
        setState(298);
        command_debuglog();
        break;
      }

      case configGrammar::COMMAND_BORDER: {
        enterOuterAlt(_localctx, 5);
        setState(299);
        command_border();
        break;
      }

      case configGrammar::COMMAND_LAYOUT: {
        enterOuterAlt(_localctx, 6);
        setState(300);
        command_layout();
        break;
      }

      case configGrammar::COMMAND_APPEND_LAYOUT: {
        enterOuterAlt(_localctx, 7);
        setState(301);
        command_append_layout();
        break;
      }

      case configGrammar::COMMAND_WORKSPACE: {
        enterOuterAlt(_localctx, 8);
        setState(302);
        command_workspace();
        break;
      }

      case configGrammar::COMMAND_FOCUS: {
        enterOuterAlt(_localctx, 9);
        setState(303);
        command_focus();
        break;
      }

      case configGrammar::COMMAND_KILL: {
        enterOuterAlt(_localctx, 10);
        setState(304);
        command_kill();
        break;
      }

      case configGrammar::COMMAND_MODE: {
        enterOuterAlt(_localctx, 11);
        setState(305);
        command_mode();
        break;
      }

      case configGrammar::COMMAND_OPEN: {
        enterOuterAlt(_localctx, 12);
        setState(306);
        command_open();
        break;
      }

      case configGrammar::COMMAND_FULLSCREEN: {
        enterOuterAlt(_localctx, 13);
        setState(307);
        command_fullscreen();
        break;
      }

      case configGrammar::COMMAND_STICKY: {
        enterOuterAlt(_localctx, 14);
        setState(308);
        command_sticky();
        break;
      }

      case configGrammar::COMMAND_SPLIT: {
        enterOuterAlt(_localctx, 15);
        setState(309);
        command_split();
        break;
      }

      case configGrammar::COMMAND_FLOATING: {
        enterOuterAlt(_localctx, 16);
        setState(310);
        command_floating();
        break;
      }

      case configGrammar::COMMAND_RESIZE: {
        enterOuterAlt(_localctx, 17);
        setState(311);
        command_resize();
        break;
      }

      case configGrammar::COMMAND_RENAME: {
        enterOuterAlt(_localctx, 18);
        setState(312);
        command_rename();
        break;
      }

      case configGrammar::COMMAND_NOP: {
        enterOuterAlt(_localctx, 19);
        setState(313);
        command_nop();
        break;
      }

      case configGrammar::COMMAND_RELOAD: {
        enterOuterAlt(_localctx, 20);
        setState(314);
        command_reload();
        break;
      }

      case configGrammar::COMMAND_RESTART: {
        enterOuterAlt(_localctx, 21);
        setState(315);
        command_restart();
        break;
      }

      case configGrammar::COMMAND_SCRATCHPAD: {
        enterOuterAlt(_localctx, 22);
        setState(316);
        command_scratchpad();
        break;
      }

      case configGrammar::COMMAND_TITLE_FORMAT: {
        enterOuterAlt(_localctx, 23);
        setState(317);
        command_title_format();
        break;
      }

      case configGrammar::COMMAND_TITLE_WINDOW_ICON: {
        enterOuterAlt(_localctx, 24);
        setState(318);
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

configGrammar::Command_moveContext::Command_moveContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_moveContext::COMMAND_MOVE() {
  return getToken(configGrammar::COMMAND_MOVE, 0);
}

tree::TerminalNode* configGrammar::Command_moveContext::COMMAND_NUMBER() {
  return getToken(configGrammar::COMMAND_NUMBER, 0);
}

tree::TerminalNode* configGrammar::Command_moveContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_moveContext::getRuleIndex() const {
  return configGrammar::RuleCommand_move;
}

void configGrammar::Command_moveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_move(this);
}

void configGrammar::Command_moveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_move(this);
}

configGrammar::Command_moveContext* configGrammar::command_move() {
  Command_moveContext *_localctx = _tracker.createInstance<Command_moveContext>(_ctx, getState());
  enterRule(_localctx, 64, configGrammar::RuleCommand_move);
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
    setState(321);
    match(configGrammar::COMMAND_MOVE);
    setState(322);
    _la = _input->LA(1);
    if (!(_la == configGrammar::COMMAND_NUMBER

    || _la == configGrammar::COMMAND_STRING)) {
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

configGrammar::Command_execContext::Command_execContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_execContext::COMMAND_EXEC() {
  return getToken(configGrammar::COMMAND_EXEC, 0);
}

tree::TerminalNode* configGrammar::Command_execContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}

std::vector<tree::TerminalNode *> configGrammar::Command_execContext::COMMAND_OPTION() {
  return getTokens(configGrammar::COMMAND_OPTION);
}

tree::TerminalNode* configGrammar::Command_execContext::COMMAND_OPTION(size_t i) {
  return getToken(configGrammar::COMMAND_OPTION, i);
}


size_t configGrammar::Command_execContext::getRuleIndex() const {
  return configGrammar::RuleCommand_exec;
}

void configGrammar::Command_execContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_exec(this);
}

void configGrammar::Command_execContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_exec(this);
}

configGrammar::Command_execContext* configGrammar::command_exec() {
  Command_execContext *_localctx = _tracker.createInstance<Command_execContext>(_ctx, getState());
  enterRule(_localctx, 66, configGrammar::RuleCommand_exec);
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
    setState(324);
    match(configGrammar::COMMAND_EXEC);
    setState(328);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configGrammar::COMMAND_OPTION) {
      setState(325);
      match(configGrammar::COMMAND_OPTION);
      setState(330);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(331);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_exitContext ------------------------------------------------------------------

configGrammar::Command_exitContext::Command_exitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_exitContext::COMMAND_EXIT() {
  return getToken(configGrammar::COMMAND_EXIT, 0);
}


size_t configGrammar::Command_exitContext::getRuleIndex() const {
  return configGrammar::RuleCommand_exit;
}

void configGrammar::Command_exitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_exit(this);
}

void configGrammar::Command_exitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_exit(this);
}

configGrammar::Command_exitContext* configGrammar::command_exit() {
  Command_exitContext *_localctx = _tracker.createInstance<Command_exitContext>(_ctx, getState());
  enterRule(_localctx, 68, configGrammar::RuleCommand_exit);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(333);
    match(configGrammar::COMMAND_EXIT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_killContext ------------------------------------------------------------------

configGrammar::Command_killContext::Command_killContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_killContext::COMMAND_KILL() {
  return getToken(configGrammar::COMMAND_KILL, 0);
}

tree::TerminalNode* configGrammar::Command_killContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_killContext::getRuleIndex() const {
  return configGrammar::RuleCommand_kill;
}

void configGrammar::Command_killContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_kill(this);
}

void configGrammar::Command_killContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_kill(this);
}

configGrammar::Command_killContext* configGrammar::command_kill() {
  Command_killContext *_localctx = _tracker.createInstance<Command_killContext>(_ctx, getState());
  enterRule(_localctx, 70, configGrammar::RuleCommand_kill);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(335);
    match(configGrammar::COMMAND_KILL);
    setState(336);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_fullscreenContext ------------------------------------------------------------------

configGrammar::Command_fullscreenContext::Command_fullscreenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_fullscreenContext::COMMAND_FULLSCREEN() {
  return getToken(configGrammar::COMMAND_FULLSCREEN, 0);
}

std::vector<tree::TerminalNode *> configGrammar::Command_fullscreenContext::COMMAND_STRING() {
  return getTokens(configGrammar::COMMAND_STRING);
}

tree::TerminalNode* configGrammar::Command_fullscreenContext::COMMAND_STRING(size_t i) {
  return getToken(configGrammar::COMMAND_STRING, i);
}


size_t configGrammar::Command_fullscreenContext::getRuleIndex() const {
  return configGrammar::RuleCommand_fullscreen;
}

void configGrammar::Command_fullscreenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_fullscreen(this);
}

void configGrammar::Command_fullscreenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_fullscreen(this);
}

configGrammar::Command_fullscreenContext* configGrammar::command_fullscreen() {
  Command_fullscreenContext *_localctx = _tracker.createInstance<Command_fullscreenContext>(_ctx, getState());
  enterRule(_localctx, 72, configGrammar::RuleCommand_fullscreen);
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
    setState(338);
    match(configGrammar::COMMAND_FULLSCREEN);
    setState(339);
    match(configGrammar::COMMAND_STRING);
    setState(341);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_STRING) {
      setState(340);
      match(configGrammar::COMMAND_STRING);
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

configGrammar::Command_stickyContext::Command_stickyContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_stickyContext::COMMAND_STICKY() {
  return getToken(configGrammar::COMMAND_STICKY, 0);
}

tree::TerminalNode* configGrammar::Command_stickyContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_stickyContext::getRuleIndex() const {
  return configGrammar::RuleCommand_sticky;
}

void configGrammar::Command_stickyContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_sticky(this);
}

void configGrammar::Command_stickyContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_sticky(this);
}

configGrammar::Command_stickyContext* configGrammar::command_sticky() {
  Command_stickyContext *_localctx = _tracker.createInstance<Command_stickyContext>(_ctx, getState());
  enterRule(_localctx, 74, configGrammar::RuleCommand_sticky);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(343);
    match(configGrammar::COMMAND_STICKY);
    setState(344);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_debuglogContext ------------------------------------------------------------------

configGrammar::Command_debuglogContext::Command_debuglogContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_debuglogContext::COMMAND_DEBUGLOG() {
  return getToken(configGrammar::COMMAND_DEBUGLOG, 0);
}

tree::TerminalNode* configGrammar::Command_debuglogContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_debuglogContext::getRuleIndex() const {
  return configGrammar::RuleCommand_debuglog;
}

void configGrammar::Command_debuglogContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_debuglog(this);
}

void configGrammar::Command_debuglogContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_debuglog(this);
}

configGrammar::Command_debuglogContext* configGrammar::command_debuglog() {
  Command_debuglogContext *_localctx = _tracker.createInstance<Command_debuglogContext>(_ctx, getState());
  enterRule(_localctx, 76, configGrammar::RuleCommand_debuglog);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(346);
    match(configGrammar::COMMAND_DEBUGLOG);
    setState(347);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_borderContext ------------------------------------------------------------------

configGrammar::Command_borderContext::Command_borderContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_borderContext::COMMAND_BORDER() {
  return getToken(configGrammar::COMMAND_BORDER, 0);
}

tree::TerminalNode* configGrammar::Command_borderContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}

tree::TerminalNode* configGrammar::Command_borderContext::COMMAND_NUMBER() {
  return getToken(configGrammar::COMMAND_NUMBER, 0);
}


size_t configGrammar::Command_borderContext::getRuleIndex() const {
  return configGrammar::RuleCommand_border;
}

void configGrammar::Command_borderContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_border(this);
}

void configGrammar::Command_borderContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_border(this);
}

configGrammar::Command_borderContext* configGrammar::command_border() {
  Command_borderContext *_localctx = _tracker.createInstance<Command_borderContext>(_ctx, getState());
  enterRule(_localctx, 78, configGrammar::RuleCommand_border);
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
    setState(349);
    match(configGrammar::COMMAND_BORDER);
    setState(350);
    match(configGrammar::COMMAND_STRING);
    setState(352);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_NUMBER) {
      setState(351);
      match(configGrammar::COMMAND_NUMBER);
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

configGrammar::Command_layoutContext::Command_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_layoutContext::COMMAND_LAYOUT() {
  return getToken(configGrammar::COMMAND_LAYOUT, 0);
}

std::vector<tree::TerminalNode *> configGrammar::Command_layoutContext::COMMAND_STRING() {
  return getTokens(configGrammar::COMMAND_STRING);
}

tree::TerminalNode* configGrammar::Command_layoutContext::COMMAND_STRING(size_t i) {
  return getToken(configGrammar::COMMAND_STRING, i);
}


size_t configGrammar::Command_layoutContext::getRuleIndex() const {
  return configGrammar::RuleCommand_layout;
}

void configGrammar::Command_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_layout(this);
}

void configGrammar::Command_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_layout(this);
}

configGrammar::Command_layoutContext* configGrammar::command_layout() {
  Command_layoutContext *_localctx = _tracker.createInstance<Command_layoutContext>(_ctx, getState());
  enterRule(_localctx, 80, configGrammar::RuleCommand_layout);
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
    setState(354);
    match(configGrammar::COMMAND_LAYOUT);
    setState(355);
    match(configGrammar::COMMAND_STRING);
    setState(357);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_STRING) {
      setState(356);
      match(configGrammar::COMMAND_STRING);
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

configGrammar::Command_append_layoutContext::Command_append_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_append_layoutContext::COMMAND_APPEND_LAYOUT() {
  return getToken(configGrammar::COMMAND_APPEND_LAYOUT, 0);
}

tree::TerminalNode* configGrammar::Command_append_layoutContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_append_layoutContext::getRuleIndex() const {
  return configGrammar::RuleCommand_append_layout;
}

void configGrammar::Command_append_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_append_layout(this);
}

void configGrammar::Command_append_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_append_layout(this);
}

configGrammar::Command_append_layoutContext* configGrammar::command_append_layout() {
  Command_append_layoutContext *_localctx = _tracker.createInstance<Command_append_layoutContext>(_ctx, getState());
  enterRule(_localctx, 82, configGrammar::RuleCommand_append_layout);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(359);
    match(configGrammar::COMMAND_APPEND_LAYOUT);
    setState(360);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_workspaceContext ------------------------------------------------------------------

configGrammar::Command_workspaceContext::Command_workspaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_workspaceContext::COMMAND_WORKSPACE() {
  return getToken(configGrammar::COMMAND_WORKSPACE, 0);
}

tree::TerminalNode* configGrammar::Command_workspaceContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}

std::vector<tree::TerminalNode *> configGrammar::Command_workspaceContext::COMMAND_OPTION() {
  return getTokens(configGrammar::COMMAND_OPTION);
}

tree::TerminalNode* configGrammar::Command_workspaceContext::COMMAND_OPTION(size_t i) {
  return getToken(configGrammar::COMMAND_OPTION, i);
}


size_t configGrammar::Command_workspaceContext::getRuleIndex() const {
  return configGrammar::RuleCommand_workspace;
}

void configGrammar::Command_workspaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_workspace(this);
}

void configGrammar::Command_workspaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_workspace(this);
}

configGrammar::Command_workspaceContext* configGrammar::command_workspace() {
  Command_workspaceContext *_localctx = _tracker.createInstance<Command_workspaceContext>(_ctx, getState());
  enterRule(_localctx, 84, configGrammar::RuleCommand_workspace);
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
    setState(362);
    match(configGrammar::COMMAND_WORKSPACE);
    setState(366);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configGrammar::COMMAND_OPTION) {
      setState(363);
      match(configGrammar::COMMAND_OPTION);
      setState(368);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(369);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_focusContext ------------------------------------------------------------------

configGrammar::Command_focusContext::Command_focusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_focusContext::COMMAND_FOCUS() {
  return getToken(configGrammar::COMMAND_FOCUS, 0);
}

std::vector<tree::TerminalNode *> configGrammar::Command_focusContext::COMMAND_STRING() {
  return getTokens(configGrammar::COMMAND_STRING);
}

tree::TerminalNode* configGrammar::Command_focusContext::COMMAND_STRING(size_t i) {
  return getToken(configGrammar::COMMAND_STRING, i);
}


size_t configGrammar::Command_focusContext::getRuleIndex() const {
  return configGrammar::RuleCommand_focus;
}

void configGrammar::Command_focusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_focus(this);
}

void configGrammar::Command_focusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_focus(this);
}

configGrammar::Command_focusContext* configGrammar::command_focus() {
  Command_focusContext *_localctx = _tracker.createInstance<Command_focusContext>(_ctx, getState());
  enterRule(_localctx, 86, configGrammar::RuleCommand_focus);
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
    setState(371);
    match(configGrammar::COMMAND_FOCUS);
    setState(372);
    match(configGrammar::COMMAND_STRING);
    setState(374);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_STRING) {
      setState(373);
      match(configGrammar::COMMAND_STRING);
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

configGrammar::Command_splitContext::Command_splitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_splitContext::COMMAND_SPLIT() {
  return getToken(configGrammar::COMMAND_SPLIT, 0);
}

tree::TerminalNode* configGrammar::Command_splitContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_splitContext::getRuleIndex() const {
  return configGrammar::RuleCommand_split;
}

void configGrammar::Command_splitContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_split(this);
}

void configGrammar::Command_splitContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_split(this);
}

configGrammar::Command_splitContext* configGrammar::command_split() {
  Command_splitContext *_localctx = _tracker.createInstance<Command_splitContext>(_ctx, getState());
  enterRule(_localctx, 88, configGrammar::RuleCommand_split);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(376);
    match(configGrammar::COMMAND_SPLIT);
    setState(377);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_floatingContext ------------------------------------------------------------------

configGrammar::Command_floatingContext::Command_floatingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_floatingContext::COMMAND_FLOATING() {
  return getToken(configGrammar::COMMAND_FLOATING, 0);
}

tree::TerminalNode* configGrammar::Command_floatingContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_floatingContext::getRuleIndex() const {
  return configGrammar::RuleCommand_floating;
}

void configGrammar::Command_floatingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_floating(this);
}

void configGrammar::Command_floatingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_floating(this);
}

configGrammar::Command_floatingContext* configGrammar::command_floating() {
  Command_floatingContext *_localctx = _tracker.createInstance<Command_floatingContext>(_ctx, getState());
  enterRule(_localctx, 90, configGrammar::RuleCommand_floating);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(379);
    match(configGrammar::COMMAND_FLOATING);
    setState(380);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_resizeContext ------------------------------------------------------------------

configGrammar::Command_resizeContext::Command_resizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_resizeContext::COMMAND_RESIZE() {
  return getToken(configGrammar::COMMAND_RESIZE, 0);
}


size_t configGrammar::Command_resizeContext::getRuleIndex() const {
  return configGrammar::RuleCommand_resize;
}

void configGrammar::Command_resizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_resize(this);
}

void configGrammar::Command_resizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_resize(this);
}

configGrammar::Command_resizeContext* configGrammar::command_resize() {
  Command_resizeContext *_localctx = _tracker.createInstance<Command_resizeContext>(_ctx, getState());
  enterRule(_localctx, 92, configGrammar::RuleCommand_resize);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(382);
    match(configGrammar::COMMAND_RESIZE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_renameContext ------------------------------------------------------------------

configGrammar::Command_renameContext::Command_renameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_renameContext::COMMAND_RENAME() {
  return getToken(configGrammar::COMMAND_RENAME, 0);
}


size_t configGrammar::Command_renameContext::getRuleIndex() const {
  return configGrammar::RuleCommand_rename;
}

void configGrammar::Command_renameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_rename(this);
}

void configGrammar::Command_renameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_rename(this);
}

configGrammar::Command_renameContext* configGrammar::command_rename() {
  Command_renameContext *_localctx = _tracker.createInstance<Command_renameContext>(_ctx, getState());
  enterRule(_localctx, 94, configGrammar::RuleCommand_rename);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(384);
    match(configGrammar::COMMAND_RENAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_nopContext ------------------------------------------------------------------

configGrammar::Command_nopContext::Command_nopContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_nopContext::COMMAND_NOP() {
  return getToken(configGrammar::COMMAND_NOP, 0);
}

tree::TerminalNode* configGrammar::Command_nopContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_nopContext::getRuleIndex() const {
  return configGrammar::RuleCommand_nop;
}

void configGrammar::Command_nopContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_nop(this);
}

void configGrammar::Command_nopContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_nop(this);
}

configGrammar::Command_nopContext* configGrammar::command_nop() {
  Command_nopContext *_localctx = _tracker.createInstance<Command_nopContext>(_ctx, getState());
  enterRule(_localctx, 96, configGrammar::RuleCommand_nop);
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
    setState(386);
    match(configGrammar::COMMAND_NOP);
    setState(388);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configGrammar::COMMAND_STRING) {
      setState(387);
      match(configGrammar::COMMAND_STRING);
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

configGrammar::Command_reloadContext::Command_reloadContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_reloadContext::COMMAND_RELOAD() {
  return getToken(configGrammar::COMMAND_RELOAD, 0);
}


size_t configGrammar::Command_reloadContext::getRuleIndex() const {
  return configGrammar::RuleCommand_reload;
}

void configGrammar::Command_reloadContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_reload(this);
}

void configGrammar::Command_reloadContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_reload(this);
}

configGrammar::Command_reloadContext* configGrammar::command_reload() {
  Command_reloadContext *_localctx = _tracker.createInstance<Command_reloadContext>(_ctx, getState());
  enterRule(_localctx, 98, configGrammar::RuleCommand_reload);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(390);
    match(configGrammar::COMMAND_RELOAD);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_restartContext ------------------------------------------------------------------

configGrammar::Command_restartContext::Command_restartContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_restartContext::COMMAND_RESTART() {
  return getToken(configGrammar::COMMAND_RESTART, 0);
}


size_t configGrammar::Command_restartContext::getRuleIndex() const {
  return configGrammar::RuleCommand_restart;
}

void configGrammar::Command_restartContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_restart(this);
}

void configGrammar::Command_restartContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_restart(this);
}

configGrammar::Command_restartContext* configGrammar::command_restart() {
  Command_restartContext *_localctx = _tracker.createInstance<Command_restartContext>(_ctx, getState());
  enterRule(_localctx, 100, configGrammar::RuleCommand_restart);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(392);
    match(configGrammar::COMMAND_RESTART);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_modeContext ------------------------------------------------------------------

configGrammar::Command_modeContext::Command_modeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_modeContext::COMMAND_MODE() {
  return getToken(configGrammar::COMMAND_MODE, 0);
}

tree::TerminalNode* configGrammar::Command_modeContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_modeContext::getRuleIndex() const {
  return configGrammar::RuleCommand_mode;
}

void configGrammar::Command_modeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_mode(this);
}

void configGrammar::Command_modeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_mode(this);
}

configGrammar::Command_modeContext* configGrammar::command_mode() {
  Command_modeContext *_localctx = _tracker.createInstance<Command_modeContext>(_ctx, getState());
  enterRule(_localctx, 102, configGrammar::RuleCommand_mode);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(394);
    match(configGrammar::COMMAND_MODE);
    setState(395);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_openContext ------------------------------------------------------------------

configGrammar::Command_openContext::Command_openContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_openContext::COMMAND_OPEN() {
  return getToken(configGrammar::COMMAND_OPEN, 0);
}


size_t configGrammar::Command_openContext::getRuleIndex() const {
  return configGrammar::RuleCommand_open;
}

void configGrammar::Command_openContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_open(this);
}

void configGrammar::Command_openContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_open(this);
}

configGrammar::Command_openContext* configGrammar::command_open() {
  Command_openContext *_localctx = _tracker.createInstance<Command_openContext>(_ctx, getState());
  enterRule(_localctx, 104, configGrammar::RuleCommand_open);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(397);
    match(configGrammar::COMMAND_OPEN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_scratchpadContext ------------------------------------------------------------------

configGrammar::Command_scratchpadContext::Command_scratchpadContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_scratchpadContext::COMMAND_SCRATCHPAD() {
  return getToken(configGrammar::COMMAND_SCRATCHPAD, 0);
}

tree::TerminalNode* configGrammar::Command_scratchpadContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_scratchpadContext::getRuleIndex() const {
  return configGrammar::RuleCommand_scratchpad;
}

void configGrammar::Command_scratchpadContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_scratchpad(this);
}

void configGrammar::Command_scratchpadContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_scratchpad(this);
}

configGrammar::Command_scratchpadContext* configGrammar::command_scratchpad() {
  Command_scratchpadContext *_localctx = _tracker.createInstance<Command_scratchpadContext>(_ctx, getState());
  enterRule(_localctx, 106, configGrammar::RuleCommand_scratchpad);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(399);
    match(configGrammar::COMMAND_SCRATCHPAD);
    setState(400);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_title_formatContext ------------------------------------------------------------------

configGrammar::Command_title_formatContext::Command_title_formatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_title_formatContext::COMMAND_TITLE_FORMAT() {
  return getToken(configGrammar::COMMAND_TITLE_FORMAT, 0);
}

tree::TerminalNode* configGrammar::Command_title_formatContext::COMMAND_STRING() {
  return getToken(configGrammar::COMMAND_STRING, 0);
}


size_t configGrammar::Command_title_formatContext::getRuleIndex() const {
  return configGrammar::RuleCommand_title_format;
}

void configGrammar::Command_title_formatContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_title_format(this);
}

void configGrammar::Command_title_formatContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_title_format(this);
}

configGrammar::Command_title_formatContext* configGrammar::command_title_format() {
  Command_title_formatContext *_localctx = _tracker.createInstance<Command_title_formatContext>(_ctx, getState());
  enterRule(_localctx, 108, configGrammar::RuleCommand_title_format);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(402);
    match(configGrammar::COMMAND_TITLE_FORMAT);
    setState(403);
    match(configGrammar::COMMAND_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Command_title_window_iconContext ------------------------------------------------------------------

configGrammar::Command_title_window_iconContext::Command_title_window_iconContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Command_title_window_iconContext::COMMAND_TITLE_WINDOW_ICON() {
  return getToken(configGrammar::COMMAND_TITLE_WINDOW_ICON, 0);
}


size_t configGrammar::Command_title_window_iconContext::getRuleIndex() const {
  return configGrammar::RuleCommand_title_window_icon;
}

void configGrammar::Command_title_window_iconContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCommand_title_window_icon(this);
}

void configGrammar::Command_title_window_iconContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCommand_title_window_icon(this);
}

configGrammar::Command_title_window_iconContext* configGrammar::command_title_window_icon() {
  Command_title_window_iconContext *_localctx = _tracker.createInstance<Command_title_window_iconContext>(_ctx, getState());
  enterRule(_localctx, 110, configGrammar::RuleCommand_title_window_icon);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(405);
    match(configGrammar::COMMAND_TITLE_WINDOW_ICON);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CriteriaContext ------------------------------------------------------------------

configGrammar::CriteriaContext::CriteriaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::CriteriaContext::BEGIN_CRITERION() {
  return getToken(configGrammar::BEGIN_CRITERION, 0);
}

tree::TerminalNode* configGrammar::CriteriaContext::END_CRITERION() {
  return getToken(configGrammar::END_CRITERION, 0);
}

std::vector<configGrammar::CriterionContext *> configGrammar::CriteriaContext::criterion() {
  return getRuleContexts<configGrammar::CriterionContext>();
}

configGrammar::CriterionContext* configGrammar::CriteriaContext::criterion(size_t i) {
  return getRuleContext<configGrammar::CriterionContext>(i);
}

std::vector<tree::TerminalNode *> configGrammar::CriteriaContext::CRITERION_SEP() {
  return getTokens(configGrammar::CRITERION_SEP);
}

tree::TerminalNode* configGrammar::CriteriaContext::CRITERION_SEP(size_t i) {
  return getToken(configGrammar::CRITERION_SEP, i);
}


size_t configGrammar::CriteriaContext::getRuleIndex() const {
  return configGrammar::RuleCriteria;
}

void configGrammar::CriteriaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCriteria(this);
}

void configGrammar::CriteriaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCriteria(this);
}

configGrammar::CriteriaContext* configGrammar::criteria() {
  CriteriaContext *_localctx = _tracker.createInstance<CriteriaContext>(_ctx, getState());
  enterRule(_localctx, 112, configGrammar::RuleCriteria);

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
    setState(407);
    match(configGrammar::BEGIN_CRITERION);

    setState(413);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(408);
        criterion();
        setState(409);
        match(configGrammar::CRITERION_SEP); 
      }
      setState(415);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx);
    }
    setState(416);
    criterion();
    setState(418);
    match(configGrammar::END_CRITERION);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CriterionContext ------------------------------------------------------------------

configGrammar::CriterionContext::CriterionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configGrammar::Value_criterionContext* configGrammar::CriterionContext::value_criterion() {
  return getRuleContext<configGrammar::Value_criterionContext>(0);
}

tree::TerminalNode* configGrammar::CriterionContext::TILING() {
  return getToken(configGrammar::TILING, 0);
}

tree::TerminalNode* configGrammar::CriterionContext::FLOATING() {
  return getToken(configGrammar::FLOATING, 0);
}

tree::TerminalNode* configGrammar::CriterionContext::ALL() {
  return getToken(configGrammar::ALL, 0);
}


size_t configGrammar::CriterionContext::getRuleIndex() const {
  return configGrammar::RuleCriterion;
}

void configGrammar::CriterionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCriterion(this);
}

void configGrammar::CriterionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCriterion(this);
}

configGrammar::CriterionContext* configGrammar::criterion() {
  CriterionContext *_localctx = _tracker.createInstance<CriterionContext>(_ctx, getState());
  enterRule(_localctx, 114, configGrammar::RuleCriterion);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(424);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configGrammar::WORKSPACE:
      case configGrammar::CLASS:
      case configGrammar::INSTANCE:
      case configGrammar::WINDOW_ROLE:
      case configGrammar::CON_ID:
      case configGrammar::ID:
      case configGrammar::WINDOW_TYPE:
      case configGrammar::TITLE:
      case configGrammar::URGENT:
      case configGrammar::MACHINE:
      case configGrammar::FLOATING_FROM:
      case configGrammar::TILING_FROM: {
        enterOuterAlt(_localctx, 1);
        setState(420);
        value_criterion();
        break;
      }

      case configGrammar::TILING: {
        enterOuterAlt(_localctx, 2);
        setState(421);
        match(configGrammar::TILING);
        break;
      }

      case configGrammar::FLOATING: {
        enterOuterAlt(_localctx, 3);
        setState(422);
        match(configGrammar::FLOATING);
        break;
      }

      case configGrammar::ALL: {
        enterOuterAlt(_localctx, 4);
        setState(423);
        match(configGrammar::ALL);
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

//----------------- Value_criterionContext ------------------------------------------------------------------

configGrammar::Value_criterionContext::Value_criterionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configGrammar::Value_criterionContext::CRITERION_EQ() {
  return getToken(configGrammar::CRITERION_EQ, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::CRITERION_STRING() {
  return getToken(configGrammar::CRITERION_STRING, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::CLASS() {
  return getToken(configGrammar::CLASS, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::INSTANCE() {
  return getToken(configGrammar::INSTANCE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::WINDOW_ROLE() {
  return getToken(configGrammar::WINDOW_ROLE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::CON_ID() {
  return getToken(configGrammar::CON_ID, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::ID() {
  return getToken(configGrammar::ID, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::WINDOW_TYPE() {
  return getToken(configGrammar::WINDOW_TYPE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::TITLE() {
  return getToken(configGrammar::TITLE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::URGENT() {
  return getToken(configGrammar::URGENT, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::WORKSPACE() {
  return getToken(configGrammar::WORKSPACE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::MACHINE() {
  return getToken(configGrammar::MACHINE, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::FLOATING_FROM() {
  return getToken(configGrammar::FLOATING_FROM, 0);
}

tree::TerminalNode* configGrammar::Value_criterionContext::TILING_FROM() {
  return getToken(configGrammar::TILING_FROM, 0);
}


size_t configGrammar::Value_criterionContext::getRuleIndex() const {
  return configGrammar::RuleValue_criterion;
}

void configGrammar::Value_criterionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue_criterion(this);
}

void configGrammar::Value_criterionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue_criterion(this);
}

configGrammar::Value_criterionContext* configGrammar::value_criterion() {
  Value_criterionContext *_localctx = _tracker.createInstance<Value_criterionContext>(_ctx, getState());
  enterRule(_localctx, 116, configGrammar::RuleValue_criterion);
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
    setState(426);
    _la = _input->LA(1);
    if (!(((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & 4501400604114945) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(427);
    match(configGrammar::CRITERION_EQ);
    setState(428);
    match(configGrammar::CRITERION_STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

void configGrammar::initialize() {
  ::antlr4::internal::call_once(configgrammarParserOnceFlag, configgrammarParserInitialize);
}
