#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxPingPongRputNoFlag : public UpcxxBenchmarkScheme
{
public:
    int neighbor_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> global_ping_pong_object;
    uint32_t *ping_pong_value;
    upcxx::global_ptr<uint32_t> neighbor_ping_pong_ptr;

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
    };

    void benchmark_body() override
    {
        uint32_t expected_count = 0;

        for (uint32_t i = 0; i < number_count; i++)
        {
            expected_count++;

            if (world_rank == i % 2)
            {
                (*ping_pong_value)++;
                upcxx::rput(*ping_pong_value, neighbor_ping_pong_ptr).wait();
            }
            else
            {
                while (expected_count != (*ping_pong_value))
                {
                    upcxx::progress();
                }
                // std::cout << "Rank " << world_rank << " expected: " << expected_count << " object:" << *ping_pong_value << std::endl;
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
    UpcxxPingPongRputNoFlag test;
    test.run(argc, argv);
    return 0;
}