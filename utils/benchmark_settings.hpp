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
    struct BenchmarkSettings
    {
        bool help{false};
        bool verbose{false};
        std::optional<std::string> infile;
        std::optional<std::string> outfile;
        std::optional<int> value;
        bool isByteValue{false};
        std::optional<int> repetitions;
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

    typedef std::function<void(BenchmarkSettings &)> NoArgHandle;

#define S(str, f, v)                               \
    {                                              \
        str, [](BenchmarkSettings &s) { s.f = v; } \
    }

    // No argument flag behavior
    const std::unordered_map<std::string, NoArgHandle> NoArgs{
        S("--help", help, true),
        S("-h", help, true),

        S("--verbose", verbose, true),
        S("-v", verbose, true),

        S("--quiet", verbose, false),
    };
#undef S

    typedef std::function<void(BenchmarkSettings &, const std::string &)> OneArgHandle;

#define S(str, f, v)                                                       \
    {                                                                      \
        str, [](BenchmarkSettings &s, const std::string &arg) { s.f = v; } \
    }

    // One argument flag behavior
    const std::unordered_map<std::string, OneArgHandle> OneArgs{
        // Writing out the whole lambda
        {"-o", [](BenchmarkSettings &s, const std::string &arg)
         {
             s.outfile = arg;
         }},

        // Using the macro
        S("--output", outfile, arg),

        // Performing string -> int conversion
        {"--value", [](BenchmarkSettings &s, const std::string &arg)
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
         }},

        // Performing string -> int conversion
        S("--repetitions", repetitions, stoi(arg)),
    };
#undef S

    // Parse the command line arguments
    BenchmarkSettings parse_settings(int argc, const char *argv[])
    {
        BenchmarkSettings settings;

        // argv[0] is traditionally the program name, so start at 1
        for (int i{1}; i < argc; i++)
        {
            std::string opt{argv[i]};

            // Is this a NoArg?
            if (auto j{NoArgs.find(opt)}; j != NoArgs.end())
                j->second(settings); // Yes, handle it!

            // No, how about a OneArg?
            else if (auto k{OneArgs.find(opt)}; k != OneArgs.end())
                // Yes, do we have a parameter?
                if (++i < argc)
                    // Yes, handle it!
                    k->second(settings, {argv[i]});
                else
                    // No, and we cannot continue, throw an error
                    throw std::runtime_error{"missing param after " + opt};

            // No, has infile been set yet?
            else if (!settings.infile)
                // No, use this as the input file
                settings.infile = argv[i];

            // Yes, possibly throw here, or just print an error
            else
                throw std::runtime_error{"unrecognized command-line option " + opt};
        }

        return settings;
    }
} // namespace settings