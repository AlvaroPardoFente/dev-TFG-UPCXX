#!/bin/bash
##SBATCH --mail-type=end
##SBATCH --mail-user=alvaro.pardo.fente@udc.es
#
#$ -cwd
#$ -j y
#


NREPS=1

for i in "$@"; do
	case $i in
    	-n=*|--nreps=*)
    		NREPS="${i#*=}"
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

for ((i=1; i<=$NREPS; i++)); do
	echo srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $*
	srun -N $nodes -n $procs --ntasks-per-node=$ntaskspernode $*
done

