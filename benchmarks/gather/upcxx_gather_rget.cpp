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
    std::vector<uint32_t> result;
    if (world_rank == 0)
    {
        result.resize(number_count);
    }

    // Fetch pointers for all processes
    std::vector<upcxx::global_ptr<uint32_t>> global_ptrs;

    if (world_rank == 0)
    {
        global_ptrs.resize(world_size);
        for (int i = 0; i < world_size; i++)
        {
            global_ptrs.at(i) = value_g.fetch(i).wait();
        }
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
                    upcxx::rget(global_ptrs.at(i), &result.data()[i * nums_per_rank], nums_per_rank, upcxx::operation_cx::as_promise(p));
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
            timer.start();
        }

        if (world_rank == 0)
        {
            // Init promise
            upcxx::promise<> p;

            for (int i = 0; i < world_size; i++)
            {
                upcxx::rget(global_ptrs.at(i), &result.data()[i * nums_per_rank], nums_per_rank, upcxx::operation_cx::as_promise(p));
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
            timer.stop();
            timer.add_time();
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
        timer.print_times();
    }

    upcxx::finalize();
}