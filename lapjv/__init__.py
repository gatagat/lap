"""Python wrapper of LAPJV
``python-lapjv`` is a wrapper around a linear assignment problem solver by
Jonker and Volgenant.

Functions
---------

lap
    Find optimal (minimum-cost) assignment.
"""

import sys

__version__ = '0.1.dev0'

try:
    __LAPJV_SETUP__
except NameError:
    __LAPJV_SETUP__ = False
if __LAPJV_SETUP__:
    sys.stderr.write('Partial import of lapjv during the build process.\n')
else:
    from ._lapjv import lapjv
    __all__ = ['lapjv']
