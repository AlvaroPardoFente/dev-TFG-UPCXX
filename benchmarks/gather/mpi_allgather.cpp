#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiAllgather : public MpiBenchmarkScheme
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
            value.at(i) = i + world_rank * (number_count / world_size);
        }

        // Result vector
        result.resize(number_count);
    };

    void benchmark_body() override
    {
        MPI_Allgather(value.data(), number_count / world_size, MPI_INT, result.data(), number_count / world_size, MPI_INT, MPI_COMM_WORLD);
    }

    void reset_result() override
    {
        // for (size_t i = 0; i < result.size(); i++)
        // {
        //     std::cout << result[i] << " ";
        // }
        std::fill(result.begin(), result.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    MpiAllgather test;
    test.run(argc, argv);
    return 0;
}