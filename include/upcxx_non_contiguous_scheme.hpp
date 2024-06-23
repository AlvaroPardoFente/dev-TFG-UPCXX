#pragma once

#include <upcxx_benchmark_scheme.hpp>
#include <non_contiguous_settings.hpp>

class UpcxxNonContiguousScheme : public UpcxxBenchmarkScheme
{
public:
    NonContiguousSettings *non_contiguous_settings;

    size_t nchunks_per_rank, chunk_size, in_stride, out_inter_chunk_stride, out_inter_rank_stride;

    UpcxxNonContiguousScheme() : UpcxxBenchmarkScheme(non_contiguous_settings = new NonContiguousSettings()) {}
    virtual void init(int argc, char *argv[]) override;
};