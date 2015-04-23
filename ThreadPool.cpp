//
// Created by lyc-fedora on 15-4-22.
//

#include <iostream>
#include "ThreadPool.h"

using namespace simpleton;

void ThreadPool::Start(unsigned int max)
{
    unsigned int recommend = thread::hardware_concurrency();
    _maxNum = max > recommend ? max : recommend;

    _isRunning = true;
    _threads.reserve(_maxNum);
    for(int i = 0;i < _maxNum;i++)
        _threads.emplace_back(&ThreadPool::workerThread,this);
}

/*void ThreadPool::Submit(const TaskType& task)
{
    unique_lock<mutex> guard(_mtx);
    while(_taskQueue.size() > _maxNum)
        _cond.wait(guard);
    _taskQueue.push_back(task);
    _cond.notify_all();
}*/

void ThreadPool::workerThread()
{
    for (; ;) {
        this_thread::sleep_for(chrono::seconds(1));
        cout << this_thread::get_id() << endl;
    }
}
