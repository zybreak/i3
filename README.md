![Logo](docs/logo-30.png) i3: A tiling window manager
=====================================================

i3 is a tiling window manager for X11.

For more information about i3, please see [the project's website](https://i3wm.org/) and [online documentation](https://i3wm.org/docs/).

For information about contributing to i3, please see [CONTRIBUTING.md](.github/CONTRIBUTING.md).

---------

This is a fork of https://github.com/i3/i3/ where I have migrated the sourcecode from C to C++23 just for fun.

I have experimented with the following:
 * Converted the source code from C to C++23
 * Used C++20 modules
 * Played around with ANTLR4 to parse the configuration file
 * Use XPP as a wrapper for XCB
 * Used GMock and GTest for mocking and unittests
 * Conan2 as a dependency manager
 * CMake + ninja as a build system

It builds (on my machine) with a minor patch to libev (its broken when used in conjunction with C++20 modules) with clang 18 and Cmake 3.28.3