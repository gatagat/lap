#!/usr/bin/env python

from __future__ import print_function

import numpy as np
import Cython

for m in (np, Cython):
    version = m.__version__
    print(m.__name__.rjust(10), ' ', version)
