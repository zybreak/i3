#!/bin/sh

pushd parser-specs/new
antlr4 \
    -Dlanguage=Cpp \
    -o ../../i3/src/config/new/parser-specs \
    configLexer.g4 \
    configGrammar.g4
popd