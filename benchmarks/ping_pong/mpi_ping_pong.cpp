#include <mpi.h>

int main(int argc, char **argv)
{
    int ping_pong_count = 1024;
    if (argc > 1)
    {
        try
        {
            ping_pong_count = std::stoi(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            // Argument is not a number, do nothing
        }
    }
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int ping_pong_value = 0;
    int neighbor_rank = (world_rank + 1) % 2;
    for (int i = 0; i < ping_pong_count; i++)
    {
        if (world_rank == i % 2)
        {
            ping_pong_value++;
            MPI_Send(&ping_pong_value, 1, MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Recv(&ping_pong_value, 1, MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();

    return 0;
}