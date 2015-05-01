//
// Created by lyc-fedora on 15-4-27.
//
//多路复用器

#ifndef SIMPLETON_MULTIPLEXER_H
#define SIMPLETON_MULTIPLEXER_H

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <map>
#include "Dispatcher.h"

using namespace std;

namespace simpleton
{
class Multiplexer
{
public:
    Multiplexer();
    ~Multiplexer();

    //不可拷贝
    Multiplexer(const Multiplexer&) = delete;
    Multiplexer& operator=(const Multiplexer&) = delete;

    /*//移动语义
    Multiplexer(Multiplexer&& );
    Multiplexer& operator=(Multiplexer&& );*/

    //BUG！！！这里的两个方法应该仅由Reactor调用！！是否设置友元？
    //添加或更新复用器内部的映射关系并
    //更新内核事件表（epoll_ctl）
    //ADD 和 MOD
    void UpdateDispathcer(Dispatcher*);

    //删除一个分派器并更新内核事件表
    //DEL
    void DeleteDispatcher(Dispatcher*);

    //执行IO复用函数（epoll_wait）
    //等待获得可用的分派器列表
    void Wait(int,vector<Dispatcher*>&);
private:
    //表示内核事件表描述符（epoll_create）
    int _epollfd;
    //表示每次epoll_wait返回的可用事件列表
    vector<struct epoll_event> _eventList;
    //表示描述符和事件分发器的映射关系
    map<int,Dispatcher*> _dispatcherMap;
};
}

#endif //SIMPLETON_MULTIPLEXER_H
