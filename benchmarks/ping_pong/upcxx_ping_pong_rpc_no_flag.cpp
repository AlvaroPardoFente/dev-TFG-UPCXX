#include <upcxx/upcxx.hpp>
#include <chrono>
#include <iostream>
#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>
#include <numeric>

// ping_pong_count used on warmup
constexpr uint32_t warmup_count = 1024;
// warmup executions
constexpr uint32_t warmup_repetitions = 10;

int main(int argc, char **argv)
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

    // UPCXX initialization
    upcxx::init();

    int world_rank, world_size;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    if (world_size != 2)
    {
        if (world_rank == 0)
        {
            std::cerr << "This benchmark must be run with 2 processes" << std::endl;
        }
        upcxx::finalize();
        return 1;
    }

    // Clocks
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
    }

    int neighbor_rank = (world_rank + 1) % 2;

    upcxx::dist_object<uint32_t> ping_pong_object(0);

    // Warmup
    if (settings.warmup)
    {
        for (int i = 0; i < warmup_repetitions; i++)
        {
            uint32_t expected_count = *ping_pong_object;

            upcxx::barrier();

            for (uint32_t i = 0; i < warmup_count; i++)
            {
                expected_count++;

                // std::cout << "Rank " << world_rank << " expected: " << expected_count << "object:" << *ping_pong_object << std::endl;

                if (world_rank == i % 2)
                {
                    (*ping_pong_object)++;
                    upcxx::rpc(
                        neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t> &ping_pong_object)
                        { *ping_pong_object = value; },
                        *ping_pong_object, ping_pong_object)
                        .wait();
                }
                else
                {
                    while (expected_count != *ping_pong_object)
                    {
                        upcxx::progress();
                    }
                }
            }
            // Reset counter
            *ping_pong_object = 0;
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        uint32_t expected_count = *ping_pong_object;

        upcxx::barrier();

        // Start clock
        if (world_rank == 0)
        {
            timer.start();
        }

        for (uint32_t i = 0; i < ping_pong_count; i++)
        {
            expected_count++;

            if (world_rank == i % 2)
            {
                (*ping_pong_object)++;
                upcxx::rpc(
                    neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t> &ping_pong_object)
                    { *ping_pong_object = value; },
                    *ping_pong_object, ping_pong_object)
                    .wait();
            }
            else
            {
                while (expected_count != *ping_pong_object)
                {
                    upcxx::progress();
                }
            }

            // std::cout << "Rank " << world_rank << " expected: " << expected_count << " object:" << *ping_pong_object << std::endl;
        }

        // End clock
        if (world_rank == 0)
        {
            timer.stop();
            timer.add_time();
        }

        // Reset counter
        *ping_pong_object = 0;
    }

    // Done
    if (world_rank == 0)
    {
        std::cout << "Ping-pong count: " << ping_pong_count << std::endl;
        std::cout << "Repetitions: " << reps << std::endl;
        timer.print_times();
    }

    upcxx::finalize();

    return 0;
}