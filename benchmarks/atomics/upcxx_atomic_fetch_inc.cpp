#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxAtomicFetchInc : public UpcxxBenchmarkScheme
{
public:
    upcxx::global_ptr<uint64_t> value;
    upcxx::atomic_domain<uint64_t> inc64;

    upcxx::global_ptr<uint64_t> root_ptr;

    std::vector<upcxx::future<uint64_t>> fetch_futures;
    std::vector<uint64_t> fetch_results;

    ~UpcxxAtomicFetchInc()
    {
        inc64.destroy();
    }

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        value = upcxx::global_ptr<uint64_t>(upcxx::new_<uint64_t>(0));

        root_ptr = upcxx::broadcast(value, 0).wait();

        inc64 = upcxx::atomic_domain<uint64_t>({upcxx::atomic_op::fetch_inc, upcxx::atomic_op::load});

        fetch_futures.reserve(number_count);
        fetch_results.reserve(number_count);
    };

    void benchmark_body() override
    {
        for (size_t i = 0; i < number_count; i++)
        {
            fetch_futures.push_back(inc64.fetch_inc(root_ptr, std::memory_order_relaxed));
        }

        for (size_t i = 0; i < number_count; i++)
        {
            fetch_results.push_back(fetch_futures[i].wait());
        }

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

        fetch_futures.clear();
        fetch_results.clear();
        fetch_futures.reserve(number_count);
        fetch_results.reserve(number_count);

        // Reset result
        *(value.local()) = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxAtomicFetchInc test;
    test.run(argc, argv);
    return 0;
}