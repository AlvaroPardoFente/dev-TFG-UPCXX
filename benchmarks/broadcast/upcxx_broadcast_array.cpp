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
    uint32_t reps = 1;

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

    // UPCXX Initialization
    upcxx::init();

    int world_rank, world_size;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    // Clocks
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
        timer.set_settings(&settings);
    }

    // Vector initialization
    std::vector<uint32_t> value(number_count);
    for (uint32_t i = 0; i < number_count; i++)
    {
        value.at(i) = i;
    }

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            upcxx::broadcast(value.data(), number_count, 0).wait();
            // Reset result
            if (world_rank != 0)
                std::fill(value.begin(), value.end(), 0);
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

        upcxx::broadcast(value.data(), number_count, 0).wait();

        // End clock
        if (world_rank == 0)
        {
            timer.stop();
            timer.add_time();
        }

        // Check result in debug mode
        for (size_t i = 0; i < value.size(); i++)
            UPCXX_ASSERT(value[i] == (int)i);

        // Reset result
        if (world_rank != 0)
            std::fill(value.begin(), value.end(), 0);
    }

    // Done
    if (world_rank == 0)
    {
        timer.print_times();
    }

    upcxx::finalize();
}