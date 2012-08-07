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

def lap(cost):
    '''
    Solve linear assignment problem using Jonker-Volgenant algorithm.

    cost: square matrix containing the assignment costs

    Returns (lapcost, rowsol, colsol, u, v) where:
      lapcost: cost of the assignment
      rowsol:  vector of columns assigned to rows
      colsol:  vector of rows assigned to columns
      u:       row reduction numbers (dual variables)
      v:       column reduction numbers (dual variables)
    '''

    assert cost.shape[0] == cost.shape[1]

    cdef np.ndarray[np.double_t, ndim=2, mode='c'] cost_c
    cost_c = np.ascontiguousarray(cost, dtype=np.double)
    cdef int i
    cdef double **cost_ptr
    cost_ptr = <double **> malloc(cost_c.shape[0] * sizeof(double *))
    for i in range(cost_c.shape[0]):
        cost_ptr[i] = &cost_c[i, 0]

    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] rowsol_c = np.zeros((cost.shape[0],), dtype=np.int32)
    cdef np.ndarray[np.int32_t, ndim=1, mode='c'] colsol_c = np.zeros((cost.shape[0],), dtype=np.int32)
    cdef np.ndarray[np.double_t, ndim=1, mode='c'] u_c = np.zeros((cost.shape[0],), dtype=np.double)
    cdef np.ndarray[np.double_t, ndim=1, mode='c'] v_c = np.zeros((cost.shape[0],), dtype=np.double)

    ret = _solve(cost_c.shape[0], cost_ptr, &rowsol_c[0], &colsol_c[0], &u_c[0], &v_c[0])

    free(cost_ptr)

    return ret, rowsol_c, colsol_c, u_c, v_c

