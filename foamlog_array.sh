#!/bin/bash
#SBATCH --job-name=foamLog
#SBATCH -N 1                   # Number of nodes
#SBATCH -c 1                   # Number of cores
#SBATCH -t 00-01:00            # Runtime in D-HH:MM, minimum of 10 minutes
#SBATCH -p shared              # Partition to submit to
#SBATCH --mem=32G              # Memory pool for all cores (see also --mem-per-cpu)
#SBATCH -o foamlog/myoutput_%A_%a.out  # File to which STDOUT will be written, %j inserts jobid
#SBATCH -e foamlog/myerrors_%A_%a.err  # File to which STDERR will be written, %j inserts jobid

set -o xtrace # print command

cd "$SCRATCH"/raayai_ardakani_lab/bszhu/openfoam/denticle_R_denticle_4253043/

for f in denticle_R_denticle_4253043_*_"$SLURM_ARRAY_TASK_ID"; do
  echo "$f"
  cd "$f" || exit
  singularity exec fertinaz-hpc_openfoam_of-8.sif foamLog log
  cd ..
done
