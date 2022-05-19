alphas=(0.025 0.05 0.075 0.10 0.125 0.15)
ns=({4..15} 20 30 50)
grids=(1 2 4)
for alpha in "${alphas[@]}"; do
  echo "alpha: $alpha"
  for n in "${ns[@]}"; do
    echo "n: $n"
    for grid in "${grids[@]}"; do
      echo "grid: $grid"
      sbatch \ `# submit a batch serial job`
        -o myoutput_a"${alpha}"_n"${n}"_g"${grid}".txt \ `# output file, override -o inside runscript_single.sh`
        -e myerrors_a"${alpha}"_n"${n}"_g"${grid}".txt \ `# error  file, override -o inside runscript_single.sh`
        --job-name=openfoam_denticle_a"${alpha}"_n"${n}"_g"${grid}" \ `# add parameters to job name`
        --export=alpha="$alpha",n="$n",grid="$grid" runscript_single.sh # pass parameters as environment variables
      sleep 1 # pause 1 second to be kind to the Slurm scheduler when submitting a large number of jobs
    done
  done
done
