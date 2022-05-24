alphas=(0.025 0.05 0.075 0.10 0.125 0.15)
ns=({4..15} 20 30 50)
grids=(1 2 4)
for alpha in "${alphas[@]}"; do
  echo "alpha: $alpha"
  for n in "${ns[@]}"; do
    echo "n: $n"
    for grid in "${grids[@]}"; do
      echo "grid: $grid"
      # submit a batch serial job
      # set output and error filenames
      # set job name
      # pass parameters as environment variables
      sbatch \
        -o myoutput_a"${alpha}"_n"${n}"_g"${grid}".txt \
        -e myerrors_a"${alpha}"_n"${n}"_g"${grid}".txt \
        --job-name=openfoam_denticle_a"${alpha}"_n"${n}"_g"${grid}" \
        --export=alpha="$alpha",n="$n",grid="$grid" \
        runscript_single.sh
      sleep 1 # pause 1 second to be kind to the Slurm scheduler when submitting a large number of jobs
    done
  done
done
