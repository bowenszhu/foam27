#!/bin/bash
#SBATCH --job-name=openfoam
#SBATCH --nodes=1              # Number of nodes
#SBATCH --ntasks-per-node=6
#SBATCH -t 0-04:00             # Runtime in D-HH:MM, minimum of 10 minutes
#SBATCH -p shared              # Partition to submit to
#SBATCH --mem=2000             # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH -o myoutput_%A_%a.out  # File to which STDOUT will be written, %j inserts jobid
#SBATCH -e myerrors_%A_%a.err  # File to which STDERR will be written, %j inserts jobid

module load GCCcore/8.2.0 CMake/3.14.4 #gcc/10.2.0-fasrc01 openmpi/4.1.1-fasrc01

cd openfoam/ || exit

export alpha=0.15                        # angle parameter of denticle [rad]
export n_denticle=10                     # number of blocks in a circle
export n_R_outer=24                      # number of cells in R direction in outer part
export n_R_denticle=$SLURM_ARRAY_TASK_ID # number of cells in R direction in denticle part
export n_theta=69                        # number of cells in theta direction for both parts
export n_z=7                             # number of cells in z direction

export MPI_NP=6

export folder="denticle_${SLURM_JOB_ID}"
if ! [ -d "$folder" ]; then
  cp -r denticle/ "$folder" # create new OpenFOAM folder
  {
    echo "alpha=${alpha}"
    echo "n_denticle=${n_denticle}"
    echo "n_R_outer=${n_R_outer}"
    echo "n_R_denticle=${n_R_denticle}"
    echo "n_theta=${n_theta}"
    echo "n_z=${n_z}"
    echo "MPI_NP=${MPI_NP}"
  } >"$folder"/README.md

  cd "$folder"/create_mesh/ || exit
  cmake .
  cmake --build . --target denticle
  cmake --build . --target decompose
  cd ..

  ./create_mesh/denticle $alpha $n_denticle $n_R_outer "$n_R_denticle" $n_theta $n_z # create blockMeshDict
  mv blockMeshDict system/

  ./create_mesh/decompose MPI_NP # create decomposeParDict
  mv decomposeParDict system/
fi

singularity exec ../fertinaz-hpc_openfoam_of-8.sif blockMesh    # generate meshes
singularity exec ../fertinaz-hpc_openfoam_of-8.sif decomposePar # decompose a mesh and fields for parallel execution

#singularity exec ../fertinaz-hpc_openfoam_of-8.sif export MPI_BUFFER_SIZE=2000000000
#mpirun -np $MPI_NP singularity exec ../fertinaz-hpc_openfoam_of-8.sif simpleFoam -parallel >log &
singularity exec ../fertinaz-hpc_openfoam_of-8.sif simpleFoam >log &

#pgrep -f mpirun -a # print mpi process IDs
