//
// Created by lyc-fedora on 15-4-22.
//
//线程池类

#ifndef SIMPLETON_THREADPOOL_H
#define SIMPLETON_THREADPOOL_H

#include <vector>
#include <mutex>
#include <functional>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <deque>

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
    ThreadPool(unsigned int maxQueue, unsigned int maxNum = 0);

    //不可拷贝或移动
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    //析构函数先设置_isRunning为false
    //然后唤醒所有等待线程使其结束
    ~ThreadPool();


    //提交一个函数对象在线程池中运行

    //默认情况下会阻塞调用线程
    void Submit(const TaskType &);
    /*void Submit(TaskType && );

    //重载函数：如果需要阻塞则返回-1
    bool TrySubmit(const TaskType &);
    bool TrySubmit(TaskType && );
*/
private:
    //本进程池运行的工作线程函数
    void workerThread();

    //条件变量和互斥锁
    condition_variable _condFull;
    condition_variable _condEmpty;
    mutex _mtx;
    //最大线程数目
    //默认为hardware_currency()返回值
    unsigned int _maxNum;
    //最大队列长度
    unsigned int _maxQueue;
    //是否在运行标志
    atomic<bool> _isRunning;
    //存储任务队列
    deque<TaskType> _taskQueue;
    //存储线程对象
    vector<thread> _threads;
    //线程的等待器
    ThreadJoiner _joiner;
};
}

#endif //SIMPLETON_THREADPOOL_H
