cd "$SCRATCH"/raayai_ardakani_lab/$(whoami)/openfoam/denticle_multiple
rm -r denticle_alpha0.025
rm -r denticle_alpha0.05

alphas=(0.025 0.05)
ns=({4..15} 20 30 50)
grids=(1 2 4)
for alpha in "${alphas[@]}"; do
  echo "alpha: $alpha"
  for n in "${ns[@]}"; do
    echo "n: $n"
    for grid in "${grids[@]}"; do
      echo "grid: $grid"
      sbatch --export=alpha="$alpha",n="$n",grid="$grid" runscript_single.sh
    done
  done
done
