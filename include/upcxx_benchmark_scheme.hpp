#pragma once

#include <benchmark_scheme.hpp>

/**
 * @brief UPC++ specialization of BenchmarkScheme
 *
 */
class UpcxxBenchmarkScheme : public BenchmarkScheme
{
public:
    UpcxxBenchmarkScheme(uint32_t processes = 0) : BenchmarkScheme(processes) {};
    UpcxxBenchmarkScheme(BenchmarkSettings *settings, uint32_t processses = 0) : BenchmarkScheme(settings, processses) {};
    virtual ~UpcxxBenchmarkScheme();

    virtual void init(int argc, char *argv[]) override;
    virtual void join_results() override;
    void barrier() override;
    void finalize() override;
};