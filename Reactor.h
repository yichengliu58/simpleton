//
// Created by lyc-fedora on 15-4-25.
//
//反应器（Reactor）类
//负责执行IO复用和事件循环
//当事件到来时通知事件分发器
//每个工作线程只有一个Reactor
//需要检查是否满足要求isSingle()

#ifndef SIMPLETON_REACTOR_H
#define SIMPLETON_REACTOR_H


#include <thread>
#include <atomic>

using namespace std;

namespace simpleton
{
class Reactor
{
public:
    Reactor();
    ~Reactor();

    //开启反应器
    void Start();
private:
    //用于检测当前线程是否存在多个Reactor对象
    Reactor* _check;
    //是否在执行事件循环
    atomic<bool> _isReacting;
    //代表当前线程id
    thread::id _threadId;
};
}

#endif //SIMPLETON_REACTOR_H
