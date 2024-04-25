#pragma once

#include <benchmark_scheme.hpp>

class UpcxxBenchmarkScheme : public BenchmarkScheme
{
public:
    UpcxxBenchmarkScheme() = default;
    virtual ~UpcxxBenchmarkScheme() = default;

    virtual void init(int argc, char *argv[]) override;
    void barrier() override;
    void finalize() override;
};