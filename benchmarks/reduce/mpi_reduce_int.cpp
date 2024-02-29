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

    int value = 0;
    for (int i = world_rank; i < number_count; i += world_size)
    {
        value += i;
    }

    int result;

    MPI_Reduce(&value, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

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

    MPI_Finalize();
}