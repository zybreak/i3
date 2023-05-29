#!/bin/sh

pushd parser-specs/new
antlr4 \
    -Dlanguage=Cpp \
    -o ../../i3/src/config/new/parser-specs \
    configLexer.g4 \
    configGrammar.g4

antlr4 \
    -Dlanguage=Cpp \
    -o ../../i3/src/commands/new/parser-specs \
    commandsLexer.g4 \
    commandsGrammar.g4
popd