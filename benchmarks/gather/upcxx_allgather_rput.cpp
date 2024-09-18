#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

// Count for remote completions
int count = 0;

/**
 * @brief Allgather implemented in UPC++ using crossed sequences of rput
 *
 */
class UpcxxAllgatherRput : public UpcxxBenchmarkScheme
{
public:
    int32_t nums_per_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Result vector
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> result;

    // Global ptr vector
    std::vector<upcxx::global_ptr<uint32_t>> result_ptrs;

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
        result = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(nums_per_rank * world_size));

        // Global ptr vector
        result_ptrs.resize(world_size);
        for (int i = 0; i < world_size; i++)
        {
            result_ptrs[i] = result.fetch(i).wait();
        }
    };

    void benchmark_body() override
    {
        for (int i = 0; i < world_size; i++)
        {
            if (world_rank != i)
            {
                upcxx::rput(value, result_ptrs.at(i) + world_rank * nums_per_rank, nums_per_rank, upcxx::remote_cx::as_rpc([]()
                                                                                                                           { count++; }));
            }
            else
            {
                std::copy(value, value + nums_per_rank, result->local() + world_rank * nums_per_rank);
                count++;
            }
        }

        // Check for completion
        while (count < world_size)
        {
            upcxx::progress();
        }
    }

    void reset_result() override
    {
        // Print result
        // for (int i = 0; i < nums_per_rank * world_size; i++)
        // {
        //     std::cout << result->local()[i] << " ";
        // }
        // std::cout << std::endl;
        // Reset result
        count = 0;
        auto *result_l = result->local();
        std::fill(result_l, result_l + nums_per_rank * world_size, 0);
    }
};

int main(int argc, char *argv[])
{
    UpcxxAllgatherRput test;
    test.run(argc, argv);
    return 0;
}