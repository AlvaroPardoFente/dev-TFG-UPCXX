#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxReduceAll : public UpcxxBenchmarkScheme
{
public:
    std::vector<uint32_t> value;
    std::vector<uint32_t> result;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        // Vector initialization
        value.resize(number_count / world_size);
        for (uint32_t i = 0; i < number_count / world_size; i++)
        {
            value.at(i) = i * world_size + world_rank;
        }

        // Result vector
        result.resize(number_count / world_size);
    };

    void benchmark_body() override
    {
        upcxx::reduce_all(value.data(), result.data(), number_count / world_size, upcxx::op_fast_add).wait();
    }

    void reset_result() override
    {
        if (world_rank == 0)
        {
            // if (world_rank == 0)
            // {
            //     std::cout << "Result: ";
            //     for (uint32_t i = 0; i < number_count / world_size; i++)
            //     {
            //         std::cout << result.at(i) << " ";
            //     }
            //     std::cout << std::endl;
            // }

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