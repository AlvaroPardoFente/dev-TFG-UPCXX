#include <cstdio>
#include <mpi.h>
#include <string>
#include <tuple>
#include <utility>
#include <mpi_benchmark_scheme.hpp>
#include <iostream>

/****************************************************************/
// https://gist.github.com/tbenthompson/94fd4095f89da23dec20 tbenthompson/function_serialize.cpp

void serialization_baseline()
{
    puts("BASELINE");
};

static auto baseline_loc = reinterpret_cast<intptr_t>(
    static_cast<void (*)()>(serialization_baseline));

template <typename ResultT, typename... Args>
intptr_t serialize(ResultT (*f)(Args...))
{
    auto f_loc = reinterpret_cast<intptr_t>(f);
    return (f_loc - baseline_loc);
}

// Syntax for returning a function pointer from a function is really gross.
template <typename ResultT, typename... Args>
ResultT (*deserialize(intptr_t offset))(Args...)
{
    auto f_loc = baseline_loc + offset;
    return reinterpret_cast<ResultT (*)(Args...)>(f_loc);
}

/****************************************************************/

template <typename Function, typename... Ts>
struct generic_arg
{
    intptr_t rpc_wrapper_addr;
    Function kernel;
    std::tuple<Ts...> args;

    generic_arg(const Function &k, const Ts &...as) : kernel(k), args{as...} {}

    template <std::size_t... index>
    inline void call(std::index_sequence<index...>)
    {
        kernel(std::get<index>(args)...);
    }

    inline void apply()
    {
        call(std::make_index_sequence<sizeof...(Ts)>{});
    }
};

/* RPC wrapper function */
template <typename Function, typename... Ts>
void rpc_wrapper(void *args)
{
    generic_arg<Function, Ts...> *a = (generic_arg<Function, Ts...> *)args;

    a->apply();
}

template <typename Function, typename... Ts>
void remote_run(Function k, const Ts &...as)
{
    generic_arg<Function, Ts...> args(k, as...);
    args.rpc_wrapper_addr = serialize(rpc_wrapper<Function, Ts...>);
    MPI_Send((void *)&args, sizeof(args), MPI_BYTE, 1, 0, MPI_COMM_WORLD);
}

void run_rpc()
{
    MPI_Status status;
    int count;

    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_BYTE, &count);
    char *const buf = new char[count];
    MPI_Recv(buf, count, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    auto f_deserialized = deserialize<void, void *>(*(reinterpret_cast<intptr_t *>(buf)));
    f_deserialized((void *)buf);
    delete[] buf;
}

uint32_t count = 0;

class MpiRpc : public MpiBenchmarkScheme
{
public:
    void init(int argc, char *argv[]) override
    {
        MpiBenchmarkScheme::init(argc, argv);
    };

    void benchmark_body() override
    {
        if (world_rank == 0)
        {
            for (uint32_t i = 0; i < number_count; i++)
            {
                remote_run([]()
                           { count++; });
            }
        }
        else
        {
            while (count < number_count)
            {
                run_rpc();
            }
        }
    }

    void reset_result() override
    {
        count = 0;
    }
};

int main(int argc, char *argv[])
{
    MpiRpc test;
    test.run(argc, argv);
    return 0;
}