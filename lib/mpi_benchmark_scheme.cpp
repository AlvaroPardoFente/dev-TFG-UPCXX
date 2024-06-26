#include <mpi.h>
#include <mpi_benchmark_scheme.hpp>

MpiBenchmarkScheme::~MpiBenchmarkScheme()
{
    MPI_Finalize();
}

void MpiBenchmarkScheme::init(int argc, char *argv[])
{
    // MPI initialization
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    BenchmarkScheme::init(argc, argv);
}

void MpiBenchmarkScheme::join_results()
{
    barrier();

    MPI_Op op;

    if (settings->measurement_mode == BenchmarkSettings::NodeMeasurementMode::max)
    {
        op = MPI_MAX;
    }
    else if (settings->measurement_mode == BenchmarkSettings::NodeMeasurementMode::min)
    {
        op = MPI_MIN;
    }
    else if (settings->measurement_mode == BenchmarkSettings::NodeMeasurementMode::avg)
    {
        op = MPI_SUM;
    }
    else
    {
        throw std::runtime_error("Invalid measurement mode");
    }

    for (auto &time_point_pair : timer.m_times)
    {
        std::vector<double> &time_point = time_point_pair.second;

        if (world_rank == 0)
        {
            MPI_Reduce(MPI_IN_PLACE, time_point.data(), time_point.size(), MPI_DOUBLE, op, 0, MPI_COMM_WORLD);

            if (settings->measurement_mode == BenchmarkSettings::NodeMeasurementMode::avg)
            {
                // Compute the average
                for (double &value : time_point)
                {
                    value /= world_size;
                }
            }
        }
        else
        {
            MPI_Reduce(time_point.data(), NULL, time_point.size(), MPI_DOUBLE, op, 0, MPI_COMM_WORLD);
        }
    }
}

void MpiBenchmarkScheme::barrier()
{
    MPI_Barrier(MPI_COMM_WORLD);
}

void MpiBenchmarkScheme::finalize()
{
    MPI_Finalize();
}