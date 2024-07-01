#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>

class UpcxxSenderPromise : public UpcxxBenchmarkScheme
{
public:
    upcxx::dist_object<upcxx::global_ptr<uint32_t>> value_g;
    uint32_t *value;

    // Completion bool as a dist_object
    upcxx::dist_object<upcxx::global_ptr<bool>> completion_flag_g;
    bool *completion_flag_l;

    // Broadcasting root completion flag
    upcxx::global_ptr<bool> root_flag_ptr;

    // Root pointer
    upcxx::global_ptr<uint32_t> root_ptr;

    UpcxxSenderPromise() : UpcxxBenchmarkScheme(2) {}

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

        // Completion bool as a dist_object
        completion_flag_g = upcxx::dist_object<upcxx::global_ptr<bool>>(upcxx::new_<bool>(false));
        completion_flag_l = completion_flag_g->local();

        // Broadcasting root completion flag
        root_flag_ptr = completion_flag_g.fetch(0).wait();

        root_ptr = value_g.fetch(0).wait();
    };

    void benchmark_body() override
    {
        // Perform rput for every value, waiting for the promise to flip the completion flag
        if (world_rank == 1)
        {
            // Init promise
            upcxx::promise<> p;

            for (uint32_t i = 0; i < number_count; i++)
            {
                upcxx::rput(value[i], root_ptr + i, upcxx::operation_cx::as_promise(p));
            }

            p.finalize().then([this]()
                              { upcxx::rput(true, root_flag_ptr).wait(); });
        }

        // Check for completion
        if (world_rank == 0)
        {
            while (*completion_flag_l == false)
            {
                upcxx::progress();
            }

            *completion_flag_l = false;
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
    UpcxxSenderPromise test;
    test.run(argc, argv);
    return 0;
}