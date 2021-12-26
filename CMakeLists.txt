cmake_minimum_required(VERSION 3.16)

project(ThreadPool)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

add_executable(ThreadPool main.cpp)
find_package(Threads REQUIRED)
target_link_libraries(ThreadPool Threads::Threads)
