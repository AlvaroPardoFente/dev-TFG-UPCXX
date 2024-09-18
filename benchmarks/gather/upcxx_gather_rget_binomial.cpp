#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

/**
 * @brief Gather implemented in UPC++ using a binomial algorithm with rget
 *
 */
class UpcxxGatherRgetBinomial : public UpcxxBenchmarkScheme
{
public:
    int32_t nums_per_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    std::vector<upcxx::global_ptr<uint32_t>> global_ptrs;

    uint32_t final_size;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        // Vector initialization
        nums_per_rank = number_count;
        final_size = nums_per_rank;

        for (int world_iter = world_size; world_iter > 1; world_iter /= 2)
        {
            if (world_rank % world_iter == 0)
            {
                final_size *= 2;
            }
        }

        value_g = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(final_size));
        value = value_g->local();

        // Init values
        for (uint32_t i = 0; i < final_size; i++)
        {
            value[i] = 0;
        }
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value[i] = i + world_rank * nums_per_rank;
        }

        // Global ptr vector
        global_ptrs.resize(world_size);

        for (int i = 0; i < world_size; i++)
        {
            global_ptrs.at(i) = value_g.fetch(i).wait();
        }
    };

    void benchmark_body() override
    {
        uint32_t filled_nums = nums_per_rank;
        int stride = 1;
        for (int stride = 1; stride < world_size; stride *= 2)
        {
            if (world_rank % (2 * stride) == 0 && world_rank + stride < world_size)
            {
                // Gather data from process world_rank + stride
                upcxx::rget(global_ptrs.at(world_rank + stride), &(value[filled_nums]), filled_nums).wait();

                filled_nums *= 2;
            }

            upcxx::barrier();
        }
    }

    void reset_result() override
    {
        // // Print result
        // if (world_rank == 0)
        // {
        //     std::cout << "Result in rank " << world_rank << ": ";
        //     for (uint32_t i = 0; i < final_size; i++)
        //     {
        //         std::cout << value[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // Reset result
        std::fill(value, value + final_size, 0);
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value[i] = i + world_rank * nums_per_rank;
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxGatherRgetBinomial test;
    test.run(argc, argv);
    return 0;
}