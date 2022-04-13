import glob
import numpy as np
import matplotlib.pyplot as plt


# from postprocess import converge

def converge(force):
    diff = np.diff(force)
    nonzero = np.abs(diff / force[:-1]) < 1e-5
    return np.argmax(nonzero)


force_files = glob.glob('postProcessing/forces1/*/forces*.dat')

forces = [np.loadtxt(force_f,
                     dtype=[('time_step', int),
                            ('moment_pressure_z', float),
                            ('moment_viscous_z', float)],
                     converters={12: lambda s: s[:-1],
                                 15: lambda s: s[:-1]},
                     usecols=(0, 12, 15))
          for force_f in force_files]
forces = np.concatenate(forces)
forces.sort(axis=0)

fig, axs = plt.subplots(2, figsize=(7, 10))
axs[0].loglog(forces['time_step'], -forces['moment_pressure_z'])
# arg0 = converge(forces['moment_pressure_z'])
# axs[0].loglog(forces['time'][:-1][arg0], -forces['moment_pressure_z'][:-1][arg0], 'o')
axs[0].set(xlabel='iteration')
axs[0].set_title('moment pressure z')

axs[1].loglog(forces['time_step'], -forces['moment_viscous_z'])
# arg1 = converge(forces['moment_viscous_z'])
# axs[1].loglog(forces['time'][:-1][arg1], -forces['moment_viscous_z'][:-1][arg1], 'o')
axs[1].set(xlabel='iteration')
axs[1].set_title('moment viscous z')
plt.savefig('torque.pdf')
plt.show()
