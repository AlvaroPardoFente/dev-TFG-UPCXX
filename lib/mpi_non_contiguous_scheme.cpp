#include <mpi_non_contiguous_scheme.hpp>
#include <iostream>

void MpiNonContiguousScheme::init(int argc, char *argv[])
{
    MpiBenchmarkScheme::init(argc, argv);

    // Argument checking
    if (non_contiguous_settings->value ||
        !non_contiguous_settings->chunks_per_rank ||
        !non_contiguous_settings->chunk_size ||
        !non_contiguous_settings->in_stride ||
        !non_contiguous_settings->out_inter_chunk_stride ||
        !non_contiguous_settings->out_inter_rank_stride)
    {
        if (world_rank == 0)
            std::cerr << "Args:" << std::endl
                      << "  --chunks-per-rank" << std::endl
                      << "  --chunk-size" << std::endl
                      << "  --in-stride" << std::endl
                      << "  --out-inter-chunk-stride" << std::endl
                      << "  --out-inter-rank-stride" << std::endl;
        finalize();
        exit(EXIT_FAILURE);
    }

    chunk_size = non_contiguous_settings->chunk_size.value();
    nchunks_per_rank = non_contiguous_settings->chunks_per_rank.value();
    in_stride = non_contiguous_settings->in_stride.value();
    out_inter_rank_stride = non_contiguous_settings->out_inter_rank_stride.value();
    out_inter_chunk_stride = non_contiguous_settings->out_inter_chunk_stride.value();

    if (chunk_size > in_stride)
    {
        if (world_rank == 0)
        {
            std::cerr << "chunk_size > in_stride\n";
            std::cerr << chunk_size << " >" << in_stride << '\n';
        }
        finalize();
        exit(EXIT_FAILURE);
    }
    if (chunk_size > out_inter_rank_stride)
    {
        if (world_rank == 0)
        {
            std::cerr << "chunk_size > out_inter_rank_stride\n";
            std::cerr << chunk_size << " > " << out_inter_rank_stride << '\n';
        }
        finalize();
        exit(EXIT_FAILURE);
    }
    if (chunk_size > out_inter_chunk_stride)
    {
        if (world_rank == 0)
        {
            std::cerr << "chunk_size > out_inter_chunk_stride\n";
            std::cerr << chunk_size << " > " << out_inter_chunk_stride << '\n';
        }
        finalize();
        exit(EXIT_FAILURE);
    }
    if (out_inter_chunk_stride > out_inter_rank_stride)
    {
        // std::cout << "Cyclic storage of the chunks of different processes";
        if ((out_inter_rank_stride * world_size) > out_inter_chunk_stride)
        {
            if (world_rank == 0)
            {
                std::cerr << "(out_inter_rank_stride * world_size) > out_inter_chunk_stride\n";
                std::cerr << '(' << out_inter_rank_stride << " * " << world_size << ") > " << out_inter_chunk_stride << '\n';
            }
            finalize();
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // std::cout << "Consecutive (non-sequential) storage of the chunks of different processes";
        if ((out_inter_chunk_stride * nchunks_per_rank) > out_inter_rank_stride)
        {
            if (world_rank == 0)
            {
                std::cerr << "(out_inter_chunk_stride * nchunks_per_rank) > out_inter_rank_stride\n";
                std::cerr << '(' << out_inter_chunk_stride << " * " << nchunks_per_rank << ") > " << out_inter_rank_stride << '\n';
            }
            finalize();
            exit(EXIT_FAILURE);
        }
    }

    print_columns.clear();
    print_columns["Chunks-per-rank"] = std::to_string(nchunks_per_rank);
    print_columns["Chunk-size"] = std::to_string(chunk_size);
    print_columns["In-stride"] = std::to_string(in_stride);
    print_columns["Out-inter-chunk-stride"] = std::to_string(out_inter_chunk_stride);
    print_columns["Out-inter-rank-stride"] = std::to_string(out_inter_rank_stride);
    print_columns["Size-per-rank"] = std::to_string(nchunks_per_rank * chunk_size);
    print_columns["Total-size"] = std::to_string(world_size * nchunks_per_rank * chunk_size);
}