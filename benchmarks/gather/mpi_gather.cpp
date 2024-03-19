#include <mpi.h>
#include "../../utils/benchmark_settings.hpp"
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
    settings::BenchmarkSettings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
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
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

    // Vector initialization
    std::vector<uint32_t> value(number_count / world_size);
    for (uint32_t i = 0; i < number_count / world_size; i++)
    {
        value.at(i) = i + world_rank * (number_count / world_size);
    }

    // Result vector
    std::vector<uint32_t> result(number_count);

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Gather(value.data(), number_count / world_size, MPI_INT, result.data(), number_count / world_size, MPI_INT, 0, MPI_COMM_WORLD);
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
            start_ops = std::chrono::high_resolution_clock::now();
        }

        MPI_Gather(value.data(), number_count / world_size, MPI_INT, result.data(), number_count / world_size, MPI_INT, 0, MPI_COMM_WORLD);

        // End clock
        if (world_rank == 0)
        {
            end_ops = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
            times[rep] = time_span.count();
        }

        if (world_rank == 0)
        {
            // for (int i = 0; i < number_count; i++)
            //     std::cout << result[i] << " ";
            // std::cout << std::endl;

            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Done
    if (world_rank == 0)
    {
        std::cout << "Number count: " << number_count << std::endl;
        std::cout << "Repetitions: " << reps << std::endl;
        std::cout << "Average time: " << std::accumulate(times.begin(), times.end(), 0.0) / reps << std::endl;
        std::cout << "Times: ";
        std::cout << times[0];
        for (uint rep = 1; rep < reps; ++rep)
        {
            std::cout << ", " << times[rep];
        }
        std::cout << std::endl;
    }

    MPI_Finalize();
}