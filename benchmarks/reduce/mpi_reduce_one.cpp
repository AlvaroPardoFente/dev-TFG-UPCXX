#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiReduceOne : public MpiBenchmarkScheme
{
public:
    std::vector<uint32_t> value;
    std::vector<uint32_t> result;

    uint32_t nums_per_rank;

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        nums_per_rank = number_count;

        // Vector initialization
        value.resize(nums_per_rank);
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value.at(i) = i * world_size + world_rank;
        }

        // Result vector
        if (world_rank == 0)
        {
            result.resize(nums_per_rank);
        }
    };

    void benchmark_body() override
    {
        MPI_Reduce(value.data(), result.data(), nums_per_rank, MPI_UINT32_T, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    void reset_result() override
    {
        // Print result
        // if (world_rank == 0)
        // {
        //     std::cout << "Result: ";
        //     for (uint32_t i = 0; i < nums_per_rank; i++)
        //     {
        //         std::cout << result[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // Reset result
        std::fill(result.begin(), result.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    MpiReduceOne test;
    test.run(argc, argv);
    return 0;
}