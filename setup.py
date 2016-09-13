#!/usr/bin/env python

def configuration(parent_package='', top_path=None):
    from numpy.distutils.misc_util import Configuration
    config = Configuration(None, parent_package, top_path)
    config.set_options(
        ignore_setup_xxx_py=True,
        assume_default_configuration=True,
        delegate_options_to_subpackages=True,
        quiet=False)
    config.add_subpackage('lapjv')
    return config

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

if __name__ == "__main__":
    from numpy.distutils.core import setup
    setup(configuration=configuration,
          name="python-lapjv",
          version=VERSION,
          url='https://github.com/gatagat/lapjv',
          description='Python wrapper of LAPJV',
          author='Tomas Kazmar',
          setup_requires=['cython>=0.21'],
          install_requires=INSTALL_REQUIRES,
          requires=REQUIRES,
    )
