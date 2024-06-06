#pragma once

#include <benchmark_settings.hpp>

class PingPongSettings : public BenchmarkSettings
{
public:
    std::optional<uint32_t> block_size;

    PingPongSettings()
    {
        OneArgs["--block-size"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            PingPongSettings &new_settings = static_cast<PingPongSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.block_size = value;
            }
            else
            {
                new_settings.block_size = stoi(arg);
            }

            if (new_settings.block_size <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
    };
};