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

    int value = 0;
    for (int i = world_rank; i < number_count; i += world_size)
    {
        value += i;
    }

    int result;

    result = upcxx::reduce_one(value, upcxx::op_fast_add, 0).wait();

    // Done

    if (world_rank == 0)
    {
        std::cout << "Result: " << result << std::endl;

        int expected_result = 0;
        for (int i = 0; i < number_count; i++)
        {
            expected_result += i;
        }

        if (result != expected_result)
        {
            std::cout << "Error: " << result << " != " << expected_result << std::endl;
        }
    }

    upcxx::finalize();
}