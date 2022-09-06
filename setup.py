from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


class build_ext_lap(build_ext):
    def finalize_options(self):
        build_ext.finalize_options(self)
        if isinstance(__builtins__, dict):
            __builtins__["__NUMPY_SETUP__"] = False
        else:
            setattr(__builtins__, "__NUMPY_SETUP__", False)
        import numpy
        self.numpy_include = numpy.get_include()
        # TODO: Do we need LAPACK/BLAS settings?

    def build_extension(self, ext):
        if not getattr(ext, "include_dirs", None):
            setattr(ext, "include_dirs", [])
        getattr(ext, "include_dirs").append(self.numpy_include)

        return build_ext.build_extension(self, ext)


if __name__ == "__main__":
    setup(
        ext_modules=[
            Extension(
                name="lap._lapjv",
                sources=["src/lap/_lapjv.cpp", "src/lap/lapjv.cpp", "src/lap/lapmod.cpp"],
                include_dirs=["src/lap"],
            ),
        ],
        cmdclass={"build_ext": build_ext_lap},
    )
