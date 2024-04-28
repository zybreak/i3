lexer grammar configLexer;

import commandsLexer, criterionLexer;

COMMENT: '#' ~('\r' | '\n')* -> channel(HIDDEN);

STRING
   : '"' ('\\"' | ~'"')* '"'
   ;

NUMBER: [0-9]+;
DIMENSION_SEP: 'x';

OPTION: '--' [a-z0-9][a-z\-0-9]*;

VAR: '$' [a-zA-Z][a-zA-Z0-9_-]*;

WS: ' ' -> skip;
EOL : ('\r'|'\n')+;

SET: 'set';
EQ: '=';
EXEC: 'exec';
EXEC_ALWAYS: 'exec_always';
BINDSYM: 'bindsym';
FONT: 'font';
POPUP_DURING_FULLSCREEN: 'popup_during_fullscreen';
FLOATING_MINIMUM_SIZE: 'floating_minimum_size';
FLOATING_MAXIMUM_SIZE: 'floating_maximum_size';
FLOATING_MODIFIER: 'floating_modifier';
DEFAULT_ORIENTATION: 'default_orientation';
WORKSPACE_LAYOUT: 'workspace_layout';
DEFAULT_BORDER: 'default_border';
NEW_WINDOW: 'new_window';
DEFAULT_FLOATING_BORDER: 'default_floating_border';
NEW_FLOAT: 'new_float';
HIDE_EDGE_BORDERS: 'hide_edge_borders';
FOR_WINDOW: 'for_window';
ASSIGN: 'assign';
NO_FOCUS: 'no_focus';
FOCUS_FOLLOWS_MOUSE: 'focus_follows_mouse';
MOUSE_WARPING: 'mouse_warping';
FOCUS_WRAPPING: 'focus_wrapping';
FORCE_FOCUS_WRAPPING: 'force_focus_wrapping';
WORKSPACE_AUTO_BACK_AND_FORTH: 'workspace_auto_back_and_forth';
FORCE_DISPLAY_URGENCY_HINT: 'force_display_urgency_hint';
FOCUS_ON_WINDOW_ACTIVATION: 'focus_on_window_activation';
TITLE_ALIGN: 'title_align';
WORKSPACE: 'workspace';
IPC_SOCKET: 'ipc_socket';
IPC_KILL_TIMEOUT: 'ipc_kill_timeout';
RESTART_STATE: 'restart_state';
