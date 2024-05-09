#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

uint32_t count = 0;

class UpcxxRpcPromise : public UpcxxBenchmarkScheme
{
    std::vector<upcxx::future<uint32_t>> fut_vector;
    std::vector<uint32_t> result;

public:
    UpcxxRpcPromise() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        if (world_rank == 0)
        {
            fut_vector.reserve(number_count);
            result.reserve(number_count);
        }
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                fut_vector.push_back(upcxx::rpc(1, []()
                                                { return ++count; }));
            }

            // Record time!!!

            for (uint32_t i = 0; i < number_count; i++)
            {
                result.push_back(fut_vector.at(i).wait());
            }
        }
        // else
        // {
        //     for (int i = 0; i < number_count; i++)
        //     {
        //         if (i % 10 == 0)
        //             upcxx::progress();
        //     }
        // }
    }

    void reset_result() override
    {
        if (world_rank == 0)
        {
            // for (auto elem : result)
            // {
            //     std::cout << elem << " ";
            // }
            // std::cout << std::endl;

            result.clear();
            fut_vector.clear();
        }
        count = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcPromise test;
    test.run(argc, argv);
    return 0;
}