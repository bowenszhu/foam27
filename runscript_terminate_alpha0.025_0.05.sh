for id in $(squeue --me --Format=JobID); do
  echo "$id"

  output_file="${HOME}/myoutput_${id}.out"
  line1=$(sed -n '1p' "$output_file")
  alpha="${line1#alpha: }"
  if [[ $alpha == 0.05 || $alpha == 0.025 ]]; then
    line2=$(sed -n '2p' "$output_file")
    n="${line2#n: }"
    line3=$(sed -n '3p' "$output_file")
    grid="${line3#grid: }"
    echo "alpha: ${alpha}"
    echo "n: ${n}"
    echo "grid: ${grid}"
    scancel "$id"
    echo "cancel $id"
    rm "$output_file"
    error_file="${HOME}/myerrors_${id}.err"
    rm "$error_file"
    echo "delete ${output_file} and ${error_file}"
    echo ""
  fi
done
