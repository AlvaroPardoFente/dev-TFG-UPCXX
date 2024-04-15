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
        timer.set_settings(&settings);
    }

    // Vector initialization
    int32_t nums_per_rank = number_count / world_size;

    uint32_t final_size = nums_per_rank;

    for (int world_iter = world_size; world_iter > 1; world_iter /= 2)
    {
        if (world_rank % world_iter == 0)
        {
            final_size *= 2;
        }
    }

    // std::cout << "Rank " << world_rank << " has final size: " << final_size << std::endl;

    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g(upcxx::new_array<uint32_t>(final_size));
    uint32_t *value = value_g->local();

    // Init values
    for (uint32_t i = 0; i < final_size; i++)
    {
        value[i] = 0;
    }
    for (uint32_t i = 0; i < nums_per_rank; i++)
    {
        value[i] = i + world_rank * nums_per_rank;
    }

    // std::cout << "Rank " << world_rank << " has: ";
    // for (uint32_t i = 0; i < final_size; i++)
    // {
    //     std::cout << value[i] << " ";
    // }
    // std::cout << std::endl;

    // Fetch pointers for all processes
    std::vector<upcxx::global_ptr<uint32_t>> global_ptrs;
    global_ptrs.resize(world_size);

    for (int i = 0; i < world_size; i++)
    {
        global_ptrs.at(i) = value_g.fetch(i).wait();
    }

    // Warmup
    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            upcxx::barrier();

            uint32_t filled_nums = nums_per_rank;

            int stride = 1;
            for (int stride = 1; stride < world_size; stride *= 2)
            {
                if (world_rank % (2 * stride) == 0 && world_rank + stride < world_size)
                {
                    // Gather data from process world_rank + stride
                    upcxx::rget(global_ptrs.at(world_rank + stride), &(value[filled_nums]), filled_nums).wait();

                    filled_nums *= 2;
                }

                upcxx::barrier();
            }

            // Reset result
            for (uint32_t i = 0; i < final_size; i++)
            {
                value[i] = 0;
            }
            for (uint32_t i = 0; i < nums_per_rank; i++)
            {
                value[i] = i + world_rank * nums_per_rank;
            }
        }
    }

    // Benchmark
    for (uint rep = 0; rep < reps; ++rep)
    {
        // Sync all nodes
        upcxx::barrier();

        uint32_t filled_nums = nums_per_rank;

        // Start clock
        if (world_rank == 0)
        {
            timer.start();
        }

        int stride = 1;
        for (int stride = 1; stride < world_size; stride *= 2)
        {
            if (world_rank % (2 * stride) == 0 && world_rank + stride < world_size)
            {
                // Gather data from process world_rank + stride
                upcxx::rget(global_ptrs.at(world_rank + stride), &(value[filled_nums]), filled_nums).wait();

                filled_nums *= 2;

                // // Print state of value
                // std::cout << "State of value in rank " << world_rank << ": ";
                // for (uint32_t i = 0; i < final_size; i++)
                // {
                //     std::cout << value[i] << " ";
                // }
                // std::cout << std::endl;
            }

            upcxx::barrier();
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

        // // Print result
        // if (world_rank == 0)
        // {
        //     std::cout << "Result in rank " << world_rank << ": ";
        //     for (uint32_t i = 0; i < final_size; i++)
        //     {
        //         std::cout << value[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // Reset result
        for (uint32_t i = 0; i < final_size; i++)
        {
            value[i] = 0;
        }
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value[i] = i + world_rank * nums_per_rank;
        }
    }

    // Done
    if (world_rank == 0)
    {
        timer.print_times();
    }

    upcxx::finalize();
}