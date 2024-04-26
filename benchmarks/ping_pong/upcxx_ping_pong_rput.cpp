#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

bool received_flag = false;

class UpcxxPingPongRput : public UpcxxBenchmarkScheme
{
public:
    int neighbor_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> global_ping_pong_object;
    uint32_t *ping_pong_value;
    upcxx::global_ptr<uint32_t> neighbor_ping_pong_ptr;

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

        global_ping_pong_object = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_<uint32_t>(0));
        ping_pong_value = global_ping_pong_object->local();
        neighbor_ping_pong_ptr = global_ping_pong_object.fetch(neighbor_rank).wait();

        // received_flag = upcxx::dist_object<bool>(false);
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            if (world_rank == i % 2)
            {
                (*ping_pong_value)++;
                upcxx::rput(*ping_pong_value, neighbor_ping_pong_ptr, upcxx::remote_cx::as_rpc([](bool &received_flag)
                                                                                               { received_flag = true; },
                                                                                               received_flag));
            }
            else
            {
                while (!received_flag)
                {
                    upcxx::progress();
                }
                // std::cout << "Rank " << world_rank << " object:" << *ping_pong_value << std::endl;
                received_flag = false;
            }
        }
    }

    void reset_result() override
    {
        // Reset counter
        (*ping_pong_value) = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxPingPongRput test;
    test.run(argc, argv);
    return 0;
}