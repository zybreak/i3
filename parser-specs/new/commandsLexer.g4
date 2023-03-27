lexer grammar commandsLexer;

OPEN_COMMAND : '$('    -> pushMode(COMMAND);
mode COMMAND;
CLOSE_COMMAND : ')'    -> popMode;

COMMAND_SEP: ';';
COMMAND_MOVE: 'move';
COMMAND_EXEC: 'exec';
COMMAND_DEBUGLOG: 'debuglog';
COMMAND_BORDER: 'border';
COMMAND_LAYOUT: 'layout';
COMMAND_APPEND_LAYOUT: 'append_layout';
COMMAND_WORKSPACE: 'workspace';
COMMAND_FOCUS: 'focus';
COMMAND_KILL: 'kill';
COMMAND_OPEN: 'open';
COMMAND_FULLSCREEN: 'fullscreen';
COMMAND_STICKY: 'sticky';
COMMAND_SPLIT: 'split';
COMMAND_FLOATING: 'floating';
COMMAND_RESIZE: 'resize';
COMMAND_RENAME: 'rename';
COMMAND_NOP: 'nop';
COMMAND_SCRATCHPAD: 'scratchpad';
COMMAND_TITLE_FORMAT: 'title_format';
COMMAND_TITLE_WINDOW_ICON: 'title_window_icon';

COMMAND_WS: ' ' -> skip;
COMMAND_OPTION: '--' [a-z0-9][a-z\-0-9]*;
COMMAND_NUMBER: [0-9]+;
COMMAND_STRING
   : '\u0022' ~'\u0022'* '\u0022'
   ;
