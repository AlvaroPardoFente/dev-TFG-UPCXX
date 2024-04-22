#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_template.hpp>
#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>
#include <iostream>
#include <chrono>
#include <numeric>

class upcxx_broadcast_array : public UpcxxBenchmarkTemplate
{
public:
    std::vector<uint32_t> value;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkTemplate::init(argc, argv);

        // Vector initialization
        value.resize(number_count);
        for (uint32_t i = 0; i < number_count; i++)
        {
            value.at(i) = i;
        }
    };

    void warmup() override
    {
        upcxx::barrier();
        UpcxxBenchmarkTemplate::warmup();
    }

    void run_benchmark() override
    {
        upcxx::barrier();
        UpcxxBenchmarkTemplate::run_benchmark();
    }

    void benchmark_body() override
    {
        upcxx::broadcast(value.data(), number_count, 0).wait();

        // Check result in debug mode
        for (size_t i = 0; i < value.size(); i++)
            UPCXX_ASSERT(value[i] == (int)i);
    }

    void reset_result() override
    {
        // Reset result
        if (world_rank != 0)
            std::fill(value.begin(), value.end(), 0);
    }
};

int main(int argc, char *argv[])
{
    upcxx_broadcast_array test;
    test.run(argc, argv);
    return 0;
}