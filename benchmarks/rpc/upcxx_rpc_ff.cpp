#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxRpc : public UpcxxBenchmarkScheme
{
public:
    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        if (world_size != 2)
        {
            if (world_rank == 0)
            {
                std::cerr << "This benchmark must be run with 2 processes" << std::endl;
            }
            upcxx::finalize();
            std::exit(1);
        }
    };

    void benchmark_body() override
    {
        // Perform rput for every value
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
    UpcxxRpc test;
    test.run(argc, argv);
    return 0;
}