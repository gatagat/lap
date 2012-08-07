import numpy
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension(
    name='lapjv',
    sources=["python_lapjv.pyx", "wrap_lapjv.cpp", "../lapjv/lap.cpp"],
    include_dirs = [numpy.get_include(), "../lapjv/"],
    language="c++"
    )]

setup(
    name = 'lapjv',
    cmdclass = {'build_ext': build_ext},
    ext_modules = ext_modules,
    )
