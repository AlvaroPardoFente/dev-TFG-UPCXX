module --force purge

#module load gnu8/8.3.0 openblas/0.3.7 openmpi3/3.1.4 boost/1.71.0

module load gnu8/8.3.0 openblas/0.3.7 openmpi4/4.0.5 boost/1.75.0

export CPATH=$INCLUDE
export LIBRARY_PATH=$LD_LIBRARY_PATH

export PATH=${PATH}:${HOME}/new_upcxx_202403/bin
export UPCXX_NETWORK=ibv

