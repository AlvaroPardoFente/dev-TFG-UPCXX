#include <upcxx/upcxx.hpp>
#include <chrono>
#include <iostream>
#include "../../utils/benchmark_settings.hpp"

// ping_pong_count used on warmup
constexpr uint32_t warmup_count = 1024;
// warmup executions
constexpr uint32_t warmup_repetitions = 10;

int main(int argc, char **argv)
{
    uint32_t ping_pong_count = 1024;
    uint reps = 1;

    settings::BenchmarkSettings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
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

    upcxx::init();

    int world_rank, world_size;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

    int neighbor_rank = (world_rank + 1) % 2;

    upcxx::dist_object<upcxx::global_ptr<uint32_t>> global_ping_pong_object = upcxx::new_<uint32_t>(0);
    uint32_t &ping_pong_value = *global_ping_pong_object->local();
    upcxx::global_ptr<uint32_t> neighbor_ping_pong_ptr = global_ping_pong_object.fetch(neighbor_rank).wait();

    // Warmup
    if (settings.warmup)
    {
        for (int i = 0; i < warmup_repetitions; i++)
        {
            for (uint32_t i = 0; i < warmup_count; i++)
            {

                if (world_rank == i % 2)
                {
                    ping_pong_value++;
                    upcxx::rput(ping_pong_value, neighbor_ping_pong_ptr).wait();
                    upcxx::barrier();
                }
                else
                {
                    upcxx::barrier();
                }
            }
            // Reset counter
            ping_pong_value = 0;
        }
    }

    for (uint rep = 0; rep < reps; ++rep)
    {
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
                upcxx::rput(ping_pong_value, neighbor_ping_pong_ptr).wait();
                upcxx::barrier();
            }
            else
            {
                upcxx::barrier();
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