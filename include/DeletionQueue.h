#pragma once
#include <deque>
#include <functional>
namespace tgl {
    class DeletionQueue {
    private:
        static std::deque<std::function<void()>> deletions;
    public:
        static void queue(const std::function<void()>& deletion);
        static void flush();
    };
}