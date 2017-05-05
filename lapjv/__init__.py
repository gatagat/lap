"""LAPJV
``python-lapjv`` is a linear assignment problem solver using Jonker-Volgenant
algorithm for dense (LAPJV) or sparse (LAPMOD) matrices.

Functions
---------

lapjv
    Find optimal (minimum-cost) assignment for a dense cost matrix.
lapmod
    Find optimal (minimum-cost) assignment for a sparse cost matrix.
"""

import sys

__version__ = '0.2.dev0'

try:
    __LAPJV_SETUP__
except NameError:
    __LAPJV_SETUP__ = False
if __LAPJV_SETUP__:
    sys.stderr.write('Partial import of lapjv during the build process.\n')
else:
    from ._lapjv import lapjv
    from lapmod import lapmod
    __all__ = ['lapjv', 'lapmod']
