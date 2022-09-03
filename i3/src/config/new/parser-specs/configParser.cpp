
// Generated from parser-specs/config.g4 by ANTLR 4.10.1


#include "configListener.h"

#include "configParser.h"


using namespace antlrcpp;

using namespace antlr4;

namespace {

struct ConfigParserStaticData final {
  ConfigParserStaticData(std::vector<std::string> ruleNames,
                        std::vector<std::string> literalNames,
                        std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  ConfigParserStaticData(const ConfigParserStaticData&) = delete;
  ConfigParserStaticData(ConfigParserStaticData&&) = delete;
  ConfigParserStaticData& operator=(const ConfigParserStaticData&) = delete;
  ConfigParserStaticData& operator=(ConfigParserStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

std::once_flag configParserOnceFlag;
ConfigParserStaticData *configParserStaticData = nullptr;

void configParserInitialize() {
  assert(configParserStaticData == nullptr);
  auto staticData = std::make_unique<ConfigParserStaticData>(
    std::vector<std::string>{
      "file", "stmt", "arguments", "stmt_mode", "stmt_mode_options", "stmt_mode_stmts", 
      "color", "color_single", "color_multiple", "exec", "exec_options", 
      "popup_during_fullscreen", "popup_during_fullscreen_value", "restart_state", 
      "ipc_kill_timeout", "ipc_socket", "workspace", "workspace_name", "title_align", 
      "title_align_value", "focus_on_window_activation", "focus_on_window_activation_value", 
      "force_display_urgency_hint", "workspace_auto_back_and_forth", "focus_wrapping", 
      "force_focus_wrapping", "mouse_warping", "mouse_warping_value", "focus_follows_mouse", 
      "no_focus", "assign", "assign_target", "assign_target_workspace", 
      "assign_target_number", "assign_target_output", "for_window", "criteria", 
      "criterion", "value_criterion", "hide_edge_borders", "border_type", 
      "default_border", "default_border_type", "border_style", "workspace_layout", 
      "workspace_layout_value", "default_orientation", "default_orientation_value", 
      "floating_modifier", "floating_minimum_size", "floating_maximum_size", 
      "dimension", "font", "set", "set_from_resource", "include", "binding", 
      "binding_options", "modifier", "keybinding"
    },
    std::vector<std::string>{
      "", "'mode'", "'{'", "'}'", "'--pango_markup'", "'client.background'", 
      "'client.focused_inactive'", "'client.focused'", "'client.unfocused'", 
      "'client.urgent'", "'client.placeholder'", "'exec'", "'exec_always'", 
      "'--no-startup-id'", "'popup_during_fullscreen'", "'ignore'", "'leave_fullscreen'", 
      "'smart'", "'restart_state'", "'ipc_kill_timeout'", "'ipc_socket'", 
      "'ipc-socket'", "'workspace'", "'output'", "'title_align'", "'left'", 
      "'center'", "'right'", "'focus_on_window_activation'", "'urgent'", 
      "'focus'", "'none'", "'force_display_urgency_hint'", "'ms'", "'workspace_auto_back_and_forth'", 
      "'focus_wrapping'", "'force'", "'force_focus_wrapping'", "'mouse_warping'", 
      "'focus_follows_mouse'", "'no_focus'", "'assign'", "'\\u2192'", "'number'", 
      "'up'", "'down'", "'primary'", "'for_window'", "'['", "']'", "'tiling'", 
      "'floating'", "'all'", "'class'", "'instance'", "'window_role'", "'con_id'", 
      "'id'", "'window_type'", "'title'", "'machine'", "'floating_from'", 
      "'tiling_from'", "'='", "'hide_edge_borders'", "'vertical'", "'horizontal'", 
      "'both'", "'default_border'", "'new_window'", "'default_floating_border'", 
      "'new_float'", "'normal'", "'pixel'", "'1pixel'", "'workspace_layout'", 
      "'default'", "'stacking'", "'stacked'", "'tabbed'", "'default_orientation'", 
      "'auto'", "'floating_modifier'", "'floating_minimum_size'", "'floating_maximum_size'", 
      "'x'", "'font'", "'set'", "'set_from_resource'", "'include'", "'bindsym'", 
      "'bindcode'", "'bind'", "'--release'", "'--border'", "'--whole-window'", 
      "'--exclude-titlebar'", "'+'", "'Mod1'", "'Mod2'", "'Mod3'", "'Mod4'", 
      "'Mod5'", "'Shift'", "'Control'", "'Ctrl'", "'Mode_switch'", "'Group1'", 
      "'Group2'", "'Group3'", "'Group4'"
    },
    std::vector<std::string>{
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
      "", "", "", "", "", "", "", "", "", "COMMENT", "WS", "BOOL", "VARIABLE", 
      "STRING", "NUMBER", "EOL"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,1,117,430,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,6,2,
  	7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,7,
  	14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,7,
  	21,2,22,7,22,2,23,7,23,2,24,7,24,2,25,7,25,2,26,7,26,2,27,7,27,2,28,7,
  	28,2,29,7,29,2,30,7,30,2,31,7,31,2,32,7,32,2,33,7,33,2,34,7,34,2,35,7,
  	35,2,36,7,36,2,37,7,37,2,38,7,38,2,39,7,39,2,40,7,40,2,41,7,41,2,42,7,
  	42,2,43,7,43,2,44,7,44,2,45,7,45,2,46,7,46,2,47,7,47,2,48,7,48,2,49,7,
  	49,2,50,7,50,2,51,7,51,2,52,7,52,2,53,7,53,2,54,7,54,2,55,7,55,2,56,7,
  	56,2,57,7,57,2,58,7,58,2,59,7,59,1,0,3,0,122,8,0,1,0,1,0,3,0,126,8,0,
  	5,0,128,8,0,10,0,12,0,131,9,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  	1,1,1,1,1,1,1,1,1,1,1,3,1,166,8,1,1,2,5,2,169,8,2,10,2,12,2,172,9,2,1,
  	3,1,3,5,3,176,8,3,10,3,12,3,179,9,3,1,3,1,3,1,3,3,3,184,8,3,1,3,1,3,3,
  	3,188,8,3,4,3,190,8,3,11,3,12,3,191,1,3,1,3,1,4,1,4,1,5,1,5,3,5,200,8,
  	5,1,6,1,6,3,6,204,8,6,1,7,1,7,1,7,1,8,1,8,1,8,1,8,1,8,3,8,214,8,8,1,8,
  	3,8,217,8,8,1,9,1,9,5,9,221,8,9,10,9,12,9,224,9,9,1,9,1,9,1,10,1,10,1,
  	11,1,11,1,11,1,12,1,12,1,13,1,13,1,13,1,14,1,14,1,14,1,15,1,15,1,15,1,
  	16,1,16,1,16,1,16,4,16,248,8,16,11,16,12,16,249,1,17,1,17,1,18,1,18,1,
  	18,1,19,1,19,1,20,1,20,1,20,1,21,1,21,1,22,1,22,1,22,3,22,267,8,22,1,
  	23,1,23,1,23,1,24,1,24,1,24,1,25,1,25,1,25,1,26,1,26,1,26,1,27,1,27,1,
  	28,1,28,1,28,1,29,1,29,1,29,1,30,1,30,1,30,3,30,292,8,30,1,30,1,30,1,
  	31,1,31,1,31,3,31,299,8,31,1,32,1,32,1,32,1,33,1,33,1,33,1,34,1,34,1,
  	34,1,35,1,35,1,35,1,35,1,36,1,36,4,36,316,8,36,11,36,12,36,317,1,36,1,
  	36,1,37,1,37,1,37,1,37,3,37,326,8,37,1,38,1,38,1,38,1,38,1,39,1,39,1,
  	39,1,40,1,40,1,41,1,41,1,41,1,42,1,42,1,43,1,43,1,43,1,43,3,43,346,8,
  	43,1,44,1,44,1,44,1,45,1,45,1,46,1,46,1,46,1,47,1,47,1,48,1,48,4,48,360,
  	8,48,11,48,12,48,361,1,49,1,49,1,49,1,50,1,50,1,50,1,51,1,51,1,51,1,51,
  	1,52,1,52,1,52,1,53,1,53,1,53,1,53,1,54,1,54,1,54,1,54,3,54,385,8,54,
  	1,55,1,55,1,55,1,56,1,56,5,56,392,8,56,10,56,12,56,395,9,56,1,56,1,56,
  	1,56,1,57,1,57,1,58,1,58,1,58,1,58,1,58,1,58,1,58,1,58,1,58,1,58,1,58,
  	1,58,1,58,1,58,1,58,3,58,417,8,58,1,58,1,58,1,58,5,58,422,8,58,10,58,
  	12,58,425,9,58,1,59,3,59,428,8,59,1,59,1,170,1,116,60,0,2,4,6,8,10,12,
  	14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,
  	60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,
  	106,108,110,112,114,116,118,0,18,1,0,6,10,1,0,11,12,1,0,15,17,1,0,20,
  	21,1,0,115,116,1,0,25,27,2,0,17,17,29,31,3,0,22,22,36,36,113,113,2,0,
  	23,23,31,31,3,0,25,25,27,27,44,46,3,0,22,22,29,29,53,62,4,0,17,17,31,
  	31,65,67,113,113,1,0,68,71,1,0,72,73,1,0,76,79,2,0,65,66,81,81,1,0,90,
  	92,1,0,93,96,441,0,121,1,0,0,0,2,165,1,0,0,0,4,170,1,0,0,0,6,173,1,0,
  	0,0,8,195,1,0,0,0,10,199,1,0,0,0,12,203,1,0,0,0,14,205,1,0,0,0,16,208,
  	1,0,0,0,18,218,1,0,0,0,20,227,1,0,0,0,22,229,1,0,0,0,24,232,1,0,0,0,26,
  	234,1,0,0,0,28,237,1,0,0,0,30,240,1,0,0,0,32,243,1,0,0,0,34,251,1,0,0,
  	0,36,253,1,0,0,0,38,256,1,0,0,0,40,258,1,0,0,0,42,261,1,0,0,0,44,263,
  	1,0,0,0,46,268,1,0,0,0,48,271,1,0,0,0,50,274,1,0,0,0,52,277,1,0,0,0,54,
  	280,1,0,0,0,56,282,1,0,0,0,58,285,1,0,0,0,60,288,1,0,0,0,62,298,1,0,0,
  	0,64,300,1,0,0,0,66,303,1,0,0,0,68,306,1,0,0,0,70,309,1,0,0,0,72,313,
  	1,0,0,0,74,325,1,0,0,0,76,327,1,0,0,0,78,331,1,0,0,0,80,334,1,0,0,0,82,
  	336,1,0,0,0,84,339,1,0,0,0,86,345,1,0,0,0,88,347,1,0,0,0,90,350,1,0,0,
  	0,92,352,1,0,0,0,94,355,1,0,0,0,96,357,1,0,0,0,98,363,1,0,0,0,100,366,
  	1,0,0,0,102,369,1,0,0,0,104,373,1,0,0,0,106,376,1,0,0,0,108,380,1,0,0,
  	0,110,386,1,0,0,0,112,389,1,0,0,0,114,399,1,0,0,0,116,416,1,0,0,0,118,
  	427,1,0,0,0,120,122,3,2,1,0,121,120,1,0,0,0,121,122,1,0,0,0,122,129,1,
  	0,0,0,123,125,5,117,0,0,124,126,3,2,1,0,125,124,1,0,0,0,125,126,1,0,0,
  	0,126,128,1,0,0,0,127,123,1,0,0,0,128,131,1,0,0,0,129,127,1,0,0,0,129,
  	130,1,0,0,0,130,132,1,0,0,0,131,129,1,0,0,0,132,133,5,0,0,1,133,1,1,0,
  	0,0,134,166,3,106,53,0,135,166,3,108,54,0,136,166,3,110,55,0,137,166,
  	3,112,56,0,138,166,3,6,3,0,139,166,3,104,52,0,140,166,3,98,49,0,141,166,
  	3,100,50,0,142,166,3,96,48,0,143,166,3,92,46,0,144,166,3,88,44,0,145,
  	166,3,82,41,0,146,166,3,78,39,0,147,166,3,70,35,0,148,166,3,60,30,0,149,
  	166,3,58,29,0,150,166,3,56,28,0,151,166,3,52,26,0,152,166,3,48,24,0,153,
  	166,3,50,25,0,154,166,3,46,23,0,155,166,3,44,22,0,156,166,3,40,20,0,157,
  	166,3,36,18,0,158,166,3,32,16,0,159,166,3,30,15,0,160,166,3,28,14,0,161,
  	166,3,26,13,0,162,166,3,22,11,0,163,166,3,18,9,0,164,166,3,12,6,0,165,
  	134,1,0,0,0,165,135,1,0,0,0,165,136,1,0,0,0,165,137,1,0,0,0,165,138,1,
  	0,0,0,165,139,1,0,0,0,165,140,1,0,0,0,165,141,1,0,0,0,165,142,1,0,0,0,
  	165,143,1,0,0,0,165,144,1,0,0,0,165,145,1,0,0,0,165,146,1,0,0,0,165,147,
  	1,0,0,0,165,148,1,0,0,0,165,149,1,0,0,0,165,150,1,0,0,0,165,151,1,0,0,
  	0,165,152,1,0,0,0,165,153,1,0,0,0,165,154,1,0,0,0,165,155,1,0,0,0,165,
  	156,1,0,0,0,165,157,1,0,0,0,165,158,1,0,0,0,165,159,1,0,0,0,165,160,1,
  	0,0,0,165,161,1,0,0,0,165,162,1,0,0,0,165,163,1,0,0,0,165,164,1,0,0,0,
  	166,3,1,0,0,0,167,169,9,0,0,0,168,167,1,0,0,0,169,172,1,0,0,0,170,171,
  	1,0,0,0,170,168,1,0,0,0,171,5,1,0,0,0,172,170,1,0,0,0,173,177,5,1,0,0,
  	174,176,3,8,4,0,175,174,1,0,0,0,176,179,1,0,0,0,177,175,1,0,0,0,177,178,
  	1,0,0,0,178,180,1,0,0,0,179,177,1,0,0,0,180,181,5,115,0,0,181,183,5,2,
  	0,0,182,184,3,10,5,0,183,182,1,0,0,0,183,184,1,0,0,0,184,189,1,0,0,0,
  	185,187,5,117,0,0,186,188,3,10,5,0,187,186,1,0,0,0,187,188,1,0,0,0,188,
  	190,1,0,0,0,189,185,1,0,0,0,190,191,1,0,0,0,191,189,1,0,0,0,191,192,1,
  	0,0,0,192,193,1,0,0,0,193,194,5,3,0,0,194,7,1,0,0,0,195,196,5,4,0,0,196,
  	9,1,0,0,0,197,200,3,106,53,0,198,200,3,112,56,0,199,197,1,0,0,0,199,198,
  	1,0,0,0,200,11,1,0,0,0,201,204,3,14,7,0,202,204,3,16,8,0,203,201,1,0,
  	0,0,203,202,1,0,0,0,204,13,1,0,0,0,205,206,5,5,0,0,206,207,5,115,0,0,
  	207,15,1,0,0,0,208,209,7,0,0,0,209,210,5,115,0,0,210,211,5,115,0,0,211,
  	213,5,115,0,0,212,214,5,115,0,0,213,212,1,0,0,0,213,214,1,0,0,0,214,216,
  	1,0,0,0,215,217,5,115,0,0,216,215,1,0,0,0,216,217,1,0,0,0,217,17,1,0,
  	0,0,218,222,7,1,0,0,219,221,3,20,10,0,220,219,1,0,0,0,221,224,1,0,0,0,
  	222,220,1,0,0,0,222,223,1,0,0,0,223,225,1,0,0,0,224,222,1,0,0,0,225,226,
  	3,4,2,0,226,19,1,0,0,0,227,228,5,13,0,0,228,21,1,0,0,0,229,230,5,14,0,
  	0,230,231,3,24,12,0,231,23,1,0,0,0,232,233,7,2,0,0,233,25,1,0,0,0,234,
  	235,5,18,0,0,235,236,5,115,0,0,236,27,1,0,0,0,237,238,5,19,0,0,238,239,
  	5,116,0,0,239,29,1,0,0,0,240,241,7,3,0,0,241,242,5,115,0,0,242,31,1,0,
  	0,0,243,244,5,22,0,0,244,245,3,34,17,0,245,247,5,23,0,0,246,248,5,115,
  	0,0,247,246,1,0,0,0,248,249,1,0,0,0,249,247,1,0,0,0,249,250,1,0,0,0,250,
  	33,1,0,0,0,251,252,7,4,0,0,252,35,1,0,0,0,253,254,5,24,0,0,254,255,3,
  	38,19,0,255,37,1,0,0,0,256,257,7,5,0,0,257,39,1,0,0,0,258,259,5,28,0,
  	0,259,260,3,42,21,0,260,41,1,0,0,0,261,262,7,6,0,0,262,43,1,0,0,0,263,
  	264,5,32,0,0,264,266,5,116,0,0,265,267,5,33,0,0,266,265,1,0,0,0,266,267,
  	1,0,0,0,267,45,1,0,0,0,268,269,5,34,0,0,269,270,5,113,0,0,270,47,1,0,
  	0,0,271,272,5,35,0,0,272,273,7,7,0,0,273,49,1,0,0,0,274,275,5,37,0,0,
  	275,276,5,113,0,0,276,51,1,0,0,0,277,278,5,38,0,0,278,279,3,54,27,0,279,
  	53,1,0,0,0,280,281,7,8,0,0,281,55,1,0,0,0,282,283,5,39,0,0,283,284,5,
  	113,0,0,284,57,1,0,0,0,285,286,5,40,0,0,286,287,3,72,36,0,287,59,1,0,
  	0,0,288,289,5,41,0,0,289,291,3,72,36,0,290,292,5,42,0,0,291,290,1,0,0,
  	0,291,292,1,0,0,0,292,293,1,0,0,0,293,294,3,62,31,0,294,61,1,0,0,0,295,
  	299,3,64,32,0,296,299,3,66,33,0,297,299,3,68,34,0,298,295,1,0,0,0,298,
  	296,1,0,0,0,298,297,1,0,0,0,299,63,1,0,0,0,300,301,5,22,0,0,301,302,7,
  	4,0,0,302,65,1,0,0,0,303,304,5,43,0,0,304,305,5,115,0,0,305,67,1,0,0,
  	0,306,307,5,23,0,0,307,308,7,9,0,0,308,69,1,0,0,0,309,310,5,47,0,0,310,
  	311,3,72,36,0,311,312,3,4,2,0,312,71,1,0,0,0,313,315,5,48,0,0,314,316,
  	3,74,37,0,315,314,1,0,0,0,316,317,1,0,0,0,317,315,1,0,0,0,317,318,1,0,
  	0,0,318,319,1,0,0,0,319,320,5,49,0,0,320,73,1,0,0,0,321,326,3,76,38,0,
  	322,326,5,50,0,0,323,326,5,51,0,0,324,326,5,52,0,0,325,321,1,0,0,0,325,
  	322,1,0,0,0,325,323,1,0,0,0,325,324,1,0,0,0,326,75,1,0,0,0,327,328,7,
  	10,0,0,328,329,5,63,0,0,329,330,5,115,0,0,330,77,1,0,0,0,331,332,5,64,
  	0,0,332,333,3,80,40,0,333,79,1,0,0,0,334,335,7,11,0,0,335,81,1,0,0,0,
  	336,337,3,84,42,0,337,338,3,86,43,0,338,83,1,0,0,0,339,340,7,12,0,0,340,
  	85,1,0,0,0,341,342,7,13,0,0,342,346,5,116,0,0,343,346,5,31,0,0,344,346,
  	5,74,0,0,345,341,1,0,0,0,345,343,1,0,0,0,345,344,1,0,0,0,346,87,1,0,0,
  	0,347,348,5,75,0,0,348,349,3,90,45,0,349,89,1,0,0,0,350,351,7,14,0,0,
  	351,91,1,0,0,0,352,353,5,80,0,0,353,354,3,94,47,0,354,93,1,0,0,0,355,
  	356,7,15,0,0,356,95,1,0,0,0,357,359,5,82,0,0,358,360,3,116,58,0,359,358,
  	1,0,0,0,360,361,1,0,0,0,361,359,1,0,0,0,361,362,1,0,0,0,362,97,1,0,0,
  	0,363,364,5,83,0,0,364,365,3,102,51,0,365,99,1,0,0,0,366,367,5,84,0,0,
  	367,368,3,102,51,0,368,101,1,0,0,0,369,370,5,116,0,0,370,371,5,85,0,0,
  	371,372,5,116,0,0,372,103,1,0,0,0,373,374,5,86,0,0,374,375,3,4,2,0,375,
  	105,1,0,0,0,376,377,5,87,0,0,377,378,5,114,0,0,378,379,3,4,2,0,379,107,
  	1,0,0,0,380,381,5,88,0,0,381,382,5,114,0,0,382,384,5,115,0,0,383,385,
  	5,115,0,0,384,383,1,0,0,0,384,385,1,0,0,0,385,109,1,0,0,0,386,387,5,89,
  	0,0,387,388,5,115,0,0,388,111,1,0,0,0,389,393,7,16,0,0,390,392,3,114,
  	57,0,391,390,1,0,0,0,392,395,1,0,0,0,393,391,1,0,0,0,393,394,1,0,0,0,
  	394,396,1,0,0,0,395,393,1,0,0,0,396,397,3,118,59,0,397,398,3,4,2,0,398,
  	113,1,0,0,0,399,400,7,17,0,0,400,115,1,0,0,0,401,402,6,58,-1,0,402,417,
  	5,98,0,0,403,417,5,99,0,0,404,417,5,100,0,0,405,417,5,101,0,0,406,417,
  	5,102,0,0,407,417,5,103,0,0,408,417,5,104,0,0,409,417,5,105,0,0,410,417,
  	5,106,0,0,411,417,5,107,0,0,412,417,5,108,0,0,413,417,5,109,0,0,414,417,
  	5,110,0,0,415,417,5,114,0,0,416,401,1,0,0,0,416,403,1,0,0,0,416,404,1,
  	0,0,0,416,405,1,0,0,0,416,406,1,0,0,0,416,407,1,0,0,0,416,408,1,0,0,0,
  	416,409,1,0,0,0,416,410,1,0,0,0,416,411,1,0,0,0,416,412,1,0,0,0,416,413,
  	1,0,0,0,416,414,1,0,0,0,416,415,1,0,0,0,417,423,1,0,0,0,418,419,10,15,
  	0,0,419,420,5,97,0,0,420,422,3,116,58,16,421,418,1,0,0,0,422,425,1,0,
  	0,0,423,421,1,0,0,0,423,424,1,0,0,0,424,117,1,0,0,0,425,423,1,0,0,0,426,
  	428,5,115,0,0,427,426,1,0,0,0,427,428,1,0,0,0,428,119,1,0,0,0,27,121,
  	125,129,165,170,177,183,187,191,199,203,213,216,222,249,266,291,298,317,
  	325,345,361,384,393,416,423,427
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  configParserStaticData = staticData.release();
}

}

configParser::configParser(TokenStream *input) : configParser(input, antlr4::atn::ParserATNSimulatorOptions()) {}

configParser::configParser(TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options) : Parser(input) {
  configParser::initialize();
  _interpreter = new atn::ParserATNSimulator(this, *configParserStaticData->atn, configParserStaticData->decisionToDFA, configParserStaticData->sharedContextCache, options);
}

configParser::~configParser() {
  delete _interpreter;
}

const atn::ATN& configParser::getATN() const {
  return *configParserStaticData->atn;
}

std::string configParser::getGrammarFileName() const {
  return "config.g4";
}

const std::vector<std::string>& configParser::getRuleNames() const {
  return configParserStaticData->ruleNames;
}

const dfa::Vocabulary& configParser::getVocabulary() const {
  return configParserStaticData->vocabulary;
}

antlr4::atn::SerializedATNView configParser::getSerializedATN() const {
  return configParserStaticData->serializedATN;
}


//----------------- FileContext ------------------------------------------------------------------

configParser::FileContext::FileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::FileContext::EOF() {
  return getToken(configParser::EOF, 0);
}

std::vector<configParser::StmtContext *> configParser::FileContext::stmt() {
  return getRuleContexts<configParser::StmtContext>();
}

configParser::StmtContext* configParser::FileContext::stmt(size_t i) {
  return getRuleContext<configParser::StmtContext>(i);
}

std::vector<tree::TerminalNode *> configParser::FileContext::EOL() {
  return getTokens(configParser::EOL);
}

tree::TerminalNode* configParser::FileContext::EOL(size_t i) {
  return getToken(configParser::EOL, i);
}


size_t configParser::FileContext::getRuleIndex() const {
  return configParser::RuleFile;
}

void configParser::FileContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFile(this);
}

void configParser::FileContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFile(this);
}

configParser::FileContext* configParser::file() {
  FileContext *_localctx = _tracker.createInstance<FileContext>(_ctx, getState());
  enterRule(_localctx, 0, configParser::RuleFile);
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
    setState(121);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__0)
      | (1ULL << configParser::T__4)
      | (1ULL << configParser::T__5)
      | (1ULL << configParser::T__6)
      | (1ULL << configParser::T__7)
      | (1ULL << configParser::T__8)
      | (1ULL << configParser::T__9)
      | (1ULL << configParser::T__10)
      | (1ULL << configParser::T__11)
      | (1ULL << configParser::T__13)
      | (1ULL << configParser::T__17)
      | (1ULL << configParser::T__18)
      | (1ULL << configParser::T__19)
      | (1ULL << configParser::T__20)
      | (1ULL << configParser::T__21)
      | (1ULL << configParser::T__23)
      | (1ULL << configParser::T__27)
      | (1ULL << configParser::T__31)
      | (1ULL << configParser::T__33)
      | (1ULL << configParser::T__34)
      | (1ULL << configParser::T__36)
      | (1ULL << configParser::T__37)
      | (1ULL << configParser::T__38)
      | (1ULL << configParser::T__39)
      | (1ULL << configParser::T__40)
      | (1ULL << configParser::T__46))) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 64)) & ((1ULL << (configParser::T__63 - 64))
      | (1ULL << (configParser::T__67 - 64))
      | (1ULL << (configParser::T__68 - 64))
      | (1ULL << (configParser::T__69 - 64))
      | (1ULL << (configParser::T__70 - 64))
      | (1ULL << (configParser::T__74 - 64))
      | (1ULL << (configParser::T__79 - 64))
      | (1ULL << (configParser::T__81 - 64))
      | (1ULL << (configParser::T__82 - 64))
      | (1ULL << (configParser::T__83 - 64))
      | (1ULL << (configParser::T__85 - 64))
      | (1ULL << (configParser::T__86 - 64))
      | (1ULL << (configParser::T__87 - 64))
      | (1ULL << (configParser::T__88 - 64))
      | (1ULL << (configParser::T__89 - 64))
      | (1ULL << (configParser::T__90 - 64))
      | (1ULL << (configParser::T__91 - 64)))) != 0)) {
      setState(120);
      stmt();
    }
    setState(129);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configParser::EOL) {
      setState(123);
      match(configParser::EOL);
      setState(125);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << configParser::T__0)
        | (1ULL << configParser::T__4)
        | (1ULL << configParser::T__5)
        | (1ULL << configParser::T__6)
        | (1ULL << configParser::T__7)
        | (1ULL << configParser::T__8)
        | (1ULL << configParser::T__9)
        | (1ULL << configParser::T__10)
        | (1ULL << configParser::T__11)
        | (1ULL << configParser::T__13)
        | (1ULL << configParser::T__17)
        | (1ULL << configParser::T__18)
        | (1ULL << configParser::T__19)
        | (1ULL << configParser::T__20)
        | (1ULL << configParser::T__21)
        | (1ULL << configParser::T__23)
        | (1ULL << configParser::T__27)
        | (1ULL << configParser::T__31)
        | (1ULL << configParser::T__33)
        | (1ULL << configParser::T__34)
        | (1ULL << configParser::T__36)
        | (1ULL << configParser::T__37)
        | (1ULL << configParser::T__38)
        | (1ULL << configParser::T__39)
        | (1ULL << configParser::T__40)
        | (1ULL << configParser::T__46))) != 0) || ((((_la - 64) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 64)) & ((1ULL << (configParser::T__63 - 64))
        | (1ULL << (configParser::T__67 - 64))
        | (1ULL << (configParser::T__68 - 64))
        | (1ULL << (configParser::T__69 - 64))
        | (1ULL << (configParser::T__70 - 64))
        | (1ULL << (configParser::T__74 - 64))
        | (1ULL << (configParser::T__79 - 64))
        | (1ULL << (configParser::T__81 - 64))
        | (1ULL << (configParser::T__82 - 64))
        | (1ULL << (configParser::T__83 - 64))
        | (1ULL << (configParser::T__85 - 64))
        | (1ULL << (configParser::T__86 - 64))
        | (1ULL << (configParser::T__87 - 64))
        | (1ULL << (configParser::T__88 - 64))
        | (1ULL << (configParser::T__89 - 64))
        | (1ULL << (configParser::T__90 - 64))
        | (1ULL << (configParser::T__91 - 64)))) != 0)) {
        setState(124);
        stmt();
      }
      setState(131);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(132);
    match(configParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

configParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::SetContext* configParser::StmtContext::set() {
  return getRuleContext<configParser::SetContext>(0);
}

configParser::Set_from_resourceContext* configParser::StmtContext::set_from_resource() {
  return getRuleContext<configParser::Set_from_resourceContext>(0);
}

configParser::IncludeContext* configParser::StmtContext::include() {
  return getRuleContext<configParser::IncludeContext>(0);
}

configParser::BindingContext* configParser::StmtContext::binding() {
  return getRuleContext<configParser::BindingContext>(0);
}

configParser::Stmt_modeContext* configParser::StmtContext::stmt_mode() {
  return getRuleContext<configParser::Stmt_modeContext>(0);
}

configParser::FontContext* configParser::StmtContext::font() {
  return getRuleContext<configParser::FontContext>(0);
}

configParser::Floating_minimum_sizeContext* configParser::StmtContext::floating_minimum_size() {
  return getRuleContext<configParser::Floating_minimum_sizeContext>(0);
}

configParser::Floating_maximum_sizeContext* configParser::StmtContext::floating_maximum_size() {
  return getRuleContext<configParser::Floating_maximum_sizeContext>(0);
}

configParser::Floating_modifierContext* configParser::StmtContext::floating_modifier() {
  return getRuleContext<configParser::Floating_modifierContext>(0);
}

configParser::Default_orientationContext* configParser::StmtContext::default_orientation() {
  return getRuleContext<configParser::Default_orientationContext>(0);
}

configParser::Workspace_layoutContext* configParser::StmtContext::workspace_layout() {
  return getRuleContext<configParser::Workspace_layoutContext>(0);
}

configParser::Default_borderContext* configParser::StmtContext::default_border() {
  return getRuleContext<configParser::Default_borderContext>(0);
}

configParser::Hide_edge_bordersContext* configParser::StmtContext::hide_edge_borders() {
  return getRuleContext<configParser::Hide_edge_bordersContext>(0);
}

configParser::For_windowContext* configParser::StmtContext::for_window() {
  return getRuleContext<configParser::For_windowContext>(0);
}

configParser::AssignContext* configParser::StmtContext::assign() {
  return getRuleContext<configParser::AssignContext>(0);
}

configParser::No_focusContext* configParser::StmtContext::no_focus() {
  return getRuleContext<configParser::No_focusContext>(0);
}

configParser::Focus_follows_mouseContext* configParser::StmtContext::focus_follows_mouse() {
  return getRuleContext<configParser::Focus_follows_mouseContext>(0);
}

configParser::Mouse_warpingContext* configParser::StmtContext::mouse_warping() {
  return getRuleContext<configParser::Mouse_warpingContext>(0);
}

configParser::Focus_wrappingContext* configParser::StmtContext::focus_wrapping() {
  return getRuleContext<configParser::Focus_wrappingContext>(0);
}

configParser::Force_focus_wrappingContext* configParser::StmtContext::force_focus_wrapping() {
  return getRuleContext<configParser::Force_focus_wrappingContext>(0);
}

configParser::Workspace_auto_back_and_forthContext* configParser::StmtContext::workspace_auto_back_and_forth() {
  return getRuleContext<configParser::Workspace_auto_back_and_forthContext>(0);
}

configParser::Force_display_urgency_hintContext* configParser::StmtContext::force_display_urgency_hint() {
  return getRuleContext<configParser::Force_display_urgency_hintContext>(0);
}

configParser::Focus_on_window_activationContext* configParser::StmtContext::focus_on_window_activation() {
  return getRuleContext<configParser::Focus_on_window_activationContext>(0);
}

configParser::Title_alignContext* configParser::StmtContext::title_align() {
  return getRuleContext<configParser::Title_alignContext>(0);
}

configParser::WorkspaceContext* configParser::StmtContext::workspace() {
  return getRuleContext<configParser::WorkspaceContext>(0);
}

configParser::Ipc_socketContext* configParser::StmtContext::ipc_socket() {
  return getRuleContext<configParser::Ipc_socketContext>(0);
}

configParser::Ipc_kill_timeoutContext* configParser::StmtContext::ipc_kill_timeout() {
  return getRuleContext<configParser::Ipc_kill_timeoutContext>(0);
}

configParser::Restart_stateContext* configParser::StmtContext::restart_state() {
  return getRuleContext<configParser::Restart_stateContext>(0);
}

configParser::Popup_during_fullscreenContext* configParser::StmtContext::popup_during_fullscreen() {
  return getRuleContext<configParser::Popup_during_fullscreenContext>(0);
}

configParser::ExecContext* configParser::StmtContext::exec() {
  return getRuleContext<configParser::ExecContext>(0);
}

configParser::ColorContext* configParser::StmtContext::color() {
  return getRuleContext<configParser::ColorContext>(0);
}


size_t configParser::StmtContext::getRuleIndex() const {
  return configParser::RuleStmt;
}

void configParser::StmtContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt(this);
}

