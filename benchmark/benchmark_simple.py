import sys
sys.stdout.reconfigure(encoding='utf-8')

import timeit
import lap
import numpy as np
from scipy.optimize import linear_sum_assignment

def do_lap(input):
    start_time = timeit.default_timer()
    ext_cost = input.shape[0] != input.shape[1]
    _, x, y = lap.lapjv(input, extend_cost=ext_cost)
    res_lap = np.array([[y[i],i] for i in x if i >= 0])
    elapsed_lap = timeit.default_timer() - start_time
    return res_lap, elapsed_lap

def do_scipy(input):
    start_time = timeit.default_timer()
    x, y = linear_sum_assignment(input)
    res_scipy = np.array(list(zip(x, y)))
    elapsed_scipy = timeit.default_timer() - start_time
    return res_scipy, elapsed_scipy

def test(n, m, tries=3):
    print("test(" + str(n) + ", " + str(m) + ")")
    print("-----------------------------------------")
    same_result = []
    elapsed_scipy = []
    elapsed_lap = []
    for i in range(tries):
        a = np.random.rand(n, m)
        res_scipy, elapsed = do_scipy(a)
        elapsed_scipy.append(elapsed)
        res_lap, elapsed = do_lap(a)
        elapsed_lap.append(elapsed)
        same_result.append((res_lap == res_scipy).all())
    if all(same_result):
        print(" * ✅ PASS !!!")
        elapsed_scipy = sum(elapsed_scipy) / tries
        elapsed_lap = sum(elapsed_lap) / tries
        print(f" scipy completed in {elapsed_scipy:.8f}s")
        print(f" lap completed in {elapsed_lap:.8f}s")
        if elapsed_lap <= elapsed_scipy:
            print(f" * 🏆 lap is faster by {round(elapsed_scipy/elapsed_lap, 4)}x time.")
        else:
            print(f" * 🐌 lap is slower by {round(elapsed_lap/elapsed_scipy, 4)}x time.")
    else:
        print(" * ❌ FAIL !!!")
    print("-----------------------------------------")


if __name__ == '__main__':

    test(n=4, m=5)
    test(n=5, m=5)
    test(n=5, m=6)

    test(n=45, m=50)
    test(n=50, m=50)
    test(n=50, m=55)

    test(n=450, m=500)
    test(n=500, m=500)
    test(n=500, m=550)
    
    test(n=2500, m=5000)
    test(n=5000, m=5000)
    test(n=5000, m=7500)
