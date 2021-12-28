add_library(ThreadPool threadpool.cpp)

find_package(Threads REQUIRED)
target_link_libraries(ThreadPool Threads::Threads)

add_executable(test_threadpool threadpool_test.cpp)
target_link_libraries(test_threadpool PRIVATE ThreadPool)
