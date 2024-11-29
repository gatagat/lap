import distutils.cmd
from setuptools import Extension, setup

LICENSE = "BSD-2-Clause"
DESCRIPTION = "Linear Assignment Problem solver (LAPJV/LAPMOD)."
LONG_DESCRIPTION = open("README.md", encoding="utf-8").read()

package_name = "lap"
package_path = "lap"
_lapjv_src = "_lapjv_cpp"

def get_version_string():
    with open("lap/__init__.py") as version_file:
        for line in version_file.read().splitlines():
            if line.startswith('__version__'):
                delim = '"' if '"' in line else "'"
                return line.split(delim)[1]

def include_numpy():
    import numpy as np
    try:
        numpy_include = np.get_include()
    except AttributeError:
        numpy_include = np.get_numpy_include()
    return numpy_include

def compile_cpp(cython_file):
    """Compile a C++ file from Cython's pyx or py.
    """
    import os
    import subprocess
    cpp_file = os.path.splitext(cython_file)[0] + ".cpp"
    flags = ['--fast-fail', '--cplus']
    rc = subprocess.call(['cython'] + flags + ['-o', cpp_file, cython_file])
    if rc != 0:
        raise Exception('Cythonizing %s failed' % cython_file)
    else:
        return cpp_file

class ExportCythonCommand(distutils.cmd.Command):
    description = "Export _lapjv binary from source."
    def run(self):
        super().run()
        import os
        import shutil
        this_dir = os.path.dirname(os.path.realpath(__file__))
        lap = os.path.join(this_dir, package_path)
        _lapjv_src = os.path.join(this_dir, _lapjv_src)
        for file in os.listdir(_lapjv_src):
            if file[-2:].lower() in "soyd":
                print(f"Moving {_lapjv_src}/{file} to {lap}/")
                shutil.copy2(os.path.join(_lapjv_src, file), lap)
                break

def main_setup():
    """Use modern setup() by setuptools.
    """
    import os
    from Cython.Build import cythonize
    _lapjvpyx = os.path.join(_lapjv_src, "_lapjv.pyx")
    _lapjvcpp = compile_cpp(_lapjvpyx)
    lapjvcpp = os.path.join(_lapjv_src, "lapjv.cpp")
    lapmodcpp = os.path.join(_lapjv_src, "lapmod.cpp")

    ext_modules = [
        Extension(
            name='lap._lapjv',
            sources=[_lapjvcpp, lapjvcpp, lapmodcpp],
            include_dirs=[include_numpy(), _lapjv_src, package_path],
        )
    ]

    package_data = {}
    tests_package = package_path + ".tests"
    packages = [package_path, tests_package]
    for p in packages: package_data.update({p: ["*"]})

    setup(
        name=package_name,
        version=get_version_string(),
        description=DESCRIPTION,
        license=LICENSE,
        long_description=LONG_DESCRIPTION,
        long_description_content_type="text/markdown",
        keywords=['Linear Assignment', 'LAPJV', 'LAPMOD', 'lap', 'lapx'],
        url="https://github.com/gatagat/lap",
        author="gatagat, rathaROG, and co.",
        packages=packages,
        package_data=package_data,
        include_package_data=True,
        install_requires=['numpy>=1.21.6',],
        python_requires=">=3.7",
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
                     'Programming Language :: Python :: 3.12',
                     'Programming Language :: Python :: 3.13',
                     'Topic :: Education',
                     'Topic :: Education :: Testing',
                     'Topic :: Scientific/Engineering',
                     'Topic :: Scientific/Engineering :: Mathematics',
                     'Topic :: Software Development',
                     'Topic :: Scientific/Engineering',
                     'Topic :: Scientific/Engineering :: Artificial Intelligence',
                     'Topic :: Scientific/Engineering :: Image Recognition',
                     'Operating System :: Microsoft :: Windows',                                  
                     'Operating System :: POSIX',
                     'Operating System :: Unix',
                     'Operating System :: MacOS',],
        ext_modules=cythonize(ext_modules),
        cmdclass={'cmdexport': ExportCythonCommand,},
    )

if __name__ == "__main__":
    """
    Recommend using :py:mod:`build` to build the package as it does not 
    disrupt your current environment.

    >>> pip install wheel build
    >>> python -m build --sdist
    >>> python -m build --wheel
    """ 
    main_setup()
