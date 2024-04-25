#include <benchmark_scheme.hpp>

void BenchmarkScheme::init(int argc, char *argv[])
{
    settings = settings::parse_settings(argc, const_cast<const char **>(argv));
    if (settings.value.has_value())
    {
        if (settings.isByteValue)
        {
            number_count = settings.value.value();
        }
        else
        {
            number_count = settings.value.value();
        }
    }

    if (settings.repetitions.has_value())
    {
        reps = settings.repetitions.value();
    }

    if (settings.warmup_repetitions.has_value())
    {
        warmup_repetitions = settings.warmup_repetitions.value();
    }

    if (world_rank == 0)
    {
        timer.reserve(reps);
        timer.set_settings(&settings);
    }
}

void BenchmarkScheme::run_benchmark(bool use_barrier)
{
    if (use_barrier)
    {
        barrier();
    }

    // Start clock
    if (world_rank == 0)
    {
        timer.start();
    }

    benchmark_body();

    // End clock
    if (world_rank == 0)
    {
        timer.stop();
        timer.add_time();
    }

    reset_result();
}

void BenchmarkScheme::print_results()
{
    if (world_rank == 0)
    {
        timer.print_times();
    }
}

void BenchmarkScheme::run(int argc, char *argv[])
{

    init(argc, argv);

    if (settings.warmup)
    {
        for (uint32_t i = 0; i < warmup_repetitions; i++)
        {
            run_benchmark();
            timer.reset_times();
        }
    }

    for (uint32_t i = 0; i < reps; i++)
    {
        run_benchmark();
    }

    print_results();

    finalize();
}