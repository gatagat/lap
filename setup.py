import numpy
from setuptools import setup, find_packages
from setuptools.extension import Extension
from Cython.Distutils import build_ext

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

ext_modules = [Extension(
    name='lapjv',
    sources=["./python-lapjv/python_lapjv.pyx", "./python-lapjv/wrap_lapjv.cpp", "./lapjv/lap.cpp"],
    include_dirs=[numpy.get_include(), "./lapjv/"],
    language="c++"
    )]

setup(
    description='Python wrapper of LAPJV',
    author='Tomas Kazmar',
    url='https://github.com/gatagat/lapjv',
    author_email='',
    version='0.03',
    install_requires=[],
    packages=find_packages(),
    scripts=[],
    name="python-lapjv",
    cmdclass = {'build_ext': build_ext},
    ext_modules = ext_modules,
)
