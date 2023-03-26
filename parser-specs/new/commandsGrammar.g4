parser grammar commandsGrammar;

options {
    tokenVocab=commandsLexer;
}

command
    : COMMAND_MOVE
    | command_exec
    | COMMAND_DEBUGLOG
    | COMMAND_BORDER
    | COMMAND_LAYOUT
    | COMMAND_APPEND_LAYOUT
    | COMMAND_WORKSPACE
    | COMMAND_FOCUS
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
command_kill : COMMAND_KILL;

/*
criteria: '[' (criterion)+ ']';
criterion
    : value_criterion
    | 'tiling'
    | 'floating'
    | 'all';

value_criterion
    : ('class'
    | 'instance'
    | 'window_role'
    | 'con_id'
    | 'id'
    | 'window_type'
    | 'title'
    | 'urgent'
    | 'workspace'
    | 'machine'
    | 'floating_from'
    | 'tiling_from') '=' STRING
    ;
*/
