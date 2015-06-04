//
// Created by lyc-fedora on 15-4-22.
//

#include "ThreadPool.h"

using namespace simpleton;

ThreadPool::ThreadPool(unsigned int maxNum)
    :_maxNum(0),
     _isRunning(false),
     _joiner(_threads),
     _tmp(nullptr),
     _index(0)
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
            initReactors();
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
    //如果到达上限重新置零
    if(_index >= _maxNum - 1)
        _index = 0;
    //循环分配
    return _reactors.at(_index++);
}

void ThreadPool::initReactors()
{
    for (int i = 0; i < _maxNum; i++)
    {
        _tmp = nullptr;
        _threads.emplace_back(&ThreadPool::workerThread, this);
        unique_lock<mutex> guard(_mtx);
        while (_tmp == nullptr)
            _cond.wait(guard);
        guard.unlock();
        //将Reactor指针加进去
        _reactors.push_back(_tmp);
    }
}

void ThreadPool::workerThread()
{
    try
    {
        while(_isRunning)
        {
            //在栈上新建一个Reactor对象
            Reactor reactor;
            _tmp = &reactor;
            //创建好后通知initReactor将指针存入vector
            _cond.notify_one();
            //然后开始循环
            reactor.Start();
        }
    }
    catch(...)
    {
        throw;
    }
}
