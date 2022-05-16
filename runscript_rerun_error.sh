for err_file in myerrors_*.err; do
  if grep -q "exit" "$err_file"; then
    slurm_job_id="${err_file%.err}"
    slurm_job_id="${slurm_job_id#myerrors_}"
    out_file="myoutput_${slurm_job_id}.out"
    echo "$out_file"
    line1=$(sed -n '1p' "$out_file")
    alpha="${line1#alpha: }"
    line2=$(sed -n '2p' "$out_file")
    n="${line2#n: }"
    line3=$(sed -n '3p' "$out_file")
    grid="${line3#grid: }"
    echo "alpha: ${alpha}"
    echo "n: ${n}"
    echo "grid: ${grid}"
    echo "slurm_job_id: ${slurm_job_id}"

    f_alpha="denticle_alpha$alpha"
    f_n="${f_alpha}_n$n"
    f_grid="${f_n}_grid$grid"
    folder_parent="denticle_multiple/${f_alpha}/${f_n}/${f_grid}"
    folder="${SCRATCH}/raayai_ardakani_lab/$(whoami)/openfoam/${folder_parent}/${f_grid}_${slurm_job_id}"
    rm -r "$folder"
    echo "deleted $folder"
    rm "$err_file"
    echo "deleted $err_file"
    rm "$out_file"
    echo "deleted $out_file"

    sbatch --export=alpha="$alpha",n="$n",grid="$grid" runscript_single.sh

    echo ""
  fi
done
