"""Python wrapper of LAPJV
``python-lapjv`` is a wrapper around a linear assignment problem solver by
Jonker and Volgenant.

Functions
---------

lap
    Find optimal (minimum-cost) assignment.
"""

__version__ = '0.1dev'

from ._lapjv import lapjv
