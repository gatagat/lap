from __future__ import print_function

from pytest import mark
from joblib import Memory

from lap import lapjv, lapmod
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
        get_platform_maxint)

max_time_per_benchmark = 15

szs = [1000, 5000, 10000]
sparsities = [0.5, 0.05, 0.005, 0.0005, 0.00005]
seeds = [1299821, 15485867, 32452867, 49979693]


cachedir = '/tmp/lapjv-cache'
memory = Memory(cachedir=cachedir, verbose=1)


@memory.cache
def get_data(sz, sparsity, seed, rng=100):
    cost, mask = get_sparse_int(sz, rng, sparsity, hard=False, seed=seed)
    print('Requested sparsity %f generated %f' % (sparsity, mask.mean()))
    cost[~mask] = get_platform_maxint()
    return cost, mask


if lapjv_old is not None:
    @mark.timeout(max_time_per_benchmark)
    @mark.parametrize('sz,sparsity,seed', [
        (sz, sparsity, seed)
        for sz in szs for sparsity in sparsities for seed in seeds])
    def test_JV_old(benchmark, sz, sparsity, seed):
        cost, mask = get_data(sz, sparsity, seed)
        benchmark(lapjv_old, cost)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,sparsity,seed', [
    (sz, sparsity, seed)
    for sz in szs for sparsity in sparsities for seed in seeds])
def test_JV(benchmark, sz, sparsity, seed):
    cost, mask = get_data(sz, sparsity, seed)
    benchmark(lapjv, cost, return_cost=False)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,sparsity,seed', [
    (sz, sparsity, seed)
    for sz in szs for sparsity in sparsities for seed in seeds])
def test_MOD_c(benchmark, sz, sparsity, seed):
    cost, mask = get_data(sz, sparsity, seed)
    _, cc, ii, kk = sparse_from_masked(cost, mask)
    benchmark(lapmod, sz, cc, ii, kk, fast=True, return_cost=False)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,sparsity,seed', [
    (sz, sparsity, seed)
    for sz in szs for sparsity in sparsities for seed in seeds])
def test_CSCY(benchmark, sz, sparsity, seed):
    cost, mask = get_data(sz, sparsity, seed)
    i, j, cc = sparse_from_masked_CS(cost, mask)
    benchmark(lapjv_centrosome, i, j, cc)
