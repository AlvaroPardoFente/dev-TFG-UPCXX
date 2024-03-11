#include <mpi.h>
#include <chrono>
#include "../../utils/benchmark_settings.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    uint32_t ping_pong_count = 1024;

    settings::BenchmarkSettings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
    if (settings.value.has_value())
    {
        if (settings.isByteValue)
        {
            ping_pong_count = settings.value.value();
        }
        else
        {
            ping_pong_count = settings.value.value();
        }
    }

    // MPI initialization
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;

    // Int to increase
    uint32_t ping_pong_value = 0;

    // Rank to send to and receive from
    int neighbor_rank = (world_rank + 1) % 2;

    // Start clock
    if (world_rank == 0)
    {
        start_ops = std::chrono::high_resolution_clock::now();
    }

    for (uint32_t i = 0; i < ping_pong_count; i++)
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

    if (world_rank == 0)
    {
        end_ops = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
        std::cout << "Ping-pong time: " << time_span.count() << " seconds" << std::endl;
        std::cout << "Ping-pong value: " << ping_pong_value << std::endl;
    }

    MPI_Finalize();

    return 0;
}