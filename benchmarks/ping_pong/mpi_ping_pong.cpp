#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <ping_pong_settings.hpp>
#include <iostream>
#include <algorithm>

class MpiPingPong : public MpiBenchmarkScheme
{
public:
    PingPongSettings *ping_pong_settings;

    uint32_t block_size{1};
    std::vector<uint32_t> ping_pong_values;
    int neighbor_rank;

    MpiPingPong() : MpiBenchmarkScheme(ping_pong_settings = new PingPongSettings()) {}

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        if (world_size % 2 != 0 && world_rank == 0)
        {
            std::cerr << "This benchmark requires an even number of processes" << std::endl;
            finalize();
            exit(1);
        }

        block_size = ping_pong_settings->block_size.has_value() ? ping_pong_settings->block_size.value() : 1;

        // Int block to increase
        ping_pong_values.resize(block_size, 0);

        // Rank to send to and receive from
        neighbor_rank = world_rank % 2 == 0 ? world_rank + 1 : world_rank - 1;
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            if (world_rank % 2 == i % 2)
            {
                // std::transform(ping_pong_values.begin(), ping_pong_values.end(), ping_pong_values.begin(), [](int n)
                //                { return n + 1; });
                ping_pong_values[0]++;
                MPI_Send(ping_pong_values.data(), ping_pong_values.size(), MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(ping_pong_values.data(), ping_pong_values.size(), MPI_INT, neighbor_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }

    void reset_result() override
    {
        // std::cout << "Final values: " << std::endl;
        // for (auto num : ping_pong_values)
        // {
        //     std::cout << num << ", ";
        // }
        // std::cout << std::endl;
        // Reset counter
        std::fill(ping_pong_values.begin(), ping_pong_values.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    MpiPingPong test;
    test.run(argc, argv);
    return 0;
}