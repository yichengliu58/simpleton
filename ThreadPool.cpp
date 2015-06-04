//
// Created by lyc-fedora on 15-4-22.
//

#include <iostream>
#include "ThreadPool.h"

using namespace simpleton;

ThreadPool::ThreadPool(unsigned int maxNum)
    :_maxNum(0),
     _isRunning(false),
     _joiner(_threads)
{
    //指定了大于0的线程数目才创建各种对象
    //否则都是默认构造
    if(maxNum > 0)
    {
        unsigned int recommend = ThreadPool::RecommendNumber();
        _maxNum = maxNum > recommend ? maxNum : recommend;

        _isRunning = true;
        _threads.reserve(_maxNum);

        try
        {
            for (int i = 0; i < _maxNum; i++)
                _threads.emplace_back(&ThreadPool::workerThread, this);
        }
        catch (...)
        {
            _isRunning = false;
            throw;
        }
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

unsigned int ThreadPool::RecommendNumber()
{
    return thread::hardware_concurrency();
}

unsigned int ThreadPool::CurrentNumber() const
{
    return _maxNum;
}

Reactor* ThreadPool::GetAvailReactor()
{

}

void ThreadPool::workerThread()
{
    try
    {
        while(_isRunning)
        {
            //在栈上新建一个Reactor对象
            Reactor reactor;
            //等待TcpConnection调用
            //...
            //接到可运行通知后开始运行
            reactor.Start();
        }
    }
    catch(...)
    {
        throw;
    }
}
