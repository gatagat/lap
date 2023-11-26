#!/usr/bin/env python

import os
from pkg_resources import parse_version
import shutil
import subprocess
import sys
import traceback

if sys.version_info[0] < 3:
    import __builtin__ as builtins
else:
    import builtins
builtins.__LAP_SETUP__ = True

from distutils.command.clean import clean as Clean

class CleanCommand(Clean):
    description = "Remove build artifacts from the source tree"

    def run(self):
        Clean.run(self)
        if os.path.exists('build'):
            shutil.rmtree('build')
        # Remove c files if we are not within a sdist package
        cwd = os.path.abspath(os.path.dirname(__file__))
        remove_c_files = not os.path.exists(os.path.join(cwd, 'PKG-INFO'))
        if remove_c_files:
            if os.path.exists('lap/_lapjv.cpp'):
                os.unlink('lap/_lapjv.cpp')
        for dirpath, dirnames, filenames in os.walk('lap'):
            for filename in filenames:
                if any(filename.endswith(suffix) for suffix in
                       (".so", ".pyd", ".dll", ".pyc")):
                    os.unlink(os.path.join(dirpath, filename))
            for dirname in dirnames:
                if dirname == '__pycache__':
                    shutil.rmtree(os.path.join(dirpath, dirname))

cmdclass = {'clean': CleanCommand}


from distutils.version import LooseVersion

def cythonize(cython_file, gen_file):
    try:
        from Cython.Compiler.Version import version as cython_version
        if LooseVersion(cython_version) < LooseVersion('0.21'):
            raise ImportError('Installed cython is too old (0.21 required), '
                              'please "pip install -U cython".')
    except ImportError:
        raise ImportError('Building lapjv requires cython, '
                          'please "pip install cython".')
        pass

    flags = ['--fast-fail']
    if gen_file.endswith('.cpp'):
        flags += ['--cplus']

    try:
        try:
            rc = subprocess.call(['cython'] +
                                 flags + ["-o", gen_file, cython_file])
            if rc != 0:
                raise Exception('Cythonizing %s failed' % cython_file)
        except OSError:
            # There are ways of installing Cython that don't result in a cython
            # executable on the path, see scipy issue gh-2397.
            rc = subprocess.call([sys.executable, '-c',
                                  'import sys; from Cython.Compiler.Main '
                                  'import setuptools_main as main;'
                                  ' sys.exit(main())'] + flags +
                                 ["-o", gen_file, cython_file])
            if rc != 0:
                raise Exception('Cythonizing %s failed' % cython_file)
    except OSError:
        raise OSError('Cython needs to be installed')


def get_wrapper_pyx():
    return os.path.join('lap', '_lapjv.pyx')


def generate_cython():
    wrapper_pyx_file = get_wrapper_pyx()
    wrapper_c_file = os.path.splitext(wrapper_pyx_file)[0] + '.cpp'
    cythonize(wrapper_pyx_file, wrapper_c_file)


def configuration(parent_package='', top_path=None):
    from numpy.distutils.misc_util import Configuration, get_numpy_include_dirs

    config = Configuration(None, parent_package, top_path)

    config.set_options(
        ignore_setup_xxx_py=True,
        assume_default_configuration=True,
        delegate_options_to_subpackages=True,
        quiet=True)

    config.add_data_dir('lap/tests')

    wrapper_pyx_file = get_wrapper_pyx()
    wrapper_c_file = os.path.splitext(wrapper_pyx_file)[0] + '.cpp'
    c_files = [
            os.path.join(os.path.dirname(wrapper_pyx_file), 'lapjv.cpp'),
            os.path.join(os.path.dirname(wrapper_pyx_file), 'lapmod.cpp')]
    config.add_extension('lap._lapjv', sources=[wrapper_c_file, c_files],
                         include_dirs=[get_numpy_include_dirs(), 'lap'])

    return config


def setup_package():
    metadata = dict(
        packages=['lap'],
        cmdclass=cmdclass,
    )

    if len(sys.argv) == 1 or (
            len(sys.argv) >= 2 and ('--help' in sys.argv[1:] or
                                    sys.argv[1] in ('--help-commands',
                                                    'egg_info',
                                                    '--version',
                                                    'clean'))):
        try:
            from setuptools import setup
        except ImportError:
            from distutils.core import setup
    else:
        from numpy.distutils.core import setup
        metadata['configuration'] = configuration

        if len(sys.argv) >= 2 and sys.argv[1] not in 'config':
            print('Generating cython files')
            cwd = os.path.abspath(os.path.dirname(__file__))
            if not os.path.exists(os.path.join(cwd, 'PKG-INFO')):
                generate_cython()

    setup(**metadata)


if __name__ == "__main__":
    setup_package()