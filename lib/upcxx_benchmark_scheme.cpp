#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>

void UpcxxBenchmarkScheme::init(int argc, char *argv[])
{
    // UPC++ initialization
    upcxx::init();
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    BenchmarkScheme::init(argc, argv);
}

void UpcxxBenchmarkScheme::join_results()
{
    barrier();

    for (auto time_point_pair : timer.m_times)
    {
        std::vector<double> time_point = time_point_pair.second;
        upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_max, 0).wait();
    }
}

void UpcxxBenchmarkScheme::barrier()
{
    upcxx::barrier();
}

void UpcxxBenchmarkScheme::finalize()
{
    upcxx::finalize();
}