void configParser::StmtContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt(this);
}

configParser::StmtContext* configParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 2, configParser::RuleStmt);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(165);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__86: {
        enterOuterAlt(_localctx, 1);
        setState(134);
        set();
        break;
      }

      case configParser::T__87: {
        enterOuterAlt(_localctx, 2);
        setState(135);
        set_from_resource();
        break;
      }

      case configParser::T__88: {
        enterOuterAlt(_localctx, 3);
        setState(136);
        include();
        break;
      }

      case configParser::T__89:
      case configParser::T__90:
      case configParser::T__91: {
        enterOuterAlt(_localctx, 4);
        setState(137);
        binding();
        break;
      }

      case configParser::T__0: {
        enterOuterAlt(_localctx, 5);
        setState(138);
        stmt_mode();
        break;
      }

      case configParser::T__85: {
        enterOuterAlt(_localctx, 6);
        setState(139);
        font();
        break;
      }

      case configParser::T__82: {
        enterOuterAlt(_localctx, 7);
        setState(140);
        floating_minimum_size();
        break;
      }

      case configParser::T__83: {
        enterOuterAlt(_localctx, 8);
        setState(141);
        floating_maximum_size();
        break;
      }

      case configParser::T__81: {
        enterOuterAlt(_localctx, 9);
        setState(142);
        floating_modifier();
        break;
      }

      case configParser::T__79: {
        enterOuterAlt(_localctx, 10);
        setState(143);
        default_orientation();
        break;
      }

      case configParser::T__74: {
        enterOuterAlt(_localctx, 11);
        setState(144);
        workspace_layout();
        break;
      }

      case configParser::T__67:
      case configParser::T__68:
      case configParser::T__69:
      case configParser::T__70: {
        enterOuterAlt(_localctx, 12);
        setState(145);
        default_border();
        break;
      }

      case configParser::T__63: {
        enterOuterAlt(_localctx, 13);
        setState(146);
        hide_edge_borders();
        break;
      }

      case configParser::T__46: {
        enterOuterAlt(_localctx, 14);
        setState(147);
        for_window();
        break;
      }

      case configParser::T__40: {
        enterOuterAlt(_localctx, 15);
        setState(148);
        assign();
        break;
      }

      case configParser::T__39: {
        enterOuterAlt(_localctx, 16);
        setState(149);
        no_focus();
        break;
      }

      case configParser::T__38: {
        enterOuterAlt(_localctx, 17);
        setState(150);
        focus_follows_mouse();
        break;
      }

      case configParser::T__37: {
        enterOuterAlt(_localctx, 18);
        setState(151);
        mouse_warping();
        break;
      }

      case configParser::T__34: {
        enterOuterAlt(_localctx, 19);
        setState(152);
        focus_wrapping();
        break;
      }

      case configParser::T__36: {
        enterOuterAlt(_localctx, 20);
        setState(153);
        force_focus_wrapping();
        break;
      }

      case configParser::T__33: {
        enterOuterAlt(_localctx, 21);
        setState(154);
        workspace_auto_back_and_forth();
        break;
      }

      case configParser::T__31: {
        enterOuterAlt(_localctx, 22);
        setState(155);
        force_display_urgency_hint();
        break;
      }

      case configParser::T__27: {
        enterOuterAlt(_localctx, 23);
        setState(156);
        focus_on_window_activation();
        break;
      }

      case configParser::T__23: {
        enterOuterAlt(_localctx, 24);
        setState(157);
        title_align();
        break;
      }

      case configParser::T__21: {
        enterOuterAlt(_localctx, 25);
        setState(158);
        workspace();
        break;
      }

      case configParser::T__19:
      case configParser::T__20: {
        enterOuterAlt(_localctx, 26);
        setState(159);
        ipc_socket();
        break;
      }

      case configParser::T__18: {
        enterOuterAlt(_localctx, 27);
        setState(160);
        ipc_kill_timeout();
        break;
      }

      case configParser::T__17: {
        enterOuterAlt(_localctx, 28);
        setState(161);
        restart_state();
        break;
      }

      case configParser::T__13: {
        enterOuterAlt(_localctx, 29);
        setState(162);
        popup_during_fullscreen();
        break;
      }

      case configParser::T__10:
      case configParser::T__11: {
        enterOuterAlt(_localctx, 30);
        setState(163);
        exec();
        break;
      }

      case configParser::T__4:
      case configParser::T__5:
      case configParser::T__6:
      case configParser::T__7:
      case configParser::T__8:
      case configParser::T__9: {
        enterOuterAlt(_localctx, 31);
        setState(164);
        color();
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

//----------------- ArgumentsContext ------------------------------------------------------------------

configParser::ArgumentsContext::ArgumentsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::ArgumentsContext::getRuleIndex() const {
  return configParser::RuleArguments;
}

void configParser::ArgumentsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArguments(this);
}

