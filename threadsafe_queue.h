//
// Created by lumbe on 22/02/2021.
//
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <queue>
#include <memory>

#ifndef SIMPLECLIENT_THREADSAFE_QUEUE_H
#define SIMPLECLIENT_THREADSAFE_QUEUE_H
template <typename T>
class threadsafe_queue{
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue();
    threadsafe_queue(const threadsafe_queue& other);
    void push(T value);
    void wait_and_pop(T& value);
    void try_pop(T& value);
    bool empty() const;

};
#endif //SIMPLECLIENT_THREADSAFE_QUEUE_H
