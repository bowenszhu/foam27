#!/bin/bash
#SBATCH --job-name=openfoam
#SBATCH -N 1                      # Number of nodes
#SBATCH -c 1                      # Number of cores
#SBATCH -t 7-00:00                # Runtime in D-HH:MM, minimum of 10 minutes
#SBATCH -p shared                 # Partition to submit to
#SBATCH --mem=32G                 # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH -o myoutput_%j.out  # File to which STDOUT will be written, %j inserts jobid
#SBATCH -e myerrors_%j.err  # File to which STDERR will be written, %j inserts jobid

set -o xtrace # print commands in output file, shows which commands have been executed

module load GCCcore/8.2.0 CMake/3.14.4 # load CMake and C++ compiler

cd "$SCRATCH"/raayai_ardakani_lab/$(whoami)/openfoam/ || exit # change to the scratch folder

# set the parameters unless they have been set as environment variables
if [ -z "$alpha" ]; then
  alpha=0.05
fi
if [ -z "$n" ]; then
  n=10
fi
if [ -z "$grid" ]; then
  grid=1
fi

# define different levels of directory structure
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
record_error() { # record the configuration and exit in case of error
  print_config >>error.txt
  echo "error"
  exit
}
print_config # print parameters and file directory to the output file

cd create_mesh/ || record_error # change to the folder for blockMeshDict generation
cmake .
cmake --build . --target denticle || record_error # CMake compile
cd ..

# create blockMeshDict
./create_mesh/denticle \
  -alpha $alpha \
  -n $n \
  -grid $grid \
  -sampleLines >README.md || record_error # generate blockMeshDict and write configuration in README.md
mv blockMeshDict system/ || record_error # move blockMeshDict to system/
mv singleGraph_* system/ || record_error # mode function files (for sampling data) to system/

# since the Cannon cluster software module doesn't have OpenFOAM available as of May 2022,
# we use Singularity container for OpenFOAM 8 for now.
# However, the Singularity container doesn't work properly with MPI.
# A request to install OpenFOAM versions 7, 8, 9 was submitted to the research computing group in May 2022.
# We should change back to use normal installation of OpenFOAM with MPI once installation is complete.
singularity exec fertinaz-hpc_openfoam_of-8.sif blockMesh || record_error # mesh generation
singularity exec fertinaz-hpc_openfoam_of-8.sif simpleFoam >log || record_error # solve the OpenFOAM case and save log

echo "finished"
