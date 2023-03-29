parser grammar configGrammar;

options {
    tokenVocab=configLexer;
}

import commandsGrammar, criterionGrammar;

config: EOL* ((stmt EOL+)* stmt EOL*)? EOF;
stmt
    : set
    | binding
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
    ;

commands: OPEN_COMMAND (command COMMAND_SEP)* command COMMAND_SEP? CLOSE_COMMAND;

set: SET VAR EQ STRING;
binding: BINDSYM OPTION* STRING commands;
font: FONT STRING;
floating_minimum_size: FLOATING_MINIMUM_SIZE dimension;
floating_maximum_size: FLOATING_MAXIMUM_SIZE dimension;
floating_modifier: FLOATING_MODIFIER STRING;
default_orientation: DEFAULT_ORIENTATION STRING;
workspace_layout: WORKSPACE_LAYOUT STRING;
default_border: DEFAULT_BORDER STRING;
hide_edge_borders: HIDE_EDGE_BORDERS STRING;
for_window: FOR_WINDOW criteria commands;
assign: ASSIGN criteria STRING;
no_focus: NO_FOCUS criteria;
focus_follows_mouse: FOCUS_FOLLOWS_MOUSE STRING;
mouse_warping: MOUSE_WARPING STRING;
focus_wrapping: FOCUS_WRAPPING STRING;
force_focus_wrapping: FORCE_FOCUS_WRAPPING STRING;
workspace_auto_back_and_forth: WORKSPACE_AUTO_BACK_AND_FORTH STRING;
force_display_urgency_hint: FORCE_DISPLAY_URGENCY_HINT NUMBER;
focus_on_window_activation: FOCUS_ON_WINDOW_ACTIVATION STRING;
title_align: TITLE_ALIGN STRING;
workspace: WORKSPACE STRING (EQ STRING)?;
ipc_socket: IPC_SOCKET STRING;
ipc_kill_timeout: IPC_KILL_TIMEOUT NUMBER;
restart_state: RESTART_STATE STRING;
popup_during_fullscreen: POPUP_DURING_FULLSCREEN STRING;
exec: EXEC OPTION* STRING;

dimension: NUMBER DIMENSION_SEP NUMBER;