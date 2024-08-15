#!/usr/bin/bash
set -e

target=${1:-i3} #all
run_tests=${2:-false}
clear_build=${3:-true}

if [ "$clear_build" = "true" ]; then
    rm -rf build
    ccache -C
    mkdir build
    clean_first="--clean-first"
fi
#conan install . -pr:a $CONAN_PROFILE -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s:a build_type=Debug --output-folder=.  --build="missing"
if [ "$clear_build" = "true" ]; then
    conan install . -pr:a clang -s:a build_type=Debug --output-folder=.  --build="missing"
    #./antlr.sh
    cmake --preset conan-debug -DCMAKE_BUILD_TYPE=Debug
fi

cmake --build build --target $target $clean_first --preset conan-debug

if [ "$run_tests" = "true" ]; then
    ctest --test-dir build/Debug
    cmake --build build --target coverage --preset conan-debug
fi

