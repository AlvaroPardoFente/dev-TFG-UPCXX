/**
 * @file benchmark_scheme.hpp
 * @author Álvaro Pardo Fente (alvaro.pardo.fente@udc.es)
 * @brief
 * @version 1.0
 * @date 2024-09-08
 *
 *
 */
#pragma once

#include <benchmark_settings.hpp>
#include <benchmark_timer.hpp>

/**
 * @brief Base abstract class for all benchmarks
 *
 * This class provides a common interface for all benchmarks. It includes the main
 * functions that need to be implemented by derived classes.
 *
 */
class BenchmarkScheme
{
public:
    uint32_t number_count = 1024; /// Generic number of elements or iterations
    uint32_t reps = 1;
    uint32_t warmup_repetitions = 10;
    u_int32_t processes_required;

    int world_size, world_rank;

    /// Settings based on args
    BenchmarkSettings *settings;

    /// Timer wrapper with time measurements
    BenchmarkTimer timer;

    /// Additional printing columns (constants)
    std::unordered_map<std::string, std::string> print_columns;

    /**
     * @brief Construct a new Benchmark Scheme object
     *
     * @param processes Number of processes required for the benchmark
     */
    BenchmarkScheme(uint32_t processes = 0) : settings(new BenchmarkSettings()), processes_required(processes) {};

    /**
     * @brief Construct a new Benchmark Scheme object
     *
     * @param settings Specific settings for the benchmark
     * @param processes Number of processes required for the benchmark
     */
    BenchmarkScheme(BenchmarkSettings *settings, uint32_t processes = 0) : settings(settings), processes_required(processes) {}

    /**
     * @brief Destroy the Benchmark Scheme object
     *
     */
    virtual ~BenchmarkScheme();

    /**
     * @brief Initialize the values needed for the benchmark
     *
     * @param argc
     * @param argv
     */
    virtual void init(int argc, char *argv[]);

    /**
     * @brief Code to run and measure.
     *
     * This pure virtual function should contain the code that needs to be
     * benchmarked. It must be implemented by derived classes.
     *
     * @see run_benchmark()
     */
    virtual void benchmark_body() = 0;

    /**
     * @brief Reset benchmark results for the next repetition.
     *
     * This pure virtual function should reset any results or state
     * information to prepare for the next repetition of the benchmark.
     * It must be implemented by derived classes.
     *
     * @see run_benchmark()
     */
    virtual void reset_result() = 0;

    /**
     * @brief Implementation-dependent barrier.
     *
     * This pure virtual function should implement a barrier synchronization
     * mechanism. It must be implemented by derived classes.
     */
    virtual void barrier() = 0;

    /**
     * @brief Wrapper for benchmark body with measurements.
     *
     * This function runs the @ref benchmark_body function, measures its time,
     * and calls @ref reset_result.
     *
     * @param use_barrier If true, a barrier is used before and after the benchmark body.
     *
     * @see benchmark_body()
     * @see reset_result()
     */
    virtual void run_benchmark(bool use_barrier = true);

    /**
     * @brief Join results from all processes.
     *
     * This pure virtual function should combine the results from all
     * processes. It must be implemented by derived classes.
     */
    virtual void join_results() = 0;

    /**
     * @brief Print benchmark results.
     *
     * This function prints the results of the benchmark to the standard output.
     */
    virtual void print_results();

    /**
     * @brief Finalize the benchmark.
     *
     * This pure virtual function should perform any necessary cleanup
     * and finalization steps. It must be implemented by derived classes.
     */
    virtual void finalize() = 0;

    /**
     * @brief Run the benchmark.
     *
     * This function initializes the benchmark, runs the @ref benchmark_body,
     * and finishes by calling @ref finalize.
     *
     * @param argc Argument count.
     * @param argv Argument vector.
     *
     * @see benchmark_body()
     * @see finalize()
     */
    void run(int argc, char *argv[]);
};