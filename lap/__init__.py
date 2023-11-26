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

import sys

__version__ = "0.5dev"

try:
    __LAP_SETUP__
except NameError:
    __LAP_SETUP__ = False
if __LAP_SETUP__:
    sys.stderr.write("Partial import of lap during the build process.\n")
else:
    from ._lapjv import FP_1_ as FP_1
    from ._lapjv import FP_2_ as FP_2
    from ._lapjv import FP_DYNAMIC_ as FP_DYNAMIC
    from ._lapjv import LARGE_ as LARGE
    from ._lapjv import lapjv
    from .lapmod import lapmod

    __all__ = ["lapjv", "lapmod", "FP_1", "FP_2", "FP_DYNAMIC", "LARGE"]
