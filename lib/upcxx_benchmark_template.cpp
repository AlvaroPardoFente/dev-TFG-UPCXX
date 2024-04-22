#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_template.hpp>

void UpcxxBenchmarkTemplate::init(int argc, char *argv[])
{
    // UPC++ initialization
    upcxx::init();
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    BenchmarkTemplate::init(argc, argv);
}

void UpcxxBenchmarkTemplate::finalize()
{
    upcxx::finalize();
}