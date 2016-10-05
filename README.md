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

  * Python (2.7 or 3.5)
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

Tested under Mac OS X 10.11 El Capitan (Python 2.7), Debian 7 (Python 2.7 and 3.5).

Tomas Kazmar, 2012-2016
