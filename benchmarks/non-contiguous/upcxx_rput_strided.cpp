#include <upcxx/upcxx.hpp>
#include <upcxx_non_contiguous_scheme.hpp>
#include <non_contiguous_settings.hpp>
#include <iostream>

// Count for remote completions
int count = 0;

constexpr size_t dim = 2;

class UpcxxRputStrided : public UpcxxNonContiguousScheme
{
public:
    std::vector<uint32_t> src_vector;
    upcxx::global_ptr<uint32_t> dst_vector_g;
    uint32_t dst_size;

    // Variables for non-contiguous operations
    uint32_t *src_base;
    upcxx::global_ptr<uint32_t> dst_base;
    ptrdiff_t *src_strides;
    ptrdiff_t *dst_strides;
    size_t *extents;

    upcxx::global_ptr<uint32_t> root_ptr;

    void init(int argc, char *argv[]) override
    {
        UpcxxNonContiguousScheme::init(argc, argv);

        src_vector.resize(nchunks_per_rank * in_stride);
        std::iota(src_vector.begin(), src_vector.end(), 0);

        if (world_rank == 0)
        {
            dst_size = std::max(world_size * out_inter_rank_stride, nchunks_per_rank * out_inter_chunk_stride);
            dst_vector_g = upcxx::new_array<uint32_t>(dst_size);
            std::fill(dst_vector_g.local(), dst_vector_g.local() + dst_size, 0);
        }

        // Broadcast root ptr to all processes
        root_ptr = upcxx::broadcast(dst_vector_g, 0).wait();

        // Initialize variables for non-contiguous operations
        const ptrdiff_t elem_sz = (ptrdiff_t)sizeof(uint32_t);
        src_base = src_vector.data();
        dst_base = root_ptr + world_rank * out_inter_rank_stride;
        src_strides = new ptrdiff_t[2]{elem_sz, elem_sz * (ptrdiff_t)in_stride};
        dst_strides = new ptrdiff_t[2]{elem_sz, elem_sz * (ptrdiff_t)out_inter_chunk_stride};
        extents = new size_t[2]{chunk_size, nchunks_per_rank};
    };

    void benchmark_body() override
    {
        upcxx::rput_strided<dim>(src_base,
                                 src_strides,
                                 dst_base,
                                 dst_strides,
                                 extents,
                                 upcxx::remote_cx::as_rpc([]()
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
    UpcxxRputStrided test;
    test.run(argc, argv);
    return 0;
}