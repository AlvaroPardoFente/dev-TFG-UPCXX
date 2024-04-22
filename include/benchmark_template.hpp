#pragma once

#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>

constexpr uint32_t warmup_repetitions = 10;

class BenchmarkTemplate
{
public:
    uint32_t number_count = 1024;
    uint32_t reps = 1;

    // MPI and UPCXX world size and rank
    int world_size, world_rank;

    // Settings based on args
    settings::benchmark_settings settings;

    // Timer wrapper with time measurements
    benchmark_timer timer;

    BenchmarkTemplate() = default;
    virtual ~BenchmarkTemplate() = default;

    // Initialize all data needed
    virtual void init(int argc, char *argv[]);

    // Code to measure
    virtual void benchmark_body() = 0;

    // Reset result for next iteration
    virtual void reset_result() = 0;

    // Warmup wrapper for benchmark_body
    virtual void warmup();

    // Wrapper for benchmark body with measurements
    virtual void run_benchmark();

    virtual void print_results();

    virtual void finalize() = 0;

    // Run the benchmark
    void run(int argc, char *argv[]);
};