#include <mpi.h>
#include <chrono>
#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>
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
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
        timer.set_settings(&settings);
    }

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
            timer.start();
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
            timer.stop();
            timer.add_time();
        }

        // Reset counter
        ping_pong_value = 0;
    }

    // Done
    if (world_rank == 0)
    {
        timer.print_times();
    }

    MPI_Finalize();

    return 0;
}