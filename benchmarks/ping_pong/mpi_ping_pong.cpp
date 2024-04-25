#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiPingPong : public MpiBenchmarkScheme
{
public:
    uint32_t ping_pong_value;
    int neighbor_rank;

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        if (world_size != 2)
        {
            if (world_rank == 0)
            {
                std::cerr << "This benchmark must be run with 2 processes" << std::endl;
            }
            MPI_Finalize();
            std::exit(1);
        }

        // Int to increase
        ping_pong_value = 0;

        // Rank to send to and receive from
        neighbor_rank = (world_rank + 1) % 2;
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            if (world_rank == i % 2)
            {
                ping_pong_value++;
                MPI_Send(&ping_pong_value, 1, MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(&ping_pong_value, 1, MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }

    void reset_result() override
    {
        if (world_rank == 0)
        {
            // std::cout << "Final value: " << ping_pong_value << std::endl;
            // Reset counter
            ping_pong_value = 0;
        }
    }
};

int main(int argc, char *argv[])
{
    MpiPingPong test;
    test.run(argc, argv);
    return 0;
}