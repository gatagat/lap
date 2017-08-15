from pytest import mark
from joblib import Memory

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
        get_dense_int, get_cost_CS, sparse_from_dense_CS, sparse_from_dense)

max_time_per_benchmark = 20

szs = [10, 100, 200, 500, 1000, 2000, 5000]
rngs = [100, 1000, 10000, 100000]
seeds = [1299821, 15485867, 32452867, 49979693]


cachedir = '/tmp/lapjv-cache'
memory = Memory(cachedir=cachedir, verbose=1)


@memory.cache
def get_hard_data(sz, rng, seed):
    cost = get_dense_int(sz, 100, hard=True, seed=seed)
    opt = lapjv(cost)[0]
    return cost, opt


if lapjv_old is not None:
    @mark.timeout(max_time_per_benchmark)
    @mark.parametrize('sz,rng,seed', [
        (sz, rng, seed) for sz in szs for rng in rngs for seed in seeds])
    def test_JV_old(benchmark, sz, rng, seed):
        cost, opt = get_hard_data(sz, rng, seed)
        ret = benchmark(lapjv_old, cost)
        assert ret[0] == opt


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,rng,seed', [
    (sz, rng, seed) for sz in szs for rng in rngs for seed in seeds])
def test_JV(benchmark, sz, rng, seed):
    cost, opt = get_hard_data(sz, rng, seed)
    ret = benchmark(lapjv, cost, return_cost=False)
    assert cost[range(cost.shape[0]), ret[0]].sum() == opt


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,rng,seed', [
    (sz, rng, seed) for sz in szs for rng in rngs for seed in seeds])
def test_MOD_c(benchmark, sz, rng, seed):
    cost, opt = get_hard_data(sz, rng, seed)
    _, cc, ii, kk = sparse_from_dense(cost)
    ret = benchmark(lapmod, sz, cc, ii, kk, fast=True, return_cost=False)
    assert get_cost(sz, cc, ii, kk, ret[0]) == opt


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,rng,seed', [
    (sz, rng, seed) for sz in szs for rng in rngs for seed in seeds])
def test_CSCY(benchmark, sz, rng, seed):
    cost, opt = get_hard_data(sz, rng, seed)
    i, j, cc = sparse_from_dense_CS(cost)
    ret = benchmark(lapjv_centrosome, i, j, cc)
    assert get_cost_CS(cost, ret[0]) == opt
