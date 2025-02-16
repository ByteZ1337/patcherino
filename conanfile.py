from conan import ConanFile
from conan.tools.files import copy
from os import path


class Chatterino(ConanFile):
    name = "Chatterino"
    requires = "libavif/1.0.4"
    settings = "os", "compiler", "build_type", "arch"
    default_options = {
        "with_benchmark": False,
        "with_openssl3": True,
        "openssl*:shared": True,
        "boost*:header_only": True,
    }
    options = {
        "with_benchmark": [True, False],
        # Qt is built with OpenSSL 3 from version 6.5.0 onwards
        "with_openssl3": [True, False],
    }
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        if self.settings.os != "Windows":
            return

        self.requires("boost/1.85.0")
        if self.options.get_safe("with_benchmark", False):
            self.requires("benchmark/1.8.4")

        self.requires("openssl/3.2.2")

    def generate(self):
        def copy_bin(dep, selector, subdir):
            src = path.realpath(dep.cpp_info.bindirs[0])
            dst = path.realpath(path.join(self.build_folder, subdir))

            if src == dst:
                return

            copy(self, selector, src, dst, keep_path=False)

        for dep in self.dependencies.values():
            # macOS
            copy_bin(dep, "*.dylib", "bin")
            # Windows
            copy_bin(dep, "*.dll", "bin")
            copy_bin(dep, "*.dll", "Chatterino2")  # used in CI
            # Linux
            copy(
                self,
                "*.so*",
                dep.cpp_info.libdirs[0],
                path.join(self.build_folder, "bin"),
                keep_path=False,
            )
