import os

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


class SloptourConan(ConanFile):
    name = "sloptour"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):
        self.requires("ftxui/6.1.9")
        self.requires("reproc/14.2.5")
        self.requires("cli11/2.5.0")
        self.requires("nlohmann_json/3.12.0")

    def build_requirements(self):
        self.test_requires("gtest/1.17.0")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        CMakeDeps(self).generate()
        toolchain = CMakeToolchain(self, generator="Ninja")
        if os.getenv("SLOPTOUR_NO_USER_PRESETS"):
            toolchain.user_presets_path = False
        toolchain.generate()
