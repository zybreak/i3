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


    def requirements(self):
        self.requires("zlib/1.2.13")
        self.requires("libev/4.33")
        self.requires("pcre2/10.42")
        self.requires("fmt/10.2.1", force=True)
        self.requires("antlr4-cppruntime/4.13.1")
        self.requires("glib/2.76.2")
        self.requires("gtest/1.13.0")
        self.requires("spdlog/1.13.0")
        self.requires("nlohmann_json/3.11.3")


    def layout(self):
        cmake_layout(self)
