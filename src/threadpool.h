#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
    std::condition_variable cond_;
    std::mutex              mutex_;
    bool                    terminate_ = false;

    std::queue<std::thread>               works_;
    std::queue<std::function<void(void)>> jobs_;

    ThreadPool();

protected:
    void callback();

public:
    static ThreadPool& getInstance();
    ~ThreadPool();
    void push(std::function<void(void)> func);
};