void configParser::ArgumentsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArguments(this);
}

configParser::ArgumentsContext* configParser::arguments() {
  ArgumentsContext *_localctx = _tracker.createInstance<ArgumentsContext>(_ctx, getState());
  enterRule(_localctx, 4, configParser::RuleArguments);

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
    setState(170);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    while (alt != 1 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1 + 1) {
        setState(167);
        matchWildcard(); 
      }
      setState(172);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Stmt_modeContext ------------------------------------------------------------------

configParser::Stmt_modeContext::Stmt_modeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Stmt_modeContext::STRING() {
  return getToken(configParser::STRING, 0);
}

std::vector<configParser::Stmt_mode_optionsContext *> configParser::Stmt_modeContext::stmt_mode_options() {
  return getRuleContexts<configParser::Stmt_mode_optionsContext>();
}

configParser::Stmt_mode_optionsContext* configParser::Stmt_modeContext::stmt_mode_options(size_t i) {
  return getRuleContext<configParser::Stmt_mode_optionsContext>(i);
}

std::vector<configParser::Stmt_mode_stmtsContext *> configParser::Stmt_modeContext::stmt_mode_stmts() {
  return getRuleContexts<configParser::Stmt_mode_stmtsContext>();
}

configParser::Stmt_mode_stmtsContext* configParser::Stmt_modeContext::stmt_mode_stmts(size_t i) {
  return getRuleContext<configParser::Stmt_mode_stmtsContext>(i);
}

std::vector<tree::TerminalNode *> configParser::Stmt_modeContext::EOL() {
  return getTokens(configParser::EOL);
}

tree::TerminalNode* configParser::Stmt_modeContext::EOL(size_t i) {
  return getToken(configParser::EOL, i);
}


size_t configParser::Stmt_modeContext::getRuleIndex() const {
  return configParser::RuleStmt_mode;
}

void configParser::Stmt_modeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt_mode(this);
}

