#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiReduceAll : public MpiBenchmarkScheme
{
public:
    std::vector<uint32_t> value;
    std::vector<uint32_t> result;

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        // Vector initialization
        value.resize(number_count / world_size);
        for (uint32_t i = 0; i < number_count / world_size; i++)
        {
            value.at(i) = i * world_size + world_rank;
        }

        // Result vector
        result.resize(number_count / world_size);
    };

    void benchmark_body() override
    {
        MPI_Allreduce(value.data(), result.data(), number_count / world_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    }

    void reset_result() override
    {
        // if (world_rank == 0)
        // {
        //     std::cout << "Result: ";
        //     for (uint32_t i = 0; i < number_count / world_size; i++)
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
    MpiReduceAll test;
    test.run(argc, argv);
    return 0;
}