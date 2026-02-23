from setuptools import Extension, setup, find_packages

LICENSE = "BSD-2-Clause"
DESCRIPTION = "Linear Assignment Problem solver (LAPJV/LAPMOD)."
LONG_DESCRIPTION = open("README.md", encoding="utf-8").read()

PACKAGE_NAME = "lap"
PACKAGE_PATH = "lap"
SRC_DIR = "_lapjv_cpp"

def get_version_string():
    with open("lap/__init__.py") as version_file:
        for line in version_file.read().splitlines():
            if line.startswith('__version__'):
                delim = '"' if '"' in line else "'"
                return line.split(delim)[1]
    raise RuntimeError('[!] Unable to find version string.')

def include_numpy():
    import numpy as np
    return np.get_include()

def main_setup():
    """Use modern setup() by setuptools.
    """
    import os
    from Cython.Build import cythonize
    _lapjvpyx = os.path.join(SRC_DIR, "_lapjv.pyx")
    lapjvcpp = os.path.join(SRC_DIR, "lapjv.cpp")
    lapmodcpp = os.path.join(SRC_DIR, "lapmod.cpp")

    ext_modules = [
        Extension(
            name='lap._lapjv',
            sources=[_lapjvpyx, lapjvcpp, lapmodcpp],
            include_dirs=[include_numpy(), SRC_DIR, PACKAGE_PATH],
            language="c++",
        )
    ]

    setup(
        name=PACKAGE_NAME,
        version=get_version_string(),
        description=DESCRIPTION,
        license=LICENSE,
        long_description=LONG_DESCRIPTION,
        long_description_content_type="text/markdown",
        keywords=['Linear Assignment', 'LAPJV', 'LAPMOD', 'lap', 'lapx'],
        url="https://github.com/gatagat/lap",
        author="gatagat, rathaROG, and co.",
        packages=find_packages(include=[PACKAGE_PATH, f"{PACKAGE_PATH}.*"]),
        include_package_data=True,
        install_requires=['numpy>=1.21.6',],
        python_requires=">=3.7",
        classifiers=['Development Status :: 4 - Beta',
                     'Environment :: Console',
                     'Intended Audience :: Developers',
                     'Intended Audience :: Education',
                     'Intended Audience :: Science/Research',
                     'Programming Language :: Python :: 3',
                     'Programming Language :: Python :: 3.7',
                     'Programming Language :: Python :: 3.8',
                     'Programming Language :: Python :: 3.9',
                     'Programming Language :: Python :: 3.10',
                     'Programming Language :: Python :: 3.11',
                     'Programming Language :: Python :: 3.12',
                     'Programming Language :: Python :: 3.13',
                     'Programming Language :: Python :: 3.14',
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
