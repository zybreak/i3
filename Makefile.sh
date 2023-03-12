#!/bin/sh
#CONAN_PROFILE=clang
CONAN_PROFILE=default
#export CXX=$HOME/.local/bin/clang++
mkdir build && cd build
conan install .. -pr $CONAN_PROFILE -s build_type=Debug --build=missing
cmake -G "Ninja Multi-Config" .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --config Debug