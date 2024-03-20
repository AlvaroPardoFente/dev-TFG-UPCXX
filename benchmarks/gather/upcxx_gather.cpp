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
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;
    std::vector<double> times(reps);

    // Vector initialization
    int32_t nums_per_rank = number_count / world_size;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g(upcxx::new_array<uint32_t>(nums_per_rank));
    uint32_t *value = value_g->local();

    for (uint32_t i = 0; i < nums_per_rank; i++)
    {
        value[i] = i + world_rank * nums_per_rank;
    }

    // Result vector
    std::vector<uint32_t> result;
    if (world_rank == 0)
    {
        result.resize(number_count);
    }

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            if (world_rank == 0)
            {
                // Init promise
                upcxx::promise<> p;

                // Gather data adding to promise
                for (int i = 0; i < world_size; i++)
                {
                    auto value_n = value_g.fetch(i).wait();
                    upcxx::rget(value_n, &result.data()[i * nums_per_rank], nums_per_rank, upcxx::operation_cx::as_promise(p));
                }

                // Wait for all operations to finish
                p.finalize().wait();

                // Reset result
                std::fill(result.begin(), result.end(), 0);
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
            start_ops = std::chrono::high_resolution_clock::now();
        }

        if (world_rank == 0)
        {
            // Init promise
            upcxx::promise<> p;

            for (int i = 0; i < world_size; i++)
            {
                auto value_n = value_g.fetch(i).wait();
                upcxx::rget(value_n, &result.data()[i * nums_per_rank], nums_per_rank, upcxx::operation_cx::as_promise(p));
            }

            // Wait for all operations to finish
            p.finalize().wait();
        }

        // if (world_rank == 0)
        // {
        //     for (auto num : result)
        //     {
        //         std::cout << num << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // End clock
        if (world_rank == 0)
        {
            end_ops = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
            times[rep] = time_span.count();
        }

        if (world_rank == 0)
        {
            // for (auto num : result)
            // {
            //     std::cout << num << " ";
            // }
            // std::cout << std::endl;

            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }

    // Done
    if (world_rank == 0)
    {
        std::cout << "Number count: " << number_count << std::endl;
        std::cout << "Repetitions: " << reps << std::endl;
        std::cout << "Average time: " << std::accumulate(times.begin(), times.end(), 0.0) / reps << std::endl;
        std::cout << "Times: ";
        std::cout << times[0];
        for (uint rep = 1; rep < reps; ++rep)
        {
            std::cout << ", " << times[rep];
        }
        std::cout << std::endl;
    }

    upcxx::finalize();
}