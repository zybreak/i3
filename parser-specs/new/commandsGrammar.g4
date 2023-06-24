parser grammar commandsGrammar;

options {
    tokenVocab=commandsLexer;
}

//import criterionGrammar;

//commands: OPEN_COMMAND (criteria? command COMMAND_SEP)* criteria? command COMMAND_SEP? CLOSE_COMMAND;
commands: OPEN_COMMAND (command COMMAND_SEP)* command COMMAND_SEP? CLOSE_COMMAND;

command
    : command_move
    | command_exec
    | command_exit
    | command_debuglog
    | command_border
    | command_layout
    | command_append_layout
    | command_workspace
    | command_focus
    | command_kill
    | command_mode
    | command_open
    | command_fullscreen
    | command_sticky
    | command_split
    | command_floating
    | command_resize
    | command_rename
    | command_nop
    | command_reload
    | command_restart
    | command_title_format
    | command_title_window_icon
;

command_move: COMMAND_MOVE (COMMAND_NUMBER | COMMAND_STRING); // TODO
command_exec: COMMAND_EXEC COMMAND_OPTION* COMMAND_STRING;
command_exit: COMMAND_EXIT;
command_kill : COMMAND_KILL COMMAND_STRING;
command_fullscreen: COMMAND_FULLSCREEN COMMAND_STRING COMMAND_STRING?;
command_sticky: COMMAND_STICKY COMMAND_STRING;
command_debuglog: COMMAND_DEBUGLOG COMMAND_STRING;
command_border: COMMAND_BORDER COMMAND_STRING COMMAND_NUMBER?;
command_layout: COMMAND_LAYOUT COMMAND_STRING COMMAND_STRING?;
command_append_layout: COMMAND_APPEND_LAYOUT COMMAND_STRING;
command_workspace: COMMAND_WORKSPACE COMMAND_OPTION* COMMAND_STRING;
command_focus: COMMAND_FOCUS COMMAND_STRING COMMAND_STRING?;
command_split: COMMAND_SPLIT COMMAND_STRING;
command_floating: COMMAND_FLOATING COMMAND_STRING;
command_resize: COMMAND_RESIZE; // TODO
command_rename: COMMAND_RENAME; // TODO
command_nop: COMMAND_NOP COMMAND_STRING?;
command_reload: COMMAND_RELOAD;
command_restart: COMMAND_RESTART;
command_mode: COMMAND_MODE COMMAND_STRING;
command_open: COMMAND_OPEN;
command_title_format: COMMAND_TITLE_FORMAT COMMAND_STRING;
command_title_window_icon: COMMAND_TITLE_WINDOW_ICON; // TODO