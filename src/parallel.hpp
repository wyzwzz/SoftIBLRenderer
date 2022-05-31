#ifndef SOFTPBRRENDERER_PARALLEL_HPP
#define SOFTPBRRENDERER_PARALLEL_HPP

#include <thread>
#include <mutex>
#include <vector>
#include <optional>
#include <functional>
#include <queue>
#include <future>

inline int actual_worker_count(int worker_count) noexcept
{
    if(worker_count <= 0)
        worker_count += static_cast<int>(std::thread::hardware_concurrency());
    return (std::max)(1, worker_count);
}

struct ThreadPool
{
    ThreadPool(size_t);

    ~ThreadPool();

    template <typename F, typename... Args>
    auto AppendTask(F &&f, Args &&... args);

    void Wait();

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mut;
    std::atomic<size_t> idle;
    std::condition_variable cond;
    std::condition_variable waitCond;
    size_t nthreads;
    bool stop;
};

// add new work item to the pool
template <class F, class... Args> auto ThreadPool::AppendTask(F &&f, Args &&... args)
{
    using return_type = typename std::invoke_result<F,Args...>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mut);
        // don't allow enqueueing after stopping the pool
        if (stop)
        {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace([task]() { (*task)(); });
    }
    cond.notify_one();
    return res;
}

//parallel_forrange will frequently call so using a thread pool is a good choice
extern ThreadPool thread_pool;

template<typename T, typename Func>
void parallel_forrange(T beg, T end, Func &&func, int worker_count = 0)
{
    std::mutex it_mutex;
    T it = beg;
    auto next_item = [&]() -> std::optional<T>
    {
        std::lock_guard lk(it_mutex);
        if(it == end)
            return std::nullopt;
        return std::make_optional(it++);
    };

    std::mutex except_mutex;
    std::exception_ptr except_ptr = nullptr;

    worker_count = actual_worker_count(worker_count);

    auto worker_func = [&](int thread_index)
    {
        for(;;)
        {
            auto item = next_item();
            if(!item)
                break;

            try
            {
                func(thread_index, *item);
            }
            catch(...)
            {
                std::lock_guard lk(except_mutex);
                if(!except_ptr)
                    except_ptr = std::current_exception();
            }

            std::lock_guard lk(except_mutex);
            if(except_ptr)
                break;
        }
    };

    for(int i = 0; i < worker_count; ++i){
        thread_pool.AppendTask(worker_func,i);
    }

    thread_pool.Wait();

    if(except_ptr)
        std::rethrow_exception(except_ptr);
}

#endif // SOFTPBRRENDERER_PARALLEL_HPP
