#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

/**
 * @brief Atomic increase benchmark implemented in UPC++ through atomic_domain
 *
 */
class UpcxxAtomicInc : public UpcxxBenchmarkScheme
{
public:
    upcxx::global_ptr<uint64_t> value;
    upcxx::atomic_domain<uint64_t> inc64;

    upcxx::global_ptr<uint64_t> root_ptr;

    ~UpcxxAtomicInc()
    {
        inc64.destroy();
    }

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        value = upcxx::global_ptr<uint64_t>(upcxx::new_<uint64_t>(0));

        root_ptr = upcxx::broadcast(value, 0).wait();

        inc64 = upcxx::atomic_domain<uint64_t>({upcxx::atomic_op::inc, upcxx::atomic_op::load});
    };

    void benchmark_body() override
    {
        upcxx::promise<> p;
        for (size_t i = 0; i < number_count; i++)
        {
            inc64.inc(root_ptr, std::memory_order_relaxed, upcxx::operation_cx::as_promise(p));
        }
        p.finalize().wait();

        if (world_rank == 0)
        {
            while (inc64.load(root_ptr, std::memory_order_relaxed).wait() < number_count * world_size)
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        upcxx::barrier();
        if (world_rank == 0)
            UPCXX_ASSERT(inc64.load(root_ptr, std::memory_order_relaxed).wait() == number_count * world_size);
        // Reset result
        *(value.local()) = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxAtomicInc test;
    test.run(argc, argv);
    return 0;
}