#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

uint32_t count = 0;

class UpcxxRpcNotice : public UpcxxBenchmarkScheme
{
public:
    UpcxxRpcNotice() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rpc(1, upcxx::operation_cx::as_promise(p), []()
                           { count++; });
            }

            p.finalize().wait();
        }
        else
        {
            while (count < number_count)
            {
                upcxx::progress();
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
    UpcxxRpcNotice test;
    test.run(argc, argv);
    return 0;
}