#pragma once

#include <benchmark_settings.hpp>

#include <chrono>
#include <vector>

constexpr const char TIMER_END[] = "End";

class BenchmarkTimer
{
private:
    // Start and end timepoints
    std::chrono::high_resolution_clock::time_point m_start, m_end;

    BenchmarkSettings *m_settings;

public:
    std::unordered_map<std::string, std::vector<double>> m_times;

    BenchmarkTimer(BenchmarkSettings *p_settings = nullptr);
    BenchmarkTimer(const int32_t p_size, BenchmarkSettings *p_settings = nullptr);

    void set_settings(BenchmarkSettings *p_settings);

    void add_time_point(const std::string &p_name);

    void reserve(const int32_t p_size);

    // Start the timer
    void start();

    // Stop the timer
    void stop();

    void add_time(std::string p_time_point = TIMER_END);

    void print_times(int world_rank = 0, std::unordered_map<std::string, std::string> p_columns = {});

    void reset_times();

    double get_average_time(std::string p_time_point = TIMER_END);

    double get_min_time(std::string p_time_point = TIMER_END);

    // Get the time in seconds
    double get_time();

    // Get the time in miliseconds
    double get_time_ms();

    // Get the time in microseconds
    double get_time_us();

    // Get the time in nanoseconds
    double get_time_ns();
};