#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <regex>

// Helper function to convert a string representation of bytes to an integer
static std::optional<int> parseBytes(const std::string &arg)
{
    std::regex regex(R"((\d+)(([KMG]B?)|B))");
    std::smatch match;

    if (std::regex_match(arg, match, regex))
    {
        int value = std::stoi(match[1]);
        std::string unit = match[2];

        if (unit.at(0) == 'K')
            value *= 1024;
        else if (unit.at(0) == 'M')
            value *= 1024 * 1024;
        else if (unit.at(0) == 'G')
            value *= 1024 * 1024 * 1024;
        else if (unit.empty())
            return std::nullopt;

        return value;
    }

    return std::nullopt;
}

class BenchmarkSettings
{
public:
    // Output mode enum
    enum class OutputMode
    {
        none,
        verbose,
        quiet
    };

    // Which nodes will measure time and how
    enum class NodeMeasurementMode
    {
        root,
        all,
        max,
        min,
        avg
    };

private:
    static NodeMeasurementMode parseNodeMeasurementMode(const std::string &arg)
    {
        if (arg == "root")
        {
            return NodeMeasurementMode::root;
        }
        else if (arg == "all")
        {
            return NodeMeasurementMode::all;
        }
        else if (arg == "max")
        {
            return NodeMeasurementMode::max;
        }
        else if (arg == "min")
        {
            return NodeMeasurementMode::min;
        }
        else if (arg == "avg")
        {
            return NodeMeasurementMode::avg;
        }
        else
        {
            throw std::runtime_error{"measurement-mode must be one of root, all, max, min, avg"};
        }
    }

public:
    // bool help{false};
    // bool verbose{false};
    OutputMode o_mode{OutputMode::none};
    std::optional<std::string> raw_value;
    std::optional<int> value;
    bool isByteValue{false};
    std::optional<u_int32_t> repetitions;
    bool warmup{true};
    std::optional<u_int32_t> warmup_repetitions;
    NodeMeasurementMode measurement_mode{NodeMeasurementMode::root};
    bool show_headers{true};

    typedef std::function<void(BenchmarkSettings &)> NoArgHandle;
// No argument flag behavior
#define S(str, f, v)                               \
    {                                              \
        str, [](BenchmarkSettings &s) { s.f = v; } \
    }

    // No argument flag behavior
    std::unordered_map<std::string, NoArgHandle> NoArgs{
        // S("--help", help, true),
        // S("-h", help, true),

        // S("--verbose", verbose, true),
        // S("-v", verbose, true),

        S("--quiet", o_mode, BenchmarkSettings::OutputMode::quiet),
        S("-q", o_mode, BenchmarkSettings::OutputMode::quiet),

        S("--no-warmup", warmup, false),
        S("--no-headers", show_headers, false)};
#undef S

    typedef std::function<void(BenchmarkSettings &, const std::string &)> OneArgHandle;

#define S(str, f, v)                                                       \
    {                                                                      \
        str, [](BenchmarkSettings &s, const std::string &arg) { s.f = v; } \
    }

    // One argument flag behavior
    std::unordered_map<std::string, OneArgHandle> OneArgs{

        // Performing string -> int conversion
        {"--value", [](BenchmarkSettings &s, const std::string &arg)
         {
             s.raw_value = arg;
             std::optional<int> value = parseBytes(arg);
             if (value.has_value())
             {
                 s.isByteValue = true;
                 s.value = value;
             }
             else
             {
                 s.isByteValue = false;
                 s.value = stoi(arg);
             }

             if (s.value <= 0)
             {
                 throw std::runtime_error{"value must be a positive integer"};
             }
         }},

        // Performing string -> int conversion
        {"--repetitions", [](BenchmarkSettings &s, const std::string &arg)
         {
             int reps_arg = stoi(arg);
             if (reps_arg > 0)
             {
                 s.repetitions = reps_arg;
             }
             else
             {
                 throw std::runtime_error{"repetitions must be a positive integer"};
             }
         }},
        // Performing string -> int conversion
        {"--warmup-repetitions", [](BenchmarkSettings &s, const std::string &arg)
         {
             int reps_arg = stoi(arg);
             if (reps_arg > 0)
             {
                 s.warmup_repetitions = reps_arg;
             }
             else
             {
                 throw std::runtime_error{"repetitions must be a positive integer"};
             }
         }},

        {"--measurement-mode", [](BenchmarkSettings &s, const std::string &arg)
         {
             s.measurement_mode = parseNodeMeasurementMode(arg);
         }},
        {"-m", [](BenchmarkSettings &s, const std::string &arg)
         { s.measurement_mode = parseNodeMeasurementMode(arg); }}};
#undef S

    // Parse the command line arguments
    void parse_settings(int argc, const char *argv[]);
};