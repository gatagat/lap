#!/usr/bin/env python

import os
import sys

try:
    from setuptools import setup
    _have_setuptools = True
except ImportError:
    # no setuptools installed
    from distutils.core import setup
    _have_setuptools = False

if any([arg.startswith('bdist') for arg in sys.argv]):
    if not _have_setuptools:
        raise ImportError('Building binary distributions requires setuptools, please "pip install setuptools".')

from distutils.version import LooseVersion
from distutils.extension import Extension

min_cython_ver = '0.19.1'
try:
    import Cython
    from Cython.Build import cythonize
    ver = Cython.__version__
    if ver < LooseVersion(min_cython_ver):
        raise ImportError('Installed cython is too old, please "pip install -U cython".')
except ImportError:
    raise ImportError('Building lapjv requires cython, please "pip install cython".')

with open('lapjv/__init__.py') as fid:
    for line in fid:
        if line.startswith('__version__'):
            VERSION = line.strip().split()[-1][1:-1]
            break

with open('requirements.txt') as fid:
    INSTALL_REQUIRES = [l.strip() for l in fid.readlines() if l]
REQUIRES = [r.replace('>=', ' (>= ') + ')' for r in INSTALL_REQUIRES]
REQUIRES = [r.replace('==', ' (== ') for r in REQUIRES]
REQUIRES = [r.replace('[array]', '') for r in REQUIRES]

# Do not try to upgrade libraries that are already installed.
for lib in ['numpy']:
    try:
        __import__(lib)
        INSTALL_REQUIRES = [i for i in INSTALL_REQUIRES if lib not in i]
    except ImportError:
        pass

CLASSIFIERS = [
    'Development Status :: 4 - Beta',
    'Environment :: Console',
    'Operating System :: OS Independent',
    'Intended Audience :: Science/Research',
    'Programming Language :: Python',
    'Programming Language :: Python :: 2',
    'Programming Language :: Python :: 3',
    'Programming Language :: Python :: 2.7',
    'Programming Language :: Python :: 3.5',
    'Programming Language :: Cython',
    'Topic :: Scientific/Engineering',
]

LONG_DESCRIPTION = """
**python-lapjv** is a Python wrapper around a linear assignment problem solver
by Jonker and Volgenant (LAPJV).

The wrapper code is under a 2-clause BSD license, however, the original code is
under a special restrictive license. Please check the lapjv/interal/README.md
for details on whether you are allowed to use it.
"""

base_path = os.path.abspath(os.path.dirname(__file__))
source_files = [os.path.join(base_path, 'lapjv', '_lapjv.pyx'), os.path.join(base_path, 'lapjv', 'internal', 'lap.c')]
import pkg_resources
numpy_incl = pkg_resources.resource_filename('numpy', 'core/include')
lapjv_ext = Extension('lapjv._lapjv', source_files, include_dirs=[numpy_incl, 'lapjv/internal'])
lapjv_ext = cythonize(lapjv_ext)
#lapjv_ext = cythonize('lapjv/_lapjv.pyx', sources=['lapjv/internal/lap.c'], include_dirs=[numpy_incl])

setup(name="python-lapjv",
      version=VERSION,
      url='https://github.com/gatagat/lapjv',
      description='Python wrapper of LAPJV',
      long_description=LONG_DESCRIPTION,
      license='BSD (wrapper) + restrictive license (LAPJV)',
      author='Tomas Kazmar',
      install_requires=INSTALL_REQUIRES,
      requires=REQUIRES,
      packages=['lapjv'],
      ext_modules=lapjv_ext,
      classifiers=CLASSIFIERS,
      platforms=['any']
)
