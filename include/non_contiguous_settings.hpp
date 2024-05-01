#include <benchmark_settings.hpp>

class NonContiguousSettings : public BenchmarkSettings
{
public:
    std::optional<uint32_t> block_count;

    NonContiguousSettings()
    {
        OneArgs["--block-count"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.block_count = value;
            }
            else
            {
                new_settings.block_count = stoi(arg);
            }

            if (new_settings.block_count <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
    };
};