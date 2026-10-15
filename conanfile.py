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
        self.requires("zlib/1.2.13")
        self.requires("libev/4.33")
        self.requires("pcre2/10.42")
        self.requires("fmt/12.1.0", force=True)
        self.requires("antlr4-cppruntime/4.13.1")
        self.requires("glib/2.78.3")
        self.requires("spdlog/1.16.0", options={"use_std_fmt": True})
        self.requires("nlohmann_json/3.12.0")
        self.requires("cairo/1.18.0")
        self.requires("pango/1.54.0")
        self.requires("di/1.3.0")
        self.requires("xpp/1.0.0#02e0c86b25ebbd654e0373212e3fe86a")


    def layout(self):
        cmake_layout(self)
