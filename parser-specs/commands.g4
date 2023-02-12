grammar commands;

command: criteria? stmt EOF;

stmt
    : move
    | exec
    | 'exit'
    | 'restart'
    | 'reload'
    | debuglog
    | border
    | layout
    | append_layout
    | workspace
    | focus
    | kill
    | open
    | fullscreen
    | sticky
    | split
    | floating
    | resize
    | rename
    | nop
    | scratchpad
    | title_format
    | title_window_icon
    | stmt_mode
    ;

move: 'move';
exec: 'exec';
debuglog: 'debuglog';
border: 'border';
layout: 'layout';
append_layout: 'append_layout';
workspace: 'workspace';
focus: 'focus';
kill: 'kill';
open: 'open';
fullscreen: 'fullscreen';
sticky: 'sticky';
split: 'split';
floating: 'floating';
resize: 'resize';
rename: 'rename';
nop: 'nop';
scratchpad: 'scratchpad';
title_format: 'title_format';
title_window_icon: 'title_window_icon';
stmt_mode: 'stmt_mode';

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

WS: [ \t]+ -> skip;
BOOL: TRUE | FALSE;
fragment TRUE: '1' | 'yes' | 'true' | 'on' | 'enable' | 'active';
fragment FALSE: '0' | 'no' | 'false' | 'off' | 'disable' | 'inactive';
VARIABLE: '$' IDENTIFIER;
fragment WORD: ~[ \t\r\n[\]{}=,|&]+;
STRING: WORD | QUOTED_STRING;
fragment QUOTED_STRING: '"' ( ~[\\"] | '\\' . )* '"';
fragment IDENTIFIER: [a-zA-Z] [a-zA-Z0-9_\-]*;
NUMBER: [0-9]+;
DIMENSION: NUMBER'x'NUMBER;
EOL : ('\r'|'\n')+;