void configParser::Stmt_modeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt_mode(this);
}

configParser::Stmt_modeContext* configParser::stmt_mode() {
  Stmt_modeContext *_localctx = _tracker.createInstance<Stmt_modeContext>(_ctx, getState());
  enterRule(_localctx, 6, configParser::RuleStmt_mode);
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
    setState(173);
    match(configParser::T__0);
    setState(177);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == configParser::T__3) {
      setState(174);
      stmt_mode_options();
      setState(179);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(180);
    match(configParser::STRING);
    setState(181);
    match(configParser::T__1);
    setState(183);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (((((_la - 87) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 87)) & ((1ULL << (configParser::T__86 - 87))
      | (1ULL << (configParser::T__89 - 87))
      | (1ULL << (configParser::T__90 - 87))
      | (1ULL << (configParser::T__91 - 87)))) != 0)) {
      setState(182);
      stmt_mode_stmts();
    }
    setState(189); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(185);
      match(configParser::EOL);
      setState(187);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (((((_la - 87) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 87)) & ((1ULL << (configParser::T__86 - 87))
        | (1ULL << (configParser::T__89 - 87))
        | (1ULL << (configParser::T__90 - 87))
        | (1ULL << (configParser::T__91 - 87)))) != 0)) {
        setState(186);
        stmt_mode_stmts();
      }
      setState(191); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == configParser::EOL);
    setState(193);
    match(configParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Stmt_mode_optionsContext ------------------------------------------------------------------

configParser::Stmt_mode_optionsContext::Stmt_mode_optionsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Stmt_mode_optionsContext::getRuleIndex() const {
  return configParser::RuleStmt_mode_options;
}

void configParser::Stmt_mode_optionsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt_mode_options(this);
}

void configParser::Stmt_mode_optionsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt_mode_options(this);
}

configParser::Stmt_mode_optionsContext* configParser::stmt_mode_options() {
  Stmt_mode_optionsContext *_localctx = _tracker.createInstance<Stmt_mode_optionsContext>(_ctx, getState());
  enterRule(_localctx, 8, configParser::RuleStmt_mode_options);

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
    match(configParser::T__3);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Stmt_mode_stmtsContext ------------------------------------------------------------------

configParser::Stmt_mode_stmtsContext::Stmt_mode_stmtsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::SetContext* configParser::Stmt_mode_stmtsContext::set() {
  return getRuleContext<configParser::SetContext>(0);
}

configParser::BindingContext* configParser::Stmt_mode_stmtsContext::binding() {
  return getRuleContext<configParser::BindingContext>(0);
}


size_t configParser::Stmt_mode_stmtsContext::getRuleIndex() const {
  return configParser::RuleStmt_mode_stmts;
}

void configParser::Stmt_mode_stmtsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStmt_mode_stmts(this);
}

void configParser::Stmt_mode_stmtsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStmt_mode_stmts(this);
}

configParser::Stmt_mode_stmtsContext* configParser::stmt_mode_stmts() {
  Stmt_mode_stmtsContext *_localctx = _tracker.createInstance<Stmt_mode_stmtsContext>(_ctx, getState());
  enterRule(_localctx, 10, configParser::RuleStmt_mode_stmts);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(199);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__86: {
        enterOuterAlt(_localctx, 1);
        setState(197);
        set();
        break;
      }

      case configParser::T__89:
      case configParser::T__90:
      case configParser::T__91: {
        enterOuterAlt(_localctx, 2);
        setState(198);
        binding();
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

//----------------- ColorContext ------------------------------------------------------------------

configParser::ColorContext::ColorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Color_singleContext* configParser::ColorContext::color_single() {
  return getRuleContext<configParser::Color_singleContext>(0);
}

configParser::Color_multipleContext* configParser::ColorContext::color_multiple() {
  return getRuleContext<configParser::Color_multipleContext>(0);
}


size_t configParser::ColorContext::getRuleIndex() const {
  return configParser::RuleColor;
}

void configParser::ColorContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColor(this);
}

void configParser::ColorContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColor(this);
}

configParser::ColorContext* configParser::color() {
  ColorContext *_localctx = _tracker.createInstance<ColorContext>(_ctx, getState());
  enterRule(_localctx, 12, configParser::RuleColor);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(203);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__4: {
        enterOuterAlt(_localctx, 1);
        setState(201);
        color_single();
        break;
      }

      case configParser::T__5:
      case configParser::T__6:
      case configParser::T__7:
      case configParser::T__8:
      case configParser::T__9: {
        enterOuterAlt(_localctx, 2);
        setState(202);
        color_multiple();
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

//----------------- Color_singleContext ------------------------------------------------------------------

configParser::Color_singleContext::Color_singleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Color_singleContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Color_singleContext::getRuleIndex() const {
  return configParser::RuleColor_single;
}

void configParser::Color_singleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColor_single(this);
}

void configParser::Color_singleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColor_single(this);
}

configParser::Color_singleContext* configParser::color_single() {
  Color_singleContext *_localctx = _tracker.createInstance<Color_singleContext>(_ctx, getState());
  enterRule(_localctx, 14, configParser::RuleColor_single);

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
    match(configParser::T__4);
    setState(206);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Color_multipleContext ------------------------------------------------------------------

configParser::Color_multipleContext::Color_multipleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> configParser::Color_multipleContext::STRING() {
  return getTokens(configParser::STRING);
}

tree::TerminalNode* configParser::Color_multipleContext::STRING(size_t i) {
  return getToken(configParser::STRING, i);
}


size_t configParser::Color_multipleContext::getRuleIndex() const {
  return configParser::RuleColor_multiple;
}

void configParser::Color_multipleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterColor_multiple(this);
}

void configParser::Color_multipleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitColor_multiple(this);
}

configParser::Color_multipleContext* configParser::color_multiple() {
  Color_multipleContext *_localctx = _tracker.createInstance<Color_multipleContext>(_ctx, getState());
  enterRule(_localctx, 16, configParser::RuleColor_multiple);
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
    setState(208);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__5)
      | (1ULL << configParser::T__6)
      | (1ULL << configParser::T__7)
      | (1ULL << configParser::T__8)
      | (1ULL << configParser::T__9))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(209);
    match(configParser::STRING);
    setState(210);
    match(configParser::STRING);
    setState(211);
    match(configParser::STRING);
    setState(213);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 11, _ctx)) {
    case 1: {
      setState(212);
      match(configParser::STRING);
      break;
    }

    default:
      break;
    }
    setState(216);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configParser::STRING) {
      setState(215);
      match(configParser::STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExecContext ------------------------------------------------------------------

configParser::ExecContext::ExecContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::ArgumentsContext* configParser::ExecContext::arguments() {
  return getRuleContext<configParser::ArgumentsContext>(0);
}

std::vector<configParser::Exec_optionsContext *> configParser::ExecContext::exec_options() {
  return getRuleContexts<configParser::Exec_optionsContext>();
}

configParser::Exec_optionsContext* configParser::ExecContext::exec_options(size_t i) {
  return getRuleContext<configParser::Exec_optionsContext>(i);
}


size_t configParser::ExecContext::getRuleIndex() const {
  return configParser::RuleExec;
}

void configParser::ExecContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExec(this);
}

void configParser::ExecContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExec(this);
}

configParser::ExecContext* configParser::exec() {
  ExecContext *_localctx = _tracker.createInstance<ExecContext>(_ctx, getState());
  enterRule(_localctx, 18, configParser::RuleExec);
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
    setState(218);
    _la = _input->LA(1);
    if (!(_la == configParser::T__10

    || _la == configParser::T__11)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(222);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(219);
        exec_options(); 
      }
      setState(224);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx);
    }
    setState(225);
    arguments();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Exec_optionsContext ------------------------------------------------------------------

configParser::Exec_optionsContext::Exec_optionsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Exec_optionsContext::getRuleIndex() const {
  return configParser::RuleExec_options;
}

void configParser::Exec_optionsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterExec_options(this);
}

void configParser::Exec_optionsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitExec_options(this);
}

