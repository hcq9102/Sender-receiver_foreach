
#include <hpx/config.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/datapar.hpp>
#include <hpx/include/compute.hpp>

#include <hpx/execution_base/sender.hpp>
#include <hpx/execution_base/receiver.hpp>
#include <hpx/modules/execution.hpp>
#include <hpx/execution_base/this_thread.hpp>
#include <hpx/local/thread.hpp>
#include <hpx/modules/execution.hpp>
#include <execution>
#include <hpx/execution/algorithms/sync_wait.hpp>
#include <hpx/config.hpp>
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/include/datapar.hpp>
#include <hpx/include/compute.hpp>

#include <string>
#include <type_traits>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstddef>


struct gen_int_t{
    std::mt19937 mersenne_engine {42};
    std::uniform_int_distribution<int> dist_int {1, 1024};
    auto operator()()
    {
        return dist_int(mersenne_engine);
    }
};
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
        x = 42.0f + x;
    }
} f{};

//std::size_t threads;
using namespace hpx::execution;

template <typename ExPolicy> 
auto foreach_async(ExPolicy policy, std::size_t n)
{
    using allocator_type = hpx::compute::host::block_allocator<float>;
    using executor_type = hpx::compute::host::block_executor<>;
    
    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);
    executor_type executor(numa_domains);

   
    hpx::compute::vector<float, allocator_type> c(n, 0.0, alloc);
    //namespace ex = hpx::execution::experimental;
    //namespace tt = hpx::this_thread::experimental;
    // explicit_scheduler_executor ??

    // define launch policy p
    hpx::execution::experimental::scheduler_executor<
        hpx::execution::experimental::thread_pool_scheduler> p;

    //auto f = [](std::size_t& v) { v = v + 42.0f ; };

    // lanuch executor
    //using scheduler_t = ex::thread_pool_policy_scheduler<ExPolicy>;
    //auto exec = ex::explicit_scheduler_executor(scheduler_t(policy));
    auto exec = hpx::execution::experimental::with_priority(p, hpx::threads::thread_priority::bound);

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
    //hpx::for_each(policy.on(exec), c.begin(), c.end(), f);
    //hpx::for_each(par(task).on(exec), c.begin(), c.end(), f); // Segmentation fault: Segmentation fault (core dumped)
    if constexpr (hpx::is_parallel_execution_policy_v<ExPolicy>){ // par_sr
        hpx::for_each(par(task).on(exec), c.begin(), c.end(), f) | hpx::this_thread::experimental::sync_wait();
    }
    else
    {
        hpx::for_each(policy, c.begin(), c.end(), f); //seq
    }

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
    std::cout << "for each test Sender&Receiver \n";
    int threadsNum = hpx::get_os_thread_count();
    std::cout << "Threads : " << hpx::get_os_thread_count() << '\n';
    std::vector<std::array<double, 4>> data;
    std::ofstream fout("result.csv");
    
    fout << "threadsNum = " << threadsNum <<'\n';
    fout << "n,seq,par,speed_up\n";
    for (std::size_t i = 10; i <= 20; i++)
    {
        std::size_t n = std::pow(2, i);
        double seq = test(hpx::execution::seq, 10, n);
        double par = test(hpx::execution::par, 10, n);
        
        #if defined(OUTPUT_TO_CSV)
        //data.push_back(std::array<double, 4>{(double) i, seq, par,speed });
        // std::cout << "N : " << i << '\n';
        // std::cout << "SEQ: " << seq << '\n';
        // std::cout << "PAR: " << par << "\n\n";
        // std::cout << "speed : " << seq/par << "\n";
        #endif
        
        fout << n << "," 
            << seq << ","
            << par  << ","
            << seq/par << "\n";
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



