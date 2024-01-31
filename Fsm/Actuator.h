#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include "ThreadPriority.h"

class CActuator
{
public:
    CActuator(int processor = RT_BIND_RUN_CPU, int strategy = RT_SCHED_STRATEGY, int priority = RT_MIN_PRI, std::size_t count = 1);
    template<class F, class... Args>
    auto emplace_back(F&& f, Args&&... args)
#if __cplusplus >= 201703L
        ->std::shared_future<typename std::invoke_result<F, Args...>::type>;
#else
        ->std::shared_future<typename std::result_of<F(Args...)>::type>;
#endif
    template<class F, class... Args>
    auto emplace_front(F&& f, Args&&... args)
#if __cplusplus >= 201703L
        ->std::shared_future<typename std::invoke_result<F, Args...>::type>;
#else
        ->std::shared_future<typename std::result_of<F(Args...)>::type>;
#endif
    ~CActuator();
    void stop();
    void restart();
    void restart(std::size_t count);

private:
    void create(int processor, int strategy, int priority, std::size_t count);

private:
    std::vector<ThreadPriority> workers;
    std::deque<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop_ = false;

private:
    int processor_ = RT_BIND_RUN_CPU;
    int strategy_ = RT_SCHED_STRATEGY;
    int priority_ = RT_MIN_PRI;
    std::size_t count_ = 1;

};
 
inline CActuator::CActuator(int processor, int strategy, int priority, std::size_t count)
    : stop_(false)
{
    processor_ = processor;
    strategy_ = strategy;
    priority_ = priority;
    count_ = count;

    create(processor_, strategy_, priority_, count_);
}

template<class F, class... Args>
inline auto CActuator::emplace_back(F&& f, Args&&... args)
#if __cplusplus >= 201703L
    ->std::shared_future<typename std::invoke_result<F, Args...>::type>
#else
    ->std::shared_future<typename std::result_of<F(Args...)>::type>
#endif
{
#if __cplusplus >= 201703L
    using return_type = typename std::invoke_result<F, Args...>::type;
#else
    using return_type = typename std::result_of<F(Args...)>::type;
#endif

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop_)
            throw std::runtime_error("Actuator stopped");

        tasks.emplace_back([task](){ (*task)(); });
    }
    condition.notify_one();
    return res.share();
}

template<class F, class... Args>
inline auto CActuator::emplace_front(F&& f, Args&&... args)
#if __cplusplus >= 201703L
    ->std::shared_future<typename std::invoke_result<F, Args...>::type>
#else
    ->std::shared_future<typename std::result_of<F(Args...)>::type>
#endif
{
#if __cplusplus >= 201703L
    using return_type = typename std::invoke_result<F, Args...>::type;
#else
    using return_type = typename std::result_of<F(Args...)>::type;
#endif

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if (stop_)
            throw std::runtime_error("Actuator stopped");

        tasks.emplace_front([task]() { (*task)(); });
    }
    condition.notify_one();
    return res.share();
}

inline CActuator::~CActuator()
{
    if (workers.size() > 0)
    {
        stop();
    }
}

inline void CActuator::stop()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_ = true;
    }
    condition.notify_all();
    for (auto& worker : workers)
        worker.join();

    workers.clear();
}

inline void CActuator::restart()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (false == stop_)
        {
            return;
        }
        stop_ = false;
    }

    create(processor_, strategy_, priority_, count_);
}

inline void CActuator::restart(std::size_t count)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (false == stop_)
        {
            return;
        }
        stop_ = false;
    }

    create(processor_, strategy_, priority_, count);
}

inline void CActuator::create(int processor, int strategy, int priority, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
        workers.emplace_back(
            processor,
            strategy,
            priority,
            [this]
            {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this] { return this->stop_ || !this->tasks.empty(); });
                        if (this->stop_ && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop_front();
                    }

                    task();
                }
            }
            );
}