configParser::Exec_optionsContext* configParser::exec_options() {
  Exec_optionsContext *_localctx = _tracker.createInstance<Exec_optionsContext>(_ctx, getState());
  enterRule(_localctx, 20, configParser::RuleExec_options);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(227);
    match(configParser::T__12);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Popup_during_fullscreenContext ------------------------------------------------------------------

configParser::Popup_during_fullscreenContext::Popup_during_fullscreenContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Popup_during_fullscreen_valueContext* configParser::Popup_during_fullscreenContext::popup_during_fullscreen_value() {
  return getRuleContext<configParser::Popup_during_fullscreen_valueContext>(0);
}


size_t configParser::Popup_during_fullscreenContext::getRuleIndex() const {
  return configParser::RulePopup_during_fullscreen;
}

void configParser::Popup_during_fullscreenContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPopup_during_fullscreen(this);
}

void configParser::Popup_during_fullscreenContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPopup_during_fullscreen(this);
}

configParser::Popup_during_fullscreenContext* configParser::popup_during_fullscreen() {
  Popup_during_fullscreenContext *_localctx = _tracker.createInstance<Popup_during_fullscreenContext>(_ctx, getState());
  enterRule(_localctx, 22, configParser::RulePopup_during_fullscreen);

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
    match(configParser::T__13);
    setState(230);
    popup_during_fullscreen_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Popup_during_fullscreen_valueContext ------------------------------------------------------------------

configParser::Popup_during_fullscreen_valueContext::Popup_during_fullscreen_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Popup_during_fullscreen_valueContext::getRuleIndex() const {
  return configParser::RulePopup_during_fullscreen_value;
}

void configParser::Popup_during_fullscreen_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPopup_during_fullscreen_value(this);
}

void configParser::Popup_during_fullscreen_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPopup_during_fullscreen_value(this);
}

configParser::Popup_during_fullscreen_valueContext* configParser::popup_during_fullscreen_value() {
  Popup_during_fullscreen_valueContext *_localctx = _tracker.createInstance<Popup_during_fullscreen_valueContext>(_ctx, getState());
  enterRule(_localctx, 24, configParser::RulePopup_during_fullscreen_value);
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
    setState(232);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__14)
      | (1ULL << configParser::T__15)
      | (1ULL << configParser::T__16))) != 0))) {
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

//----------------- Restart_stateContext ------------------------------------------------------------------

configParser::Restart_stateContext::Restart_stateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Restart_stateContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Restart_stateContext::getRuleIndex() const {
  return configParser::RuleRestart_state;
}

void configParser::Restart_stateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRestart_state(this);
}

void configParser::Restart_stateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRestart_state(this);
}

configParser::Restart_stateContext* configParser::restart_state() {
  Restart_stateContext *_localctx = _tracker.createInstance<Restart_stateContext>(_ctx, getState());
  enterRule(_localctx, 26, configParser::RuleRestart_state);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(234);
    match(configParser::T__17);
    setState(235);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ipc_kill_timeoutContext ------------------------------------------------------------------

configParser::Ipc_kill_timeoutContext::Ipc_kill_timeoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Ipc_kill_timeoutContext::NUMBER() {
  return getToken(configParser::NUMBER, 0);
}


size_t configParser::Ipc_kill_timeoutContext::getRuleIndex() const {
  return configParser::RuleIpc_kill_timeout;
}

void configParser::Ipc_kill_timeoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIpc_kill_timeout(this);
}

void configParser::Ipc_kill_timeoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIpc_kill_timeout(this);
}

configParser::Ipc_kill_timeoutContext* configParser::ipc_kill_timeout() {
  Ipc_kill_timeoutContext *_localctx = _tracker.createInstance<Ipc_kill_timeoutContext>(_ctx, getState());
  enterRule(_localctx, 28, configParser::RuleIpc_kill_timeout);

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
    match(configParser::T__18);
    setState(238);
    match(configParser::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Ipc_socketContext ------------------------------------------------------------------

configParser::Ipc_socketContext::Ipc_socketContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Ipc_socketContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Ipc_socketContext::getRuleIndex() const {
  return configParser::RuleIpc_socket;
}

void configParser::Ipc_socketContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterIpc_socket(this);
}

void configParser::Ipc_socketContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitIpc_socket(this);
}

configParser::Ipc_socketContext* configParser::ipc_socket() {
  Ipc_socketContext *_localctx = _tracker.createInstance<Ipc_socketContext>(_ctx, getState());
  enterRule(_localctx, 30, configParser::RuleIpc_socket);
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
    setState(240);
    _la = _input->LA(1);
    if (!(_la == configParser::T__19

    || _la == configParser::T__20)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(241);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- WorkspaceContext ------------------------------------------------------------------

configParser::WorkspaceContext::WorkspaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Workspace_nameContext* configParser::WorkspaceContext::workspace_name() {
  return getRuleContext<configParser::Workspace_nameContext>(0);
}

std::vector<tree::TerminalNode *> configParser::WorkspaceContext::STRING() {
  return getTokens(configParser::STRING);
}

tree::TerminalNode* configParser::WorkspaceContext::STRING(size_t i) {
  return getToken(configParser::STRING, i);
}


size_t configParser::WorkspaceContext::getRuleIndex() const {
  return configParser::RuleWorkspace;
}

void configParser::WorkspaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace(this);
}

void configParser::WorkspaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace(this);
}

configParser::WorkspaceContext* configParser::workspace() {
  WorkspaceContext *_localctx = _tracker.createInstance<WorkspaceContext>(_ctx, getState());
  enterRule(_localctx, 32, configParser::RuleWorkspace);
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
    setState(243);
    match(configParser::T__21);
    setState(244);
    workspace_name();
    setState(245);
    match(configParser::T__22);
    setState(247); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(246);
      match(configParser::STRING);
      setState(249); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Workspace_nameContext ------------------------------------------------------------------

configParser::Workspace_nameContext::Workspace_nameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Workspace_nameContext::NUMBER() {
  return getToken(configParser::NUMBER, 0);
}

tree::TerminalNode* configParser::Workspace_nameContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Workspace_nameContext::getRuleIndex() const {
  return configParser::RuleWorkspace_name;
}

void configParser::Workspace_nameContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_name(this);
}

void configParser::Workspace_nameContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_name(this);
}

configParser::Workspace_nameContext* configParser::workspace_name() {
  Workspace_nameContext *_localctx = _tracker.createInstance<Workspace_nameContext>(_ctx, getState());
  enterRule(_localctx, 34, configParser::RuleWorkspace_name);
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
    setState(251);
    _la = _input->LA(1);
    if (!(_la == configParser::STRING

    || _la == configParser::NUMBER)) {
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

//----------------- Title_alignContext ------------------------------------------------------------------

configParser::Title_alignContext::Title_alignContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Title_align_valueContext* configParser::Title_alignContext::title_align_value() {
  return getRuleContext<configParser::Title_align_valueContext>(0);
}


size_t configParser::Title_alignContext::getRuleIndex() const {
  return configParser::RuleTitle_align;
}

void configParser::Title_alignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTitle_align(this);
}

void configParser::Title_alignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTitle_align(this);
}

configParser::Title_alignContext* configParser::title_align() {
  Title_alignContext *_localctx = _tracker.createInstance<Title_alignContext>(_ctx, getState());
  enterRule(_localctx, 36, configParser::RuleTitle_align);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(253);
    match(configParser::T__23);
    setState(254);
    title_align_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Title_align_valueContext ------------------------------------------------------------------

configParser::Title_align_valueContext::Title_align_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Title_align_valueContext::getRuleIndex() const {
  return configParser::RuleTitle_align_value;
}

void configParser::Title_align_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTitle_align_value(this);
}

void configParser::Title_align_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTitle_align_value(this);
}

configParser::Title_align_valueContext* configParser::title_align_value() {
  Title_align_valueContext *_localctx = _tracker.createInstance<Title_align_valueContext>(_ctx, getState());
  enterRule(_localctx, 38, configParser::RuleTitle_align_value);
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
    setState(256);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__24)
      | (1ULL << configParser::T__25)
      | (1ULL << configParser::T__26))) != 0))) {
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

//----------------- Focus_on_window_activationContext ------------------------------------------------------------------

configParser::Focus_on_window_activationContext::Focus_on_window_activationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Focus_on_window_activation_valueContext* configParser::Focus_on_window_activationContext::focus_on_window_activation_value() {
  return getRuleContext<configParser::Focus_on_window_activation_valueContext>(0);
}


size_t configParser::Focus_on_window_activationContext::getRuleIndex() const {
  return configParser::RuleFocus_on_window_activation;
}

void configParser::Focus_on_window_activationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_on_window_activation(this);
}

void configParser::Focus_on_window_activationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_on_window_activation(this);
}

configParser::Focus_on_window_activationContext* configParser::focus_on_window_activation() {
  Focus_on_window_activationContext *_localctx = _tracker.createInstance<Focus_on_window_activationContext>(_ctx, getState());
  enterRule(_localctx, 40, configParser::RuleFocus_on_window_activation);

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
    match(configParser::T__27);
    setState(259);
    focus_on_window_activation_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Focus_on_window_activation_valueContext ------------------------------------------------------------------

configParser::Focus_on_window_activation_valueContext::Focus_on_window_activation_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Focus_on_window_activation_valueContext::getRuleIndex() const {
  return configParser::RuleFocus_on_window_activation_value;
}

void configParser::Focus_on_window_activation_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_on_window_activation_value(this);
}

void configParser::Focus_on_window_activation_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_on_window_activation_value(this);
}

configParser::Focus_on_window_activation_valueContext* configParser::focus_on_window_activation_value() {
  Focus_on_window_activation_valueContext *_localctx = _tracker.createInstance<Focus_on_window_activation_valueContext>(_ctx, getState());
  enterRule(_localctx, 42, configParser::RuleFocus_on_window_activation_value);
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
    setState(261);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__16)
      | (1ULL << configParser::T__28)
      | (1ULL << configParser::T__29)
      | (1ULL << configParser::T__30))) != 0))) {
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

//----------------- Force_display_urgency_hintContext ------------------------------------------------------------------

configParser::Force_display_urgency_hintContext::Force_display_urgency_hintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Force_display_urgency_hintContext::NUMBER() {
  return getToken(configParser::NUMBER, 0);
}


size_t configParser::Force_display_urgency_hintContext::getRuleIndex() const {
  return configParser::RuleForce_display_urgency_hint;
}

void configParser::Force_display_urgency_hintContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForce_display_urgency_hint(this);
}

void configParser::Force_display_urgency_hintContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForce_display_urgency_hint(this);
}

configParser::Force_display_urgency_hintContext* configParser::force_display_urgency_hint() {
  Force_display_urgency_hintContext *_localctx = _tracker.createInstance<Force_display_urgency_hintContext>(_ctx, getState());
  enterRule(_localctx, 44, configParser::RuleForce_display_urgency_hint);
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
    setState(263);
    match(configParser::T__31);
    setState(264);
    match(configParser::NUMBER);
    setState(266);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configParser::T__32) {
      setState(265);
      match(configParser::T__32);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Workspace_auto_back_and_forthContext ------------------------------------------------------------------

configParser::Workspace_auto_back_and_forthContext::Workspace_auto_back_and_forthContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Workspace_auto_back_and_forthContext::BOOL() {
  return getToken(configParser::BOOL, 0);
}


size_t configParser::Workspace_auto_back_and_forthContext::getRuleIndex() const {
  return configParser::RuleWorkspace_auto_back_and_forth;
}

void configParser::Workspace_auto_back_and_forthContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_auto_back_and_forth(this);
}

void configParser::Workspace_auto_back_and_forthContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_auto_back_and_forth(this);
}

configParser::Workspace_auto_back_and_forthContext* configParser::workspace_auto_back_and_forth() {
  Workspace_auto_back_and_forthContext *_localctx = _tracker.createInstance<Workspace_auto_back_and_forthContext>(_ctx, getState());
  enterRule(_localctx, 46, configParser::RuleWorkspace_auto_back_and_forth);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(268);
    match(configParser::T__33);
    setState(269);
    match(configParser::BOOL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Focus_wrappingContext ------------------------------------------------------------------

configParser::Focus_wrappingContext::Focus_wrappingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Focus_wrappingContext::BOOL() {
  return getToken(configParser::BOOL, 0);
}


size_t configParser::Focus_wrappingContext::getRuleIndex() const {
  return configParser::RuleFocus_wrapping;
}

void configParser::Focus_wrappingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_wrapping(this);
}

void configParser::Focus_wrappingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_wrapping(this);
}

