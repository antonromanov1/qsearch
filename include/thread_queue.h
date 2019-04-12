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
    T front();
    void push(T&);
    void pop();
    bool not_empty();
};

#include "thread_queue.hpp"
