from pytest import mark
from joblib import Memory

from lap import lapmod, FP_1, FP_2, FP_DYNAMIC
from lap.tests.test_utils import get_nnz_int

max_time_per_benchmark = 20

szs = [5000]
nnzs = [10, 100, 500, 1000, 1500, 2000, 3000, 4000]
seeds = [1299821, 15485867, 32452867, 49979693]


cachedir = '/tmp/lapjv-cache'
memory = Memory(cachedir=cachedir, verbose=1)


@memory.cache
def get_data(sz, nnz, seed, rng=100):
    return get_nnz_int(sz, nnz, rng=rng, seed=seed)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,nnz,seed', [
    (sz, nnz, seed) for sz in szs for nnz in nnzs for seed in seeds])
def test_MOD_c_3(benchmark, sz, nnz, seed):
    cc, ii, kk = get_data(sz, nnz, seed)
    benchmark(
            lapmod, sz, cc, ii, kk,
            fast=True, return_cost=False, fp_version=FP_DYNAMIC)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,nnz,seed', [
    (sz, nnz, seed) for sz in szs for nnz in nnzs for seed in seeds])
def test_MOD_c_1(benchmark, sz, nnz, seed):
    cc, ii, kk = get_data(sz, nnz, seed)
    benchmark(
            lapmod, sz, cc, ii, kk,
            fast=True, return_cost=False, fp_version=FP_1)


@mark.timeout(max_time_per_benchmark)
@mark.parametrize('sz,nnz,seed', [
    (sz, nnz, seed) for sz in szs for nnz in nnzs for seed in seeds])
def test_MOD_c_2(benchmark, sz, nnz, seed):
    cc, ii, kk = get_data(sz, nnz, seed)
    benchmark(
            lapmod, sz, cc, ii, kk,
            fast=True, return_cost=False, fp_version=FP_2)
