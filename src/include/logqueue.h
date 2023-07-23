#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename Log>
class LogQueue
{
public:
    void push(const Log& data)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace(data);
        }
        m_condVar.notify_one();
    }
    Log pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condVar.wait(lock, [&](){
            return !this->m_queue.empty();
        });
        Log data = m_queue.front();
        m_queue.pop();
        return std::move(data);
    }

private:
    std::queue<Log> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condVar;
};