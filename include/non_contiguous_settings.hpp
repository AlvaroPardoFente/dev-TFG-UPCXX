#pragma once

#include <benchmark_settings.hpp>

class NonContiguousSettings : public BenchmarkSettings
{
public:
    std::optional<size_t> chunks_per_rank;
    std::optional<size_t> chunk_size;
    std::optional<size_t> in_stride;
    std::optional<size_t> out_inter_rank_stride;
    std::optional<size_t> out_inter_chunk_stride;

    NonContiguousSettings()
    {
        OneArgs["--chunks-per-rank"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.chunks_per_rank = value;
            }
            else
            {
                new_settings.chunks_per_rank = strtoull(arg.c_str(), (char **)nullptr, 10);
            }

            if (new_settings.chunks_per_rank <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
        OneArgs["--chunk-size"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.chunk_size = value;
            }
            else
            {
                new_settings.chunk_size = strtoull(arg.c_str(), (char **)nullptr, 10);
            }

            if (new_settings.chunk_size <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
        OneArgs["--in-stride"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.in_stride = value;
            }
            else
            {
                new_settings.in_stride = strtoull(arg.c_str(), (char **)nullptr, 10);
            }

            if (new_settings.in_stride <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
        OneArgs["--out-inter-rank-stride"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.out_inter_rank_stride = value;
            }
            else
            {
                new_settings.out_inter_rank_stride = strtoull(arg.c_str(), (char **)nullptr, 10);
            }

            if (new_settings.out_inter_rank_stride <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
        OneArgs["--out-inter-chunk-stride"] = [](BenchmarkSettings &s, const std::string &arg)
        {
            NonContiguousSettings &new_settings = static_cast<NonContiguousSettings &>(s);
            std::optional<int> value = parseBytes(arg);
            if (value.has_value())
            {
                new_settings.out_inter_chunk_stride = value;
            }
            else
            {
                new_settings.out_inter_chunk_stride = strtoull(arg.c_str(), (char **)nullptr, 10);
            }

            if (new_settings.out_inter_chunk_stride <= 0)
            {
                throw std::runtime_error{"value must be a positive integer"};
            }
        };
    };
};