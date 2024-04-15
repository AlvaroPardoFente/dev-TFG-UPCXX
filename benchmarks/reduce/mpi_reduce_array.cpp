#include <mpi.h>
#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>
#include <iostream>
#include <chrono>
#include <numeric>

// warmup executions
constexpr uint32_t warmup_repetitions = 10;

int main(int argc, char *argv[])
{
    // Init default values
    uint32_t number_count = 1024;
    uint reps = 1;

    // Handle Input
    settings::benchmark_settings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
    if (settings.value.has_value())
    {
        if (settings.isByteValue)
        {
            number_count = settings.value.value();
        }
        else
        {
            number_count = settings.value.value();
        }
    }

    if (settings.repetitions.has_value())
    {
        reps = settings.repetitions.value();
    }

    // MPI initialization
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Clocks
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
        timer.set_settings(&settings);
    }

    // Vector initialization
    std::vector<uint32_t> value(number_count / world_size);
    for (uint32_t i = 0; i < number_count / world_size; i++)
    {
        value.at(i) = i * world_size + world_rank;
    }

    // Result vector
    std::vector<uint32_t> result(number_count / world_size);

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Reduce(value.data(), result.data(), number_count / world_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        // Sync all nodes
        MPI_Barrier(MPI_COMM_WORLD);

        // Start clock
        if (world_rank == 0)
        {
            timer.start();
        }

        MPI_Reduce(value.data(), result.data(), number_count / world_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        // End clock
        if (world_rank == 0)
        {
            timer.stop();
            timer.add_time();
        }

        // if (world_rank == 0)
        // {
        //     std::cout << "Result: ";
        //     for (uint32_t i = 0; i < number_count / world_size; i++)
        //     {
        //         std::cout << result[i] << " ";
        //     }
        //     std::cout << std::endl;

        //     uint32_t expected_result[number_count / world_size] = {0};
        //     for (uint32_t i = 0; i < number_count; i++)
        //     {
        //         expected_result[i / world_size] += i;
        //     }

        //     for (uint32_t i = 0; i < number_count / world_size; i++)
        //     {
        //         if (result[i] != expected_result[i])
        //         {
        //             std::cout << "Error: " << result[i] << " != " << expected_result[i] << std::endl;
        //         }
        //     }
        // }

        // Reset result
        std::fill(result.begin(), result.end(), 0);
    }

    // Done
    if (world_rank == 0)
    {
        timer.print_times();
    }

    MPI_Finalize();
}