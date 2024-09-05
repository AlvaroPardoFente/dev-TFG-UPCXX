#!/bin/bash

# Usage: ./submit_jobs.sh

# Define the parameters you want to vary
NODE_VALUES=(1 2 4 6 8 10 12) # Different amount of nodes
NTASKS_VALUES=(2 4 8 12 16 20 24)  # Corresponding number of tasks for each node value
PROGRAM_PATH="$HOME/dev-TFG-UPCXX/build/broadcast/"
PROGRAMS=("mpi_broadcast_array" "upcxx_broadcast_array") 
MEASUREMENT_MODE="max" # root, min, max, avg, all
MAX_ACTIVE_JOBS=25  # Maximum number of active jobs allowed

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

    # Determine which sbatch script to use based on the program name prefix
    if [[ $PROGRAM == mpi* ]]; then
      SBATCH_SCRIPT="new_mpi_sbatch.sh"
      sbatch --exclusive -N "$NODES" -n "$NTASKS" --ntasks-per-node=2 -c 1 -t 00:05:00 -p compute0 -o "$OUTPUT_FILE" ~/dev-TFG-UPCXX/"$SBATCH_SCRIPT" --value=sizes.txt "$PROGRAM_PATH""$PROGRAM" --repetitions 1000 -m "$MEASUREMENT_MODE" -q --warmup-repetitions 200
    elif [[ $PROGRAM == upcxx* ]]; then
      SBATCH_SCRIPT="new_upcxx_sbatch.sh"
      UPCXX_SHARED_HEAP_SIZE=2G sbatch --exclusive -N "$NODES" -n "$NTASKS" --ntasks-per-node=2 -c 1 -t 00:05:00 -p compute0 -o "$OUTPUT_FILE" ~/dev-TFG-UPCXX/"$SBATCH_SCRIPT" --value=sizes.txt "$PROGRAM_PATH""$PROGRAM" --repetitions 1000 -m "$MEASUREMENT_MODE" -q --warmup-repetitions 200
    else
      echo "Unknown program prefix for $PROGRAM"
      continue
    fi
  done
done

