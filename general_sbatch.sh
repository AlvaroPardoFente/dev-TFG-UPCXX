#!/bin/bash
# Script meant to make several executions with different node configurations

nnodefile=""
time="00:05:00"
file_format="csv"
for i in "$@"; do
	case $i in
    	--nnodefile=*)
    		NREPS="${i#*=}"
    		shift # past argument=value
    		;;
        -t=*|--time=*)
            time="${i#*=}"
            shift # past argument=value
            ;;
        --txt)
            file_format="txt"
            shift # past argument
            ;;
        --csv)
            file_format="csv"
            shift # past argument
            ;;
    	*)
        	# unknown option
    		;;
	esac
done

while IFS=' ' read -r nodes tasks tasks_per_node
do
    output_file="${filename}_${nodes}N_${tasks}n.${file_format}"
    eval "UPCXX_SHARED_HEAP_SIZE=2G sbatch --exclusive -N $nodes -n $tasks --ntasks-per-node=$tasks_per_node -c 1 -t $time -p compute0 -o $output_file ~/dev-TFG-UPCXX/new_upcxx_sbatch.sh $@"
done < "$nnodefile"