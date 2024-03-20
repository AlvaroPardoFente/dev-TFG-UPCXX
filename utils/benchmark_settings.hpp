#pragma once

// These are the headers we'll be using in this post
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <regex>
#include <unordered_map>

namespace settings
{

    // Options struct
    struct benchmark_settings
    {
        // bool help{false};
        // bool verbose{false};
        std::optional<int> value;
        bool isByteValue{false};
        std::optional<u_int> repetitions;
        bool warmup{true};
    };

    // Helper function to convert a string representation of bytes to an integer
    std::optional<int> parseBytes(const std::string &arg)
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

    typedef std::function<void(benchmark_settings &)> NoArgHandle;

#define S(str, f, v)                                \
    {                                               \
        str, [](benchmark_settings &s) { s.f = v; } \
    }

    // No argument flag behavior
    const std::unordered_map<std::string, NoArgHandle> NoArgs{
        // S("--help", help, true),
        // S("-h", help, true),

        // S("--verbose", verbose, true),
        // S("-v", verbose, true),

        // S("--quiet", verbose, false),

        S("--no-warmup", warmup, false),
    };
#undef S

    typedef std::function<void(benchmark_settings &, const std::string &)> OneArgHandle;

#define S(str, f, v)                                                        \
    {                                                                       \
        str, [](benchmark_settings &s, const std::string &arg) { s.f = v; } \
    }

    // One argument flag behavior
    const std::unordered_map<std::string, OneArgHandle> OneArgs{

        // Performing string -> int conversion
        {"--value", [](benchmark_settings &s, const std::string &arg)
         {
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
        {"--repetitions", [](benchmark_settings &s, const std::string &arg)
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
    };
#undef S

    // Parse the command line arguments
    benchmark_settings parse_settings(int argc, const char *argv[])
    {
        benchmark_settings settings;

        for (int i{1}; i < argc; i++)
        {
            std::string opt{argv[i]};

            // NoArg
            if (auto j{NoArgs.find(opt)}; j != NoArgs.end())
                j->second(settings); // Yes, handle it!

            // OneArg
            else if (auto k{OneArgs.find(opt)}; k != OneArgs.end())
                // Yes, do we have a parameter?
                if (++i < argc)
                    // Yes, handle it!
                    k->second(settings, {argv[i]});
                else
                    // No, and we cannot continue, throw an error
                    throw std::runtime_error{"missing param after " + opt};

            // Throw an error for unrecognized options
            else
                throw std::runtime_error{"unrecognized command-line option " + opt};
        }

        return settings;
    }
} // namespace settings