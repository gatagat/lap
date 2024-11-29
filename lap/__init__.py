"""LAP
``lap`` is a linear assignment problem solver using Jonker-Volgenant
algorithm for dense (LAPJV) or sparse (LAPMOD) matrices.

Functions
---------

lapjv
    Find optimal (minimum-cost) assignment for a dense cost matrix.
lapmod
    Find optimal (minimum-cost) assignment for a sparse cost matrix.
"""

__version__ = '0.5.12'

from ._lapjv import (
    lapjv,
    LARGE_ as LARGE,
    FP_1_ as FP_1,
    FP_2_ as FP_2,
    FP_DYNAMIC_ as FP_DYNAMIC
)

from .lapmod import lapmod

__all__ = ['lapjv', 'lapmod', 'FP_1', 'FP_2', 'FP_DYNAMIC', 'LARGE']
