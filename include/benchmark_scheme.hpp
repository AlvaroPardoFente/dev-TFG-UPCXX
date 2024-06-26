#pragma once

#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>

class BenchmarkScheme
{
public:
    uint32_t number_count = 1024;
    uint32_t reps = 1;
    uint32_t warmup_repetitions = 10;
    u_int32_t processes_required;

    // MPI and UPCXX world size and rank
    int world_size, world_rank;

    // Settings based on args
    BenchmarkSettings *settings;

    // Timer wrapper with time measurements
    BenchmarkTimer timer;

    // Additional printing columns (constants)
    std::unordered_map<std::string, std::string> print_columns;

    BenchmarkScheme(uint32_t processes = 0) : settings(new BenchmarkSettings()), processes_required(processes){};
    BenchmarkScheme(BenchmarkSettings *settings, uint32_t processes = 0) : settings(settings), processes_required(processes) {}
    virtual ~BenchmarkScheme();

    // Initialize all data needed
    virtual void init(int argc, char *argv[]);

    // Code to measure
    virtual void benchmark_body() = 0;

    // Reset result for next iteration
    virtual void reset_result() = 0;

    // Implementation-dependent barrier
    virtual void barrier() = 0;

    // Wrapper for benchmark body with measurements
    virtual void run_benchmark(bool use_barrier = true);

    virtual void join_results() = 0;

    virtual void print_results();

    virtual void finalize() = 0;

    // Run the benchmark
    void run(int argc, char *argv[]);
};