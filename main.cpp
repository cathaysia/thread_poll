#include <cstdio>
#include <list>
#include <queue>
#include <stdexcept>

extern "C" {
#include <pthread.h>
}

class ThreadPool;

struct Worker {
    pthread_t   thread;
    ThreadPool* pool;

    int terminate;
    Worker(ThreadPool* pool) : pool(pool) { }
};

struct Job {
    void* user_data;
    void (*func)(Job* job);
};

class ThreadPool {
    pthread_cond_t  jobs_cond;
    pthread_mutex_t jobs_mutex;

    std::list<Worker*> workers;
    std::list<Job*>    jobs;

private:
    static void* ThreadCallback(void* arg) {
        Worker* worker = static_cast<Worker*>(arg);
        while(true) {
            pthread_mutex_lock(&worker->pool->jobs_mutex);
            while(worker->pool->jobs.empty()) {
                if(worker->terminate) break;
                pthread_cond_wait(&worker->pool->jobs_cond, &worker->pool->jobs_mutex);
            }
            if(worker->terminate) {
                pthread_mutex_unlock(&worker->pool->jobs_mutex);
                break;
            }
            Job* job = worker->pool->jobs.front();
            worker->pool->jobs.pop_front();
            pthread_mutex_unlock(&worker->pool->jobs_mutex);
            job->func(static_cast<Job*>(job->user_data));
        }
        delete worker;
        pthread_exit(nullptr);
    }

public:
    ThreadPool(int numWorkers) : jobs_cond(PTHREAD_COND_INITIALIZER), jobs_mutex(PTHREAD_MUTEX_INITIALIZER) {
        if(numWorkers < 1) numWorkers = 1;
        for(size_t i = 0; i < numWorkers; ++i) {
            Worker* worker = new Worker(this);
            int     ret    = pthread_create(&worker->thread, nullptr, ThreadCallback, worker);
            if(ret) {
                delete worker;
                throw std::runtime_error("pthread_create failed");
            }
            workers.push_back(worker);
        }
    }
    ~ThreadPool() {
        for(Worker* worker: workers) worker->terminate = 1;
        pthread_mutex_lock(&this->jobs_mutex);
        pthread_cond_broadcast(&this->jobs_cond);
        pthread_mutex_unlock(&this->jobs_mutex);
    }
    void push(Job* job) {
        pthread_mutex_lock(&this->jobs_mutex);
        jobs.push_back(job);
        pthread_cond_signal(&this->jobs_cond);
        pthread_mutex_unlock(&this->jobs_mutex);
    }
};

int main(int argc, char* argv[]) {

    return 0;
}
