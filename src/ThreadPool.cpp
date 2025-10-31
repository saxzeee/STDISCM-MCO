#include "ThreadPool.h"

ThreadPool::ThreadPool(std::size_t threadCount) {
    if (threadCount == 0) threadCount = 1;
    m_workers.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        m_workers.emplace_back([this]() { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    m_stop.store(true, std::memory_order_relaxed);
    m_cv.notify_all();
    for (auto &t : m_workers) {
        if (t.joinable()) t.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.emplace(std::move(task));
    }
    m_cv.notify_one();
}

void ThreadPool::workerLoop() {
    while (!m_stop.load(std::memory_order_relaxed)) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() { return m_stop.load(std::memory_order_relaxed) || !m_tasks.empty(); });
            if (m_stop.load(std::memory_order_relaxed) && m_tasks.empty()) return;
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        if (task) task();
    }
}
