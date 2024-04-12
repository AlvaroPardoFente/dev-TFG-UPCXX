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

    // UPCXX Initialization
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

    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g(upcxx::new_array<uint32_t>(number_count));
    uint32_t *value = value_g->local();

    if (world_rank == 1)
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            value[i] = i;
        }
    }
    else if (world_rank == 0)
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            value[i] = 0;
        }
    }

    // Counter as a dist object
    upcxx::dist_object<uint32_t> count(0);

    // Root pointer
    upcxx::global_ptr<uint32_t> root_ptr = value_g.fetch(0).wait();

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            if (world_rank == 1)
            {
                for (uint32_t i = 0; i < number_count; i++)
                {
                    upcxx::rput(value[i], root_ptr + i, upcxx::remote_cx::as_rpc([](upcxx::dist_object<uint32_t> &count)
                                                                                 { (*count)++; },
                                                                                 count));
                }
            }

            // Check for completion
            if (world_rank == 0)
            {
                while (*count < number_count)
                {
                    upcxx::progress();
                }

                *count = 0;
            }

            if (world_rank == 0)
            {
                auto *value_l = value_g->local();
                for (uint32_t i = 0; i < number_count; i++)
                {
                    value_l[i] = 0;
                }
            }
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

        // Perform rput for every value, increasing counter with rpc
        if (world_rank == 1)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(value[i], root_ptr + i, upcxx::remote_cx::as_rpc([](upcxx::dist_object<uint32_t> &count)
                                                                             { (*count)++; },
                                                                             count));
            }
        }

        // Check for completion
        if (world_rank == 0)
        {
            while (*count < number_count)
            {
                upcxx::progress();
            }

            *count = 0;
        }

        // End clock
        if (world_rank == 0)
        {
            timer.stop();
            timer.add_time();
        }

        if (world_rank == 0)
        {
            // // Print result
            // if (world_rank == 0)
            // {
            //     for (int i = 0; i < number_count; i++)
            //     {
            //         std::cout << value_g->local()[i] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            // Reset result
            if (world_rank == 0)
            {
                auto *value_l = value_g->local();
                for (uint32_t i = 0; i < number_count; i++)
                {
                    value_l[i] = 0;
                }
            }
        }
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