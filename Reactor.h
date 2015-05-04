//
// Created by lyc-fedora on 15-4-25.
//
//反应器（Reactor）类
//负责执行IO复用和事件循环
//当事件到来时通知事件分发器
//每个工作线程只有一个Reactor

#ifndef SIMPLETON_REACTOR_H
#define SIMPLETON_REACTOR_H


#include <thread>
#include <atomic>
#include <vector>
#include "Dispatcher.h"
#include "Multiplexer.h"

using namespace std;

namespace simpleton
{
class Reactor
{
public:
    Reactor();
    ~Reactor();

    Reactor(const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    //向反应器中注册（更新或添加）事件分派器
    void UpdateDispatcher(Dispatcher*);
    //删除分派器
    void DeleteDispatcher(Dispatcher*);

    //开启反应器
    void Start();
    //关闭反应器
    void Stop();
private:
    //用于检测当前线程是否存在多个Reactor对象
    thread_local static Reactor* _check;
    //是否在执行事件循环
    atomic<bool> _isReacting;
    //代表当前线程id
    thread::id _threadId;
    //表示这次事件循环后得到的可用事件分派器列表
    vector<Dispatcher*> _availDispatchers;
    //考虑到异常处理情况的话需要使用指针而不是对象本身
    unique_ptr<Multiplexer> _multiplexer;
};
}

#endif //SIMPLETON_REACTOR_H
