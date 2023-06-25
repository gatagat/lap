# Tomas Kazmar, 2012-2017, BSD 2-clause license, see LICENSE.

# Updated 2023/06/22 by rathaROG
# Force compiling with Python 3 
# cython: language_level=3

import numpy as np
cimport numpy as cnp
cimport cython
from libc.stdlib cimport malloc, free


cdef extern from "lapjv.h" nogil:
    ctypedef signed int int_t
    ctypedef unsigned int uint_t
    cdef int LARGE
    cdef enum fp_t:
        FP_1
        FP_2
        FP_DYNAMIC
    int lapjv_internal(
            const uint_t n,
            double *cost[],
            int_t *x,
            int_t *y)
    int lapmod_internal(
            const uint_t n,
            double *cc,
            uint_t *ii,
            uint_t *kk,
            int_t *x,
            int_t *y,
            fp_t fp_version)

LARGE_ = LARGE
FP_1_ = FP_1
FP_2_ = FP_2
FP_DYNAMIC_ = FP_DYNAMIC


@cython.boundscheck(False)
@cython.wraparound(False)
def lapjv(cnp.ndarray cost not None, char extend_cost=False,
          double cost_limit=np.inf, char return_cost=True):
    """Solve linear assignment problem using Jonker-Volgenant algorithm.

    Parameters
    ----------
    cost: (N,N) ndarray
        Cost matrix. Entry `cost[i, j]` is the cost of assigning row `i` to
        column `j`.
    extend_cost: bool, optional
        Whether or not extend a non-square matrix. Default: False.
    cost_limit: double, optional
        An upper limit for a cost of a single assignment. Default: `np.inf`.
    return_cost: bool, optional
        Whether or not to return the assignment cost.

    Returns
    -------
    opt: double
        Assignment cost. Not returned if `return_cost is False`.
    x: (N,) ndarray
        Assignment. `x[i]` specifies the column to which row `i` is assigned.
    y: (N,) ndarray
        Assignment. `y[j]` specifies the row to which column `j` is assigned.

    Notes
    -----
    For non-square matrices (with `extend_cost is True`) or `cost_limit` set
    low enough, there will be unmatched rows, columns in the solution `x`, `y`.
    All such entries are set to -1.
    """
    if cost.ndim != 2:
        raise ValueError('2-dimensional array expected')
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c'] cost_c = \
        np.ascontiguousarray(cost, dtype=np.double)
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c'] cost_c_extended
    cdef uint_t n_rows = cost_c.shape[0]
    cdef uint_t n_cols = cost_c.shape[1]
    cdef uint_t n = 0
    if n_rows == n_cols:
        n = n_rows
    else:
        if not extend_cost:
            raise ValueError(
                    'Square cost array expected. If cost is intentionally '
                    'non-square, pass extend_cost=True.')
    if extend_cost or cost_limit < np.inf:
        n = n_rows + n_cols
        cost_c_extended = np.empty((n, n), dtype=np.double)
        if cost_limit < np.inf:
            cost_c_extended[:] = cost_limit / 2.
        else:
            cost_c_extended[:] = cost_c.max() + 1
        cost_c_extended[n_rows:, n_cols:] = 0
        cost_c_extended[:n_rows, :n_cols] = cost_c
        cost_c = cost_c_extended

    cdef double **cost_ptr
    cost_ptr = <double **> malloc(n * sizeof(double *))
    cdef int i
    for i in range(n):
        cost_ptr[i] = &cost_c[i, 0]

    cdef cnp.ndarray[int_t, ndim=1, mode='c'] x_c = \
        np.empty((n,), dtype=np.int32)
    cdef cnp.ndarray[int_t, ndim=1, mode='c'] y_c = \
        np.empty((n,), dtype=np.int32)

    cdef int ret = lapjv_internal(n, cost_ptr, &x_c[0], &y_c[0])
    free(cost_ptr)
    if ret != 0:
        if ret == -1:
            raise MemoryError('Out of memory.')
        raise RuntimeError('Unknown error (lapjv_internal returned %d).' % ret)


    cdef double opt = np.nan
    if n != n_rows:
        x_c[x_c >= n_cols] = -1
        y_c[y_c >= n_rows] = -1
        x_c = x_c[:n_rows]
        y_c = y_c[:n_cols]
        if return_cost:
            opt = cost_c[np.nonzero(x_c != -1)[0], x_c[x_c != -1]].sum()
    elif return_cost:
        opt = cost_c[np.arange(n_rows), x_c].sum()

    if return_cost:
        return opt, x_c, y_c
    else:
        return x_c, y_c


@cython.boundscheck(False)
@cython.wraparound(False)
def _lapmod(
        const uint_t n,
        cnp.ndarray cc not None,
        cnp.ndarray ii not None,
        cnp.ndarray kk not None,
        fp_t fp_version=FP_DYNAMIC):
    """Internal function called from lapmod(..., fast=True)."""
    cdef cnp.ndarray[cnp.double_t, ndim=1, mode='c'] cc_c = \
        np.ascontiguousarray(cc, dtype=np.double)
    cdef cnp.ndarray[uint_t, ndim=1, mode='c'] ii_c = \
        np.ascontiguousarray(ii, dtype=np.uint32)
    cdef cnp.ndarray[uint_t, ndim=1, mode='c'] kk_c = \
        np.ascontiguousarray(kk, dtype=np.uint32)
    cdef cnp.ndarray[int_t, ndim=1, mode='c'] x_c = \
        np.empty((n,), dtype=np.int32)
    cdef cnp.ndarray[int_t, ndim=1, mode='c'] y_c = \
        np.empty((n,), dtype=np.int32)

    cdef int_t ret = lapmod_internal(
                n, &cc_c[0], &ii_c[0], &kk_c[0],
                &x_c[0], &y_c[0], fp_version)
    if ret != 0:
        if ret == -1:
            raise MemoryError('Out of memory.')
        raise RuntimeError('Unknown error (lapmod_internal returned %d).' % ret)

    return x_c, y_c
