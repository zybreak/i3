#!/bin/sh
#CONAN_PROFILE=default
CONAN_PROFILE=clang
mkdir build
#conan install . -pr:b=$CONAN_PROFILE -c:h tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s build_type=Debug --output-folder=build --build=missing
conan install . -pr:h=$CONAN_PROFILE -pr:b=$CONAN_PROFILE -c:h tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s build_type=Debug --output-folder=build --build=missing
#./antlr.sh
cmake --preset conan-default
cmake --build build --config Debug --preset conan-debug
ctest -C Debug --test-dir build