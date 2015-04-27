//
// Created by lyc-fedora on 15-4-25.
//

#include "Reactor.h"

using namespace simpleton;

thread_local Reactor* Reactor::_check = nullptr;

Reactor::Reactor()
:_isReacting(false),_threadId(this_thread::get_id())
{
    //检测本线程是否存在另一个Reactor
    //如果存在则将本对象销毁退出
    if(Reactor::_check)
        throw runtime_error("创建了重复的Reactor对象！");
    else
        _check = this;
}

Reactor::~Reactor()
{
    _isReacting = false;
    Reactor::_check = nullptr;
}

void Reactor::Start()
{
    if(this_thread::get_id() != _threadId)
        throw runtime_error("无法跨线程开启Reactor！");
    _isReacting = true;
    //开始IO复用
    //...
}