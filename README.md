[![Travis](https://travis-ci.org/gatagat/lapjv.svg?branch=master)](https://travis-ci.org/gatagat/lapjv/)
[![Appveyor](https://ci.appveyor.com/api/projects/status/github/gatagat/lapjv?branch=master&svg=true)](https://ci.appveyor.com/project/gatagat/lapjv/history)
![Python 2.7](https://img.shields.io/badge/python-2.7-blue.svg)
![Python 3.5](https://img.shields.io/badge/python-3.5-blue.svg)
![Python 3.6](https://img.shields.io/badge/python-3.6-blue.svg)

python-lapjv - LAPJV/LAPMOD implementation
==========================================

LAPJV is an algorithm by Jonker and Volgenant to solve the linear assignment
problem.  This repository contains two versions of the algorithm for dense
(LAPJV [1]) and sparse (LAPMOD [2]) matrices.

*News*: The newly added LAPMOD algorithm was implemented based on the papers [1,2] and the public
domain Pascal implementation provided by A. Volgenant [3]. The original LAPJV
code ([branch old](/gatagat/lapjv/tree/old)) was completely replaced by a version derived from the new
LAPMOD code. As a result the new LAPJV code is faster and as far as I can tell
there should be no restrictions on using it for commercial purposes.

In my tests the LAPMOD implementation seems to be faster than the LAPJV implementation for matrices with a
side of more than ~5000 and with less than 50% finite coefficients.

[1] R. Jonker and A. Volgenant, "A Shortest Augmenting Path Algorithm for Dense
and Sparse Linear Assignment Problems", Computing 38, 325-340 (1987)<br>
[2] A. Volgenant, "Linear and Semi-Assignment Problems: A Core Oriented
Approach", Computer Ops Res. 23, 917-932 (1996)<br>
[3] http://www.assignmentproblems.com/LAPJV.htm

License
-------

BSD 2-clause.


Installation
------------

#### Dependencies

Python-lapjv requires:

  * Python (2.7, 3.5, 3.6)
  * NumPy (>=1.10.1)
  * Cython (>=0.21) - to compile the wrapper
  * pytest and pytest-timeout - for testing

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
