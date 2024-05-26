#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxAtomicDomainInc : public UpcxxBenchmarkScheme
{
public:
    upcxx::atomic_domain<uint64_t> inc64;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);
    };

    void benchmark_body() override
    {
        inc64 = upcxx::atomic_domain<uint64_t>({upcxx::atomic_op::inc, upcxx::atomic_op::load});
    }

    void reset_result() override
    {
        upcxx::barrier();
        inc64.destroy();
    }
};

int main(int argc, char *argv[])
{
    UpcxxAtomicDomainInc test;
    test.run(argc, argv);
    return 0;
}