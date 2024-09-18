#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

/**
 * @brief Atomic increment benchmark implemented in MPI with MPI_Win_fence and MPI_Accumulate
 *
 */
class MpiAtomicInc : public MpiBenchmarkScheme
{
public:
    uint64_t *value;
    uint64_t src_value;

    MPI_Win win;

    ~MpiAtomicInc()
    {
        MPI_Win_free(&win);
    }

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        src_value = 1;

        int win_size = world_rank ? 0 : sizeof(value);
        MPI_Win_allocate(win_size, sizeof(value), MPI_INFO_NULL, MPI_COMM_WORLD, &value, &win);
    };

    void benchmark_body() override
    {
        MPI_Win_fence(0, win);
        for (size_t i = 0; i < number_count; i++)
        {
            MPI_Accumulate(&src_value, 1, MPI_UINT64_T, 0, 0, 1, MPI_UINT64_T, MPI_SUM, win);
        }
        MPI_Win_fence(0, win);

        if (world_rank == 0)
        {
            while (*value < number_count * world_size)
            {
            }
        }
    }

    void reset_result() override
    {
        // Print result
        // if (world_rank == 0)
        // {
        //     std::cout << "Result: " << *value << std::endl;
        // }

        MPI_Barrier(MPI_COMM_WORLD);
        if (world_rank == 0)
            *value = 0;
    }
};

int main(int argc, char *argv[])
{
    MpiAtomicInc test;
    test.run(argc, argv);
    return 0;
}