#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <ping_pong_settings.hpp>
#include <iostream>

// Flag to check if the message was received
bool received_flag = false;

class UpcxxPingPongRpc : public UpcxxBenchmarkScheme
{
public:
    int neighbor_rank;
    PingPongSettings *ping_pong_settings;

    uint32_t block_size{1};
    uint32_t *ping_pong_values;
    upcxx::dist_object<uint32_t *> ping_pong_object;

    UpcxxPingPongRpc() : UpcxxBenchmarkScheme(ping_pong_settings = new PingPongSettings()) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        if (world_size % 2 != 0 && world_rank == 0)
        {
            std::cerr << "This benchmark requires an even number of processes" << std::endl;
            finalize();
            exit(1);
        }

        block_size = ping_pong_settings->block_size.has_value() ? ping_pong_settings->block_size.value() : 1;

        print_columns.clear();
        print_columns["Iterations"] = std::to_string(number_count);
        print_columns["Block size"] = std::to_string(block_size);

        neighbor_rank = world_rank % 2 == 0 ? world_rank + 1 : world_rank - 1;

        ping_pong_object = upcxx::dist_object<uint32_t *>(new uint32_t[block_size]);
        ping_pong_values = *ping_pong_object;

        // Initialize values
        std::fill(ping_pong_values, ping_pong_values + block_size, 0);
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {

            if (world_rank % 2 == i % 2)
            {
                ping_pong_values[0]++;
                upcxx::rpc(
                    neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t *> &ping_pong_object)
                    {
                        (*ping_pong_object)[0] = value;
                        received_flag = true; },
                    ping_pong_values[0], ping_pong_object)
                    .wait();
            }
            else
            {
                while (!received_flag)
                {
                    upcxx::progress();
                }

                received_flag = false;
            }

            // std::cout << "Rank " << world_rank << " object:" << *ping_pong_object << std::endl;
        }
    }

    void reset_result() override
    {
        // std::cout << "Final values: " << std::endl;
        // for (size_t i = 0; i < block_size; i++)
        // {
        //     std::cout << ping_pong_values[i] << ", ";
        // }
        // std::cout << std::endl;
        // Reset counter
        ping_pong_values[0] = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxPingPongRpc test;
    test.run(argc, argv);
    return 0;
}