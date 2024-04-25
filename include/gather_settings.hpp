#include <benchmark_settings.hpp>

class GatherSettings : public BenchmarkSettings
{
public:
    bool measure_fetch{false};

    GatherSettings()
    {
        NoArgs["--measure-fetch"] = [](BenchmarkSettings &s)
        { 
            GatherSettings &gs = static_cast<GatherSettings&>(s);
            gs.measure_fetch = true; };
    };
};