parser grammar commandsGrammar;

options {
    tokenVocab=commandsLexer;
}

command
    : COMMAND_MOVE
    | command_exec
    | command_debuglog
    | command_border
    | command_layout
    | command_append_layout
    | command_workspace
    | command_focus
    | command_kill
    | COMMAND_OPEN
    | COMMAND_FULLSCREEN
    | COMMAND_STICKY
    | COMMAND_SPLIT
    | COMMAND_FLOATING
    | COMMAND_RESIZE
    | COMMAND_RENAME
    | COMMAND_NOP
    | COMMAND_SCRATCHPAD
    | COMMAND_TITLE_FORMAT
    | COMMAND_TITLE_WINDOW_ICON
;

command_exec: COMMAND_EXEC COMMAND_OPTION* command;
command_kill : COMMAND_KILL COMMAND_STRING;
command_debuglog: COMMAND_DEBUGLOG COMMAND_STRING;
command_border: COMMAND_BORDER COMMAND_STRING COMMAND_NUMBER?;
command_layout: COMMAND_LAYOUT COMMAND_STRING COMMAND_STRING?;
command_append_layout: COMMAND_APPEND_LAYOUT COMMAND_STRING;
command_workspace: COMMAND_WORKSPACE COMMAND_OPTION* COMMAND_STRING;
command_focus: COMMAND_FOCUS COMMAND_STRING COMMAND_STRING?;