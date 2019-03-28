#pragma once

#include <queue>
#include <mutex>

template <class T>
class Thread_queue
{
private:
    mutable std::recursive_mutex _lock;
    std::queue<T> _elements;

public:
    T front() {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        return _elements.front();
    }

    void push(T& arg) {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        _elements.push(arg);
    }

    void pop() {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        _elements.pop();
    }

    bool not_empty() {
        std::lock_guard<std::recursive_mutex> locker(_lock);
        return !(_elements.empty());
    }
};
