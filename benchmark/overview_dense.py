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
from pymatgen.optimization.linear_assignment import LinearAssignment
from centrosome.lapjv import lapjv as lapjv_centrosome

from lap.tests.test_utils import (
    sparse_from_dense,
    sparse_from_dense_CS, get_cost_CS,
    get_dense_int
)

cachedir = '/tmp/lapjv-cache'
memory = Memory(cachedir=cachedir, verbose=1)


@memory.cache
def get_data(seed):
    cost = get_dense_int(100, 1000, hard=True, seed=seed)
    opt = lapjv(cost)[0]
    return cost, opt

seeds = [1299821, 15485867, 32452867, 49979693]


if lapjv_old is not None:
    @mark.parametrize('seed', seeds)
    def test_JV_old(benchmark, seed):
        cost, opt = get_data(seed)
        ret = benchmark(lapjv_old, cost)
        assert ret[0] == opt


@mark.parametrize('seed', seeds)
def test_JV(benchmark, seed):
    cost, opt = get_data(seed)
    ret = benchmark(lapjv, cost)
    assert ret[0] == opt


@mark.parametrize('seed', seeds)
def test_MODPY(benchmark, seed):
    cost, opt = get_data(seed)
    n, cc, ii, kk = sparse_from_dense(cost)
    ret = benchmark(lapmod, n, cc, ii, kk, fast=False, return_cost=False)
    assert get_cost(n, cc, ii, kk, ret[0]) == opt


@mark.parametrize('seed', seeds)
def test_MOD_c(benchmark, seed):
    cost, opt = get_data(seed)
    n, cc, ii, kk = sparse_from_dense(cost)
    ret = benchmark(lapmod, n, cc, ii, kk, fast=True, return_cost=False)
    assert get_cost(n, cc, ii, kk, ret[0]) == opt


@mark.parametrize('seed', seeds)
def test_PMG(benchmark, seed):
    cost, opt = get_data(seed)
    ret = benchmark(LinearAssignment, cost)
    assert ret.min_cost == opt


@mark.parametrize('seed', seeds)
def test_CSCY(benchmark, seed):
    cost, opt = get_data(seed)
    i, j, cc = sparse_from_dense_CS(cost)
    ret = benchmark(lapjv_centrosome, i, j, cc)
    assert get_cost_CS(cost, ret[0]) == opt
