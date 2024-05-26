#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

uint64_t value = 0;

class UpcxxNoAtomicInc : public UpcxxBenchmarkScheme
{
public:
    upcxx::global_ptr<uint64_t> root_ptr;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        value = 0;
    };

    void benchmark_body() override
    {
        if (world_rank)
        {
            for (size_t i = 0; i < number_count; i++)
            {
                upcxx::rpc_ff(
                    0, []()
                    { value++; });
            }
        }
        else
        {
            while (value < number_count * (world_size - 1))
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        upcxx::barrier();
        if (world_rank == 0)
            UPCXX_ASSERT(value == number_count * (world_size - 1));
        // Reset result
        value = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxNoAtomicInc test;
    test.run(argc, argv);
    return 0;
}