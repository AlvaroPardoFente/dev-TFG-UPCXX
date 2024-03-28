#include <upcxx/upcxx.hpp>
#include <chrono>
#include <iostream>
#include "../../utils/benchmark_settings.hpp"
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
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

    int neighbor_rank = (world_rank + 1) % 2;

    upcxx::dist_object<uint32_t> ping_pong_object(0);

    // Flag to check if the message was received
    upcxx::dist_object<bool> received_flag(false);

    // Warmup
    if (settings.warmup)
    {
        for (int i = 0; i < warmup_repetitions; i++)
        {
            for (uint32_t i = 0; i < warmup_count; i++)
            {

                if (world_rank == i % 2)
                {
                    (*ping_pong_object)++;
                    upcxx::rpc(
                        neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t> &ping_pong_object, upcxx::dist_object<bool> &received_flag)
                        {
                            *ping_pong_object = value;
                            *received_flag = true; },
                        *ping_pong_object, ping_pong_object, received_flag)
                        .wait();
                }
                else
                {
                    while (!*received_flag)
                    {
                        upcxx::progress();
                    }

                    *received_flag = false;
                }
            }
            // Reset counter
            *ping_pong_object = 0;
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        upcxx::barrier();

        // Start clock
        if (world_rank == 0)
        {
            start_ops = std::chrono::high_resolution_clock::now();
        }

        for (uint32_t i = 0; i < ping_pong_count; i++)
        {

            if (world_rank == i % 2)
            {
                (*ping_pong_object)++;
                upcxx::rpc(
                    neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t> &ping_pong_object, upcxx::dist_object<bool> &received_flag)
                    {
                        *ping_pong_object = value;
                        *received_flag = true; },
                    *ping_pong_object, ping_pong_object, received_flag)
                    .wait();
            }
            else
            {
                while (!*received_flag)
                {
                    upcxx::progress();
                }

                *received_flag = false;
            }
        }

        // End clock
        if (world_rank == 0)
        {
            end_ops = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
            times[rep] = time_span.count();
        }

        // Reset counter
        *ping_pong_object = 0;
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

    upcxx::finalize();

    return 0;
}