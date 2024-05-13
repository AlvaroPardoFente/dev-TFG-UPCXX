#include <upcxx/upcxx.hpp>
#include <upcxx_non_contiguous_scheme.hpp>
#include <non_contiguous_settings.hpp>
#include <iostream>

// Count for remote completions
int count = 0;

class UpcxxRputRegular : public UpcxxNonContiguousScheme
{
public:
    std::vector<size_t> src_vector;
    upcxx::global_ptr<size_t> dst_vector_g;
    size_t dst_size;

    // Iterables for non-contiguous operations
    std::vector<size_t *> srcs;
    std::vector<upcxx::global_ptr<size_t>> dsts;

    upcxx::global_ptr<size_t> root_ptr;

    void init(int argc, char *argv[]) override
    {
        UpcxxNonContiguousScheme::init(argc, argv);

        src_vector.resize(nchunks_per_rank * in_stride);
        std::iota(src_vector.begin(), src_vector.end(), 0);

        if (world_rank == 0)
        {
            dst_size = std::max(world_size * out_inter_rank_stride, nchunks_per_rank * out_inter_chunk_stride);
            dst_vector_g = upcxx::new_array<size_t>(dst_size);
            std::fill(dst_vector_g.local(), dst_vector_g.local() + dst_size, 0);
        }

        // Broadcast root ptr to all processes
        root_ptr = upcxx::broadcast(dst_vector_g, 0).wait();

        // std::cout << "* Rank " << world_rank << " initializes:\n";

        for (size_t i = 0; i < nchunks_per_rank; i++)
        {
            srcs.push_back(&(src_vector[i * in_stride]));
            dsts.push_back(root_ptr + world_rank * out_inter_rank_stride + (i * out_inter_chunk_stride));
            // std::cout << "  srcs[" << i << "] = " << i * in_stride << " : " << i * in_stride + chunk_size - 1 << " | ";
            // std::cout << "dsts[" << i << "] = " << world_rank * out_inter_rank_stride + (i * out_inter_chunk_stride) << " : " << (world_rank * out_inter_rank_stride + (i * out_inter_chunk_stride) + chunk_size - 1) << std::endl;
        }
    };

    void benchmark_body() override
    {
        upcxx::rput_regular(srcs.begin(), srcs.end(), chunk_size, dsts.begin(), dsts.end(), chunk_size, upcxx::remote_cx::as_rpc([]()
                                                                                                                                 { count++; }));

        if (world_rank == 0)
        {
            while (count < world_size)
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        if (world_rank == 0)
        {
            // if (world_rank == 0)
            // {
            //     for (int i = 0; i < dst_size; i++)
            //     {
            //         std::cout << dst_vector_g.local()[i] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            // Reset result
            count = 0;
            if (world_rank == 0)
            {
                std::fill(dst_vector_g.local(), dst_vector_g.local() + dst_size, 0);
            }
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxRputRegular test;
    test.run(argc, argv);
    return 0;
}