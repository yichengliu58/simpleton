//
// Created by lyc-fedora on 15-4-22.
//
//线程池类
//直接作为Reactor池的封装器

#ifndef SIMPLETON_THREADPOOL_H
#define SIMPLETON_THREADPOOL_H

#include <vector>
#include <mutex>
#include <functional>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <deque>
#include "Reactor.h"

using namespace std;

namespace  simpleton
{

//用于等待指定线程集线程结束
//仅供线程池使用
class ThreadJoiner
{
public:
    //构造时必须指定要等待的线程集合
    explicit ThreadJoiner(vector<thread>& threadSet)
    :_threadSet(threadSet)
    { }

    //不可拷贝或移动
    ThreadJoiner(const ThreadJoiner&) = delete;
    ThreadJoiner& operator=(const ThreadJoiner&) = delete;
    ThreadJoiner(ThreadJoiner&&) = delete;
    ThreadJoiner& operator=(ThreadJoiner&&) = delete;

    //析构时等待所有线程结束
    ~ThreadJoiner()
    {
        for(auto& th : _threadSet)
        {
            if(th.joinable())
                th.join();
        }
    }
private:
    //一个vector引用！
    //用于引用指定的线程集合对象并且在析构时join该对象内部所有线程
    vector<thread>& _threadSet;
};

class ThreadPool
{
public:
    //定义线程函数类型
    using TaskType = function<void()>;

    //开启线程池
    //可以设置线程数目
    //否则为默认值
    ThreadPool(unsigned int maxNum = 0);

    //不可拷贝或移动
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    //析构函数先设置_isRunning为false
    //然后唤醒所有等待线程使其结束
    ~ThreadPool();

    //获取线程池推荐线程数目
    static unsigned int RecommendNumber();
    //获取线程池内当前线程数量
    unsigned int CurrentNumber() const;
    //选取一个负载最轻的线程
    //返回其中的Reactor
    Reactor* GetAvailReactor();
private:
    //本进程池运行的工作线程函数
    void workerThread();
    //用于初始化各个Reactor
    void initReactors();

    //条件变量和互斥锁
    condition_variable _cond;
    mutex _mtx;
    //最大线程数目
    //默认为hardware_currency()返回值
    unsigned int _maxNum;
    //是否在运行标志
    atomic<bool> _isRunning;
    //临时存储Reactor指针
    Reactor* _tmp;
    //当前指定的Reactor
    unsigned int _index;
    //存储Reactor对象指针
    //实际的Reactor本身是线程函数的栈上对象
    //可自动析构并设置这里的指针为空
    //所以可以安全使用
    vector<Reactor*> _reactors;
    //存储线程对象
    vector<thread> _threads;
    //线程的等待器
    ThreadJoiner _joiner;
};
}

#endif //SIMPLETON_THREADPOOL_H
