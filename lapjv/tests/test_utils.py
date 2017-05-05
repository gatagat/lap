import numpy as np
import os
from gzip import GzipFile


def make_hard(cost, lo, hi):
    hard = cost.copy()
    for row in range(hard.shape[0]):
        hard[row, :] += np.random.randint(lo, hi)
    for col in range(hard.shape[1]):
        hard[:, col] += np.random.randint(lo, hi)
    return hard


def get_dense_8x8_int():
    cost = np.array([[1000, 2, 11, 10, 8, 7, 6, 5],
                     [6, 1000, 1, 8, 8, 4, 6, 7],
                     [5, 12, 1000, 11, 8, 12, 3, 11],
                     [11, 9, 10, 1000, 1, 9, 8, 10],
                     [11, 11, 9, 4, 1000, 2, 10, 9],
                     [12, 8, 5, 2, 11, 1000, 11, 9],
                     [10, 11, 12, 10, 9, 12, 1000, 3],
                     [10, 10, 10, 10, 6, 3, 1, 1000]])
    opt = 17.
    return cost, opt


def get_dense_100x100_int():
    np.random.seed(1299821)
    cost = np.random.randint(1, 101, size=(100, 100))
    opt = 198.
    return cost, opt


def get_dense_100x100_int_hard():
    cost = get_dense_100x100_int()[0]
    cost = make_hard(cost, 1, 101)
    opt = 11599.
    return cost, opt


def get_sparse_100x100_int():
    cost = get_dense_100x100_int()[0]
    np.random.seed(963245)
    mask = np.random.rand(cost.shape[0], cost.shape[1]) > 0.04
    opt = 3970.
    return cost, np.logical_not(mask), opt


def get_dense_1kx1k_int():
    np.random.seed(1299821)
    cost = np.random.randint(1, 101, size=(1000, 1000))
    opt = 1000.
    return cost, opt


def get_dense_1kx1k_int_hard():
    cost = get_dense_1kx1k_int()[0]
    cost = make_hard(cost, 1, 101)
    opt = 103078.0
    return cost, opt


def get_sparse_1kx1k_int():
    cost = get_dense_1kx1k_int()[0]
    mask = np.random.rand(cost.shape[0], cost.shape[1]) > 0.01
    opt = 17031.
    return cost, np.logical_not(mask), opt


def get_dense_4kx4k_int():
    np.random.seed(1299821)
    cost = np.random.randint(1, 101, size=(4000, 4000))
    opt = 1000.
    return cost, opt


def get_sparse_4kx4k_int():
    cost = get_dense_4kx4k_int()[0]
    mask = np.random.rand(cost.shape[0], cost.shape[1]) > 0.004
    opt = 43147.
    return cost, np.logical_not(mask), opt


# Thanks to Michael Lewis for providing this cost matrix.
def get_dense_eps():
    from pytest import approx
    datadir = os.path.abspath(os.path.dirname(__file__))
    filename = os.path.join(datadir, 'cost_eps.csv.gz')
    cost = np.genfromtxt(GzipFile(filename), delimiter=",")
    opt = approx(224.8899507294651, 0.0000000000001)
    return cost, opt


def sparse_CS_from_dense(cost):
    i = np.tile(
            np.atleast_2d(np.arange(cost.shape[0])).T,
            cost.shape[1]).flatten()
    j = np.tile(np.arange(cost.shape[1]), cost.shape[0])
    cc = cost.flatten()
    return i, j, cc


def sparse_CS_from_mask(cost, mask):
    i = np.tile(
            np.atleast_2d(np.arange(cost.shape[0])).T,
            cost.shape[1])[mask]
    j = np.tile(np.arange(cost.shape[1]), cost.shape[0])[mask.flat]
    cc = cost[mask].flatten()
    return i, j, cc


def get_cost_CS(cost, x):
    return cost[np.arange(cost.shape[0]), x].sum()
