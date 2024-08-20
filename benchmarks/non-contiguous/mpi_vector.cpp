#include <mpi.h>
#include <mpi_non_contiguous_scheme.hpp>
#include <numeric>

class MpiVector : public MpiNonContiguousScheme
{
public:
    std::vector<uint32_t> src_vector;
    std::vector<uint32_t> dst_vector;
    MPI_Datatype src_type, dst_type;

    void init(int argc, char *argv[]) override
    {
        MpiNonContiguousScheme::init(argc, argv);

        src_vector.resize(nchunks_per_rank * in_stride);
        std::iota(src_vector.begin(), src_vector.end(), 0);

        if (world_rank == 0)
        {
            dst_vector.resize(std::max(world_size * out_inter_rank_stride, nchunks_per_rank * out_inter_chunk_stride));
            std::fill(dst_vector.begin(), dst_vector.end(), 0);
        }

        MPI_Type_vector(nchunks_per_rank, chunk_size, in_stride, MPI_UINT32_T, &src_type);
        MPI_Type_commit(&src_type);

        if (world_rank == 0)
        {
            MPI_Type_vector(nchunks_per_rank, chunk_size, out_inter_chunk_stride, MPI_UINT32_T, &dst_type);
            MPI_Type_commit(&dst_type);
        }
    }

    void benchmark_body() override
    {
        MPI_Request request;
        MPI_Isend(src_vector.data(), 1, src_type, 0, 0, MPI_COMM_WORLD, &request);

        if (world_rank == 0)
        {
            for (int i = 0; i < world_size; i++)
            {
                MPI_Recv(&dst_vector[i * out_inter_rank_stride], 1, dst_type, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        MPI_Wait(&request, MPI_STATUS_IGNORE);
    }

    void reset_result() override
    {
        // if (world_rank == 0)
        // {
        //     for (auto elem : dst_vector)
        //     {
        //         std::cout << elem << " ";
        //     }
        //     std::cout << std::endl;
        // }
        if (world_rank == 0)
        {
            std::fill(dst_vector.begin(), dst_vector.end(), 0);
        }
    }
};

int main(int argc, char *argv[])
{
    MpiVector test;
    test.run(argc, argv);
    return 0;
}