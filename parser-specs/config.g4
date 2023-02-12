grammar config;

file: stmt? (EOL stmt?)* EOF;

stmt
    : set
    | include
    | binding
    // | bar IGNORE BAR FOR NOW, WE DONT USE IT ANYHOW
    | stmt_mode
    | font
    | floating
    | floating_modifier
    | default_orientation
    | workspace_layout
    | default_border
    | hide_edge_borders
    | for_window
    | assign
    | no_focus
    | focus_follows_mouse
    | mouse_warping
    | focus_wrapping
    | force_focus_wrapping
    | workspace_auto_back_and_forth
    | force_display_urgency_hint
    | focus_on_window_activation
    | title_align
    | workspace
    | ipc_socket
    | ipc_kill_timeout
    | restart_state
    | popup_during_fullscreen
    | exec
    | color
    ;

// mode
stmt_mode: 'mode' '--pango_markup'? STRING '{' stmt_mode_stmts? (EOL stmt_mode_stmts?)+ '}';
stmt_mode_stmts
    : set
    | binding
    ;

// color
color
    : color_single
    | color_multiple
    ;

color_single: 'client.background' STRING;
color_multiple
    : ('client.focused_inactive'
    | 'client.focused'
    | 'client.unfocused'
    | 'client.urgent'
    | 'client.placeholder') STRING STRING STRING STRING? STRING?
    ;

// exec
exec: ('exec' | 'exec_always') '--no-startup-id'? STRING;

// popup_during_fullscreen
popup_during_fullscreen: 'popup_during_fullscreen' ('ignore' | 'leave_fullscreen' | 'smart');

// restart_state
restart_state: 'restart_state' STRING;

// ipc_kill_timeout
ipc_kill_timeout: 'ipc_kill_timeout' NUMBER;

// ipc_socket
ipc_socket: ('ipc_socket' | 'ipc-socket') STRING;

// workspace
workspace: 'workspace' workspace_name 'output' STRING+;
workspace_name: NUMBER | STRING;

// title_align
title_align: 'title_align' ('left'|'center'|'right');

// focus_on_window_activation
focus_on_window_activation: 'focus_on_window_activation' ('smart'|'urgent'|'focus'|'none');

// force_display_urgency_hint
force_display_urgency_hint: 'force_display_urgency_hint' NUMBER ('ms')?;

// workspace_auto_back_and_forth
workspace_auto_back_and_forth: 'workspace_auto_back_and_forth' BOOL;

// focus wrapping
focus_wrapping: 'focus_wrapping' (BOOL | 'force' | 'workspace');
force_focus_wrapping: 'force_focus_wrapping' BOOL;

// mouse_warping
mouse_warping: 'mouse_warping' ('none' | 'output');

// FOCUS FOLLOWS MOUSE
focus_follows_mouse: 'focus_follows_mouse' BOOL;

// NO FOCUS
no_focus: 'no_focus' criteria;

// ASSIGN
assign: 'assign' criteria '→'?  assign_target;
assign_target
    : ('workspace' | 'number') workspace_name
    | 'output' ('left'|'right'|'up'|'down'|'primary')
    ;

// FOR WINDOW
for_window: 'for_window' criteria STRING;
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

// HIDE EDGE BORDERS
hide_edge_borders: 'hide_edge_borders' (border_type);
border_type
    : BOOL
    | 'none'
    | 'vertical'
    | 'horizontal'
    | 'both'
    | 'smart'
    ;

// DEFAULT BORDER
default_border: ('default_border' | 'new_window' | 'default_floating_border' | 'new_float') border_style;

border_style
    : ('normal' | 'pixel') NUMBER
    | 'none'
    | '1pixel'
    ;

// WORKSPACE LAYOUT
workspace_layout: 'workspace_layout' ('default' | 'stacking' | 'stacked' | 'tabbed');

// DEFAULT ORIENTATION
default_orientation: 'default_orientation' ('horizontal' | 'vertical' | 'auto');

// FLOATING MODIFIER
floating_modifier: 'floating_modifier' (modifier)+;

// FLOATING
floating: ('floating_minimum_size' | 'floating_maximum_size') DIMENSION;

// FONT
font: 'font' STRING;

// SET
set: ('set' | 'set_from_resource') VARIABLE STRING STRING?;

// INCLUDE
include: 'include' STRING;

// BINDING
binding: ('bindsym' | 'bindcode' | 'bind') (binding_option)* keybinding STRING;

binding_option
    : '--release'
    | '--border'
    | '--whole-window'
    | '--exclude-titlebar'
    ;

modifier
    : modifier '+' modifier
    | 'Mod1'
    | 'Mod2'
    | 'Mod3'
    | 'Mod4'
    | 'Mod5'
    | 'Shift'
    | 'Control'
    | 'Ctrl'
    | 'Mode_switch'
    | 'Group1'
    | 'Group2'
    | 'Group3'
    | 'Group4'
    | VARIABLE
    ;

keybinding
    : (VARIABLE | STRING)
    | (modifier)+ '+' (VARIABLE | STRING)
    ;

COMMENT: '#' ~[\r\n]* -> channel(HIDDEN);
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
