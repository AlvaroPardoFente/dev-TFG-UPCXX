#include <mpi.h>

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
    MPI_Init(NULL, NULL);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int value[number_count / world_size];
    for (int i = 0; i < number_count / world_size; i++)
    {
        value[i] = i * world_size + world_rank;
    }

    int result[number_count / world_size];

    MPI_Reduce(&value, &result, number_count / world_size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

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

    MPI_Finalize();
}