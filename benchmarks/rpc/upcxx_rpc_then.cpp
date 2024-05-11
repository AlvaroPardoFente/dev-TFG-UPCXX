#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

uint32_t count = 0, mivar = 0;

constexpr const char loop_end[] = "Loop_end";

class UpcxxRpcPromise : public UpcxxBenchmarkScheme
{

public:
    UpcxxRpcPromise() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        timer.add_time_point(loop_end);
        timer.reserve(reps);
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            upcxx::future<> fut_all = upcxx::make_future();

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::future<> fut = (upcxx::rpc(1, []()
                                                  { return ++count; })
                                           .then([&](uint32_t c)
                                                 { mivar += c; }));
                fut_all = upcxx::when_all(fut_all, fut);
            }

            timer.stop();
            timer.add_time(loop_end);

            fut_all.wait();
        }
        // else
        // {
        //     for (int i = 0; i < number_count; i++)
        //     {
        //         if (i % 10 == 0)
        //             upcxx::progress();
        //     }
        // }
    }

    void reset_result() override
    {
        // if (world_rank == 0)
        // {
        //     std::cout << "mivar: " << mivar << std::endl;
        // }
        count = 0;
        mivar = 0;
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcPromise test;
    test.run(argc, argv);
    return 0;
}