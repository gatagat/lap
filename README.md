[![Test Simple](https://github.com/gatagat/lap/actions/workflows/test_simple.yaml/badge.svg)](https://github.com/gatagat/lap/actions/workflows/test_simple.yaml)
[![Benchmark](https://github.com/gatagat/lap/actions/workflows/benchmark.yaml/badge.svg)](https://github.com/gatagat/lap/actions/workflows/benchmark.yaml)
[![Test PyPI Build](https://github.com/gatagat/lap/actions/workflows/prepublish.yaml/badge.svg)](https://github.com/gatagat/lap/actions/workflows/prepublish.yaml)
[![Publish to PyPI](https://github.com/gatagat/lap/actions/workflows/publish.yaml/badge.svg)](https://github.com/gatagat/lap/actions/workflows/publish.yaml)

# lap: Linear Assignment Problem Solver

[`lap`](https://github.com/gatagat/lap) is a [linear assignment
problem](https://en.wikipedia.org/wiki/Assignment_problem) solver using
Jonker-Volgenant algorithm for dense LAPJV¹ or sparse LAPMOD² matrices. Both
algorithms are implemented from scratch based solely on the papers¹˒² and the
public domain Pascal implementation provided by A. Volgenant³. The LAPMOD
implementation seems to be faster than the LAPJV implementation for matrices
with a side of more than ~5000 and with less than 50% finite coefficients.

<sup>¹ R. Jonker and A. Volgenant, "A Shortest Augmenting Path Algorithm for
Dense and Sparse Linear Assignment Problems", Computing 38, 325-340 (1987)
</sup><br> <sup>² A. Volgenant, "Linear and Semi-Assignment Problems: A Core
Oriented Approach", Computer Ops Res. 23, 917-932 (1996) </sup><br> <sup>³
http://www.assignmentproblems.com/LAPJV.htm |
[[archive.org](https://web.archive.org/web/20220221010749/http://www.assignmentproblems.com/LAPJV.htm)]
</sup><br>

## 💽 Installation

### Install from [PyPI](https://pypi.org/project/lap/):

[![PyPI version](https://badge.fury.io/py/lap.svg)](https://badge.fury.io/py/lap)
[![Downloads](https://static.pepy.tech/badge/lap)](https://pepy.tech/project/lap)
[![Downloads](https://static.pepy.tech/badge/lap/month)](https://pepy.tech/project/lap)

```
pip install lap
```

| **Pre-built Wheels** 🛞 | **Windows** ✅ | **Linux** ✅ | **macOS** ✅ |
|:---:|:---:|:---:|:---:|
| Python 3.7 | AMD64 | x86_64/aarch64 | x86_64 |
| Python 3.8 | AMD64 | x86_64/aarch64 | x86_64/arm64 |
| Python 3.9-3.13 ¹ | AMD64/ARM64 ² | x86_64/aarch64 | x86_64/arm64 |

<sup>¹ v0.5.12 supports both numpy 1.x and 2.x for Python 3.8-3.13. 🆕 </sup><br>
<sup>² Windows ARM64 is experimental.</sup><br>

<details><summary>Other options</summary>

### Install from GitHub repo (requires C++ compiler):

```
pip install git+https://github.com/gatagat/lap.git
```

### Build and install (requires C++ compiler):

```
git clone https://github.com/gatagat/lap.git
cd lap
pip install "setuptools>=67.8.0"
pip install wheel build
python -m build --wheel
cd dist
```

</details>

## 🧪 Usage

```
import lap
import numpy as np
print(lap.lapjv(np.random.rand(4, 5), extend_cost=True))
```

<details><summary>More details</summary>

### `cost, x, y = lap.lapjv(C)`

The function `lapjv(C)` returns the assignment cost `cost` and two arrays `x`
and `y`. If cost matrix `C` has shape NxM, then `x` is a size-N array
specifying to which column each row is assigned, and `y` is a size-M array
specifying to which row each column is assigned. For example, an output of `x =
[1, 0]` indicates that row 0 is assigned to column 1 and row 1 is assigned to
column 0. Similarly, an output of `x = [2, 1, 0]` indicates that row 0 is
assigned to column 2, row 1 is assigned to column 1, and row 2 is assigned to
column 0.

Note that this function *does not* return the assignment matrix (as done by
scipy's
[`linear_sum_assignment`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.linear_sum_assignment.html)
and lapsolver's [`solve dense`](https://github.com/cheind/py-lapsolver)). The
assignment matrix can be constructed from `x` as follows: ``` A = np.zeros((N,
M)) for i in range(N): A[i, x[i]] = 1 ```

Equivalently, we could construct the assignment matrix from `y`:
```
A = np.zeros((N, M))
for j in range(M):
    A[y[j], j] = 1
```

Finally, note that the outputs are redundant: we can construct `x` from `y`,
and vise versa:
```
x = [np.where(y == i)[0][0] for i in range(N)]
y = [np.where(x == j)[0][0] for j in range(M)]
```

</details>

# License

Released under the 2-clause BSD license, see [LICENSE](./LICENSE).

Copyright (C) 2012-2025, Tomas Kazmar

Contributors (in alphabetic order):
- Benjamin Eysenbach
- Léo Duret
- Pieter Lenaerts
- Raphael Reme
- Ratha Siv
- Robert Wen
- Steven
- Tom White
- Tomas Kazmar
- Wok
