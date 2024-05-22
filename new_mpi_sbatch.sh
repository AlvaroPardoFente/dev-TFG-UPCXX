#!/bin/bash
##SBATCH --mail-type=end
##SBATCH --mail-user=alvaro.pardo.fente@udc.es
#
#$ -cwd
#$ -j y
#


NREPS=1
IS_QUIET=0
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

if [[ -n $SIZES_FILE ]]; then
    for size in $(cat $SIZES_FILE);
    do
        # Add the --value $size argument
        cmd="$args --value $size"
        for ((i=1; i<=$NREPS; i++)); do
            echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $cmd
            srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $cmd
        done
    done
else
    for ((i=1; i<=$NREPS; i++)); do
        echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
        srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $args
    done
fi

