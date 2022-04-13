import glob
import numpy as np
import matplotlib.pyplot as plt


def converge(force):
    diff = np.diff(force)
    nonzero = np.abs(diff / force[:-1]) < 1e-5
    return np.argmax(nonzero)


def plot(d, attribute):
    plt.figure()
    plt.plot(d['n_R_denticle'], d[attribute], '-o')
    plt.xlabel('n_R_denticle')
    plt.ylabel(attribute)
    plt.savefig(f'{attribute}_30.pdf')


foam_folders = glob.glob('denticle_R_denticle_4253043_*')
data = np.empty(len(foam_folders),
                dtype=[('n_R_denticle', int),
                       ('time_step', int),
                       ('moment_pressure_z', float),
                       ('moment_viscous_z', float),
                       ('execution_time', float)])
for i, f in enumerate(foam_folders):
    print(f)
    try:
        data[i]['n_R_denticle'] = int(f[-2:])
    except ValueError:
        data[i]['n_R_denticle'] = int(f[-1])

    force_files = glob.glob(f'{f}/postProcessing/forces1/*/forces*.dat')

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
    data[i]['time_step'] = forces[-1]['time_step']
    data[i]['moment_pressure_z'] = -(forces[-1]['moment_pressure_z'])
    data[i]['moment_viscous_z'] = -(forces[-1]['moment_viscous_z'])

    # execution_times = np.loadtxt(f'{f}/logs/executionTime_0',
    #                              dtype=[('step', int),
    #                                     ('second', float)])
#     arg = execution_times.size
#     data[i]['time_step'] = arg
#     data[i]['moment_pressure_z'] = -(forces['moment_viscous_z'][arg])
#     data[i]['moment_viscous_z'] = -(forces['moment_viscous_z'][arg])
#     data[i]['execution_time'] = execution_times[-1]
data.sort(axis=0)
data = data[data['n_R_denticle'] < 30]
plot(data, 'time_step')
plot(data, 'moment_pressure_z')
plot(data, 'moment_viscous_z')
for d in data:
    print(d)
# plot(data, 'execution_time')
