[![Travis](https://travis-ci.org/gatagat/lap.svg?branch=master)](https://travis-ci.org/gatagat/lap/)
[![Appveyor](https://ci.appveyor.com/api/projects/status/github/gatagat/lap?branch=master&svg=true)](https://ci.appveyor.com/project/gatagat/lap/history)
![Python 2.7](https://img.shields.io/badge/python-2.7-blue.svg)
![Python 3.6](https://img.shields.io/badge/python-3.6-blue.svg)

lap: Linear Assignment Problem solver
=====================================

**lap** is a [linear assignment
problem](https://en.wikipedia.org/wiki/Assignment_problem) solver using
Jonker-Volgenant algorithm for dense (LAPJV [1]) or sparse (LAPMOD [2])
matrices.

Both algorithms are implemented from scratch based solely on the papers [1,2]
and the public domain Pascal implementation provided by A. Volgenant [3].

In my tests the LAPMOD implementation seems to be faster than the LAPJV
implementation for matrices with a side of more than ~5000 and with less than
50% finite coefficients.

[1] R. Jonker and A. Volgenant, "A Shortest Augmenting Path Algorithm for Dense
and Sparse Linear Assignment Problems", Computing 38, 325-340 (1987)<br>
[2] A. Volgenant, "Linear and Semi-Assignment Problems: A Core Oriented
Approach", Computer Ops Res. 23, 917-932 (1996)<br>
[3] http://www.assignmentproblems.com/LAPJV.htm

Installation
------------

#### Dependencies

lap requires:

  * Python (2.7, 3.6)
  * NumPy (>=1.10.1)
  * Cython (>=0.21) - to compile the wrapper
  * SciPy, pytest, pytest-timeout - only for testing

#### Using pip

    pip install git+git://github.com/gatagat/lap.git

#### Install from source

  1. Clone

         git clone https://github.com/gatagat/lap.git

  2. Under the root of the repo

         python setup.py build
         python setup.py install

Tested under Linux, OS X, Windows.


License
-------

Released under the 2-clause BSD license, see `LICENSE`.

Copyright (C) 2012-2017, Tomas Kazmar
