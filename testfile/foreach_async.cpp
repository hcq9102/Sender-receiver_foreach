
#include <hpx/modules/algorithms.hpp>
#include <hpx/modules/program_options.hpp>
#include <hpx/modules/testing.hpp>

#include <hpx/execution_base/sender.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/execution_base/this_thread.hpp>
#include <hpx/local/thread.hpp>
#include <execution>
#include <hpx/execution/algorithms/sync_wait.hpp>
#include <experimental/executor>
#include <hpx/config.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/datapar.hpp>
#include <hpx/include/compute.hpp>

#include <random>
#include <utility>
#include <string>
#include <type_traits>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstddef>
#include <chrono>
#include <iostream>

struct gen_float_t{
    std::mt19937 mersenne_engine {42};
    std::uniform_real_distribution<float> dist_float {1, 1024};
    auto operator()()
    {
        return dist_float(mersenne_engine);
   }
} gen_float{};
// lambda
struct test_t
{    
    template <typename T>
    void operator()(T &x)
    {
        x = 5.0f + x;
    }
} f{};

//std::size_t threads;
using namespace hpx::execution;
namespace ex = hpx::execution::experimental;
namespace tt = hpx::this_thread::experimental;

template <typename ExPolicy> // still need launch policy?
auto foreach_async(ExPolicy policy, std::size_t n)
{
    using allocator_type = hpx::compute::host::block_allocator<float>;
    using executor_type = hpx::compute::host::block_executor<>;
    
    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);
    executor_type executor(numa_domains);

   
    hpx::compute::vector<float, allocator_type> c(n, 0.0, alloc);
    

    // generate numbers according to gen() function
    if constexpr (hpx::is_parallel_execution_policy_v<std::decay_t<ExPolicy>>){
        hpx::generate(par, c.begin(), c.end(), gen_float_t{});
    }
    else
    {
        hpx::generate(seq, c.begin(), c.end(), gen_float_t{});
    }
 
    // count time begin
    auto t1 = std::chrono::high_resolution_clock::now();
    hpx::for_each(policy, c.begin(), c.end(), f);   
    auto t2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = t2 - t1;
    return diff.count();
}

template <typename ExPolicy>
auto test(ExPolicy policy, std::size_t iterations, std::size_t n)
{
    double avg_time = 0.0;
    for (std::size_t i = 0; i < iterations; i++)
    {
        avg_time += foreach_async<ExPolicy>(policy, n);
    }
    avg_time /= (double) iterations;
    return avg_time;
}

int hpx_main()
{
    // define launch policy p
    hpx::execution::experimental::explicit_scheduler_executor<
        hpx::execution::experimental::thread_pool_scheduler> p;

    //auto f = [](std::size_t& v) { v = v + 42.0f ; };

    // lanuch executor
    //using scheduler_t = ex::thread_pool_policy_scheduler<ExPolicy>;
    //auto exec = ex::explicit_scheduler_executor(scheduler_t(policy));
    auto exec = hpx::execution::experimental::with_priority(p, hpx::threads::thread_priority::bound);
    
    std::cout << "for each test Sender&Receiver \n";
    int threadsNum = hpx::get_os_thread_count();
    std::cout << "Threads : " << hpx::get_os_thread_count() << '\n';
    std::ofstream fout("medusa_threads=8_res_build2_debug.csv");
    
    fout << "threadsNum = " << threadsNum <<'\n';
    fout << "n,SEQ,PAR,par_sr,SEQ/PAR, SEQ/par_sr \n";
    for (std::size_t i = 10; i <= 28; i++)
    {
        std::size_t n = std::pow(2, i);
        double SEQ = test(hpx::execution::seq, 10, n);
        double PAR = test(hpx::execution::par, 10, n);
        double par_sr = test(hpx::execution::par.on(exec), 10, n);
        //auto par_sr_async = test(par(task).on(exec), 10, n) | tt::sync_wait();
        
        #if defined(OUTPUT_TO_CSV)
        //data.push_back(std::array<double, 4>{(double) i, seq, par,speed });
        // std::cout << "N : " << i << '\n';
        // std::cout << "SEQ: " << seq << '\n';
        // std::cout << "PAR: " << par << "\n\n";
        // std::cout << "speed : " << seq/par << "\n";
        #endif
        
        fout << n << "," 
            << SEQ << ","
            << PAR  << ","
            << par_sr <<","
            << SEQ/PAR <<","
            << SEQ/par_sr << "\n";
    }
    //std::cout << "DUMP : " << res << "\n";
    fout.close();
    return hpx::local::finalize();

}

int main(int argc, char *argv[]) {
    namespace po = hpx::program_options;

    po::options_description desc_commandline;
    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args); 
  //return hpx::local::init(hpx_main, argc, argv);
}



