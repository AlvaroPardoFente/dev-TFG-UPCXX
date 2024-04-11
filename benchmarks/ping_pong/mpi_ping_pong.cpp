#include <mpi.h>
#include <chrono>
#include "../../utils/benchmark_settings.hpp"
#include <iostream>
#include <numeric>

// ping_pong_count used on warmup
constexpr uint32_t warmup_count = 1024;
// warmup executions
constexpr uint32_t warmup_repetitions = 10;

int main(int argc, char *argv[])
{
    // Init default values
    uint32_t ping_pong_count = 1024;
    uint reps = 1;

    // Handle Input
    settings::benchmark_settings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
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

    if (settings.repetitions.has_value())
    {
        reps = settings.repetitions.value();
    }

    // MPI initialization
    MPI_Init(&argc, &argv);

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2)
    {
        if (world_rank == 0)
        {
            std::cerr << "This benchmark must be run with 2 processes" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

    // Int to increase
    uint32_t ping_pong_value = 0;

    // Rank to send to and receive from
    int neighbor_rank = (world_rank + 1) % 2;

    // Warmup
    if (settings.warmup)
    {

        for (int i = 0; i < warmup_repetitions; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);

            for (uint32_t i = 0; i < warmup_count; i++)
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
            // Reset counter
            ping_pong_value = 0;
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        MPI_Barrier(MPI_COMM_WORLD);

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

        // End clock
        if (world_rank == 0)
        {
            end_ops = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
            times[rep] = time_span.count();
            // std::cout << "value: " << ping_pong_value << std::endl;
        }

        // Reset counter
        ping_pong_value = 0;
    }

    if (world_rank == 0)
    {
        std::cout << "Ping-pong count: " << ping_pong_count << std::endl;
        std::cout << "Repetitions: " << reps << std::endl;
        std::cout << "Average time: " << std::accumulate(times.begin(), times.end(), 0.0) / reps << std::endl;
        std::cout << "Times (seconds): " << std::endl;
        std::cout << times[0];
        for (uint rep = 1; rep < reps; ++rep)
        {
            std::cout << ", " << times[rep];
        }
        std::cout << std::endl;
    }

    MPI_Finalize();

    return 0;
}