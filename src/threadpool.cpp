#include "threadpool.h"
#include <iostream>
#include <thread>

void ThreadPool::callback() {
    while(true) {
        // unique_lock 会自动加锁
        std::unique_lock<std::mutex> lock(mutex_);
        while(jobs_.empty()) {
            if(this->terminate_) return;
            cond_.wait(lock);
        }
        if(this->terminate_) break;

        auto job = jobs_.front();
        jobs_.pop();
        lock.unlock(); // TODO: 这个必须要去掉吗？
        job();
    }
}

ThreadPool::ThreadPool() {
    unsigned numWorksers = std::thread::hardware_concurrency();
    for(size_t i = 0; i < numWorksers; ++i) {
        auto t = std::thread(&ThreadPool::callback, this);
        // thread 只有移动语义
        works_.push(std::move(t));
    }
}

ThreadPool::~ThreadPool() {
    this->terminate_ = true;
    std::cout << std::endl << 1 << std::endl;
    mutex_.lock();    // WARNING: 这里有时会抛异常，不知道为什么
    std::cout << std::endl << 2 << std::endl;
    cond_.notify_all();
    mutex_.unlock();
    while(!this->works_.empty()) {
        std::thread t = std::move(works_.front());
        works_.pop();
        t.join();
    }
}

void ThreadPool::push(std::function<void(void)> func) {
    mutex_.lock();
    jobs_.push(func);
    cond_.notify_one();
    mutex_.unlock();
}

ThreadPool& ThreadPool::getInstance() {
    static ThreadPool pool;
    return pool;
}
