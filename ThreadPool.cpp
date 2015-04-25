//
// Created by lyc-fedora on 15-4-22.
//

#include <iostream>
#include "ThreadPool.h"

using namespace simpleton;

ThreadPool::ThreadPool(unsigned int maxQueue, unsigned int maxNum)
:_maxNum(0),_maxQueue(maxQueue),_isRunning(false),_joiner(_threads)
{
    unsigned int recommend = thread::hardware_concurrency();
    _maxNum = maxNum > recommend ? maxNum : recommend;

    _isRunning = true;
    _threads.reserve(_maxNum);

    try
    {
        for (int i = 0; i < _maxNum; i++)
            _threads.emplace_back(&ThreadPool::workerThread, this);
    }
    catch(...)
    {
        _isRunning = false;
        throw;
    }
}

ThreadPool::~ThreadPool()
{
    unique_lock<mutex> gurad(_mtx);
    //先设置为停止运行
    _isRunning = false;
    //通知所有等待在条件变量的线程
    _cond.notify_all();
}

void ThreadPool::Submit(const TaskType& task) {
    unique_lock<mutex> guard(_mtx);
    while (_taskQueue.size() >= _maxQueue)
        _cond.wait(guard);
    if (_isRunning && _taskQueue.size() < _maxQueue)
    {
        _taskQueue.push_back(task);
        _cond.notify_one();
    }
}

void ThreadPool::workerThread()
{
    try
    {
        while(_isRunning)
        {
            unique_lock<mutex> guard(_mtx);
            //获取新任务
            while(_taskQueue.empty())
                _cond.wait(guard);
            TaskType task;
            if(!_taskQueue.empty() && _isRunning)
            {
                task = std::move(_taskQueue.front());
                _taskQueue.pop_front();
            }
            guard.unlock();
            //运行新任务
            task();
        }
    }
    catch(...)
    {
        throw;
    }
}
