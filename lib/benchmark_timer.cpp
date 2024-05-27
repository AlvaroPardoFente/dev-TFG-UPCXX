#include <benchmark_timer.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

BenchmarkTimer::BenchmarkTimer(BenchmarkSettings *p_settings)
{
    m_settings = p_settings;

    m_times[TIMER_END] = std::vector<double>();
}
BenchmarkTimer::BenchmarkTimer(const int32_t p_size, BenchmarkSettings *p_settings)
    : BenchmarkTimer(p_settings)
{
    m_times.at(TIMER_END).reserve(p_size);
}

void BenchmarkTimer::set_settings(BenchmarkSettings *p_settings)
{
    m_settings = p_settings;
}

void BenchmarkTimer::add_time_point(const std::string &p_name)
{
    m_times.insert(std::make_pair(p_name, std::vector<double>(m_times.at(TIMER_END).capacity())));
}

void BenchmarkTimer::reserve(const int32_t p_size)
{
    for (auto &time_point : m_times)
    {
        time_point.second.reserve(p_size);
    }
}

// Start the timer
void BenchmarkTimer::start()
{
    m_start = std::chrono::high_resolution_clock::now();
}

// Stop the timer
void BenchmarkTimer::stop()
{
    m_end = std::chrono::high_resolution_clock::now();
}

void BenchmarkTimer::add_time(std::string p_time_point)
{
    double duration = std::chrono::duration<double>(m_end - m_start).count();

    if (p_time_point.empty())
    {
        m_times.at(TIMER_END).push_back(duration);
    }
    else
    {
        m_times.at(p_time_point).push_back(duration);
    }
}

void BenchmarkTimer::print_times(int world_rank)
{
    if (m_settings->o_mode == BenchmarkSettings::OutputMode::none)
    {
        std::cout << "Rank: " << world_rank << std::endl;

        if (m_settings->raw_value)
            std::cout << "Count: " << m_settings->raw_value.value() << std::endl;

        std::cout << "Repetitions: " << m_settings->repetitions.value_or(1)
                  << std::endl
                  << std::endl;

        std::cout << "Average times: " << std::endl;
        for (auto time_point : m_times)
            std::cout << time_point.first << ": " << this->get_average_time(time_point.first) << std::endl;
        std::cout << std::endl;

        std::cout << "Min times: " << std::endl;
        for (auto time_point : m_times)
        {
            std::cout << time_point.first << ": " << this->get_min_time(time_point.first) << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Times:" << std::endl;
        if (!m_times.empty())
        {
            for (auto time_point : m_times)
            {
                std::cout << time_point.first << ": " << std::endl
                          << time_point.second.at(0);
                for (auto it = time_point.second.begin() + 1; it != time_point.second.end(); ++it)
                {
                    std::cout << ", " << *it;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    else if (m_settings->o_mode == BenchmarkSettings::OutputMode::quiet)
    {
        if (!m_times.empty())
        {
            if (world_rank == 0 && m_settings->show_headers)
            {
                std::cout << "Rank, Size, Timepoint, Index, Time" << std::endl;
            }
            for (auto time_point : m_times)
            {
                for (auto it = time_point.second.begin(); it != time_point.second.end(); ++it)
                {
                    // rank, size, time_point, repetition, time
                    std::cout << world_rank << ", "
                              << m_settings->value.value_or(1024) << ", "
                              << time_point.first << ", "
                              << std::distance(time_point.second.begin(), it) + 1 << ", "
                              << *it
                              << std::endl;
                }
            }
        }
    }
}

void BenchmarkTimer::reset_times()
{
    for (auto &time_point : m_times)
    {
        time_point.second.clear();
    }
}

double BenchmarkTimer::get_average_time(std::string p_time_point)
{
    std::vector<double> time_point = m_times.at(p_time_point);
    return std::accumulate(time_point.begin(), time_point.end(), 0.0) / time_point.size();
}

double BenchmarkTimer::get_min_time(std::string p_time_point)
{
    std::vector<double> time_point = m_times.at(p_time_point);
    return *std::min_element(time_point.begin(), time_point.end());
}

// Get the time in seconds
double BenchmarkTimer::get_time()
{
    return std::chrono::duration<double>(m_end - m_start).count();
}

// Get the time in miliseconds
double BenchmarkTimer::get_time_ms()
{
    return std::chrono::duration<double, std::milli>(m_end - m_start).count();
}

// Get the time in microseconds
double BenchmarkTimer::get_time_us()
{
    return std::chrono::duration<double, std::micro>(m_end - m_start).count();
}

// Get the time in nanoseconds
double BenchmarkTimer::get_time_ns()
{
    return std::chrono::duration<double, std::nano>(m_end - m_start).count();
}