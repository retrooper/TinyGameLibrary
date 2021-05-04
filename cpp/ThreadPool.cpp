#include "ThreadPool.h"
#include <iostream>
namespace tgl {
    ThreadPool::ThreadPool() :
    ThreadPool::ThreadPool(std::thread::hardware_concurrency())
    {}

    ThreadPool::ThreadPool(uint32_t threadCount) {
        this->threadCount = threadCount = (threadCount == 0 ? 1 : threadCount);
        for (uint32_t i = 0; i < threadCount; i++) {
            threads.emplace_back([this]() {
                while (true) {
                    taskMutex.lock();
                    if (tasks.find(std::this_thread::get_id()) != tasks.end()) {
                        for (const auto& task : tasks[std::this_thread::get_id()]) {
                            task();
                        }
                        tasks[std::this_thread::get_id()].clear();
                    }
                    taskMutex.unlock();
                }
            });
        }
        threads.reserve(threadCount);
    }

    void ThreadPool::sendTask(uint32_t threadIndex, const std::function<void()>& task) {
        std::thread::id threadID = threads[threadIndex].get_id();
        taskMutex.lock();
        tasks[threadID].push_back(task);
        taskMutex.unlock();
    }

    void ThreadPool::finishTasks() {
        taskMutex.lock();
        for (std::thread& t : threads) {
            for (const auto& task : tasks[t.get_id()]) {
                task();
            }
            tasks[t.get_id()].clear();
        }
        taskMutex.unlock();
    }
}