configParser::Focus_wrappingContext* configParser::focus_wrapping() {
  Focus_wrappingContext *_localctx = _tracker.createInstance<Focus_wrappingContext>(_ctx, getState());
  enterRule(_localctx, 48, configParser::RuleFocus_wrapping);
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
    setState(271);
    match(configParser::T__34);
    setState(272);
    _la = _input->LA(1);
    if (!(_la == configParser::T__21

    || _la == configParser::T__35 || _la == configParser::BOOL)) {
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

//----------------- Force_focus_wrappingContext ------------------------------------------------------------------

configParser::Force_focus_wrappingContext::Force_focus_wrappingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Force_focus_wrappingContext::BOOL() {
  return getToken(configParser::BOOL, 0);
}


size_t configParser::Force_focus_wrappingContext::getRuleIndex() const {
  return configParser::RuleForce_focus_wrapping;
}

void configParser::Force_focus_wrappingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterForce_focus_wrapping(this);
}

void configParser::Force_focus_wrappingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitForce_focus_wrapping(this);
}

configParser::Force_focus_wrappingContext* configParser::force_focus_wrapping() {
  Force_focus_wrappingContext *_localctx = _tracker.createInstance<Force_focus_wrappingContext>(_ctx, getState());
  enterRule(_localctx, 50, configParser::RuleForce_focus_wrapping);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(274);
    match(configParser::T__36);
    setState(275);
    match(configParser::BOOL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Mouse_warpingContext ------------------------------------------------------------------

configParser::Mouse_warpingContext::Mouse_warpingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Mouse_warping_valueContext* configParser::Mouse_warpingContext::mouse_warping_value() {
  return getRuleContext<configParser::Mouse_warping_valueContext>(0);
}


size_t configParser::Mouse_warpingContext::getRuleIndex() const {
  return configParser::RuleMouse_warping;
}

void configParser::Mouse_warpingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMouse_warping(this);
}

void configParser::Mouse_warpingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMouse_warping(this);
}

configParser::Mouse_warpingContext* configParser::mouse_warping() {
  Mouse_warpingContext *_localctx = _tracker.createInstance<Mouse_warpingContext>(_ctx, getState());
  enterRule(_localctx, 52, configParser::RuleMouse_warping);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(277);
    match(configParser::T__37);
    setState(278);
    mouse_warping_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Mouse_warping_valueContext ------------------------------------------------------------------

configParser::Mouse_warping_valueContext::Mouse_warping_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Mouse_warping_valueContext::getRuleIndex() const {
  return configParser::RuleMouse_warping_value;
}

void configParser::Mouse_warping_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMouse_warping_value(this);
}

void configParser::Mouse_warping_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMouse_warping_value(this);
}

configParser::Mouse_warping_valueContext* configParser::mouse_warping_value() {
  Mouse_warping_valueContext *_localctx = _tracker.createInstance<Mouse_warping_valueContext>(_ctx, getState());
  enterRule(_localctx, 54, configParser::RuleMouse_warping_value);
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
    setState(280);
    _la = _input->LA(1);
    if (!(_la == configParser::T__22

    || _la == configParser::T__30)) {
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

//----------------- Focus_follows_mouseContext ------------------------------------------------------------------

configParser::Focus_follows_mouseContext::Focus_follows_mouseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Focus_follows_mouseContext::BOOL() {
  return getToken(configParser::BOOL, 0);
}


size_t configParser::Focus_follows_mouseContext::getRuleIndex() const {
  return configParser::RuleFocus_follows_mouse;
}

void configParser::Focus_follows_mouseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFocus_follows_mouse(this);
}

void configParser::Focus_follows_mouseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFocus_follows_mouse(this);
}

configParser::Focus_follows_mouseContext* configParser::focus_follows_mouse() {
  Focus_follows_mouseContext *_localctx = _tracker.createInstance<Focus_follows_mouseContext>(_ctx, getState());
  enterRule(_localctx, 56, configParser::RuleFocus_follows_mouse);

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
    match(configParser::T__38);
    setState(283);
    match(configParser::BOOL);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- No_focusContext ------------------------------------------------------------------

configParser::No_focusContext::No_focusContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::CriteriaContext* configParser::No_focusContext::criteria() {
  return getRuleContext<configParser::CriteriaContext>(0);
}


size_t configParser::No_focusContext::getRuleIndex() const {
  return configParser::RuleNo_focus;
}

void configParser::No_focusContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNo_focus(this);
}

void configParser::No_focusContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNo_focus(this);
}

configParser::No_focusContext* configParser::no_focus() {
  No_focusContext *_localctx = _tracker.createInstance<No_focusContext>(_ctx, getState());
  enterRule(_localctx, 58, configParser::RuleNo_focus);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(285);
    match(configParser::T__39);
    setState(286);
    criteria();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssignContext ------------------------------------------------------------------

configParser::AssignContext::AssignContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::CriteriaContext* configParser::AssignContext::criteria() {
  return getRuleContext<configParser::CriteriaContext>(0);
}

configParser::Assign_targetContext* configParser::AssignContext::assign_target() {
  return getRuleContext<configParser::Assign_targetContext>(0);
}


size_t configParser::AssignContext::getRuleIndex() const {
  return configParser::RuleAssign;
}

void configParser::AssignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign(this);
}

void configParser::AssignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign(this);
}

configParser::AssignContext* configParser::assign() {
  AssignContext *_localctx = _tracker.createInstance<AssignContext>(_ctx, getState());
  enterRule(_localctx, 60, configParser::RuleAssign);
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
    setState(288);
    match(configParser::T__40);
    setState(289);
    criteria();
    setState(291);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configParser::T__41) {
      setState(290);
      match(configParser::T__41);
    }
    setState(293);
    assign_target();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Assign_targetContext ------------------------------------------------------------------

configParser::Assign_targetContext::Assign_targetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Assign_target_workspaceContext* configParser::Assign_targetContext::assign_target_workspace() {
  return getRuleContext<configParser::Assign_target_workspaceContext>(0);
}

configParser::Assign_target_numberContext* configParser::Assign_targetContext::assign_target_number() {
  return getRuleContext<configParser::Assign_target_numberContext>(0);
}

configParser::Assign_target_outputContext* configParser::Assign_targetContext::assign_target_output() {
  return getRuleContext<configParser::Assign_target_outputContext>(0);
}


size_t configParser::Assign_targetContext::getRuleIndex() const {
  return configParser::RuleAssign_target;
}

void configParser::Assign_targetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign_target(this);
}

void configParser::Assign_targetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign_target(this);
}

configParser::Assign_targetContext* configParser::assign_target() {
  Assign_targetContext *_localctx = _tracker.createInstance<Assign_targetContext>(_ctx, getState());
  enterRule(_localctx, 62, configParser::RuleAssign_target);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(298);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__21: {
        enterOuterAlt(_localctx, 1);
        setState(295);
        assign_target_workspace();
        break;
      }

      case configParser::T__42: {
        enterOuterAlt(_localctx, 2);
        setState(296);
        assign_target_number();
        break;
      }

      case configParser::T__22: {
        enterOuterAlt(_localctx, 3);
        setState(297);
        assign_target_output();
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

//----------------- Assign_target_workspaceContext ------------------------------------------------------------------

configParser::Assign_target_workspaceContext::Assign_target_workspaceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Assign_target_workspaceContext::STRING() {
  return getToken(configParser::STRING, 0);
}

tree::TerminalNode* configParser::Assign_target_workspaceContext::NUMBER() {
  return getToken(configParser::NUMBER, 0);
}


size_t configParser::Assign_target_workspaceContext::getRuleIndex() const {
  return configParser::RuleAssign_target_workspace;
}

void configParser::Assign_target_workspaceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign_target_workspace(this);
}

void configParser::Assign_target_workspaceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign_target_workspace(this);
}

configParser::Assign_target_workspaceContext* configParser::assign_target_workspace() {
  Assign_target_workspaceContext *_localctx = _tracker.createInstance<Assign_target_workspaceContext>(_ctx, getState());
  enterRule(_localctx, 64, configParser::RuleAssign_target_workspace);
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
    setState(300);
    match(configParser::T__21);
    setState(301);
    _la = _input->LA(1);
    if (!(_la == configParser::STRING

    || _la == configParser::NUMBER)) {
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

//----------------- Assign_target_numberContext ------------------------------------------------------------------

configParser::Assign_target_numberContext::Assign_target_numberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Assign_target_numberContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Assign_target_numberContext::getRuleIndex() const {
  return configParser::RuleAssign_target_number;
}

void configParser::Assign_target_numberContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign_target_number(this);
}

void configParser::Assign_target_numberContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign_target_number(this);
}

configParser::Assign_target_numberContext* configParser::assign_target_number() {
  Assign_target_numberContext *_localctx = _tracker.createInstance<Assign_target_numberContext>(_ctx, getState());
  enterRule(_localctx, 66, configParser::RuleAssign_target_number);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(303);
    match(configParser::T__42);
    setState(304);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Assign_target_outputContext ------------------------------------------------------------------

configParser::Assign_target_outputContext::Assign_target_outputContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Assign_target_outputContext::getRuleIndex() const {
  return configParser::RuleAssign_target_output;
}

void configParser::Assign_target_outputContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign_target_output(this);
}

void configParser::Assign_target_outputContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign_target_output(this);
}

configParser::Assign_target_outputContext* configParser::assign_target_output() {
  Assign_target_outputContext *_localctx = _tracker.createInstance<Assign_target_outputContext>(_ctx, getState());
  enterRule(_localctx, 68, configParser::RuleAssign_target_output);
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
    setState(306);
    match(configParser::T__22);
    setState(307);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__24)
      | (1ULL << configParser::T__26)
      | (1ULL << configParser::T__43)
      | (1ULL << configParser::T__44)
      | (1ULL << configParser::T__45))) != 0))) {
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

//----------------- For_windowContext ------------------------------------------------------------------

configParser::For_windowContext::For_windowContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::CriteriaContext* configParser::For_windowContext::criteria() {
  return getRuleContext<configParser::CriteriaContext>(0);
}

configParser::ArgumentsContext* configParser::For_windowContext::arguments() {
  return getRuleContext<configParser::ArgumentsContext>(0);
}


size_t configParser::For_windowContext::getRuleIndex() const {
  return configParser::RuleFor_window;
}

void configParser::For_windowContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFor_window(this);
}

void configParser::For_windowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFor_window(this);
}

configParser::For_windowContext* configParser::for_window() {
  For_windowContext *_localctx = _tracker.createInstance<For_windowContext>(_ctx, getState());
  enterRule(_localctx, 70, configParser::RuleFor_window);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(309);
    match(configParser::T__46);
    setState(310);
    criteria();
    setState(311);
    arguments();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CriteriaContext ------------------------------------------------------------------

configParser::CriteriaContext::CriteriaContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<configParser::CriterionContext *> configParser::CriteriaContext::criterion() {
  return getRuleContexts<configParser::CriterionContext>();
}

configParser::CriterionContext* configParser::CriteriaContext::criterion(size_t i) {
  return getRuleContext<configParser::CriterionContext>(i);
}


size_t configParser::CriteriaContext::getRuleIndex() const {
  return configParser::RuleCriteria;
}

void configParser::CriteriaContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCriteria(this);
}

void configParser::CriteriaContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCriteria(this);
}

configParser::CriteriaContext* configParser::criteria() {
  CriteriaContext *_localctx = _tracker.createInstance<CriteriaContext>(_ctx, getState());
  enterRule(_localctx, 72, configParser::RuleCriteria);
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
    setState(313);
    match(configParser::T__47);
    setState(315); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(314);
      criterion();
      setState(317); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__21)
      | (1ULL << configParser::T__28)
      | (1ULL << configParser::T__49)
      | (1ULL << configParser::T__50)
      | (1ULL << configParser::T__51)
      | (1ULL << configParser::T__52)
      | (1ULL << configParser::T__53)
      | (1ULL << configParser::T__54)
      | (1ULL << configParser::T__55)
      | (1ULL << configParser::T__56)
      | (1ULL << configParser::T__57)
      | (1ULL << configParser::T__58)
      | (1ULL << configParser::T__59)
      | (1ULL << configParser::T__60)
      | (1ULL << configParser::T__61))) != 0));
    setState(319);
    match(configParser::T__48);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CriterionContext ------------------------------------------------------------------

configParser::CriterionContext::CriterionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Value_criterionContext* configParser::CriterionContext::value_criterion() {
  return getRuleContext<configParser::Value_criterionContext>(0);
}


