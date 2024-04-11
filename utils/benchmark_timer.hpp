#pragma once

#include <chrono>
#include <iostream>
#include <vector>
#include <numeric>

class benchmark_timer
{
private:
    // Start and end timepoints
    std::chrono::high_resolution_clock::time_point m_start, m_end;

    std::vector<double> m_times;

public:
    benchmark_timer() {}
    benchmark_timer(const int32_t p_size)
    {
        m_times.reserve(p_size);
    }

    void reserve(const int32_t p_size)
    {
        m_times.reserve(p_size);
    }

    // Start the timer
    void start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    // Stop the timer
    void stop()
    {
        m_end = std::chrono::high_resolution_clock::now();
    }

    void add_time()
    {
        m_times.push_back(std::chrono::duration<double>(m_end - m_start).count());
    }

    void print_times()
    {
        std::cout << "Average time: " << this->get_average_time() << std::endl;
        std::cout << "Min time: " << this->get_min_time() << std::endl;
        std::cout << "Times: ";
        std::cout << m_times[0];
        for (auto time : m_times)
        {
            std::cout << ", " << time;
        }
        std::cout << std::endl;
    }

    double get_average_time()
    {
        return std::accumulate(m_times.begin(), m_times.end(), 0.0) / m_times.size();
    }

    double get_min_time()
    {
        return *std::min_element(m_times.begin(), m_times.end());
    }

    // Get the time in seconds
    double get_time()
    {
        return std::chrono::duration<double>(m_end - m_start).count();
    }

    // Get the time in miliseconds
    double get_time_ms()
    {
        return std::chrono::duration<double, std::milli>(m_end - m_start).count();
    }

    // Get the time in microseconds
    double get_time_us()
    {
        return std::chrono::duration<double, std::micro>(m_end - m_start).count();
    }

    // Get the time in nanoseconds
    double get_time_ns()
    {
        return std::chrono::duration<double, std::nano>(m_end - m_start).count();
    }
};