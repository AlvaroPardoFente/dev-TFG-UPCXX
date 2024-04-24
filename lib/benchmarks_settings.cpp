#include <benchmark_settings.hpp>

#include <iostream>
#include <stdexcept>
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

#define S(str, f, v)                                          \
    {                                                         \
        str, [](settings::benchmark_settings &s) { s.f = v; } \
    }

// No argument flag behavior
const std::unordered_map<std::string, settings::NoArgHandle> settings::NoArgs{
    // S("--help", help, true),
    // S("-h", help, true),

    // S("--verbose", verbose, true),
    // S("-v", verbose, true),

    S("--quiet", o_mode, settings::output_mode::quiet),
    S("-q", o_mode, settings::output_mode::quiet),

    S("--no-warmup", warmup, false),
};
#undef S

#define S(str, f, v)                                                                  \
    {                                                                                 \
        str, [](settings::benchmark_settings &s, const std::string &arg) { s.f = v; } \
    }

// One argument flag behavior
const std::unordered_map<std::string, settings::OneArgHandle> settings::OneArgs{

    // Performing string -> int conversion
    {"--value", [](settings::benchmark_settings &s, const std::string &arg)
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
    {"--repetitions", [](settings::benchmark_settings &s, const std::string &arg)
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
    {"--warmup-repetitions", [](settings::benchmark_settings &s, const std::string &arg)
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
};
#undef S

// Parse the command line arguments
settings::benchmark_settings settings::parse_settings(int argc, const char *argv[])
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