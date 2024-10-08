#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

// Counter as a global variable
int32_t count = 0;

/**
 * @brief Sequence of RMAs followed by as_rpc with a global variable as parameter
 *
 * The goal of this benchmark is to evaluate the performance of any form of rpc
 * with global variables as parameters
 *
 */
class UpcxxRpcGlobalVar : public UpcxxBenchmarkScheme
{
public:
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    uint32_t dest;

    // Root pointer
    upcxx::global_ptr<uint32_t> root_ptr;

    UpcxxRpcGlobalVar() : UpcxxBenchmarkScheme() {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        dest = (world_rank - 1) * number_count;

        value_g = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(
            upcxx::new_array<uint32_t>(
                world_rank ? number_count : (world_size - 1) * number_count));
        value = value_g->local();

        if (world_rank)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                value[i] = dest + i;
            }
        }
        else if (world_rank == 0)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                value[i] = 0;
            }
        }

        root_ptr = value_g.fetch(0).wait();
    };

    void benchmark_body() override
    {
        // Perform rput for every value, increasing counter with rpc
        if (world_rank)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(value[i], root_ptr + dest + i, upcxx::remote_cx::as_rpc([]()
                                                                                    { count++; }));
            }
        }

        // Check for completion
        if (world_rank == 0)
        {
            uint32_t goal = number_count * (world_size - 1);
            while (count < goal)
            {
                upcxx::progress();
            }

            count = 0;
        }
    }

    void reset_result() override
    {
        // // Print result
        // if (world_rank == 0)
        // {
        //     for (int i = 0; i < number_count * (world_size - 1); i++)
        //     {
        //         std::cout << value_g->local()[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // Reset result
        if (world_rank == 0)
        {
            auto *value_l = value_g->local();
            for (uint32_t i = 0; i < number_count; i++)
            {
                value_l[i] = 0;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcGlobalVar test;
    test.run(argc, argv);
    return 0;
}