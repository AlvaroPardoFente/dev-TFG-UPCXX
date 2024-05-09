#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxRpcPromise : public UpcxxBenchmarkScheme
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
        if (world_rank == 0)
        {
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rpc(1, upcxx::operation_cx::as_promise(p), []() {});
            }

            p.finalize().wait();
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
    UpcxxRpcPromise test;
    test.run(argc, argv);
    return 0;
}