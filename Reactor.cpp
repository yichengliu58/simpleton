//
// Created by lyc-fedora on 15-4-25.
//

#include "Reactor.h"

using namespace simpleton;

Reactor::Reactor()
:_isReacting(false),_threadId(this_thread::get_id()),_check(nullptr)
{
    //检测本线程是否存在另一个Reactor
    //如果存在则将本对象销毁退出
    if(_check)
        throw runtime_error("创建了重复的Reactor对象！");
    else
        _check = this;
}

Reactor::~Reactor()
{
    _isReacting = false;
    _check = nullptr;
}

void Reactor::Start()
{
    _isReacting = true;
    //开始IO复用
    //...

}