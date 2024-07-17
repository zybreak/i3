#!/bin/sh
set -e
rm -rf build
ccache -C
#CONAN_PROFILE=default
CONAN_PROFILE=clang
mkdir build
#conan install . -pr:a $CONAN_PROFILE -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" -s:a build_type=Debug --output-folder=.  --build="missing"
conan install . -pr:a $CONAN_PROFILE -s:a build_type=Debug --output-folder=.  --build="missing"
#./antlr.sh
cmake --preset conan-debug -DCMAKE_BUILD_TYPE=Debug
#cmake --build build --target i3 --config Debug --clean-first --preset conan-debug
cmake --build build --config Debug --clean-first --preset conan-debug
#ctest -C Debug --test-dir build
#cmake --build build --target coverage
