#include "threadpool.h"

void ThreadPool::callback() {
    while(true) {
        // unique_lock 会自动加锁
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() {
            return !jobs_.empty() || this->terminate_;    // 当 jobs_ 不为空或者 terminate_  == true 时跳出等待
        });

        if(this->terminate_) break;

        auto job = jobs_.front();
        jobs_.pop();
        lock.unlock();
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
    mutex_.lock();
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
