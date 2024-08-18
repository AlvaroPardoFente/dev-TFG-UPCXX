#!/bin/bash
##SBATCH --mail-type=end
##SBATCH --mail-user=alvaro.pardo.fente@udc.es
#
#$ -cwd
#$ -j y
#


NREPS=1
declare -A files_for_args

for i in "$@"; do
	case $i in
    	-n=*|--nreps=*)
    		NREPS="${i#*=}"
    		shift # past argument=value
    		;;
		--*=*)
            arg="${i%%=*}"
            file="${i#*=}"
            files_for_args[$arg]="$file"
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

export OPENBLAS_NUM_THREADS=1 OMP_NUM_THREADS=1 CNC_NUM_THREADS=$threads
#export GASNET_MAX_SEGSIZE=48G
#export UPCXX_GASNET_CONDUIT=ibv
export GASNET_PHYSMEM_MAX='41 GB'
export GASNET_ODP_VERBOSE=0
export MPIRUN_CMD="srun -N $nodes -n %N --ntasks-per-node=$ntaskspernode -c $threads %C"


echo JOBID=$SLURM_JOB_ID

echo Nodes=$nodes Procs=$procs TasksPerNode=$ntaskspernode ThreadsPerTask=$threads NREPS=$NREPS $*

# ulimit -s unlimited
ulimit -c 0

args=$@
first=0

if [[ ${#files_for_args[@]} -eq 0 ]]; then
    for ((i=1; i<=$NREPS; i++)); do
        echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
        srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
    done
else

    # Read lines from each file into arrays
    declare -A file_lines
    for arg_name in "${!files_for_args[@]}"; do
        file="${files_for_args[$arg_name]}"
        mapfile -t lines < "$file"
        file_lines["$arg_name"]="${lines[@]}"
    done

    # Get the number of lines in the first file (assuming all files have the same number of lines)
    for file in "${files_for_args[@]}"; do
        first_file=$file
        break
    done
    num_lines=$(wc -l < "$first_file")

    # Iterate over the lines and run the program
    for ((i=0; i<num_lines; i++)); do
        cmd="$args"
        for arg_name in "${!file_lines[@]}"; do
            line=$(sed -n "$((i+1))p" "${files_for_args[$arg_name]}")
            cmd="$cmd $arg_name $line"
        done
        for ((j=1; j<=$NREPS; j++)); do
            if [[ $first -eq 0 ]]; then
                first=1
            else
                cmd="$cmd --no-headers"
            fi
            # echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $cmd
            srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $cmd
        done
    done
fi