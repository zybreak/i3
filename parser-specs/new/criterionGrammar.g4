parser grammar criterionGrammar;

options {
    tokenVocab=criterionLexer;
}

criteria: BEGIN_CRITERION ((criterion CRITERION_SEP)* criterion)? END_CRITERION;
criterion
    : value_criterion
    | TILING
    | FLOATING
    | ALL;

value_criterion
    : (CLASS
    | INSTANCE
    | WINDOW_ROLE
    | CON_ID
    | ID
    | WINDOW_TYPE
    | TITLE
    | URGENT
    | WORKSPACE
    | MACHINE
    | FLOATING_FROM
    | TILING_FROM) CRITERION_EQ CRITERION_STRING
    ;