#pragma once
#include <cstdint>
#include <thread>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
namespace tgl {
    class ThreadPool {
        friend class Renderer;
        uint32_t threadCount;
        std::vector<std::thread> threads;
        std::mutex taskMutex;
        std::map<std::thread::id, std::vector<std::function<void()>>> tasks;
    public:
        ThreadPool();
        ThreadPool(uint32_t threadCount);

        void sendTask(uint32_t threadIndex, const std::function<void()>& task);
        void finishTasks();
    };
}