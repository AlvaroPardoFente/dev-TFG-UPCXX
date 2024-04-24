#pragma once

#include <benchmark_scheme.hpp>

class MpiBenchmarkScheme : public BenchmarkScheme
{
public:
    MpiBenchmarkScheme() = default;
    virtual ~MpiBenchmarkScheme() = default;

    virtual void init(int argc, char *argv[]) override;
    void barrier() override;
    void finalize() override;
};