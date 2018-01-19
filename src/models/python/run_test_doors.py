#!/usr/bin/env python2


import numpy as np
import python_module as pm
a = np.arange(100000)
b = np.random.choice([0, 1], size=100000, replace=True)
c = np.c_[a, b]
model = pm.python_function_update(c)
for i in xrange(100):
    print(pm.python_function_estimate(model, i))
