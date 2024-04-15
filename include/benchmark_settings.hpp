#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace settings
{
    // Output mode enum
    enum class output_mode
    {
        none,
        verbose,
        quiet
    };

    // Options struct
    struct benchmark_settings
    {
        // bool help{false};
        // bool verbose{false};
        output_mode o_mode{output_mode::none};
        std::optional<std::string> raw_value;
        std::optional<int> value;
        bool isByteValue{false};
        std::optional<u_int> repetitions;
        bool warmup{true};
    };

    typedef std::function<void(benchmark_settings &)> NoArgHandle;
    // No argument flag behavior
    extern const std::unordered_map<std::string, NoArgHandle> NoArgs;

    typedef std::function<void(benchmark_settings &, const std::string &)> OneArgHandle;
    // One argument flag behavior
    extern const std::unordered_map<std::string, OneArgHandle> OneArgs;

    // Parse the command line arguments
    benchmark_settings parse_settings(int argc, const char *argv[]);

} // namespace settings