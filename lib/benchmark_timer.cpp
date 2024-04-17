#include <benchmark_timer.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>

benchmark_timer::benchmark_timer(settings::benchmark_settings *p_settings)
{
    m_settings = p_settings;
}
benchmark_timer::benchmark_timer(const int32_t p_size, settings::benchmark_settings *p_settings)
{
    m_times.reserve(p_size);
    m_settings = p_settings;
}

void benchmark_timer::set_settings(settings::benchmark_settings *p_settings)
{
    m_settings = p_settings;
}

void benchmark_timer::reserve(const int32_t p_size)
{
    m_times.reserve(p_size);
}

// Start the timer
void benchmark_timer::start()
{
    m_start = std::chrono::high_resolution_clock::now();
}

// Stop the timer
void benchmark_timer::stop()
{
    m_end = std::chrono::high_resolution_clock::now();
}

void benchmark_timer::add_time()
{
    m_times.push_back(std::chrono::duration<double>(m_end - m_start).count());
}

void benchmark_timer::print_times()
{
    if (m_settings->o_mode == settings::output_mode::none)
    {
        if (m_settings->raw_value)
            std::cout << "Count: " << m_settings->raw_value.value() << std::endl;
        std::cout << "Repetitions: " << m_times.size() << std::endl;
        std::cout << "Average time: " << this->get_average_time() << std::endl;
        std::cout << "Min time: " << this->get_min_time() << std::endl;
        std::cout << "Times:" << std::endl;
        if (!m_times.empty())
        {
            std::cout << m_times[0];
            for (auto it = m_times.begin() + 1; it != m_times.end(); ++it)
            {
                std::cout << ", " << *it;
            }
            std::cout << std::endl;
        }
    }
    else if (m_settings->o_mode == settings::output_mode::quiet)
    {
        if (m_settings->raw_value)
            std::cout << m_settings->raw_value.value() << ", ";
        if (!m_times.empty())
        {
            std::cout << m_times[0];
            for (auto it = m_times.begin() + 1; it != m_times.end(); ++it)
            {
                std::cout << ", " << *it;
            }
            std::cout << std::endl;
        }
    }
}

double benchmark_timer::get_average_time()
{
    return std::accumulate(m_times.begin(), m_times.end(), 0.0) / m_times.size();
}

double benchmark_timer::get_min_time()
{
    return *std::min_element(m_times.begin(), m_times.end());
}

// Get the time in seconds
double benchmark_timer::get_time()
{
    return std::chrono::duration<double>(m_end - m_start).count();
}

// Get the time in miliseconds
double benchmark_timer::get_time_ms()
{
    return std::chrono::duration<double, std::milli>(m_end - m_start).count();
}

// Get the time in microseconds
double benchmark_timer::get_time_us()
{
    return std::chrono::duration<double, std::micro>(m_end - m_start).count();
}

// Get the time in nanoseconds
double benchmark_timer::get_time_ns()
{
    return std::chrono::duration<double, std::nano>(m_end - m_start).count();
}