#include "DeletionQueue.h"

namespace tgl {
    std::deque<std::function<void()>> DeletionQueue::deletions;
    void DeletionQueue::queue(const std::function<void()> &deletion) {
        deletions.push_back(deletion);
    }

    void DeletionQueue::flush() {
        for (auto it = deletions.rbegin(); it != deletions.rend(); it++) {
            (*it)();
        }
        deletions.clear();
    }
}