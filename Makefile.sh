#!/bin/sh
CONAN_PROFILE=default
mkdir build
conan install . -pr:b=$CONAN_PROFILE -c:h tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s build_type=Debug --output-folder=build --build=missing
cmake --preset conan-default
cmake --build build --config Debug --preset conan-debug
ctest -C Debug --test-dir build