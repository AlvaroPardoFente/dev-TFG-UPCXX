#include <upcxx/upcxx.hpp>
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

    // UPCXX initialization
    upcxx::init();
    int world_size, world_rank;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

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
            upcxx::reduce_one(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add, 0).wait();
            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        // Start clock
        if (world_rank == 0)
        {
            start_ops = std::chrono::high_resolution_clock::now();
        }

        upcxx::reduce_one(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add, 0).wait();

        // End clock
        if (world_rank == 0)
        {
            end_ops = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
            times[rep] = time_span.count();
            // std::cout << "Ping-pong value: " << number_count << std::endl;
        }

        // if (world_rank == 0)
        // {
        //     std::cout << "Result: ";
        //     for (uint32_t i = 0; i < number_count / world_size; i++)
        //     {
        //         std::cout << result.at(i) << " ";
        //     }
        //     std::cout << std::endl;

        //     std::vector<uint32_t> expected_result(number_count / world_size);
        //     for (uint32_t i = 0; i < number_count; i++)
        //     {
        //         expected_result.at(i / world_size) += i;
        //     }

        //     for (uint32_t i = 0; i < number_count / world_size; i++)
        //     {
        //         if (result.at(i) != expected_result.at(i))
        //         {
        //             std::cout << "Error: " << result.at(i) << " != " << expected_result.at(i) << std::endl;
        //         }
        //     }
        // }

        // Reset result
        std::fill(result.begin(), result.end(), 0);
    }

    // Done
    if (world_rank == 0)
    {
        std::cout << "Number count: " << number_count << std::endl;
        std::cout << "Repetitions: " << reps << std::endl;
        std::cout << "Average time: " << std::accumulate(times.begin(), times.end(), 0.0) / reps << std::endl;
        std::cout << "Times: ";
        for (uint32_t i = 0; i < reps; i++)
        {
            std::cout << times[i] << ", ";
        }
        std::cout << std::endl;
    }

    upcxx::finalize();
}