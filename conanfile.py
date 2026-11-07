from conan import ConanFile;
import os
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain, CMakeDeps;

class i3(ConanFile):
    name = "i3"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    package_type = "application"


    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        #tc.generator = "Ninja Multi-Config"
        tc.generator = "Ninja"
        tc.generate()


    def build_requirements(self):
        self.tool_requires("cmake/[>=4.0]")
        self.tool_requires("ninja/[>=1.12]")
        self.test_requires("gtest/1.17.0")

#self.requires("zlib/1.3.1")
#self.requires("libev/4.33")
#self.requires("pcre2/10.44", force=True)
#self.requires("fmt/12.1.0", force=True)
#self.requires("antlr4-cppruntime/4.13.2")
#self.requires("glib/2.85.3", force=True)
#self.requires("spdlog/1.16.0")
#self.requires("nlohmann_json/3.12.0")
#self.requires("cairo/1.18.0")
#self.requires("pango/1.54.0")
#self.requires("di/1.3.2")
    def requirements(self):
        self.requires("zlib/1.3.2")
        # A local revision of conancenter's a74e6be: its ev.h gets ev.patch's edit, so that EV_P works in a module.
        self.requires("libev/4.33#0aac1fcab07f7959366212337197f57c")
        self.requires("pcre2/10.49", force=True)
        # Header-only: built as a library under C++20 or later, fmt 12.2.0 also builds its C++ module, which doesn't
        # compile against libc++.
        self.requires("fmt/12.2.0", force=True, options={"header_only": True})
        self.requires("antlr4-cppruntime/4.13.2")
        # Shared: glib builds with the system's sysprof-capture when it finds it, and doesn't tell its users to link
        # that static library; a shared glib links it itself.
        self.requires("glib/2.86.5", options={"shared": True})
        self.requires("spdlog/1.17.0", options={"use_std_fmt": True})
        self.requires("nlohmann_json/3.12.0")
        self.requires("cairo/1.18.4")
        self.requires("pango/1.57.0")
        self.requires("di/1.3.2")
        # Newer versions of libraries that i3's dependencies require.
        for library in ("asio/1.38.2", "brotli/1.2.0", "elfutils/0.195", "expat/2.9.0", "fontconfig/2.17.1",
                        "freetype/2.14.3", "harfbuzz/12.3.0", "libffi/3.4.8", "libmount/2.40.2", "libpng/1.6.59",
                        "libxft/2.3.9", "pixman/0.46.4", "xorg-macros/1.20.0"):
            self.requires(library, override=True)
        # xcbpp opens i3's X connections. Its libxcb compatibility layer (a libxcb.so.1 on xcbpp's runtime) replaces
        # libxcb in i3's process, so that i3's libxcb calls and the C libraries share each connection with xcbpp.
        self.requires("xcbpp/3.0.0", options={"xcbcompat": True})


    def layout(self):
        cmake_layout(self)
