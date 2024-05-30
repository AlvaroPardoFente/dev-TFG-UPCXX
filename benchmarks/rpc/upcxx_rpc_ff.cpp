#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxRpcFf : public UpcxxBenchmarkScheme
{
public:
    UpcxxRpcFf() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rpc_ff(1, []() {});
            }
        }
        else
        {
            for (int i = 0; i < number_count; i++)
            {
                if (i % 10 == 0)
                    upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcFf test;
    test.run(argc, argv);
    return 0;
}