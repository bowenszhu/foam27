set -o xtrace # print command
grids=(1 2 4)

root="${SCRATCH}/raayai_ardakani_lab/$(whoami)/openfoam/denticle_multiple"
cd "$root"
f_alpha="denticle_alpha0.05"
pwd
cd "$f_alpha" || exit
dir_alpha=$(pwd)
for f_n in "${f_alpha}"_n*; do
  pwd
  cd "$f_n" || continue
  dir_n=$(pwd)
  f_grid="${f_n}_grid1"
  pwd
  cd "$f_grid" || continue
  dir_grid=$(pwd)
  for folder in "${f_grid}"_*; do
    pwd
    cd "$folder" || continue
    line3=$(sed -n '3p' README.md)
    n_R_denticle=$(echo "$line3" | sed -n "s/^.*:\s\(\S*\)\s#.*$/\1/p")
    if [ "$n_R_denticle" -lt 5 ]; then
      alpha=$(echo "$folder" | sed -n "s/denticle_alpha\(\S*\)_n.*$/\1/p")
      n=$(echo "$folder" | sed -n "s/^.*_n\(\S*\)_grid.*$/\1/p")
      grid=$(echo "$folder" | sed -n "s/^.*_grid\(\S*\)_.*$/\1/p")
      echo "alpha: ${alpha}"
      echo "n: ${n}"
      echo "grid: ${grid}"
      echo "n_R_denticle: $n_R_denticle"
#      rm -r "$dir_n"
#      echo "deleted $dir_n"
#      for grid in "${grids[@]}"; do
#        echo "sbatch grid: $grid"
#        sbatch --export=alpha="$alpha",n="$n",grid="$grid" "$HOME"/runscript_single.sh
#      done
      echo ""
      continue
    fi
    cd "$dir_grid"
  done
  cd "$dir_n"
done
