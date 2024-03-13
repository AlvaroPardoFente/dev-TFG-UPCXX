#include <upcxx/upcxx.hpp>
#include "../../utils/benchmark_settings.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char *argv[])
{
    uint32_t number_count = 1024;
    settings::BenchmarkSettings settings = settings::parse_settings(argc, const_cast<const char **>(argv));
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
    upcxx::init();
    int world_size, world_rank;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    // Clocks
    std::chrono::high_resolution_clock::time_point start_ops, end_ops;

    std::vector<uint32_t> value(number_count / world_size);
    for (uint32_t i = 0; i < number_count / world_size; i++)
    {
        value.at(i) = i * world_size + world_rank;
    }

    std::vector<uint32_t> result(number_count / world_size);

    // Start clock
    if (world_rank == 0)
    {
        start_ops = std::chrono::high_resolution_clock::now();
    }

    upcxx::reduce_one(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add, 0).wait();

    // End clock
    if (world_rank == 0)
    {
        end_ops = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end_ops - start_ops);
        std::cout << "Reduce time: " << time_span.count() << " seconds" << std::endl;
        // std::cout << "Ping-pong value: " << number_count << std::endl;
    }

    // Done

    // if (world_rank == 0)
    // {
    //     std::cout << "Result: ";
    //     for (uint32_t i = 0; i < number_count / world_size; i++)
    //     {
    //         std::cout << result.at(i) << " ";
    //     }
    //     std::cout << std::endl;

    // std::vector<uint32_t> expected_result(number_count / world_size);
    // for (uint32_t i = 0; i < number_count; i++)
    // {
    //     expected_result.at(i / world_size) += i;
    // }

    // for (uint32_t i = 0; i < number_count / world_size; i++)
    // {
    //     if (result.at(i) != expected_result.at(i))
    //     {
    //         std::cout << "Error: " << result.at(i) << " != " << expected_result.at(i) << std::endl;
    //     }
    // }
    // }

    upcxx::finalize();
}