#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>

void MpiBenchmarkScheme::init(int argc, char *argv[])
{
    // MPI initialization
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    BenchmarkScheme::init(argc, argv);
}

void MpiBenchmarkScheme::barrier()
{
    MPI_Barrier(MPI_COMM_WORLD);
}

void MpiBenchmarkScheme::finalize()
{
    MPI_Finalize();
}