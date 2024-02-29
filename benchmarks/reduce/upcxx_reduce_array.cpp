#include <upcxx/upcxx.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    int number_count = 1024;
    if (argc > 1)
    {
        try
        {
            number_count = std::stoi(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            // Argument is not a number, do nothing
        }
    }
    upcxx::init();
    int world_size, world_rank;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    int value[number_count / world_size];
    for (int i = 0; i < number_count / world_size; i++)
    {
        value[i] = i * world_size + world_rank;
    }

    int result[number_count / world_size];

    upcxx::reduce_one(value, result, number_count / world_size, upcxx::op_fast_add, 0).wait();

    // Done

    if (world_rank == 0)
    {
        std::cout << "Result: ";
        for (int i = 0; i < number_count / world_size; i++)
        {
            std::cout << result[i] << " ";
        }
        std::cout << std::endl;

        int expected_result[number_count / world_size] = {0};
        for (int i = 0; i < number_count; i++)
        {
            expected_result[i / world_size] += i;
        }

        for (int i = 0; i < number_count / world_size; i++)
        {
            if (result[i] != expected_result[i])
            {
                std::cout << "Error: " << result[i] << " != " << expected_result[i] << std::endl;
            }
        }
    }

    upcxx::finalize();
}