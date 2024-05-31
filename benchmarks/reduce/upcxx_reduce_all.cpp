#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxReduceAll : public UpcxxBenchmarkScheme
{
public:
    std::vector<uint32_t> value;
    std::vector<uint32_t> result;

    uint32_t nums_per_rank;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        nums_per_rank = number_count;

        // Vector initialization
        value.resize(nums_per_rank);
        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            value.at(i) = i * world_size + world_rank;
        }

        // Result vector
        result.resize(nums_per_rank);
    };

    void benchmark_body() override
    {
        upcxx::reduce_all(value.data(), result.data(), nums_per_rank, upcxx::op_fast_add).wait();
    }

    void reset_result() override
    {
        // Print result
        // std::cout << "Result: ";
        // for (uint32_t i = 0; i < nums_per_rank; i++)
        // {
        //     std::cout << result.at(i) << " ";
        // }
        // std::cout << std::endl;

        if (world_rank == 0)
        {
            // Reset result
            std::fill(result.begin(), result.end(), 0);
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxReduceAll test;
    test.run(argc, argv);
    return 0;
}