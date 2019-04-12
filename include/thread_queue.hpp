#include <queue>
#include <mutex>
#include "thread_queue.h"

template <class T>
T Thread_queue<T>::front() {
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return _elements.front();
}

template <class T>
void Thread_queue<T>::push(T& arg) {
    std::lock_guard<std::recursive_mutex> locker(_lock);
    _elements.push(arg);
}

template <class T>
void Thread_queue<T>::pop() {
    std::lock_guard<std::recursive_mutex> locker(_lock);
    _elements.pop();
}

template <class T>
bool Thread_queue<T>::not_empty() {
    std::lock_guard<std::recursive_mutex> locker(_lock);
    return !(_elements.empty());
}
