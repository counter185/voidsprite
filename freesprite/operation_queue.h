#pragma once

#include "globals.h"
#include <queue>
#include <mutex>

class OperationQueue {
protected:
    std::queue<std::function<void()>> operations;
    std::mutex queueMutex;
public:
    ~OperationQueue() {
        clear();
    }

    void process() {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!operations.empty()) {
            auto operation = operations.front();
            operations.pop();
            operation();
        }
    }
    void add(std::function<void()> operation) {
        std::lock_guard<std::mutex> lock(queueMutex);
        operations.push(operation);
    }
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!operations.empty()) {
            operations.pop();
        }
    }
};