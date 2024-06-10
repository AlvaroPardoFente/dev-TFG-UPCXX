#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

class MpiPut : public MpiBenchmarkScheme
{
public:
    uint32_t *data_block;
    uint32_t put_value = 1;

    MPI_Win win;

    ~MpiPut()
    {
        MPI_Win_free(&win);
    }

    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);

        if (world_rank == 0)
        {
            data_block = (uint32_t *)malloc(sizeof(uint32_t) * number_count);
        }

        int win_size = world_rank ? 0 : sizeof(uint32_t) * number_count;
        MPI_Win_allocate(win_size, sizeof(uint32_t), MPI_INFO_NULL, MPI_COMM_WORLD, &data_block, &win);
    };

    void benchmark_body() override
    {
        if (world_rank)
        {

            MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
            for (size_t i = 0; i < number_count; i++)
            {
                MPI_Put(&put_value, 1, MPI_UINT32_T, 0, i, 1, MPI_UINT32_T, win);
            }
            MPI_Win_flush(0, win);
            MPI_Win_unlock(0, win);
        }
    }

    void reset_result() override
    {

        MPI_Barrier(MPI_COMM_WORLD);

        // Print result
        // if (world_rank == 0)
        // {
        //     for (size_t i = 0; i < number_count; i++)
        //     {
        //         std::cout << data_block[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        if (world_rank == 0)
            std::fill(data_block, data_block + number_count, 0);
    }
};

int main(int argc, char *argv[])
{
    MpiPut test;
    test.run(argc, argv);
    return 0;
}