size_t configParser::CriterionContext::getRuleIndex() const {
  return configParser::RuleCriterion;
}

void configParser::CriterionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCriterion(this);
}

void configParser::CriterionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCriterion(this);
}

configParser::CriterionContext* configParser::criterion() {
  CriterionContext *_localctx = _tracker.createInstance<CriterionContext>(_ctx, getState());
  enterRule(_localctx, 74, configParser::RuleCriterion);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(325);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__21:
      case configParser::T__28:
      case configParser::T__52:
      case configParser::T__53:
      case configParser::T__54:
      case configParser::T__55:
      case configParser::T__56:
      case configParser::T__57:
      case configParser::T__58:
      case configParser::T__59:
      case configParser::T__60:
      case configParser::T__61: {
        enterOuterAlt(_localctx, 1);
        setState(321);
        value_criterion();
        break;
      }

      case configParser::T__49: {
        enterOuterAlt(_localctx, 2);
        setState(322);
        match(configParser::T__49);
        break;
      }

      case configParser::T__50: {
        enterOuterAlt(_localctx, 3);
        setState(323);
        match(configParser::T__50);
        break;
      }

      case configParser::T__51: {
        enterOuterAlt(_localctx, 4);
        setState(324);
        match(configParser::T__51);
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

configParser::Value_criterionContext::Value_criterionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Value_criterionContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::Value_criterionContext::getRuleIndex() const {
  return configParser::RuleValue_criterion;
}

void configParser::Value_criterionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterValue_criterion(this);
}

void configParser::Value_criterionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitValue_criterion(this);
}

configParser::Value_criterionContext* configParser::value_criterion() {
  Value_criterionContext *_localctx = _tracker.createInstance<Value_criterionContext>(_ctx, getState());
  enterRule(_localctx, 76, configParser::RuleValue_criterion);
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
    setState(327);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << configParser::T__21)
      | (1ULL << configParser::T__28)
      | (1ULL << configParser::T__52)
      | (1ULL << configParser::T__53)
      | (1ULL << configParser::T__54)
      | (1ULL << configParser::T__55)
      | (1ULL << configParser::T__56)
      | (1ULL << configParser::T__57)
      | (1ULL << configParser::T__58)
      | (1ULL << configParser::T__59)
      | (1ULL << configParser::T__60)
      | (1ULL << configParser::T__61))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(328);
    match(configParser::T__62);
    setState(329);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Hide_edge_bordersContext ------------------------------------------------------------------

configParser::Hide_edge_bordersContext::Hide_edge_bordersContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Border_typeContext* configParser::Hide_edge_bordersContext::border_type() {
  return getRuleContext<configParser::Border_typeContext>(0);
}


size_t configParser::Hide_edge_bordersContext::getRuleIndex() const {
  return configParser::RuleHide_edge_borders;
}

void configParser::Hide_edge_bordersContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHide_edge_borders(this);
}

void configParser::Hide_edge_bordersContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHide_edge_borders(this);
}

configParser::Hide_edge_bordersContext* configParser::hide_edge_borders() {
  Hide_edge_bordersContext *_localctx = _tracker.createInstance<Hide_edge_bordersContext>(_ctx, getState());
  enterRule(_localctx, 78, configParser::RuleHide_edge_borders);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(331);
    match(configParser::T__63);
    setState(332);
    border_type();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Border_typeContext ------------------------------------------------------------------

configParser::Border_typeContext::Border_typeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Border_typeContext::BOOL() {
  return getToken(configParser::BOOL, 0);
}


size_t configParser::Border_typeContext::getRuleIndex() const {
  return configParser::RuleBorder_type;
}

void configParser::Border_typeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBorder_type(this);
}

void configParser::Border_typeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBorder_type(this);
}

configParser::Border_typeContext* configParser::border_type() {
  Border_typeContext *_localctx = _tracker.createInstance<Border_typeContext>(_ctx, getState());
  enterRule(_localctx, 80, configParser::RuleBorder_type);
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
    setState(334);
    _la = _input->LA(1);
    if (!(_la == configParser::T__16

    || _la == configParser::T__30 || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & ((1ULL << (configParser::T__64 - 65))
      | (1ULL << (configParser::T__65 - 65))
      | (1ULL << (configParser::T__66 - 65))
      | (1ULL << (configParser::BOOL - 65)))) != 0))) {
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

//----------------- Default_borderContext ------------------------------------------------------------------

configParser::Default_borderContext::Default_borderContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Default_border_typeContext* configParser::Default_borderContext::default_border_type() {
  return getRuleContext<configParser::Default_border_typeContext>(0);
}

configParser::Border_styleContext* configParser::Default_borderContext::border_style() {
  return getRuleContext<configParser::Border_styleContext>(0);
}


size_t configParser::Default_borderContext::getRuleIndex() const {
  return configParser::RuleDefault_border;
}

void configParser::Default_borderContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_border(this);
}

void configParser::Default_borderContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_border(this);
}

configParser::Default_borderContext* configParser::default_border() {
  Default_borderContext *_localctx = _tracker.createInstance<Default_borderContext>(_ctx, getState());
  enterRule(_localctx, 82, configParser::RuleDefault_border);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(336);
    default_border_type();
    setState(337);
    border_style();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Default_border_typeContext ------------------------------------------------------------------

configParser::Default_border_typeContext::Default_border_typeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Default_border_typeContext::getRuleIndex() const {
  return configParser::RuleDefault_border_type;
}

void configParser::Default_border_typeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_border_type(this);
}

void configParser::Default_border_typeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_border_type(this);
}

configParser::Default_border_typeContext* configParser::default_border_type() {
  Default_border_typeContext *_localctx = _tracker.createInstance<Default_border_typeContext>(_ctx, getState());
  enterRule(_localctx, 84, configParser::RuleDefault_border_type);
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
    setState(339);
    _la = _input->LA(1);
    if (!(((((_la - 68) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 68)) & ((1ULL << (configParser::T__67 - 68))
      | (1ULL << (configParser::T__68 - 68))
      | (1ULL << (configParser::T__69 - 68))
      | (1ULL << (configParser::T__70 - 68)))) != 0))) {
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

//----------------- Border_styleContext ------------------------------------------------------------------

configParser::Border_styleContext::Border_styleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Border_styleContext::NUMBER() {
  return getToken(configParser::NUMBER, 0);
}


size_t configParser::Border_styleContext::getRuleIndex() const {
  return configParser::RuleBorder_style;
}

void configParser::Border_styleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBorder_style(this);
}

void configParser::Border_styleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBorder_style(this);
}

configParser::Border_styleContext* configParser::border_style() {
  Border_styleContext *_localctx = _tracker.createInstance<Border_styleContext>(_ctx, getState());
  enterRule(_localctx, 86, configParser::RuleBorder_style);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(345);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__71:
      case configParser::T__72: {
        enterOuterAlt(_localctx, 1);
        setState(341);
        _la = _input->LA(1);
        if (!(_la == configParser::T__71

        || _la == configParser::T__72)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(342);
        match(configParser::NUMBER);
        break;
      }

      case configParser::T__30: {
        enterOuterAlt(_localctx, 2);
        setState(343);
        match(configParser::T__30);
        break;
      }

      case configParser::T__73: {
        enterOuterAlt(_localctx, 3);
        setState(344);
        match(configParser::T__73);
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

//----------------- Workspace_layoutContext ------------------------------------------------------------------

configParser::Workspace_layoutContext::Workspace_layoutContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Workspace_layout_valueContext* configParser::Workspace_layoutContext::workspace_layout_value() {
  return getRuleContext<configParser::Workspace_layout_valueContext>(0);
}


size_t configParser::Workspace_layoutContext::getRuleIndex() const {
  return configParser::RuleWorkspace_layout;
}

void configParser::Workspace_layoutContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_layout(this);
}

void configParser::Workspace_layoutContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_layout(this);
}

configParser::Workspace_layoutContext* configParser::workspace_layout() {
  Workspace_layoutContext *_localctx = _tracker.createInstance<Workspace_layoutContext>(_ctx, getState());
  enterRule(_localctx, 88, configParser::RuleWorkspace_layout);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(347);
    match(configParser::T__74);
    setState(348);
    workspace_layout_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Workspace_layout_valueContext ------------------------------------------------------------------

configParser::Workspace_layout_valueContext::Workspace_layout_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Workspace_layout_valueContext::getRuleIndex() const {
  return configParser::RuleWorkspace_layout_value;
}

void configParser::Workspace_layout_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterWorkspace_layout_value(this);
}

void configParser::Workspace_layout_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitWorkspace_layout_value(this);
}

configParser::Workspace_layout_valueContext* configParser::workspace_layout_value() {
  Workspace_layout_valueContext *_localctx = _tracker.createInstance<Workspace_layout_valueContext>(_ctx, getState());
  enterRule(_localctx, 90, configParser::RuleWorkspace_layout_value);
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
    setState(350);
    _la = _input->LA(1);
    if (!(((((_la - 76) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 76)) & ((1ULL << (configParser::T__75 - 76))
      | (1ULL << (configParser::T__76 - 76))
      | (1ULL << (configParser::T__77 - 76))
      | (1ULL << (configParser::T__78 - 76)))) != 0))) {
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

//----------------- Default_orientationContext ------------------------------------------------------------------

configParser::Default_orientationContext::Default_orientationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::Default_orientation_valueContext* configParser::Default_orientationContext::default_orientation_value() {
  return getRuleContext<configParser::Default_orientation_valueContext>(0);
}


size_t configParser::Default_orientationContext::getRuleIndex() const {
  return configParser::RuleDefault_orientation;
}

void configParser::Default_orientationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_orientation(this);
}

void configParser::Default_orientationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_orientation(this);
}

configParser::Default_orientationContext* configParser::default_orientation() {
  Default_orientationContext *_localctx = _tracker.createInstance<Default_orientationContext>(_ctx, getState());
  enterRule(_localctx, 92, configParser::RuleDefault_orientation);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(352);
    match(configParser::T__79);
    setState(353);
    default_orientation_value();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Default_orientation_valueContext ------------------------------------------------------------------

configParser::Default_orientation_valueContext::Default_orientation_valueContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Default_orientation_valueContext::getRuleIndex() const {
  return configParser::RuleDefault_orientation_value;
}

void configParser::Default_orientation_valueContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefault_orientation_value(this);
}

void configParser::Default_orientation_valueContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefault_orientation_value(this);
}

configParser::Default_orientation_valueContext* configParser::default_orientation_value() {
  Default_orientation_valueContext *_localctx = _tracker.createInstance<Default_orientation_valueContext>(_ctx, getState());
  enterRule(_localctx, 94, configParser::RuleDefault_orientation_value);
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
    setState(355);
    _la = _input->LA(1);
    if (!(((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & ((1ULL << (configParser::T__64 - 65))
      | (1ULL << (configParser::T__65 - 65))
      | (1ULL << (configParser::T__80 - 65)))) != 0))) {
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

//----------------- Floating_modifierContext ------------------------------------------------------------------

configParser::Floating_modifierContext::Floating_modifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<configParser::ModifierContext *> configParser::Floating_modifierContext::modifier() {
  return getRuleContexts<configParser::ModifierContext>();
}

configParser::ModifierContext* configParser::Floating_modifierContext::modifier(size_t i) {
  return getRuleContext<configParser::ModifierContext>(i);
}


size_t configParser::Floating_modifierContext::getRuleIndex() const {
  return configParser::RuleFloating_modifier;
}

void configParser::Floating_modifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_modifier(this);
}

void configParser::Floating_modifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_modifier(this);
}

