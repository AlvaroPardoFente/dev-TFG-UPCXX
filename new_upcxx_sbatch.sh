#!/bin/bash
##SBATCH --mail-type=end
##SBATCH --mail-user=alvaro.pardo.fente@udc.es
#
#$ -cwd
#$ -j y
#


NREPS=1
IS_QUIET=0
declare -A arg_files

for i in "$@"; do
	case $i in
    	-n=*|--nreps=*)
    		NREPS="${i#*=}"
    		shift # past argument=value
    		;;
		--sizes=*)
            SIZES_FILE="${i#*=}"
            shift # past argument=value
            ;;
        -q|--quiet)
            IS_QUIET=1
            shift # past argument with no value
            ;;
        --*=*)
            arg_name=${i%%=*}
            file_name=${i#*=}
            arg_files[$arg_name]=$file_name
            shift # past argument=value
            ;;
    	*)
        	# unknown option
    		;;
	esac
done

nodes=$SLURM_NNODES
procs=$SLURM_NTASKS
threads=$SLURM_CPUS_PER_TASK
let ntaskspernode=$procs/$nodes

module --force purge 2> /dev/null
module load gnu8/8.3.0 openblas/0.3.7 openmpi4/4.0.5 boost/1.75.0 2> /dev/null

export PATH=${PATH}:${HOME}/new_upcxx_202403/bin
export UPCXX_NETWORK=ibv

export OPENBLAS_NUM_THREADS=1 OMP_NUM_THREADS=1 CNC_NUM_THREADS=$threads
#export GASNET_MAX_SEGSIZE=48G
#export UPCXX_GASNET_CONDUIT=ibv
export GASNET_PHYSMEM_MAX='41 GB'
export GASNET_ODP_VERBOSE=0
export MPIRUN_CMD="srun -N $nodes -n %N --ntasks-per-node=$ntaskspernode -c $threads %C"


echo JOBID=$SLURM_JOB_ID

if [[ $IS_QUIET -eq 0 ]]; then
    echo Nodes=$nodes Procs=$procs TasksPerNode=$ntaskspernode ThreadsPerTask=$threads NREPS=$NREPS $*
else
    echo Size, Timepoint, Index, Time
fi

# ulimit -s unlimited
ulimit -c 0

args=$@

# Add -q to args if IS_QUIET is set
if [[ $IS_QUIET -eq 1 ]]; then
    args="$args -q"
fi

# Define a function to generate all combinations of arguments
generate_combinations() {
    local current_combination=$1
    shift
    local args=("$@")
    if [ ${#args[@]} -eq 0 ]; then
        echo $current_combination
    else
        local arg=${args[0]}
        unset args[0]
        args=("${args[@]}")
        while IFS= read -r value
        do
            generate_combinations "$current_combination $arg=$value" "${args[@]}"
        done < "${arg_files[$arg]}"
    fi
}

# Generate all combinations of arguments and run the command with each combination
if [[ ${#arg_files[@]} -gt 0 ]]; then
    while IFS= read -r combination
    do
        cmd="$args $combination"
        for ((i=1; i<=$NREPS; i++)); do
            srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $cmd
        done
    done < <(generate_combinations "" "${!arg_files[@]}")
else
    for ((i=1; i<=$NREPS; i++)); do
        echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
        srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
    done
fi