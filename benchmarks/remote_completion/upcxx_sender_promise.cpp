#include <upcxx/upcxx.hpp>
#include "../../utils/benchmark_settings.hpp"
#include "../../utils/benchmark_timer.hpp"
#include <iostream>
#include <chrono>
#include <numeric>

// Returns false if at least one element in the array is zero
bool is_array_complete(uint32_t *array, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (array[i] == 0)
        {
            return false;
        }
    }
    return true;
}

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
            value[i] = i + 1;
        }
    }
    else if (world_rank == 0)
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            value[i] = 0;
        }
    }

    // Completion bool as a dist_object
    upcxx::global_ptr<bool> completion_flag_g = upcxx::new_<bool>(false);
    bool *completion_flag_l = completion_flag_g.local();

    // Broadcasting root completion flag
    upcxx::global_ptr<bool> root_flag_ptr = upcxx::broadcast(completion_flag_g, 0).wait();

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
                // Init promise
                upcxx::promise<> p;

                for (uint32_t i = 0; i < number_count; i++)
                {
                    upcxx::rput(value[i], root_ptr + i, upcxx::operation_cx::as_promise(p));
                }

                p.finalize().wait();
                upcxx::rput(true, root_flag_ptr).wait();
            }

            // Check for completion
            if (world_rank == 0)
            {
                while (*completion_flag_l == false)
                {
                    upcxx::progress();
                }

                *completion_flag_l = false;
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
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(value[i], root_ptr + i, upcxx::operation_cx::as_promise(p));
            }

            p.finalize().wait();
            upcxx::rput(true, root_flag_ptr).wait();
        }

        // Check for completion
        if (world_rank == 0)
        {
            while (*completion_flag_l == false)
            {
                upcxx::progress();
            }

            *completion_flag_l = false;
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