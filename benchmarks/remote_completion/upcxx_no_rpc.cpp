#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

// Returns false if at least one element in the array is zero
bool is_array_complete(uint32_t *array, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        if (array[i] == 0)
        {
            return false;
        }
    }
    return true;
}

class UpcxxNoRpc : public UpcxxBenchmarkScheme
{
public:
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Root pointer
    upcxx::global_ptr<uint32_t> root_ptr;

    UpcxxNoRpc() : UpcxxBenchmarkScheme(2) {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        value_g = upcxx::dist_object<upcxx::global_ptr<uint32_t>>(upcxx::new_array<uint32_t>(number_count));
        value = value_g->local();

        if (world_rank == 1)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                value[i] = i + 1;
            }
        }
        else if (world_rank == 0)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                value[i] = 0;
            }
        }

        root_ptr = value_g.fetch(0).wait();
    };

    void benchmark_body() override
    {
        // Perform rput for every value
        if (world_rank == 1)
        {
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(value[i], root_ptr + i, upcxx::operation_cx::as_promise(p));
            }

            p.finalize().wait();
        }

        // Check for completion
        if (world_rank == 0)
        {
            while (!is_array_complete(value, number_count))
            {
                upcxx::progress();
            }
        }
    }

    void reset_result() override
    {
        // // Print result
        // if (world_rank == 0)
        // {
        //     for (int i = 0; i < number_count; i++)
        //     {
        //         std::cout << value_g->local()[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }

        // Reset result
        if (world_rank == 0)
        {
            auto *value_l = value_g->local();
            for (uint32_t i = 0; i < number_count; i++)
            {
                value_l[i] = 0;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    UpcxxNoRpc test;
    test.run(argc, argv);
    return 0;
}