#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxBroadcastArray : public UpcxxBenchmarkScheme
{
public:
    std::vector<uint32_t> value;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        // Vector initialization
        value.resize(number_count);
        for (uint32_t i = 0; i < number_count; i++)
        {
            value.at(i) = i;
        }
    };

    void benchmark_body() override
    {
        upcxx::broadcast(value.data(), number_count, 0).wait();
    }

    void reset_result() override
    {
        // Check result in debug mode
        for (size_t i = 0; i < value.size(); i++)
            UPCXX_ASSERT(value[i] == (int)i);

        // Reset result
        if (world_rank != 0)
            std::fill(value.begin(), value.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    UpcxxBroadcastArray test;
    test.run(argc, argv);
    return 0;
}