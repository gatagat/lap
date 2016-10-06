# Tomas Kazmar, 2012, BSD 2-clause license, see LICENSE

import numpy as np
cimport numpy as cnp
cimport cython
from libc.stdlib cimport malloc, free

IF UNAME_SYSNAME == "Windows":
    cdef extern from "internal/int32t.h" nogil:
        ctypedef signed int int32_t;
ELSE:
    from libc.stdint cimport int32_t

cdef extern from "internal/lap.h":
    double lap_internal(int dim,
        double **assigncost,
        int32_t *rowsol,
        int32_t *colsol,
        double *u,
        double *v)

def lapjv(cnp.ndarray cost not None, char extend_cost=False, double cost_limit=np.inf):
    '''
    Solve linear assignment problem using Jonker-Volgenant algorithm.

    cost: (square) matrix containing the assignment costs
    extend_cost: whether or not extend a non-square matrix [default: False]
    cost_limit: an upper limit for a cost of a single assignment [default: np.inf]

    Returns (lapcost, rowsol, colsol) where:
      lapcost: cost of the assignment
      rowsol:  vector of columns assigned to rows
      colsol:  vector of rows assigned to columns

    When extend_cost and/or cost_limit is set, all unmatched entries will be
    marked by -1 in rowsol/colsol.
    '''
    
    if cost.ndim != 2:
        raise ValueError('2-dimensional array expected')
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c'] cost_c = np.ascontiguousarray(cost, dtype=np.double)
    cdef cnp.ndarray[cnp.double_t, ndim=2, mode='c'] cost_c_extended
    cdef int N = max(cost_c.shape[0], cost_c.shape[1])
    if cost_c.shape[0] != cost_c.shape[1]:
        if not extend_cost:
            raise ValueError('Square cost array expected. If intentional, pass extend_cost=True.')
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

    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c'] rowsol_c = np.zeros((cost_c.shape[0],), dtype=np.int32)
    cdef cnp.ndarray[cnp.int32_t, ndim=1, mode='c'] colsol_c = np.zeros((cost_c.shape[0],), dtype=np.int32)
    cdef cnp.ndarray[cnp.double_t, ndim=1, mode='c'] u_c = np.zeros((cost_c.shape[0],), dtype=np.double)
    cdef cnp.ndarray[cnp.double_t, ndim=1, mode='c'] v_c = np.zeros((cost_c.shape[0],), dtype=np.double)

    ret = lap_internal(cost_c.shape[0], cost_ptr, &rowsol_c[0], &colsol_c[0], &u_c[0], &v_c[0])
    free(cost_ptr)

    if cost_limit < np.inf or extend_cost:
        rowsol_c[rowsol_c >= cost.shape[1]] = -1
        colsol_c[colsol_c >= cost.shape[0]] = -1
        rowsol_c = rowsol_c[:cost.shape[0]]
        colsol_c = colsol_c[:cost.shape[1]]
        return cost[np.nonzero(rowsol_c != -1)[0], rowsol_c[rowsol_c != -1]].sum(), rowsol_c, colsol_c
    else:
        return ret, rowsol_c, colsol_c
