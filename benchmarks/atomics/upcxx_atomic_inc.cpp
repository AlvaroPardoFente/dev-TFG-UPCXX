#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxAtomicInc : public UpcxxBenchmarkScheme
{
public:
    upcxx::global_ptr<uint64_t> value;
    upcxx::atomic_domain<uint64_t> inc64;

    upcxx::global_ptr<uint64_t> root_ptr;

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        value = upcxx::global_ptr<uint64_t>(upcxx::new_<uint64_t>(0));

        root_ptr = upcxx::broadcast(value, 0).wait();
    };

    void benchmark_body() override
    {
        inc64 = upcxx::atomic_domain<uint64_t>({upcxx::atomic_op::inc, upcxx::atomic_op::load});

        if (world_rank)
        {
            upcxx::promise<> p;
            for (size_t i = 0; i < number_count; i++)
            {
                inc64.inc(root_ptr, std::memory_order_relaxed, upcxx::operation_cx::as_promise(p));
            }
            p.finalize().wait();
        }
        else
        {
            while (inc64.load(root_ptr, std::memory_order_relaxed).wait() < number_count * (world_size - 1))
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        upcxx::barrier();
        UPCXX_ASSERT(inc64.load(root_ptr, std::memory_order_relaxed).wait() == number_count * (world_size - 1));
        // Reset result
        inc64.destroy();
        *(value.local()) = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxAtomicInc test;
    test.run(argc, argv);
    return 0;
}