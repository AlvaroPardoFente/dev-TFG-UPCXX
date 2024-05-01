#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <non_contiguous_settings.hpp>
#include <iostream>

// Count for remote completions
int count = 0;

class UpcxxRputRegular : public UpcxxBenchmarkScheme
{
public:
    NonContiguousSettings *non_contiguous_settings;

    uint32_t block_size{0};

    int32_t nums_per_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Iterables for non-contiguous operations
    std::vector<uint32_t *> srcs;
    std::vector<upcxx::global_ptr<uint32_t>> dsts;

    // Result vector
    upcxx::global_ptr<uint32_t> result;

    upcxx::global_ptr<uint32_t> root_ptr;

    UpcxxRputRegular() : UpcxxBenchmarkScheme(non_contiguous_settings = new NonContiguousSettings()) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        // Vector initialization
        nums_per_rank = number_count / world_size;
        value_g = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(nums_per_rank));
        value = value_g->local();

        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value[i] = i + world_rank * nums_per_rank;
        }

        // Result vector
        result = upcxx::new_array<uint32_t>(world_rank == 0 ? number_count : 0);

        // Broadcast root ptr to all processes
        root_ptr = upcxx::broadcast(result, 0).wait();

        uint32_t block_count = non_contiguous_settings->block_count.has_value() ? non_contiguous_settings->block_count.value() : 1;
        block_size = nums_per_rank / block_count;

        if (nums_per_rank % block_count != 0)
        {
            if (world_rank == 0)
            {
                std::cerr << "Number of processes should be a multiple of block count" << std::endl;
            }
            upcxx::finalize();
            std::exit(1);
        }

        for (uint32_t i = 0; i < block_count; i++)
        {
            srcs.push_back(&(value_g->local()[i * block_size]));
            dsts.push_back(root_ptr + world_rank * nums_per_rank + i * block_size);
        }
    };

    void benchmark_body() override
    {
        upcxx::rput_regular(srcs.begin(),
                            srcs.end(),
                            block_size,
                            dsts.begin(),
                            dsts.end(),
                            block_size,
                            upcxx::remote_cx::as_rpc([]()
                                                     { count++; }));

        if (world_rank == 0)
        {
            while (count < world_size)
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
            //     for (int i = 0; i < number_count; i++)
            //     {
            //         std::cout << result.local()[i] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            // Reset result
            count = 0;
            auto *result_l = result.local();
            for (uint32_t i = 0; i < nums_per_rank; i++)
            {
                result_l[i] = 0;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxRputRegular test;
    test.run(argc, argv);
    return 0;
}