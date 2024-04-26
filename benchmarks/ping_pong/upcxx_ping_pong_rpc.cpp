#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

bool received_flag = false;

class UpcxxPingPongRpc : public UpcxxBenchmarkScheme
{
public:
    int neighbor_rank;
    upcxx::dist_object<uint32_t> ping_pong_object;
    // Flag to check if the message was received
    // upcxx::dist_object<bool> received_flag;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        if (world_size != 2)
        {
            if (world_rank == 0)
            {
                std::cerr << "This benchmark must be run with 2 processes" << std::endl;
            }
            upcxx::finalize();
            std::exit(1);
        }

        neighbor_rank = (world_rank + 1) % 2;

        ping_pong_object = upcxx::dist_object<uint32_t>(0);

        // received_flag = upcxx::dist_object<bool>(false);
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {

            if (world_rank == i % 2)
            {
                (*ping_pong_object)++;
                upcxx::rpc(
                    neighbor_rank, [](const uint32_t &value, upcxx::dist_object<uint32_t> &ping_pong_object)
                    {
                        *ping_pong_object = value;
                        received_flag = true; },
                    *ping_pong_object, ping_pong_object)
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
        // Reset counter
        *ping_pong_object = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxPingPongRpc test;
    test.run(argc, argv);
    return 0;
}