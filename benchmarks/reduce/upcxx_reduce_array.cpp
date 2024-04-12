#include <upcxx/upcxx.hpp>
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

    // UPCXX initialization
    upcxx::init();
    int world_size, world_rank;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    // Clocks
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
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
            upcxx::barrier();
            upcxx::reduce_one(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add, 0).wait();
            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        // Sync all nodes
        upcxx::barrier();

        // Start clock
        if (world_rank == 0)
        {
            timer.start();
        }

        upcxx::reduce_one(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add, 0).wait();

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
        timer.print_times();
    }

    upcxx::finalize();
}