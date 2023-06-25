# # # # # # # # # # # # # # # # # # # # # #
#    Rewrite on 2023/06/24 by rathaROG    #
# # # # # # # # # # # # # # # # # # # # # #


import distutils.cmd
from setuptools import setup
from setuptools.extension import Extension

###################################################################

DESCRIPTION = "Customized Tomas Kazmar's lap, Linear Assignment Problem solver (LAPJV/LAPMOD)."
LICENSE = 'BSD-2-Clause'
LONG_DESCRIPTION = """
**lap** is a linear assignment problem solver using Jonker-Volgenant
algorithm for dense (LAPJV) or sparse (LAPMOD) matrices.
"""

###################################################################

package_name = 'lapx'
package_path = 'lap'
_lapjv = "_lapjv_src"
requirments_txt = "requirements.txt"

###################################################################

def get_version_string():
    version_py = "lap/__init__.py"
    with open(version_py) as version_file:
        for line in version_file.read().splitlines():
            if line.startswith('__version__'):
                delim = '"' if '"' in line else "'"
                return line.split(delim)[1]

def numpy_include():
    import numpy as np
    try:
        numpy_include = np.get_include()
    except AttributeError:
        numpy_include = np.get_numpy_include()
    return numpy_include

def compile_cpp(cython_file):
    """Compile cpp from Cython's pyx or py.
    """
    import os
    import subprocess
    cpp_file = os.path.splitext(cython_file)[0] + '.cpp'
    flags = ['--fast-fail', '--cplus']
    rc = subprocess.call(['cython'] + flags + ["-o", cpp_file, cython_file])
    if rc != 0: raise Exception('Cythonizing %s failed' % cython_file)
    else: return cpp_file

class ExportCythonCommand(distutils.cmd.Command):
    description = 'Export _lapjv binary'
    def run(self):
        super().run()
        import os
        import shutil
        this_dir = os.path.dirname(os.path.realpath(__file__))
        lap = os.path.join(this_dir, "lap")
        _lapjv = os.path.join(this_dir, "_lapjv_src")
        for file in  os.listdir(_lapjv):
            if file[-2:].lower() in "soyd":
                shutil.copy2(os.path.join(_lapjv, file), lap)
                break

def main_setup():
    """Use modern setup() 
    """

    import os
    from Cython.Build import cythonize
    _lapjvpyx = os.path.join(_lapjv, '_lapjv.pyx')
    _lapjvcpp = compile_cpp(_lapjvpyx)
    lapjvcpp = os.path.join(_lapjv, 'lapjv.cpp')
    lapmodcpp = os.path.join(_lapjv, 'lapmod.cpp')

    ext_modules = [
        Extension(
            name='lap._lapjv',
            sources=[_lapjvcpp, lapjvcpp, lapmodcpp],
            include_dirs=[numpy_include(), package_path],
        )
    ]

    package_data = {}
    tests_package = package_path + ".tests"
    packages = [package_path, tests_package]
    for p in packages: package_data = {p: ["*"]}

    setup(
        name=package_name,
        version=get_version_string(),
        description=DESCRIPTION,
        long_description=LONG_DESCRIPTION,
        long_description_content_type="text/plain",
        author='rathaROG',
        url='https://github.com/rathaROG/lapx',
        license=LICENSE,
        packages=packages,
        package_data=package_data,
        include_package_data=True,
        keywords=['Linear Assignment', 'LAPJV', 'LAPMOD', 'lap', 'lap05'],
        install_requires=read_requirments(),
        classifiers=['Development Status :: 4 - Beta',
                     'Environment :: Console',
                     'Intended Audience :: Developers',
                     'Intended Audience :: Education',
                     'Intended Audience :: Science/Research',
                     'License :: OSI Approved :: BSD License',
                     'Programming Language :: Python :: 3',
                     'Programming Language :: Python :: 3.7',
                     'Programming Language :: Python :: 3.8',
                     'Programming Language :: Python :: 3.9',
                     'Programming Language :: Python :: 3.10',
                     'Programming Language :: Python :: 3.11',
                     'Topic :: Software Development',
                     'Topic :: Scientific/Engineering',
                     'Topic :: Scientific/Engineering :: Artificial Intelligence',
                     'Topic :: Scientific/Engineering :: Image Recognition',
                     'Operating System :: Microsoft :: Windows',                                  
                     'Operating System :: POSIX',
                     'Operating System :: Unix',
                     'Operating System :: MacOS',],
        ext_modules=cythonize(ext_modules),
        cmdclass={'ccythonize': ExportCythonCommand,},
    )

def read_requirments():
    with open(requirments_txt) as input_tmp_file:
        return [line for line in input_tmp_file.read().splitlines()]

if __name__ == "__main__":
    """
    Recommend using :py:mod:`build` to build the package as it does not 
    mess up your current enviroment.

    >>> pip install wheel build
    >>> python -m build --sdist
    >>> python -m build --wheel
    """ 
    main_setup()
