#include <mpi.h>
#include "../../utils/benchmark_settings.hpp"
#include <iostream>
#include <chrono>

// warmup executions
constexpr uint32_t warmup_repetitions = 10;

int main(int argc, char *argv[])
{
    uint32_t number_count = 1024;
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

    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;

    std::vector<uint32_t> value(number_count / world_size);
    for (uint32_t i = 0; i < number_count / world_size; i++)
    {
        value.at(i) = i * world_size + world_rank;
    }

    std::vector<uint32_t> result(number_count / world_size);

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            MPI_Reduce(value.data(), result.data(), number_count / world_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Start clock
    if (world_rank == 0)
    {
        start_ops = std::chrono::high_resolution_clock::now();
    }

    MPI_Reduce(value.data(), result.data(), number_count / world_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // End clock
    if (world_rank == 0)
    {
        end_ops = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
        std::cout << "Reduce time: " << time_span.count() << " seconds" << std::endl;
        // std::cout << "Ping-pong value: " << number_count << std::endl;
    }

    // Done

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

    MPI_Finalize();
}