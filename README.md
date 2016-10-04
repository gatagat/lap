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
commercially, see lapjv/internal/README.md.


Installation
------------

#### Using pip
  ``` pip install git+git://github.com/gatagat/lapjv.git ```

#### Install from source

  1. Clone or extract lapjv and make sure there are two directories python-lapjv
     with the wrapper and lapjv with the C++ code.

  2. Under the root of the repo
     ```
     python setup.py build
     python setup.py install
     ```

Tested under Mac OS X 10.10 Yosemite, 10.11 El Capitan, Debian 7, and Ubuntu 14.04 only.

Tomas Kazmar, 2012-2016
