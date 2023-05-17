#!/bin/sh
#CONAN_PROFILE=clang
CONAN_PROFILE=default
#export CXX=$HOME/.local/bin/clang++
mkdir build
conan install . -pr:b=$CONAN_PROFILE -s build_type=Debug --output-folder=build --build=missing
cmake -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=/home/zybreak/src/i3/build/conan_toolchain.cmake -B build
cmake --build build --config Debug
ctest -C Debug --test-dir build