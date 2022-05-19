#!/bin/bash
#SBATCH --job-name=openfoam
#SBATCH -N 1                      # Number of nodes
#SBATCH -c 1                      # Number of cores
#SBATCH -t 7-00:00                # Runtime in D-HH:MM, minimum of 10 minutes
#SBATCH -p shared                 # Partition to submit to
#SBATCH --mem=32G                 # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH -o myoutput_%j.out  # File to which STDOUT will be written, %j inserts jobid
#SBATCH -e myerrors_%j.err  # File to which STDERR will be written, %j inserts jobid

set -o xtrace # print command

module load GCCcore/8.2.0 CMake/3.14.4

cd "$SCRATCH"/raayai_ardakani_lab/$(whoami)/openfoam/ || exit

if [ -z "$alpha" ]; then
  alpha=0.05
fi
if [ -z "$n" ]; then
  n=10
fi
if [ -z "$grid" ]; then
  grid=1
fi

f_alpha="denticle_alpha$alpha"
f_n="${f_alpha}_n$n"
f_grid="${f_n}_grid$grid"

folder_parent="denticle_multiple/${f_alpha}/${f_n}/${f_grid}"
mkdir -p "$folder_parent"

folder="${folder_parent}/${f_grid}_${SLURM_JOB_ID}"
cp -r denticle/ "$folder" # create new OpenFOAM folder

cd "$folder"/ || record_error

print_config() {
  echo "alpha: $alpha"
  echo "n: $n"
  echo "grid: $grid"
  echo "slurm_job_id: ${SLURM_JOB_ID}"
  echo "dir: $folder"
  echo ""
}
record_error() {
  print_config >>error.txt
  echo "error"
  exit
}
print_config

cd create_mesh/ || record_error
cmake .
cmake --build . --target denticle || record_error
cd ..

# create blockMeshDict
./create_mesh/denticle \
  -alpha $alpha \
  -n $n \
  -grid $grid \
  -sampleLines >README.md || record_error
mv blockMeshDict system/ || record_error
mv singleGraph_* system/ || record_error

singularity exec fertinaz-hpc_openfoam_of-8.sif blockMesh || record_error
singularity exec fertinaz-hpc_openfoam_of-8.sif simpleFoam >log || record_error

echo "finished"
