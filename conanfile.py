from conan import ConanFile;
import os
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain;

class i3(ConanFile):
    name = "i3"
    version = "1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    package_type = "application"


    def generate(self):
        tc = CMakeToolchain(self)
        tc.generator = "Ninja Multi-Config"
        tc.generate()


    def requirements(self):
        self.requires("zlib/1.2.13")
        self.requires("libev/4.33")
        self.requires("pcre2/10.42")
        self.requires("fmt/9.1.0", force=True)
        self.requires("antlr4-cppruntime/4.13.1")
        self.requires("glib/2.76.2")
        self.requires("gtest/1.13.0")
        self.requires("spdlog/1.13.0")


    def layout(self):
        cmake_layout(self)
