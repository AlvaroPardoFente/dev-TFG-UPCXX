#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

uint32_t count = 0;

class UpcxxRpcFfNotice : public UpcxxBenchmarkScheme
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
                upcxx::rpc_ff(1, []()
                              { count++; });
            }
        }
        else
        {
            while (count < number_count)
            {
                upcxx::progress();
                // std::cout << "Count: " << count << std::endl;
            }
        }
    }

    void reset_result() override
    {
        count = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcFfNotice test;
    test.run(argc, argv);
    return 0;
}