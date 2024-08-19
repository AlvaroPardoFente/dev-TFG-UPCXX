#!/bin/bash

# Usage: ./submit_jobs.sh

# Define the parameters you want to vary
NODE_VALUES=(1 2 4 6 8 10 12) # Different amount of nodes
NTASKS_VALUES=(2 4 8 12 16 20 24)  # Corresponding number of tasks for each node value
PROGRAM_PATH="$HOME/dev-TFG-UPCXX/build/non-contiguous/"
PROGRAMS=("mpi_vector" "upcxx_rput_irregular" "upcxx_rput_regular" "upcxx_rput_strided" "upcxx_rput_non_contiguous") 
MEASUREMENT_MODE="root" # root, min, max, avg, all
MAX_ACTIVE_JOBS=20  # Maximum number of active jobs allowed

# Check if NODE_VALUES and NTASKS_VALUES arrays have the same length
if [ ${#NODE_VALUES[@]} -ne ${#NTASKS_VALUES[@]} ]; then
  echo "NODE_VALUES and NTASKS_VALUES must have the same length"
  exit 1
fi

# Function to check the number of active jobs
check_active_jobs() {
  while true; do
    ACTIVE_JOBS=$(squeue -u $USER | grep -c " R\| PD")
    if [ "$ACTIVE_JOBS" -lt "$MAX_ACTIVE_JOBS" ]; then
    break
    fi
    echo "Waiting for active jobs to drop below $MAX_ACTIVE_JOBS. Current active jobs: $ACTIVE_JOBS"
    sleep 30  # Wait for 30 seconds before checking again
  done
}

# Iterate over the combinations of NODE_VALUES, NTASKS_VALUES, and PROGRAMS
for i in "${!NODE_VALUES[@]}"; do
  NODES=${NODE_VALUES[$i]}
  NTASKS=${NTASKS_VALUES[$i]}
  
  for PROGRAM in "${PROGRAMS[@]}"; do
    # Construct the output file name
    OUTPUT_FILE="${PROGRAM}_${NODES}N_${NTASKS}n.csv"
    
    check_active_jobs

    if [[ $PROGRAM == mpi* ]]; then
      SBATCH_SCRIPT="new_mpi_sbatch.sh"
      ENV_PREFIX=""
    elif [[ $PROGRAM == upcxx* ]]; then
      SBATCH_SCRIPT="new_upcxx_sbatch.sh"
      ENV_PREFIX="UPCXX_SHARED_HEAP_SIZE=2G"
    else
      echo "Unknown program prefix for $PROGRAM"
      continue
    fi
      
    CMD="$ENV_PREFIX sbatch --exclusive -N \"$NODES\" -n \"$NTASKS\" --ntasks-per-node=2 -c 1 -t 00:15:00 -p compute0 -o \"$OUTPUT_FILE\" ~/dev-TFG-UPCXX/\"$SBATCH_SCRIPT\" --chunks-per-rank=chunks_per_rank.txt --out-inter-rank-stride=out_inter_rank_stride.txt \"$PROGRAM_PATH\"\"$PROGRAM\" \
    --chunk-size 4 \
    --in-stride 8 \
    --out-inter-chunk-stride 8 \
    --repetitions 200 -m \"$MEASUREMENT_MODE\" -q --warmup-repetitions 200"

    eval $CMD

  done
done

