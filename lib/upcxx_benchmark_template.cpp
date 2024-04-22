#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_template.hpp>

void UpcxxBenchmarkTemplate::init(int argc, char *argv[])
{
    BenchmarkTemplate::init(argc, argv);

    // UPC++ initialization
    upcxx::init();
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();
}

void UpcxxBenchmarkTemplate::finalize()
{
    upcxx::finalize();
}