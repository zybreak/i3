parser grammar commandsGrammar;

options {
    tokenVocab=commandsLexer;
}

command
    : command_move
    | command_exec
    | command_debuglog
    | command_border
    | command_layout
    | command_append_layout
    | command_workspace
    | command_focus
    | command_kill
    | command_mode
    | COMMAND_OPEN
    | command_fullscreen
    | COMMAND_STICKY
    | command_split
    | command_floating
    | COMMAND_RESIZE
    | COMMAND_RENAME
    | COMMAND_NOP
    | COMMAND_RELOAD
    | COMMAND_RESTART
    | command_scratchpad
    | COMMAND_TITLE_FORMAT
    | COMMAND_TITLE_WINDOW_ICON
;

command_move: COMMAND_MOVE COMMAND_STRING;
command_exec: COMMAND_EXEC COMMAND_OPTION* COMMAND_STRING;
command_kill : COMMAND_KILL;
command_fullscreen: COMMAND_FULLSCREEN COMMAND_STRING;
command_debuglog: COMMAND_DEBUGLOG COMMAND_STRING;
command_border: COMMAND_BORDER COMMAND_STRING COMMAND_NUMBER?;
command_layout: COMMAND_LAYOUT COMMAND_STRING COMMAND_STRING?;
command_append_layout: COMMAND_APPEND_LAYOUT COMMAND_STRING;
command_workspace: COMMAND_WORKSPACE COMMAND_OPTION* COMMAND_STRING;
command_focus: COMMAND_FOCUS COMMAND_STRING COMMAND_STRING?;
command_split: COMMAND_SPLIT COMMAND_STRING;
command_floating: COMMAND_FLOATING COMMAND_STRING;
command_mode: COMMAND_MODE COMMAND_STRING;
command_scratchpad: COMMAND_SCRATCHPAD COMMAND_STRING;