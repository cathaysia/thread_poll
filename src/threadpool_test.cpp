#include "threadpool.h"

#include <atomic>
#include <iostream>
#include <ostream>

int main() {
    std::atomic_int a = 0;

    auto& pool = ThreadPool::getInstance();

    auto func = [&]() {
        ++a;
        std::cout << a << ' ';
        std::flush(std::cout);
    };
    for(int i = 0; i < 50; ++i) { pool.push(func); }
    return 0;
}
