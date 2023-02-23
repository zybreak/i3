#!/bin/sh
export CXX=$HOME/.local/bin/clang++
mkdir build && cd build
conan install .. -pr clang -s build_type=Debug --build=missing
cmake -G "Ninja Multi-Config" .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build . --config Debug