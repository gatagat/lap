# Tomas Kazmar, 2012, BSD 2-clause license, see LICENSE

import numpy as np
cimport numpy as np
from libc.stdlib cimport malloc, free

from libc.stdint cimport int32_t

cdef extern from "wrap_lapjv.h":
    double _solve(int dim,
        double **assigncost,
        int32_t *rowsol,
        int32_t *colsol,
        double *u,
        double *v)

def lap(np.ndarray cost not None, extend_cost=False, cost_limit=None):
    '''
    Solve linear assignment problem using Jonker-Volgenant algorithm.

    cost: (square) matrix containing the assignment costs
    extend_cost: whether or not extend a non-square matrix
    cost_limit: an upper limit for a cost of a single assignment

    Returns (lapcost, rowsol, colsol) where:
      lapcost: cost of the assignment
      rowsol:  vector of columns assigned to rows
      colsol:  vector of rows assigned to columns

    When extend_cost and/or cost_limit is set, all unmatched entries will be
    marked by -1 in rowsol/colsol.
    '''
    
    if cost.ndim != 2:
        raise ValueError('2-dimensional array expected')
    cdef np.ndarray[np.double_t, ndim=2, mode='c'] cost_c = np.ascontiguousarray(cost, dtype=np.double)
    cdef np.ndarray[np.double_t, ndim=2, mode='c'] cost_c_extended
    if cost_c.shape[0] != cost_c.shape[1] and not extend_cost:
        raise ValueError('No cost extension allowed - square array expected')
    cdef int N
    if cost_c.shape[0] != cost_c.shape[1] or cost_limit != None:
        N = max(cost_c.shape[0], cost_c.shape[1])
        if cost_limit == None:
            cost_c_extended = np.zeros((N, N), dtype=np.double)
        else:
            cost_c_extended = np.zeros((2*N, 2*N), dtype=np.double) + cost_limit
        if extend_cost:
            cost_c_extended[:N, :N] = 2*cost_c.max()
        cost_c_extended[:cost_c.shape[0], :cost_c.shape[1]] = cost_c
        cost_c = cost_c_extended

    cdef int i
    cdef double **cost_ptr
    cost_ptr = <double **> malloc(cost_c.shape[0] * sizeof(double *))
    for i in range(cost_c.shape[0]):
        cost_ptr[i] = &cost_c[i, 0]

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] rowsol_c = np.zeros((cost_c.shape[0],), dtype=np.int32)
    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] colsol_c = np.zeros((cost_c.shape[0],), dtype=np.int32)
    cdef np.ndarray[np.double_t, ndim=1, mode='c'] u_c = np.zeros((cost_c.shape[0],), dtype=np.double)
    cdef np.ndarray[np.double_t, ndim=1, mode='c'] v_c = np.zeros((cost_c.shape[0],), dtype=np.double)

    ret = _solve(cost_c.shape[0], cost_ptr, &rowsol_c[0], &colsol_c[0], &u_c[0], &v_c[0])
    free(cost_ptr)

    if cost_limit != None or extend_cost:
        rowsol_c[rowsol_c >= cost.shape[1]] = -1
        colsol_c[colsol_c >= cost.shape[0]] = -1
        rowsol_c = rowsol_c[:cost.shape[0]]
        colsol_c = colsol_c[:cost.shape[1]]
        return cost[np.nonzero(rowsol_c != -1)[0], rowsol_c[rowsol_c != -1]].sum(), rowsol_c, colsol_c
    else:
        return ret, rowsol_c, colsol_c