configParser::Floating_modifierContext* configParser::floating_modifier() {
  Floating_modifierContext *_localctx = _tracker.createInstance<Floating_modifierContext>(_ctx, getState());
  enterRule(_localctx, 96, configParser::RuleFloating_modifier);
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
    setState(357);
    match(configParser::T__81);
    setState(359); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(358);
      modifier(0);
      setState(361); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (((((_la - 98) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 98)) & ((1ULL << (configParser::T__97 - 98))
      | (1ULL << (configParser::T__98 - 98))
      | (1ULL << (configParser::T__99 - 98))
      | (1ULL << (configParser::T__100 - 98))
      | (1ULL << (configParser::T__101 - 98))
      | (1ULL << (configParser::T__102 - 98))
      | (1ULL << (configParser::T__103 - 98))
      | (1ULL << (configParser::T__104 - 98))
      | (1ULL << (configParser::T__105 - 98))
      | (1ULL << (configParser::T__106 - 98))
      | (1ULL << (configParser::T__107 - 98))
      | (1ULL << (configParser::T__108 - 98))
      | (1ULL << (configParser::T__109 - 98))
      | (1ULL << (configParser::VARIABLE - 98)))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Floating_minimum_sizeContext ------------------------------------------------------------------

configParser::Floating_minimum_sizeContext::Floating_minimum_sizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::DimensionContext* configParser::Floating_minimum_sizeContext::dimension() {
  return getRuleContext<configParser::DimensionContext>(0);
}


size_t configParser::Floating_minimum_sizeContext::getRuleIndex() const {
  return configParser::RuleFloating_minimum_size;
}

void configParser::Floating_minimum_sizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_minimum_size(this);
}

void configParser::Floating_minimum_sizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_minimum_size(this);
}

configParser::Floating_minimum_sizeContext* configParser::floating_minimum_size() {
  Floating_minimum_sizeContext *_localctx = _tracker.createInstance<Floating_minimum_sizeContext>(_ctx, getState());
  enterRule(_localctx, 98, configParser::RuleFloating_minimum_size);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(363);
    match(configParser::T__82);
    setState(364);
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

configParser::Floating_maximum_sizeContext::Floating_maximum_sizeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::DimensionContext* configParser::Floating_maximum_sizeContext::dimension() {
  return getRuleContext<configParser::DimensionContext>(0);
}


size_t configParser::Floating_maximum_sizeContext::getRuleIndex() const {
  return configParser::RuleFloating_maximum_size;
}

void configParser::Floating_maximum_sizeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFloating_maximum_size(this);
}

void configParser::Floating_maximum_sizeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFloating_maximum_size(this);
}

configParser::Floating_maximum_sizeContext* configParser::floating_maximum_size() {
  Floating_maximum_sizeContext *_localctx = _tracker.createInstance<Floating_maximum_sizeContext>(_ctx, getState());
  enterRule(_localctx, 100, configParser::RuleFloating_maximum_size);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(366);
    match(configParser::T__83);
    setState(367);
    dimension();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DimensionContext ------------------------------------------------------------------

configParser::DimensionContext::DimensionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> configParser::DimensionContext::NUMBER() {
  return getTokens(configParser::NUMBER);
}

tree::TerminalNode* configParser::DimensionContext::NUMBER(size_t i) {
  return getToken(configParser::NUMBER, i);
}


size_t configParser::DimensionContext::getRuleIndex() const {
  return configParser::RuleDimension;
}

void configParser::DimensionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDimension(this);
}

void configParser::DimensionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDimension(this);
}

configParser::DimensionContext* configParser::dimension() {
  DimensionContext *_localctx = _tracker.createInstance<DimensionContext>(_ctx, getState());
  enterRule(_localctx, 102, configParser::RuleDimension);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(369);
    match(configParser::NUMBER);
    setState(370);
    match(configParser::T__84);
    setState(371);
    match(configParser::NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FontContext ------------------------------------------------------------------

configParser::FontContext::FontContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::ArgumentsContext* configParser::FontContext::arguments() {
  return getRuleContext<configParser::ArgumentsContext>(0);
}


size_t configParser::FontContext::getRuleIndex() const {
  return configParser::RuleFont;
}

void configParser::FontContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFont(this);
}

void configParser::FontContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFont(this);
}

configParser::FontContext* configParser::font() {
  FontContext *_localctx = _tracker.createInstance<FontContext>(_ctx, getState());
  enterRule(_localctx, 104, configParser::RuleFont);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(373);
    match(configParser::T__85);
    setState(374);
    arguments();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SetContext ------------------------------------------------------------------

configParser::SetContext::SetContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::SetContext::VARIABLE() {
  return getToken(configParser::VARIABLE, 0);
}

configParser::ArgumentsContext* configParser::SetContext::arguments() {
  return getRuleContext<configParser::ArgumentsContext>(0);
}


size_t configParser::SetContext::getRuleIndex() const {
  return configParser::RuleSet;
}

void configParser::SetContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSet(this);
}

void configParser::SetContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSet(this);
}

configParser::SetContext* configParser::set() {
  SetContext *_localctx = _tracker.createInstance<SetContext>(_ctx, getState());
  enterRule(_localctx, 106, configParser::RuleSet);

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
    match(configParser::T__86);
    setState(377);
    match(configParser::VARIABLE);
    setState(378);
    arguments();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Set_from_resourceContext ------------------------------------------------------------------

configParser::Set_from_resourceContext::Set_from_resourceContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::Set_from_resourceContext::VARIABLE() {
  return getToken(configParser::VARIABLE, 0);
}

std::vector<tree::TerminalNode *> configParser::Set_from_resourceContext::STRING() {
  return getTokens(configParser::STRING);
}

tree::TerminalNode* configParser::Set_from_resourceContext::STRING(size_t i) {
  return getToken(configParser::STRING, i);
}


size_t configParser::Set_from_resourceContext::getRuleIndex() const {
  return configParser::RuleSet_from_resource;
}

void configParser::Set_from_resourceContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSet_from_resource(this);
}

void configParser::Set_from_resourceContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSet_from_resource(this);
}

configParser::Set_from_resourceContext* configParser::set_from_resource() {
  Set_from_resourceContext *_localctx = _tracker.createInstance<Set_from_resourceContext>(_ctx, getState());
  enterRule(_localctx, 108, configParser::RuleSet_from_resource);
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
    setState(380);
    match(configParser::T__87);
    setState(381);
    match(configParser::VARIABLE);
    setState(382);
    match(configParser::STRING);
    setState(384);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == configParser::STRING) {
      setState(383);
      match(configParser::STRING);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IncludeContext ------------------------------------------------------------------

configParser::IncludeContext::IncludeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::IncludeContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::IncludeContext::getRuleIndex() const {
  return configParser::RuleInclude;
}

void configParser::IncludeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInclude(this);
}

void configParser::IncludeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInclude(this);
}

configParser::IncludeContext* configParser::include() {
  IncludeContext *_localctx = _tracker.createInstance<IncludeContext>(_ctx, getState());
  enterRule(_localctx, 110, configParser::RuleInclude);

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
    match(configParser::T__88);
    setState(387);
    match(configParser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BindingContext ------------------------------------------------------------------

configParser::BindingContext::BindingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

configParser::KeybindingContext* configParser::BindingContext::keybinding() {
  return getRuleContext<configParser::KeybindingContext>(0);
}

configParser::ArgumentsContext* configParser::BindingContext::arguments() {
  return getRuleContext<configParser::ArgumentsContext>(0);
}

std::vector<configParser::Binding_optionsContext *> configParser::BindingContext::binding_options() {
  return getRuleContexts<configParser::Binding_optionsContext>();
}

configParser::Binding_optionsContext* configParser::BindingContext::binding_options(size_t i) {
  return getRuleContext<configParser::Binding_optionsContext>(i);
}


size_t configParser::BindingContext::getRuleIndex() const {
  return configParser::RuleBinding;
}

void configParser::BindingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinding(this);
}

void configParser::BindingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinding(this);
}

configParser::BindingContext* configParser::binding() {
  BindingContext *_localctx = _tracker.createInstance<BindingContext>(_ctx, getState());
  enterRule(_localctx, 112, configParser::RuleBinding);
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
    setState(389);
    _la = _input->LA(1);
    if (!(((((_la - 90) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 90)) & ((1ULL << (configParser::T__89 - 90))
      | (1ULL << (configParser::T__90 - 90))
      | (1ULL << (configParser::T__91 - 90)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(393);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(390);
        binding_options(); 
      }
      setState(395);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx);
    }
    setState(396);
    keybinding();
    setState(397);
    arguments();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Binding_optionsContext ------------------------------------------------------------------

configParser::Binding_optionsContext::Binding_optionsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t configParser::Binding_optionsContext::getRuleIndex() const {
  return configParser::RuleBinding_options;
}

void configParser::Binding_optionsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBinding_options(this);
}

void configParser::Binding_optionsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBinding_options(this);
}

configParser::Binding_optionsContext* configParser::binding_options() {
  Binding_optionsContext *_localctx = _tracker.createInstance<Binding_optionsContext>(_ctx, getState());
  enterRule(_localctx, 114, configParser::RuleBinding_options);
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
    setState(399);
    _la = _input->LA(1);
    if (!(((((_la - 93) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 93)) & ((1ULL << (configParser::T__92 - 93))
      | (1ULL << (configParser::T__93 - 93))
      | (1ULL << (configParser::T__94 - 93))
      | (1ULL << (configParser::T__95 - 93)))) != 0))) {
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

//----------------- ModifierContext ------------------------------------------------------------------

configParser::ModifierContext::ModifierContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::ModifierContext::VARIABLE() {
  return getToken(configParser::VARIABLE, 0);
}

std::vector<configParser::ModifierContext *> configParser::ModifierContext::modifier() {
  return getRuleContexts<configParser::ModifierContext>();
}

configParser::ModifierContext* configParser::ModifierContext::modifier(size_t i) {
  return getRuleContext<configParser::ModifierContext>(i);
}


size_t configParser::ModifierContext::getRuleIndex() const {
  return configParser::RuleModifier;
}

void configParser::ModifierContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterModifier(this);
}

void configParser::ModifierContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitModifier(this);
}


configParser::ModifierContext* configParser::modifier() {
   return modifier(0);
}

configParser::ModifierContext* configParser::modifier(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  configParser::ModifierContext *_localctx = _tracker.createInstance<ModifierContext>(_ctx, parentState);
  configParser::ModifierContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 116;
  enterRecursionRule(_localctx, 116, configParser::RuleModifier, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(416);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case configParser::T__97: {
        setState(402);
        match(configParser::T__97);
        break;
      }

      case configParser::T__98: {
        setState(403);
        match(configParser::T__98);
        break;
      }

      case configParser::T__99: {
        setState(404);
        match(configParser::T__99);
        break;
      }

      case configParser::T__100: {
        setState(405);
        match(configParser::T__100);
        break;
      }

      case configParser::T__101: {
        setState(406);
        match(configParser::T__101);
        break;
      }

      case configParser::T__102: {
        setState(407);
        match(configParser::T__102);
        break;
      }

      case configParser::T__103: {
        setState(408);
        match(configParser::T__103);
        break;
      }

      case configParser::T__104: {
        setState(409);
        match(configParser::T__104);
        break;
      }

      case configParser::T__105: {
        setState(410);
        match(configParser::T__105);
        break;
      }

      case configParser::T__106: {
        setState(411);
        match(configParser::T__106);
        break;
      }

      case configParser::T__107: {
        setState(412);
        match(configParser::T__107);
        break;
      }

      case configParser::T__108: {
        setState(413);
        match(configParser::T__108);
        break;
      }

      case configParser::T__109: {
        setState(414);
        match(configParser::T__109);
        break;
      }

      case configParser::VARIABLE: {
        setState(415);
        match(configParser::VARIABLE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    _ctx->stop = _input->LT(-1);
    setState(423);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<ModifierContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleModifier);
        setState(418);

        if (!(precpred(_ctx, 15))) throw FailedPredicateException(this, "precpred(_ctx, 15)");
        setState(419);
        match(configParser::T__96);
        setState(420);
        modifier(16); 
      }
      setState(425);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- KeybindingContext ------------------------------------------------------------------

configParser::KeybindingContext::KeybindingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* configParser::KeybindingContext::STRING() {
  return getToken(configParser::STRING, 0);
}


size_t configParser::KeybindingContext::getRuleIndex() const {
  return configParser::RuleKeybinding;
}

void configParser::KeybindingContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterKeybinding(this);
}

void configParser::KeybindingContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<configListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitKeybinding(this);
}

configParser::KeybindingContext* configParser::keybinding() {
  KeybindingContext *_localctx = _tracker.createInstance<KeybindingContext>(_ctx, getState());
  enterRule(_localctx, 118, configParser::RuleKeybinding);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(427);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 26, _ctx)) {
    case 1: {
      setState(426);
      match(configParser::STRING);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool configParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 58: return modifierSempred(antlrcpp::downCast<ModifierContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool configParser::modifierSempred(ModifierContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 15);

  default:
    break;
  }
  return true;
}

void configParser::initialize() {
  std::call_once(configParserOnceFlag, configParserInitialize);
}
