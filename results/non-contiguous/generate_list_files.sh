#!/bin/bash

# Usage: ./generate_list_files.sh

CHUNKS_PER_RANK=(1 4 16 64 256 1024 $((4*1024)) $((16*1024)))  # Example list of chunks per rank
CHUNK_SIZE=4
IN_STRIDE=8
OUT_INTER_CHUNK_SPACE=4
OUT_INTER_RANK_SPACE=8

# Initialize arrays for OUT_INTER_CHUNK_STRIDE and OUT_INTER_RANK_STRIDE
OUT_INTER_CHUNK_STRIDE=()
OUT_INTER_RANK_STRIDE=()

# Calculate OUT_INTER_CHUNK_STRIDE and OUT_INTER_RANK_STRIDE for each element in CHUNKS_PER_RANK
for chunks in "${CHUNKS_PER_RANK[@]}"; do
    out_inter_chunk_stride=$((OUT_INTER_CHUNK_SPACE + CHUNK_SIZE))
    out_inter_rank_stride=$((out_inter_chunk_stride * (chunks - 1) + CHUNK_SIZE + OUT_INTER_RANK_SPACE))
    
    OUT_INTER_CHUNK_STRIDE+=($out_inter_chunk_stride)
    OUT_INTER_RANK_STRIDE+=($out_inter_rank_stride)
done

# Print the results
echo "CHUNKS_PER_RANK: ${CHUNKS_PER_RANK[@]}"
echo "OUT_INTER_CHUNK_STRIDE: ${OUT_INTER_CHUNK_STRIDE[@]}"
echo "OUT_INTER_RANK_STRIDE: ${OUT_INTER_RANK_STRIDE[@]}"

# Write each list to a separate file
echo "${CHUNKS_PER_RANK[@]}" | tr ' ' '\n' > chunks_per_rank.txt
echo "${OUT_INTER_CHUNK_STRIDE[@]}" | tr ' ' '\n' > out_inter_chunk_stride.txt
echo "${OUT_INTER_RANK_STRIDE[@]}" | tr ' ' '\n' > out_inter_rank_stride.txt
