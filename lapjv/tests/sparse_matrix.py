import numpy as np


class SparseMatrix(object):
    def __init__(self, cc, kk, number, n_columns):
        self.shape = (len(number), n_columns)
        # non-inf entries
        self.cc = cc
        # 1D-array of the number of non-inf entries on every row
        self.start = np.zeros((len(number)+1,), dtype=int)
        self.start[1:] = np.cumsum(number)
        # 1D-array of column indices of the entries (indices belonging to one
        # row must be sorted)
        self.kk = kk

    @staticmethod
    def from_dense(dense):
        cc = dense.flatten()
        n_columns = dense.shape[1]
        kk = np.tile(np.arange(n_columns, dtype=int), dense.shape[0])
        number = np.empty((dense.shape[0],), dtype=int)
        number[:] = n_columns
        return SparseMatrix(cc, kk, number, n_columns)

    @staticmethod
    def from_masked_dense(dense, mask=None):
        if mask is None:
            mask = np.logical_not(np.isinf(dense))
        cc = dense[mask].flatten()
        n_columns = dense.shape[1]
        kk = np.tile(np.arange(n_columns, dtype=int), dense.shape[0])
        kk = kk[mask.flatten()]
        number = np.empty((dense.shape[0],), dtype=int)
        number[:] = mask.sum(axis=1)
        return SparseMatrix(cc, kk, number, n_columns)
