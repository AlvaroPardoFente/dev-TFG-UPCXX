#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>
#include <algorithm>

// Count for remote completions
int count = 0;

class UpcxxGatherRput : public UpcxxBenchmarkScheme
{
public:
    int32_t nums_per_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Result vector
    upcxx::global_ptr<uint32_t> result;

    upcxx::global_ptr<uint32_t> root_ptr;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        // Vector initialization
        nums_per_rank = number_count;
        value_g = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(nums_per_rank));
        value = value_g->local();

        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value[i] = i + world_rank * nums_per_rank;
        }

        // Result vector
        result = upcxx::new_array<uint32_t>(world_rank == 0 ? nums_per_rank * world_size : 0);

        // Broadcast root ptr to all processes
        root_ptr = upcxx::broadcast(result, 0).wait();
    };

    void benchmark_body() override
    {
        if (world_rank != 0)
        {
            upcxx::rput(value, root_ptr + world_rank * nums_per_rank, nums_per_rank, upcxx::remote_cx::as_rpc([]()
                                                                                                              { count++; }));
        }
        else
        {
            std::copy(value, value + nums_per_rank, result.local());
        }

        if (world_rank == 0)
        {
            // Wait for completion
            while (count < world_size - 1)
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        if (world_rank == 0)
        {
            // if (world_rank == 0)
            // {
            //     for (int i = 0; i < nums_per_rank * world_size; i++)
            //     {
            //         std::cout << result.local()[i] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            // Reset result
            auto *result_l = result.local();
            std::fill(result_l, result_l + nums_per_rank * world_size, 0);
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxGatherRput test;
    test.run(argc, argv);
    return 0;
}