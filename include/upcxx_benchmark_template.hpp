#pragma once

#include <benchmark_template.hpp>

class UpcxxBenchmarkTemplate : public BenchmarkTemplate
{
public:
    UpcxxBenchmarkTemplate() = default;
    virtual ~UpcxxBenchmarkTemplate() = default;

    virtual void init(int argc, char *argv[]) override;
    void finalize() override;
};