from conans import ConanFile, CMake, tools
from conans.util import files
import yaml

class RegisterConan(ConanFile):
    name = "register"
    version = str(yaml.load(tools.load("settings.yml"))['project']['version'])
    license = "MIT"
    url = "https://git.hiventive.com/framework/register"
    description = "Register"
    settings = "cppstd", "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "fPIE": [True, False]}
    default_options = "shared=False", "fPIC=False", "fPIE=False"
    generators = "cmake"
    exports = "settings.yml"
    exports_sources = "src/*", "CMakeLists.txt"
    requires = "gtest/1.8.0@bincrafters/stable", \
               "common/0.2.0@hiventive/testing", \
               "cci/1.0.0@hiventive/stable", \
               "module/0.2.0@hiventive/testing", \
               "communication/0.1.0@hiventive/testing"

    def _configure_cmake(self):
        cmake = CMake(self)
        if self.settings.os != "Windows":
            cmake.definitions["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC or self.options.fPIE
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
        self.copy("HVRegister", dst="include", src="src")
        self.copy("*.h", dst="include", src="src")
        self.copy("*.hpp", dst="include", src="src")

    def package_info(self):
        self.cpp_info.libs = ["register"]
