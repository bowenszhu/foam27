#!/bin/bash
#SBATCH --job-name=openfoam
#SBATCH -N 1                      # Number of nodes
#SBATCH -c 1                      # Number of cores
#SBATCH -t 7-00:00                # Runtime in D-HH:MM, minimum of 10 minutes
#SBATCH -p shared                 # Partition to submit to
#SBATCH --mem=32G                 # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH -o myoutput_%A_%j_%a.out  # File to which STDOUT will be written, %j inserts jobid
#SBATCH -e myerrors_%A_%j_%a.err  # File to which STDERR will be written, %j inserts jobid

set -o xtrace # print command

module load GCCcore/8.2.0 CMake/3.14.4

cd "$SCRATCH"/raayai_ardakani_lab/$(whoami)/openfoam/ || exit

alpha=0.15                        # angle parameter of denticle [rad]
n_denticle=10                     # number of blocks in a circle
n_R_denticle=$SLURM_ARRAY_TASK_ID # number of cells in R direction in denticle part
n_theta=69                        # number of cells in theta direction for both parts
n_z=10                            # number of cells in z direction

folder="denticle_R_denticle_${SLURM_ARRAY_JOB_ID}/"
mkdir "$folder"

folder+="denticle_R_denticle_${SLURM_ARRAY_JOB_ID}_${SLURM_JOB_ID}_${SLURM_ARRAY_TASK_ID}"

cp -r denticle/ "$folder" # create new OpenFOAM folder

cd "$folder"/ || exit

cd create_mesh/ || exit
cmake .
cmake --build . --target denticle
cd ..

# create blockMeshDict
./create_mesh/denticle \
  -alpha $alpha \
  -n $n_denticle \
  -nRDenticle "$n_R_denticle" \
  -nTheta $n_theta \
  -nZ $n_z \
  -sampleLines >README.md
mv blockMeshDict system/
mv singleGraph_* system/ # OpenFOAM functions for sampling mag(U) along certain lines

singularity exec fertinaz-hpc_openfoam_of-8.sif blockMesh # generate meshes
singularity exec fertinaz-hpc_openfoam_of-8.sif simpleFoam >log

singularity exec fertinaz-hpc_openfoam_of-8.sif foamLog log

echo "finished"
