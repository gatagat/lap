from setuptools import Extension, setup
from numpy.distutils.misc_util import get_numpy_include_dirs


setup(
    ext_modules=[
        Extension(
            name="lap._lapjv",
            sources=["src/lap/_lapjv.cpp", "src/lap/lapjv.cpp", "src/lap/lapmod.cpp"],
            include_dirs=get_numpy_include_dirs() + ["src/lap"],
        ),
    ]
)
