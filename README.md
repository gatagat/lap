[![Travis](https://travis-ci.org/gatagat/lap.svg?branch=master)](https://travis-ci.org/gatagat/lap/)
[![Appveyor](https://ci.appveyor.com/api/projects/status/github/gatagat/lap?branch=master&svg=true)](https://ci.appveyor.com/project/gatagat/lap/history)
![Python 2.7](https://img.shields.io/badge/python-2.7-blue.svg)
![Python 3.6](https://img.shields.io/badge/python-3.6-blue.svg)
![Python 3.7](https://img.shields.io/badge/python-3.7-blue.svg)

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

  * Python (2.7, 3.6, 3.7)
  * NumPy (>=1.10.1)
  * Cython (>=0.21) - to compile the wrapper
  * SciPy, pytest, pytest-timeout - only for testing

#### Install using pip

You can install the latest release of lap from PyPI (recommended):

    pip install lap
    
Alternatively, you can install lap directly from the repository:

    pip install git+git://github.com/gatagat/lap.git
    
#### Install from source

  1. Clone

         git clone https://github.com/gatagat/lap.git

  2. Under the root of the repo

         python setup.py build
         python setup.py install

Tested under Linux, OS X, Windows.

### Usage

```
cost, x, y = lap.lapjv(C)
```

The function `lapjv(C)` returns the assignment cost (`cost`) and two arrays, `x, y`. If cost matrix `C` has shape N x M, then `x` is a size-N array specifying to which column is row is assigned, and `y` is a size-M array specifying to which row each column is assigned. For example, an output of `x = [1, 0]` indicates that row 0 is assigned to column 1 and row 1 is assigned to column 0. Similarly, an output of `x = [2, 1, 0]` indicates that row 0 is assigned to column 2, row 1 is assigned to column 1, and row 2 is assigned to column 0.

Note that this function *does not* return the assignment matrix (as done by scipy's [`linear_sum_assignment`](https://docs.scipy.org/doc/scipy-0.18.1/reference/generated/scipy.optimize.linear_sum_assignment.html) and lapsolver's [`solve dense`](https://github.com/cheind/py-lapsolver)). The assignment matrix can be constructed from `x` as follows:
```
A = np.zeros((N, M))
for i in range(N):
    A[i, x[i]] = 1
```
Equivalently, we could construct the assignment matrix from `y`:
```
A = np.zeros((N, M))
for j in range(M):
    A[y[j], j] = 1
```

Finally, note that the outputs are redundant: we can construct `x` from `y`, and vise versa:
```
x = [np.where(y == i)[0][0] for i in range(N)]
y = [np.where(x == j)[0][0] for j in range(M)]
```

License
-------

Released under the 2-clause BSD license, see `LICENSE`.

Copyright (C) 2012-2017, Tomas Kazmar
