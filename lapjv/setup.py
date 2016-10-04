#!/usr/bin/env python

import os

base_path = os.path.abspath(os.path.dirname(__file__))

def configuration(parent_package='', top_path=None):
    from numpy.distutils.misc_util import Configuration, get_numpy_include_dirs
    from Cython.Build import cythonize
    config = Configuration('lapjv', parent_package, top_path)
    cythonize([os.path.join(base_path, '_lapjv.pyx')])
    source_files = ['_lapjv.c', 'internal/lap.cpp']
    config.add_extension('_lapjv', source_files, include_dirs=[get_numpy_include_dirs(), 'internal'])
    return config


if __name__ == '__main__':
    from numpy.distutils.core import setup
    setup(configuration=configuration,
          url='https://github.com/gatagat/lapjv',
          description='Python wrapper of LAPJV',
          author='Tomas Kazmar',
    )
