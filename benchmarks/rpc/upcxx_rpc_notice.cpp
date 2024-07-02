#include <upcxx/upcxx.hpp>
#include <upcxx_benchmark_scheme.hpp>
#include <iostream>
#include <random>

uint32_t get_target_rank(uint32_t key, uint32_t nums_per_rank)
{
    return key / nums_per_rank;
}

class UpcxxRpcNotice : public UpcxxBenchmarkScheme
{
public:
    upcxx::dist_object<std::unordered_map<uint32_t, uint32_t>> values;
    std::vector<uint32_t> target_keys;

    uint32_t nums_per_rank;

    UpcxxRpcNotice() : UpcxxBenchmarkScheme() {}

    void init(int argc, char *argv[]) override
    {
        UpcxxBenchmarkScheme::init(argc, argv);

        nums_per_rank = number_count;

        values = upcxx::dist_object<std::unordered_map<uint32_t, uint32_t>>({});
        target_keys.resize(nums_per_rank);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, world_size * nums_per_rank - 1);

        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            uint32_t key_value = i + world_rank * nums_per_rank;
            values->emplace(key_value, key_value);
            target_keys.at(i) = distr(gen);
        }

        // if (world_rank == world_size - 1)
        // {
        //     std::cout << "Values:" << std::endl;
        //     for (uint32_t i = 0; i < nums_per_rank; i++)
        //     {
        //         std::cout << values->at(i + world_rank * nums_per_rank) << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // if (world_rank == 0)
        // {
        //     std::cout << "Target keys:" << std::endl;
        //     for (uint32_t i = 0; i < number_count; i++)
        //     {
        //         std::cout << target_keys.at(i) << " ";
        //     }
        //     std::cout << std::endl;
        // }
    };

    void benchmark_body() override
    {
        // Init promise
        upcxx::promise<> p;

        for (uint32_t i = 0; i < nums_per_rank; i++)
        {
            // std::cout << world_rank << " asking for: " << target_keys.at(i) << std::endl;
            upcxx::future<uint32_t> fut = upcxx::rpc(
                get_target_rank(target_keys.at(i), nums_per_rank),
                [](upcxx::dist_object<std::unordered_map<uint32_t, uint32_t>> &values, uint32_t key) -> uint32_t
                {
                    auto elem = values->find(key);
                    if (elem == values->end())
                        return uint32_t{0};
                    else
                        return elem->second;
                },
                values, target_keys.at(i));

            uint32_t val = fut.wait();
            // std::cout << world_rank << " received " << val << " from " << get_target_rank(target_keys.at(i), nums_per_rank) << std::endl;
        }

        p.finalize().wait();
    }

    void reset_result() override
    {
        barrier();
    }
};

int main(int argc, char *argv[])
{
    UpcxxRpcNotice test;
    test.run(argc, argv);
    return 0;
}