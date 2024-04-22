#pragma once

#include <benchmark_template.hpp>

class MpiBenchmarkTemplate : public BenchmarkTemplate
{
public:
    MpiBenchmarkTemplate() = default;
    virtual ~MpiBenchmarkTemplate() = default;

    virtual void init(int argc, char *argv[]) override;
    void finalize() override;
};