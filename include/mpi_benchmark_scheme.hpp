#pragma once

#include <benchmark_scheme.hpp>

class MpiBenchmarkScheme : public BenchmarkScheme
{
public:
    MpiBenchmarkScheme(uint32_t processes = 0) : BenchmarkScheme(processes){};
    MpiBenchmarkScheme(BenchmarkSettings *settings, uint32_t processes = 0) : BenchmarkScheme(settings, processes){};
    virtual ~MpiBenchmarkScheme() = default;

    virtual void init(int argc, char *argv[]) override;
    virtual void join_results() override;
    void barrier() override;
    void finalize() override;
};