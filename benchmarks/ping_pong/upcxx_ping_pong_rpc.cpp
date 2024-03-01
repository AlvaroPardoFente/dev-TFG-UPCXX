#include <upcxx/upcxx.hpp>

int main(int argc, char **argv)
{
    int ping_pong_count = 1024;
    if (argc > 1)
    {
        try
        {
            ping_pong_count = std::stoi(argv[1]);
        }
        catch (const std::invalid_argument &e)
        {
            // Argument is not a number, do nothing
        }
    }
    upcxx::init();

    int world_rank, world_size;
    world_size = upcxx::rank_n();
    world_rank = upcxx::rank_me();

    int neighbor_rank = (world_rank + 1) % 2;
    std::cout << "Rank " << world_rank << " has neighbor rank " << neighbor_rank << std::endl;

    upcxx::dist_object<upcxx::global_ptr<int>> global_ping_pong_object = upcxx::new_<int>(0);
    int &ping_pong_value = *global_ping_pong_object->local();
    upcxx::global_ptr<int> neighbor_ping_pong_ptr = global_ping_pong_object.fetch(neighbor_rank).wait();

    for (int i = 0; i < ping_pong_count; i++)
    {
        if (world_rank == i % 2)
        {
            std::cout << "Rank " << world_rank << " will increment " << ping_pong_value << " and send to rank " << neighbor_rank << std::endl;
            ping_pong_value++;
            upcxx::rpc(
                neighbor_rank, [](upcxx::dist_object<upcxx::global_ptr<int>> &dist_ptr, int value)
                { *dist_ptr->local() = value; },
                global_ping_pong_object, ping_pong_value)
                .wait();
            std::cout << "Rank " << world_rank << " sent " << ping_pong_value << " to rank " << neighbor_rank << std::endl;
            upcxx::barrier();
        }
        else
        {
            upcxx::barrier();
            std::cout << "Rank " << world_rank << " received " << ping_pong_value << " from rank " << neighbor_rank << std::endl;
        }
    }

    upcxx::finalize();

    return 0;
}