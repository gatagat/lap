import sys
sys.stdout.reconfigure(encoding='utf-8')

import timeit
import lap
import numpy as np
from scipy.optimize import linear_sum_assignment

def do_lapx(input):
    start_time = timeit.default_timer()
    ext_cost = False if input.shape[0] == input.shape[1] else True
    _, x, y = lap.lapjv(input, extend_cost=ext_cost)
    res_lapx = np.array([[y[i],i] for i in x if i >= 0])
    elapsed_lapx = timeit.default_timer() - start_time
    return res_lapx, elapsed_lapx

def do_scipy(input):
    start_time = timeit.default_timer()
    x, y = linear_sum_assignment(input)
    res_scipy = np.array(list(zip(x, y)))
    elapsed_scipy = timeit.default_timer() - start_time
    return res_scipy, elapsed_scipy

def test(n, m):
    print("test(" + str(n) + ", " + str(m) + ")")
    a = np.random.rand(n, m)
    print("-----------------------------------------")
    res_scipy, elapsed_scipy = do_scipy(a)
    print(" scipy completed in " + str(format((elapsed_scipy), '.8f')) + "s")
    res_lapx, elapsed_lapx = do_lapx(a)
    print(" lapx completed in " + str(format((elapsed_lapx), '.8f')) + "s")
    if (res_lapx == res_scipy).all():
        print(" * ✅ PASS !!!")
        if elapsed_lapx <= elapsed_scipy:
            print(" * 🏆 lapx is faster by " + str(round((elapsed_scipy/elapsed_lapx), 4)) + "x time.")
        else:
            print(" * 🐌 lapx is slower by " + str(round((elapsed_lapx/elapsed_scipy), 4)) + "x time.")
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
