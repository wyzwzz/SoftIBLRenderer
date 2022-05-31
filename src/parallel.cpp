//
// Created by wyz on 2022/5/31.
//
#include "parallel.hpp"
#include "logger.hpp"

ThreadPool thread_pool(actual_worker_count(0));

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads) : idle(threads), nthreads(threads), stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back([this] {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->mut);
                    this->cond.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                    {
                        return;
                    }
                    idle--;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
                idle++;
                {
                    std::lock_guard<std::mutex> lk(this->mut);

                    if (idle.load() == this->nthreads && this->tasks.empty())
                    {
                        waitCond.notify_all();
                    }
                }
            }
        });
}



void ThreadPool::Wait()
{
    static std::mutex m;
    std::unique_lock<std::mutex> l(m);
    waitCond.wait(l, [this]() {
        LOG_DEBUG("wait print: {} {}",this->idle.load(),tasks.size());
        return this->idle.load() == nthreads && tasks.empty();
    });
    LOG_DEBUG("print: {} {}",this->idle.load(),tasks.size());
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mut);
        stop = true;
    }
    cond.notify_all();
    for (std::thread &worker : workers)
    {
        worker.join();
    }
}