#pragma once

#include <benchmark_scheme.hpp>

class UpcxxBenchmarkScheme : public BenchmarkScheme
{
public:
    UpcxxBenchmarkScheme() = default;
    UpcxxBenchmarkScheme(BenchmarkSettings *settings) : BenchmarkScheme(settings){};
    virtual ~UpcxxBenchmarkScheme() = default;

    virtual void init(int argc, char *argv[]) override;
    void barrier() override;
    void finalize() override;
};