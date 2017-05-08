# Tomas Kazmar, 2012-2017, BSD 2-clause license, see LICENSE.

import numpy as np
cimport numpy as cnp
cimport cython
from libc.stdlib cimport malloc, free

IF UNAME_SYSNAME == "Windows":
    cdef extern from "int32t.h" nogil:
        ctypedef signed int int32_t;
ELSE:
    from libc.stdint cimport int32_t

cpdef enum:
    FP_1 = 1
    FP_2 = 2
    FP_DYNAMIC = 3

cdef extern from "lapjv.h" nogil:
    int lapjv_internal(
            const int n,
            double *cost[],
            int32_t *x,
            int32_t *y)
    int lapmod_internal(
            const int n,
            double *cc,
            int32_t *ii,
            int32_t *kk,
            int32_t *x,
            int32_t *y,
            int fp_version)


@cython.boundscheck(False)
def lapjv(cnp.ndarray cost not None, char extend_cost=False,
          double cost_limit=np.inf, char return_cost=True):
    '''
    Solve linear assignment problem using Jonker-Volgenant algorithm.

    cost: (square) matrix containing the assignment costs
    extend_cost: whether or not extend a non-square matrix [default: False]
    cost_limit: an upper limit for a cost of a single assignment
                [default: np.inf]

    Returns (lapcost, rowsol, colsol) where:
      lapcost: cost of the assignment
      rowsol:  vector of columns assigned to rows
      colsol:  vector of rows assigned to columns

    When extend_cost and/or cost_limit is set, all unmatched entries will be
    marked by -1 in rowsol/colsol.
    '''
    if cost.ndim != 2:
        raise ValueError('2-dimensional array expected')
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c', negative_indices=False] cost_c = \
        np.ascontiguousarray(cost, dtype=np.double)
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c'] cost_c_extended
    cdef int N = max(cost_c.shape[0], cost_c.shape[1])
    if cost_c.shape[0] != cost_c.shape[1]:
        if not extend_cost:
            raise ValueError('Square cost array expected. If intentional, '
                             'pass extend_cost=True.')
        if cost_limit < np.inf:
            cost_c_extended = np.empty((2*N, 2*N), dtype=np.double)
            cost_c_extended[:] = cost_limit
            cost_c_extended[:N, :N] = cost_c.max() + cost_limit + 1
        else:
            cost_c_extended = np.empty((N, N), dtype=np.double)
            cost_c_extended[:] = cost_c.max() + 1
        cost_c_extended[:cost_c.shape[0], :cost_c.shape[1]] = cost_c
        cost_c = cost_c_extended
    elif cost_limit < np.inf:
        cost_c_extended = np.empty((2*N, 2*N), dtype=np.double)
        cost_c_extended[:] = cost_limit
        cost_c_extended[:cost_c.shape[0], :cost_c.shape[1]] = cost_c
        cost_c = cost_c_extended

    cdef int i
    cdef double **cost_ptr
    cost_ptr = <double **> malloc(cost_c.shape[0] * sizeof(double *))
    for i in range(cost_c.shape[0]):
        cost_ptr[i] = &cost_c[i, 0]

    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] x_c = \
        np.empty((cost_c.shape[0],), dtype=np.int32)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] y_c = \
        np.empty((cost_c.shape[0],), dtype=np.int32)

    cdef int ret = lapjv_internal(cost_c.shape[0], cost_ptr, &x_c[0], &y_c[0])
    free(cost_ptr)
    if ret != 0:
        if ret == -1:
            raise MemoryError('Out of memory.')
        raise RuntimeError('Unknown error (lapjv_internal returned %d).' % ret)


    cdef double opt
    if cost_limit < np.inf or extend_cost:
        x_c[x_c >= cost.shape[1]] = -1
        y_c[y_c >= cost.shape[0]] = -1
        x_c = x_c[:cost.shape[0]]
        y_c = y_c[:cost.shape[1]]
        if return_cost:
            opt = cost_c[np.nonzero(x_c != -1)[0], x_c[x_c != -1]].sum()
    elif return_cost:
        opt = cost_c[np.arange(N), x_c].sum()

    if return_cost:
        return opt, x_c, y_c
    else:
        return x_c, y_c


@cython.boundscheck(False)
def _lapmod(
        const int32_t n,
        cnp.ndarray cc not None,
        cnp.ndarray ii not None,
        cnp.ndarray kk not None,
        char extend_cost=False,
        double cost_limit=np.inf,
        int fp_version=FP_DYNAMIC):
    '''
    Solve sparse linear assignment problem using Jonker-Volgenant algorithm.
    '''
    cdef cnp.ndarray[cnp.double_t, ndim=1, mode='c', negative_indices=False] cc_c = \
        np.ascontiguousarray(cc, dtype=np.double)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] ii_c = \
        np.ascontiguousarray(ii, dtype=np.int32)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] kk_c = \
        np.ascontiguousarray(kk, dtype=np.int32)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] x_c = \
        np.empty((n,), dtype=np.int32)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c', negative_indices=False] y_c = \
        np.empty((n,), dtype=np.int32)

    cdef int _fp = fp_version
    cdef int ret = lapmod_internal(
                n, &cc_c[0], &ii_c[0], &kk_c[0],
                &x_c[0], &y_c[0], _fp)
    if ret != 0:
        if ret == -1:
            raise MemoryError('Out of memory.')
        raise RuntimeError('Unknown error (lapmod_internal returned %d).' % ret)

    return x_c, y_c
