#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>

void UpcxxBenchmarkScheme::init(int argc, char *argv[])
{
    // UPC++ initialization
    upcxx::init();
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    BenchmarkScheme::init(argc, argv);
}

void UpcxxBenchmarkScheme::join_results()
{
    barrier();

    switch (settings->measurement_node)
    {
    case BenchmarkSettings::NodeMeasurementMode::max:
        for (auto time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_max, 0).wait();
        }
        break;
    case BenchmarkSettings::NodeMeasurementMode::min:
        for (auto time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_min, 0).wait();
        }
        break;
    case BenchmarkSettings::NodeMeasurementMode::avg:
        for (auto time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_add, 0).wait();

            if (world_rank == 0)
            {
                // Compute the average
                for (double &value : time_point)
                {
                    value /= world_size;
                }
            }
        }
        break;
    default:
        throw std::runtime_error("Invalid measurement mode");
        break;
    }

    for (auto time_point_pair : timer.m_times)
    {
        std::vector<double> &time_point = time_point_pair.second;
        upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_max, 0).wait();
    }
}

void UpcxxBenchmarkScheme::barrier()
{
    upcxx::barrier();
}

void UpcxxBenchmarkScheme::finalize()
{
    upcxx::finalize();
}