[![Travis](https://travis-ci.org/gatagat/lapjv.svg?branch=master)](https://travis-ci.org/gatagat/lapjv/)
[![Appveyor](https://ci.appveyor.com/api/projects/status/github/gatagat/lapjv?branch=master&svg=true)](https://ci.appveyor.com/project/gatagat/lapjv/history)
![Python 2.7](https://img.shields.io/badge/python-2.7-blue.svg)
![Python 3.5](https://img.shields.io/badge/python-3.5-blue.svg)
![Python 3.6](https://img.shields.io/badge/python-3.6-blue.svg)

python-lapjv - Python wrapper of LAPJV
======================================

LAPJV is an algorithm by Jonker and Volgenant [1] to solve the linear
assignment problem. This repository contains the code by Jonker
(lapjv/internal) and a Python wrapper (lapjv).

[1] R. Jonker and A. Volgenant (University of Amsterdam)
"A Shortest Augmenting Path Algorithm for Dense and Sparse Linear Assignment
 Problems", Computing 38, 325-340 (1987)


License
-------

BSD license applies only to the wrapper, the original code cannot be used
commercially, see [lapjv/internal/README.md](./lapjv/internal/README.md).


Installation
------------

#### Dependencies

Python-lapjv requires:

  * Python (2.7, 3.5, 3.6)
  * NumPy (>=1.10.1)
  * Cython (>=0.21) - to compile the wrapper

#### Using pip

    pip install git+git://github.com/gatagat/lapjv.git

#### Install from source

  1. Clone

         git clone https://github.com/gatagat/lapjv.git

  2. Under the root of the repo

         python setup.py build
         python setup.py install

Tested under Linux, OS X, Windows.

Tomas Kazmar, 2012-2017
