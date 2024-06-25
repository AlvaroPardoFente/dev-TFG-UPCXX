#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxRput : public UpcxxBenchmarkScheme
{
public:
    upcxx::global_ptr<uint32_t> data_block;

    UpcxxRput() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        if (world_rank == 0)
        {
            data_block = upcxx::new_array<uint32_t>(number_count);
            std::fill(data_block.local(), data_block.local() + number_count, 0);
            upcxx::broadcast(data_block, 0).wait();
        }
        else
        {
            data_block = upcxx::broadcast(data_block, 0).wait();
        }
    };

    void benchmark_body() override
    {
        if (world_rank != 0)
        {
            uint32_t put_value = world_rank;
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(put_value, data_block + i, upcxx::operation_cx::as_promise(p));
            }

            p.finalize().wait();
        }
    }

    void reset_result() override
    {
        barrier();

        // Print result
        // if (world_rank == 0)
        // {
        //     for (uint32_t i = 0; i < number_count; i++)
        //     {
        //         std::cout << data_block.local()[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        if (world_rank == 0)
        {
            std::fill(data_block.local(), data_block.local() + number_count, 0);
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxRput test;
    test.run(argc, argv);
    return 0;
}