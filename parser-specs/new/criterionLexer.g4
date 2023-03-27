lexer grammar criterionLexer;

BEGIN_CRITERION: '[' -> pushMode(CRITERION);
mode CRITERION;
END_CRITERION: ']' -> popMode;
CRITERION_STRING
   : '\u0022' ~'\u0022'* '\u0022'
   ;
TILING: 'tiling';
FLOATING: 'floating';
ALL: 'all';

CLASS: 'class';
INSTANCE: 'instance';
WINDOW_ROLE: 'window_role';
CON_ID: 'con_id';
ID: 'id';
WINDOW_TYPE: 'window_type';
TITLE: 'title';
URGENT: 'urgent';
WORKSPACE: 'workspace';
MACHINE: 'machine';
FLOATING_FROM: 'floating_from';
TILING_FROM: 'tiling_from';
CRITERION_EQ: '=';
CRITERION_SEP: ',';

COMMAND_WS: ' ' -> skip;
