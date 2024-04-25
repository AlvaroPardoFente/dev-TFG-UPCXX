#include <benchmark_settings.hpp>

// Parse the command line arguments
void BenchmarkSettings::parse_settings(int argc, const char *argv[])
{
    for (int i{1}; i < argc; i++)
    {
        std::string opt{argv[i]};

        // NoArg
        if (auto j{NoArgs.find(opt)}; j != NoArgs.end())
            j->second(*this); // Yes, handle it!

        // OneArg
        else if (auto k{OneArgs.find(opt)}; k != OneArgs.end())
            // Yes, do we have a parameter?
            if (++i < argc)
                // Yes, handle it!
                k->second(*this, {argv[i]});
            else
                // No, and we cannot continue, throw an error
                throw std::runtime_error{"missing param after " + opt};

        // Throw an error for unrecognized options
        else
            throw std::runtime_error{"unrecognized command-line option " + opt};
    }
}