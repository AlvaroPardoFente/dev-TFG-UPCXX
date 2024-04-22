#include <mpi.h>
#include <mpi_benchmark_template.hpp>
#include <iostream>
#include <chrono>
#include <numeric>

class mpi_broadcast_array : public MpiBenchmarkTemplate
{
public:
    std::vector<uint32_t> value;

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkTemplate::init(argc, argv);

        // Vector initialization
        value.resize(number_count);
        for (uint32_t i = 0; i < number_count; i++)
        {
            value.at(i) = i;
        }
    };

    void warmup() override
    {
        MPI_Barrier(MPI_COMM_WORLD);
        MpiBenchmarkTemplate::warmup();
    }

    void run_benchmark() override
    {
        MPI_Barrier(MPI_COMM_WORLD);
        MpiBenchmarkTemplate::run_benchmark();
    }

    void benchmark_body() override
    {
        MPI_Bcast(value.data(), number_count, MPI_INT, 0, MPI_COMM_WORLD);

        // if (world_rank)
        // {
        //     for (auto it = value.begin(); it != value.end(); ++it)
        //     {
        //         if (*it != std::distance(value.begin(), it))
        //         {
        //             std::cout << "Error at " << std::distance(value.begin(), it) << " with value " << *it << std::endl;
        //         }
        //     }
        // }
    }

    void reset_result() override
    {
        // Reset result
        if (world_rank != 0)
            std::fill(value.begin(), value.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    mpi_broadcast_array test;
    test.run(argc, argv);
    return 0;
}