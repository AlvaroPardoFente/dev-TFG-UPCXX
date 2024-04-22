#include <mpi.h>
#include <mpi_benchmark_template.hpp>

void MpiBenchmarkTemplate::init(int argc, char *argv[])
{
    // MPI initialization
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    BenchmarkTemplate::init(argc, argv);
}

void MpiBenchmarkTemplate::finalize()
{
    MPI_Finalize();
}