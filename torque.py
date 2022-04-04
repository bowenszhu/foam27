import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['text.usetex'] = True
plt.rcParams['figure.constrained_layout.use'] = True

forces = np.loadtxt('forces.dat',
                    dtype=[('time', int), ('moment_pressure_z', float), ('moment_viscous_z', float)],
                    converters={12: lambda s: s[:-1], 15: lambda s: s[:-1]},
                    usecols=(0, 12, 15))
plt.loglog(forces['time'], -forces['moment_pressure_z'])
plt.xlabel('iteration')
plt.show()
