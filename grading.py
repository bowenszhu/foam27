import numpy as np
import matplotlib.pyplot as plt

r = np.loadtxt('grading.txt',
               dtype=[('n_R_denticle', int), ('r_outer_R', float)])
plt.plot(r['n_R_denticle'], r['r_outer_R'])
plt.xlabel('n_R_denticle')
plt.ylabel('r_outer_R')
plt.show()
