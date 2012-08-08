# Tomas Kazmar, 2012, BSD 2-clause license, see LICENSE
#
# This code was adapted from http://syzygy.st/code.html#pyLAPJV
# Thanks to Dr N.D. van Foreest for providing this example code.

"""
The cost matrix is based on Balas and Toth, 1985, Branch and bound
# methods, in Lawler, E.L, et al., The TSP, John Wiley & Sons,
Chischester, pp 361--401.
"""

from nose.tools import raises

import numpy as np
import lapjv

large = 1000
cost = np.array( [[large,2,11,10,8,7,6,5],
                  [6,large,1,8,8,4,6,7],
                  [5,12,large,11,8,12,3,11],
                  [11,9,10,large,1,9,8,10],
                  [11,11,9,4,large,2,10,9],
                  [12,8,5,2,11,large,11,9],
                  [10,11,12,10,9,12,large,3],
                  [10,10,10,10,6,3,1,large]] )

def test_lap_square():
    ret = lapjv.lap(cost)
    assert ret[0] == 17.0
    assert np.all(ret[1] == [1, 2, 0, 4, 5, 3, 7, 6])
    assert np.all(ret[2] == [2, 0, 1, 5, 3, 4, 7, 6])
    assert cost[range(cost.shape[0]), ret[1]].sum() == ret[0]
    assert cost[ret[2], range(cost.shape[1])].sum() == ret[0]

@raises(ValueError)
def test_lap_empty():
    lapjv.lap(np.ndarray([]))

@raises(ValueError)
def test_lap_non_square_fail():
    lapjv.lap(np.zeros((3, 2)))

def test_lap_non_contigous():
    ret = lapjv.lap(cost[:3, :3])
    assert ret[0] == 8.0
    assert np.all(ret[1] == [1, 2, 0])
    assert np.all(ret[2] == [2, 0, 1])

def test_lap_extension():
    ret = lapjv.lap(cost[:2, :4], extend_cost=True)
    assert ret[0] == 3.0
    assert np.all(ret[1] == [1, 2])
    assert np.all(ret[2] == [-1, 0, 1, -1])

def test_lap_cost_limit():
    ret = lapjv.lap(cost[:3, :3], cost_limit=4.99)
    assert ret[0] == 3.0
    assert np.all(ret[1] == [1, 2, -1])
    assert np.all(ret[2] == [-1, 0, 1])
