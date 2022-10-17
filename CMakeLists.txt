cmake_minimum_required(VERSION 3.13)
project(foreach_async CXX)

find_package(HPX REQUIRED)
add_executable(foreach_async foreach_async.cpp)
target_link_libraries(foreach_async HPX::hpx HPX::wrap_main)
