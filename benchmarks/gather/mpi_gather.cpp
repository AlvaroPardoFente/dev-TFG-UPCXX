#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiGather : public MpiBenchmarkScheme
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
            value.at(i) = i + world_rank * (nums_per_rank);
        }

        // Result vector
        if (world_rank == 0)
        {
            result.resize(nums_per_rank * world_size);
        }
    };

    void benchmark_body() override
    {
        MPI_Gather(value.data(), nums_per_rank, MPI_UINT32_T, result.data(), nums_per_rank, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    }

    void reset_result() override
    {
        // if (world_rank == 0)
        // {
        //     for (size_t i = 0; i < result.size(); i++)
        //     {
        //         std::cout << result[i] << " ";
        //     }
        // }
        std::fill(result.begin(), result.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    MpiGather test;
    test.run(argc, argv);
    return 0;
}