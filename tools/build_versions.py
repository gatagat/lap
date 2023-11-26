#!/usr/bin/env python


import Cython
import numpy as np

for m in (np, Cython):
    version = m.__version__
    print(m.__name__.rjust(10), " ", version)
