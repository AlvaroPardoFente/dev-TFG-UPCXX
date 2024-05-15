#pragma once

#include <mpi_benchmark_scheme.hpp>
#include <non_contiguous_settings.hpp>

class MpiNonContiguousScheme : public MpiBenchmarkScheme
{
public:
    NonContiguousSettings *non_contiguous_settings;

    size_t nchunks_per_rank, chunk_size, in_stride, out_inter_chunk_stride, out_inter_rank_stride;

    MpiNonContiguousScheme() : MpiBenchmarkScheme(non_contiguous_settings = new NonContiguousSettings()) {}
    virtual void init(int argc, char *argv[]) override;
};