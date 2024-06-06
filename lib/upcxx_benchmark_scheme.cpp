#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>

UpcxxBenchmarkScheme::~UpcxxBenchmarkScheme()
{
    upcxx::finalize();
}

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

    switch (settings->measurement_mode)
    {
    case BenchmarkSettings::NodeMeasurementMode::max:
        for (auto &time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_max, 0).wait();
        }
        break;
    case BenchmarkSettings::NodeMeasurementMode::min:
        for (auto &time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            upcxx::reduce_one(time_point.data(), time_point.data(), time_point.size(), upcxx::op_fast_min, 0).wait();
        }
        break;
    case BenchmarkSettings::NodeMeasurementMode::avg:
        for (auto &time_point_pair : timer.m_times)
        {
            std::vector<double> &time_point = time_point_pair.second;
            std::vector<double> time_point_aux(time_point.size());
            upcxx::reduce_one(time_point.data(), time_point_aux.data(), time_point.size(), upcxx::op_fast_add, 0).wait();

            if (world_rank == 0)
            {
                time_point = time_point_aux;

                if (settings->measurement_mode == BenchmarkSettings::NodeMeasurementMode::avg)
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
}

void UpcxxBenchmarkScheme::barrier()
{
    upcxx::barrier();
}

void UpcxxBenchmarkScheme::finalize()
{
    upcxx::finalize();
}