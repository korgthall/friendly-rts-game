#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <deque>
#include <vector>
#include <cstddef>

template<typename T>
class TaskQueue {
private:
    std::deque<T> queue;
    size_t maxCapacity;

public:
    explicit TaskQueue(size_t capacity = 50) : maxCapacity(capacity) {
    }

    bool push(const T &item) {
        if (queue.size() >= maxCapacity) return false;
        queue.push_back(item);
        return true;
    }

    T pop() {
        if (queue.empty()) return T();
        T item = queue.front();
        queue.pop_front();
        return item;
    }

    T peek() const {
        if (queue.empty()) return T();
        return queue.front();
    }

    bool isEmpty() const { return queue.empty(); }
    size_t size() const { return queue.size(); }
    void clear() { queue.clear(); }

    void loadFromVector(const std::vector<T> &vec) {
        queue.clear();
        for (const auto &item: vec) {
            push(item);
        }
    }
};

#endif
