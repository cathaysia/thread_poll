#include "threadpool.h"

#include <iostream>
#include <ostream>

int main() {
    int        a = 0;
    std::mutex mtx;

    auto& pool = ThreadPool::getInstance();

    auto func = [&]() {
        mtx.lock();
        ++a;
        mtx.unlock();
        std::cout << a << ' ';
        std::flush(std::cout);
    };
    for(int i = 0; i < 50; ++i) { pool.push(func); }
    return 0;
}
