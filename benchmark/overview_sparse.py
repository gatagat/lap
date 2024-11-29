from pytest import mark
from joblib import Memory

import numpy as np
from lap import lapjv, lapmod
from lap.lapmod import get_cost
try:
    from lap_old import lapjv as lapjv_old
except ImportError:
    print(
          '''If you get here, you do not have the old lapjv to compare to.
              git clone git@github.com:gatagat/lapjv.git lapjv-old
              cd lapjv-old
              git checkout old
              python setup.py build_ext -i
              mv lapjv lapjv_old
          And run the benchmark:
              LAPJV_OLD=lapjv-old bench.sh
          ''')
    lapjv_old = None
from centrosome.lapjv import lapjv as lapjv_centrosome

from lap.tests.test_utils import (
    sparse_from_masked,
    sparse_from_masked_CS,
    get_sparse_int,
    get_platform_maxint
)

cachedir = '/tmp/lapjv-cache'
memory = Memory(cachedir=cachedir, verbose=1)


@memory.cache
def get_data(seed):
    cost, mask = get_sparse_int(5000, 1000, 0.01, hard=False, seed=seed)
    cost_ = cost.copy()
    cost_[~mask] = get_platform_maxint()
    opt = lapjv(cost_)[0]
    return cost, mask, opt

seeds = [1299821, 15485867, 32452867, 49979693]


def _get_cost_CS(cost, x):
    return cost[np.arange(cost.shape[0]), x].sum()


@mark.parametrize('seed', seeds)
def test_CSCY(benchmark, seed):
    cost, mask, opt = get_data(seed)
    i, j, cc = sparse_from_masked_CS(cost, mask)
    ret = benchmark(lapjv_centrosome, i, j, cc)
    assert _get_cost_CS(cost, ret[0]) == opt


if lapjv_old is not None:
    @mark.parametrize('seed', seeds)
    def test_JV_old(benchmark, seed):
        cost, mask, opt = get_data(seed)
        cost[~mask] = get_platform_maxint()
        ret = benchmark(lapjv_old, cost)
        assert ret[0] == opt


@mark.parametrize('seed', seeds)
def test_JV(benchmark, seed):
    cost, mask, opt = get_data(seed)
    cost[~mask] = get_platform_maxint()
    ret = benchmark(lapjv, cost)
    assert ret[0] == opt


@mark.parametrize('seed', seeds)
def test_MOD_c(benchmark, seed):
    cost, mask, opt = get_data(seed)
    n, cc, ii, kk = sparse_from_masked(cost, mask)
    ret = benchmark(lapmod, n, cc, ii, kk, fast=True, return_cost=False)
    assert get_cost(n, cc, ii, kk, ret[0]) == opt
