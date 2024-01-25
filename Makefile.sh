#!/bin/sh
set -e
rm -rf build
ccache -c
#CONAN_PROFILE=default
CONAN_PROFILE=clang
mkdir build
#conan install . -pr:b=$CONAN_PROFILE -c:h tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s build_type=Debug --output-folder=build --build=missing
conan install . --profile=$CONAN_PROFILE -s build_type=Debug --build=missing
#./antlr.sh
cmake --preset conan-default
cmake --build build --target i3 --config Debug --clean-first --preset conan-debug
#ctest -C Debug --test-dir build