#pragma once

#include <benchmark_settings.hpp>

#include <chrono>
#include <vector>

class benchmark_timer
{
private:
    // Start and end timepoints
    std::chrono::high_resolution_clock::time_point m_start, m_end;

    BenchmarkSettings *m_settings;

public:
    std::vector<double> m_times;

    benchmark_timer(BenchmarkSettings *p_settings = nullptr);
    benchmark_timer(const int32_t p_size, BenchmarkSettings *p_settings = nullptr);

    void set_settings(BenchmarkSettings *p_settings);

    void reserve(const int32_t p_size);

    // Start the timer
    void start();

    // Stop the timer
    void stop();

    void add_time();

    void print_times();

    void reset_times();

    double get_average_time();

    double get_min_time();

    // Get the time in seconds
    double get_time();

    // Get the time in miliseconds
    double get_time_ms();

    // Get the time in microseconds
    double get_time_us();

    // Get the time in nanoseconds
    double get_time_ns();
};