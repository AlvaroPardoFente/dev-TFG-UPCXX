#include <upcxx/upcxx.hpp>
#include "../../utils/benchmark_settings.hpp"
#include "../../utils/benchmark_timer.hpp"
#include <iostream>
#include <chrono>
#include <numeric>

// warmup executions
constexpr uint32_t warmup_repetitions = 10;

// Count for remote completions
int count = 0;

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

    // Clocks
    benchmark_timer timer;
    if (world_rank == 0)
    {
        timer.reserve(reps);
    }

    // Vector initialization
    int32_t nums_per_rank = number_count / world_size;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g(upcxx::new_array<uint32_t>(nums_per_rank));
    uint32_t *value = value_g->local();

    for (uint32_t i = 0; i < nums_per_rank; i++)
    {
        value[i] = i + world_rank * nums_per_rank;
    }

    // Result vector
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> result(upcxx::new_array<uint32_t>(number_count));

    // Global ptr vector
    std::vector<upcxx::global_ptr<uint32_t>> result_ptrs(world_size);
    for (int i = 0; i < world_size; i++)
    {
        result_ptrs[i] = result.fetch(i).wait();
    }

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            for (int i = 0; i < world_size; i++)
            {
                upcxx::rput(value, result_ptrs.at(i) + world_rank * nums_per_rank, nums_per_rank, upcxx::remote_cx::as_rpc([]()
                                                                                                                           { count++; }));
            }

            // Check for completion
            while (count < world_size)
            {
                upcxx::progress();
            }

            count = 0;
            auto *result_l = result->local();
            for (uint32_t i = 0; i < nums_per_rank; i++)
            {
                result_l[i] = 0;
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

        for (int i = 0; i < world_size; i++)
        {
            upcxx::rput(value, result_ptrs.at(i) + world_rank * nums_per_rank, nums_per_rank, upcxx::remote_cx::as_rpc([]()
                                                                                                                       { count++; }));
        }

        // Check for completion
        while (count < world_size)
        {
            upcxx::progress();
        }

        // End clock
        if (world_rank == 0)
        {
            timer.stop();
            timer.add_time();
        }

        // Print result
        // for (int i = 0; i < number_count; i++)
        // {
        //     std::cout << result->local()[i] << " ";
        // }
        // std::cout << std::endl;

        // Reset result
        count = 0;
        auto *result_l = result->local();
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            result_l[i] = 0;
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