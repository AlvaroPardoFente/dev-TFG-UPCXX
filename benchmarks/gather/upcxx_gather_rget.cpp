#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxGatherRget : public UpcxxBenchmarkScheme
{
public:
    int32_t nums_per_rank;
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Result vector
    std::vector<uint32_t> result;

    std::vector<upcxx::global_ptr<uint32_t>> global_ptrs;

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

        if (world_rank == 0)
        {
            result.resize(number_count);
        }

        // Fetch pointers for all processes
        if (world_rank == 0)
        {
            global_ptrs.resize(world_size);
            for (int i = 0; i < world_size; i++)
            {
                global_ptrs.at(i) = value_g.fetch(i).wait();
            }
        }
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            // Init promise
            upcxx::promise<> p;

            for (int i = 0; i < world_size; i++)
            {
                upcxx::rget(global_ptrs.at(i), &result.data()[i * nums_per_rank], nums_per_rank, upcxx::operation_cx::as_promise(p));
            }

            // Wait for all operations to finish
            p.finalize().wait();
        }
    }

    void reset_result() override
    {
        // if (world_rank == 0)
        // {
        //     for (auto num : result)
        //     {
        //         std::cout << num << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // Reset result
        std::fill(result.begin(), result.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    UpcxxGatherRget test;
    test.run(argc, argv);
    return 0;
}