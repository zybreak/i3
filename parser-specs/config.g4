grammar config;

file: stmt? (EOL stmt?)* EOF;

stmt
    : set
    | set_from_resource
    | include
    | binding
    // | bar IGNORE BAR FOR NOW, WE DONT USE IT ANYHOW
    | stmt_mode
    | font
    | floating_minimum_size
    | floating_maximum_size
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

arguments: .*?;

// mode
stmt_mode: 'mode' stmt_mode_options* STRING '{' stmt_mode_stmts? (EOL stmt_mode_stmts?)+ '}';
stmt_mode_options: '--pango_markup';
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
exec: ('exec' | 'exec_always') exec_options* arguments;

exec_options: '--no-startup-id';

// popup_during_fullscreen
popup_during_fullscreen: 'popup_during_fullscreen' popup_during_fullscreen_value;

popup_during_fullscreen_value
    : 'ignore'
    | 'leave_fullscreen'
    | 'smart';

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
title_align: 'title_align' title_align_value;
title_align_value
    : 'left'
    | 'center'
    | 'right';

// focus_on_window_activation
focus_on_window_activation: 'focus_on_window_activation' focus_on_window_activation_value;
focus_on_window_activation_value
    : 'smart'
    | 'urgent'
    | 'focus'
    | 'none';

// force_display_urgency_hint
force_display_urgency_hint: 'force_display_urgency_hint' NUMBER ('ms')?;

// workspace_auto_back_and_forth
workspace_auto_back_and_forth: 'workspace_auto_back_and_forth' BOOL;

// focus wrapping
focus_wrapping: 'focus_wrapping' (BOOL | 'force' | 'workspace');
force_focus_wrapping: 'force_focus_wrapping' BOOL;

// mouse_warping
mouse_warping: 'mouse_warping' mouse_warping_value;
mouse_warping_value
    : 'none'
    | 'output';

// FOCUS FOLLOWS MOUSE
focus_follows_mouse: 'focus_follows_mouse' BOOL;

// NO FOCUS
no_focus: 'no_focus' criteria;

// ASSIGN
assign: 'assign' criteria '→'?  assign_target;
assign_target
    : assign_target_workspace
    | assign_target_number
    | assign_target_output
    ;

assign_target_workspace: 'workspace' (STRING | NUMBER);
assign_target_number: 'number' STRING;
assign_target_output: 'output' ('left'|'right'|'up'|'down'|'primary');

// FOR WINDOW
for_window: 'for_window' criteria arguments;
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
hide_edge_borders: 'hide_edge_borders' border_type;
border_type
    : BOOL
    | 'none'
    | 'vertical'
    | 'horizontal'
    | 'both'
    | 'smart'
    ;

// DEFAULT BORDER
default_border: default_border_type border_style;
default_border_type
    : 'default_border'
    | 'new_window'
    | 'default_floating_border'
    | 'new_float';

border_style
    : ('normal' | 'pixel') NUMBER
    | 'none'
    | '1pixel'
    ;

// WORKSPACE LAYOUT
workspace_layout: 'workspace_layout' workspace_layout_value;
workspace_layout_value
    : 'default'
    | 'stacking'
    | 'stacked'
    | 'tabbed';

// DEFAULT ORIENTATION
default_orientation: 'default_orientation' default_orientation_value;
default_orientation_value
    : 'horizontal'
    | 'vertical'
    | 'auto';

// FLOATING MODIFIER
floating_modifier: 'floating_modifier' (modifier)+;

// FLOATING
floating_minimum_size: 'floating_minimum_size' dimension;
floating_maximum_size: 'floating_maximum_size' dimension;
dimension: NUMBER 'x' NUMBER;

// FONT
font: 'font' arguments;

// SET
set: 'set' VARIABLE arguments;
set_from_resource: 'set_from_resource' VARIABLE STRING STRING?;

// INCLUDE
include: 'include' STRING;

// BINDING
binding: ('bindsym' | 'bindcode' | 'bind') (binding_options)* keybinding arguments;

binding_options
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
    : STRING?
    ;

COMMENT: '#' ~[\r\n]* -> channel(HIDDEN);
WS: [ \t]+ -> skip;
BOOL: TRUE | FALSE;
fragment TRUE: '1' | 'yes' | 'true' | 'on' | 'enable' | 'active';
fragment FALSE: '0' | 'no' | 'false' | 'off' | 'disable' | 'inactive';
VARIABLE: '$' IDENTIFIER;
fragment WORD: ~[ \t\r\n[\]{}=,]+;
STRING: WORD | QUOTED_STRING;
fragment QUOTED_STRING: '"' ( ~[\\"] | '\\' . )* '"';
fragment IDENTIFIER: [a-zA-Z] [a-zA-Z0-9_\-]*;
NUMBER: [0-9]+;
EOL : ('\r'|'\n')+;
