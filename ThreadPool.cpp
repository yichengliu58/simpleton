//
// Created by lyc-fedora on 15-4-22.
//

#include "ThreadPool.h"

using namespace simpleton;

ThreadPool::ThreadPool(unsigned int max = thread::hardware_concurrency())
        :_maxNum(max),_isDone(true),_joiner(this->_threads)
{
    try
    {
        //产生_maxNum个线程
        for (int i = 0; i < _maxNum; i++)
            _threads.emplace_back(&ThreadPool::workerThread, this);
    }
    catch(...)
    {
        //如果产生任何异常则停止产生线程
        //并且等待已产生的线程结束
        _isDone = true;
        throw;
    }
}

void ThreadPool::Submit(const function<void()>& task)
{
    unique_lock<mutex> guard(_mtx);
    while(_taskQueue.size() > _maxNum)
        _cond.wait(guard);
    _taskQueue.push_back(task);
    _cond.notify_all();
}

void ThreadPool::workerThread()
{

}