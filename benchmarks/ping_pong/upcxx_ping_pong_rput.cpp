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

    upcxx::dist_object<upcxx::global_ptr<uint32_t>> global_ping_pong_object = upcxx::new_<uint32_t>(0);
    uint32_t &ping_pong_value = *global_ping_pong_object->local();
    upcxx::global_ptr<uint32_t> neighbor_ping_pong_ptr = global_ping_pong_object.fetch(neighbor_rank).wait();

    // Flag to check if the message was received
    upcxx::dist_object<bool> received_flag(false);
    // bool received_flag = false;

    // Warmup
    if (settings.warmup)
    {
        for (int i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            for (uint32_t i = 0; i < warmup_count; i++)
            {

                if (world_rank == i % 2)
                {
                    ping_pong_value++;
                    upcxx::rput(ping_pong_value, neighbor_ping_pong_ptr, upcxx::remote_cx::as_rpc([](upcxx::dist_object<bool> &received_flag)
                                                                                                  { *received_flag = true; },
                                                                                                  received_flag));
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
            ping_pong_value = 0;
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        upcxx::barrier();

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
                upcxx::rput(ping_pong_value, neighbor_ping_pong_ptr, upcxx::remote_cx::as_rpc([](upcxx::dist_object<bool> &received_flag)
                                                                                              { *received_flag = true; },
                                                                                              received_flag));
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
            timer.stop();
            timer.add_time();
        }

        // Reset counter
        ping_pong_value = 0;
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