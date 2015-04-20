//
// Created by lyc-fedora on 15-4-20.
//

#include "BlockingQueue.h"

using namespace simpleton;

template<typename T>
void BlockingQueue<T>::Push(const T& production)
{
    std::unique_lock<std::mutex> guard(lock);
    while(queue.size() == size)
    {
        cond.wait(guard);
    }
    queue.push_back(production);
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cond.notify_one();
}

template<typename T>
std::shared_ptr<T> BlockingQueue<T>::Pop()
{
    std::unique_lock<std::mutex> guard(lock);
    while(queue.empty())
    {
        cond.wait(guard);
    }
    std::shared_ptr<T> ptr = std::make_shared<T>(queue.front());
    queue.pop_front();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    cond.notify_one();
    return ptr;
}

template<typename T>
bool BlockingQueue<T>::Empty() const
{
    std::unique_lock<std::mutex> guard(lock);
    return queue.empty();
}
