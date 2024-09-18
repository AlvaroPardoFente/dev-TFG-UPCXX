#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <ping_pong_settings.hpp>
#include <iostream>

/**
 * @brief Ping-pong benchmark implementation in UPC++ using then() for counter updating
 *
 */
class UpcxxPingPongRput : public UpcxxBenchmarkScheme
{
public:
    PingPongSettings *ping_pong_settings;

    uint32_t block_size{1};

    int neighbor_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> global_ping_pong_object;
    uint32_t *ping_pong_values;
    upcxx::global_ptr<uint32_t> neighbor_ping_pong_ptr;

    upcxx::dist_object<upcxx::global_ptr<bool>> global_received_flag;
    bool *received_flag;
    upcxx::global_ptr<bool> neighbor_received_flag;

    UpcxxPingPongRput() : UpcxxBenchmarkScheme(ping_pong_settings = new PingPongSettings()) {}

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

        global_ping_pong_object = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(block_size));
        ping_pong_values = global_ping_pong_object->local();
        neighbor_ping_pong_ptr = global_ping_pong_object.fetch(neighbor_rank).wait();

        // Initialize values
        std::fill(ping_pong_values, ping_pong_values + block_size, 0);

        global_received_flag = upcxx::dist_object<upcxx::global_ptr<bool>>(upcxx::new_array<bool>(false));
        received_flag = global_received_flag->local();
        neighbor_received_flag = global_received_flag.fetch(neighbor_rank).wait();
    };

    void benchmark_body() override
    {
        for (uint32_t i = 0; i < number_count; i++)
        {
            if (world_rank % 2 == i % 2)
            {
                ping_pong_values[0]++;
                upcxx::promise<> p;
                upcxx::rput(ping_pong_values, neighbor_ping_pong_ptr, block_size).then([=]()
                                                                                       { upcxx::rput(true, neighbor_received_flag, upcxx::operation_cx::as_promise(p)); });
                p.finalize().wait();
            }
            else
            {
                while (!received_flag)
                {
                    upcxx::progress();
                }
                // std::cout << "Rank " << world_rank << " object:" << *ping_pong_value << std::endl;
                *received_flag = false;
            }
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
    UpcxxPingPongRput test;
    test.run(argc, argv);
    return 0;
}